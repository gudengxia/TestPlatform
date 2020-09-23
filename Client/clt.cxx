#include <fizz/client/AsyncFizzClient.h>
#include <fizz/crypto/Utils.h>
#include <fizz/crypto/aead/AESGCM128.h>
#include <fizz/crypto/aead/OpenSSLEVPCipher.h>
#include <fizz/server/AsyncFizzServer.h>
#include <fizz/server/TicketTypes.h>
#include <folly/String.h>
#include <folly/io/async/AsyncSSLSocket.h>
#include <folly/io/async/SSLContext.h>
#include <folly/portability/GFlags.h>
#include <iostream>
using namespace fizz;
using namespace fizz::client;
using namespace fizz::server;
using namespace folly;
using namespace folly::ssl;
using namespace std;

class BogoTestClient: public AsyncSocket::ConnectCallback,
	public AsyncFizzClient::HandshakeCallback,
	public AsyncTransportWrapper::ReadCallback
{
public:
	BogoTestClient(EventBase* evb, uint16_t port, std::shared_ptr<const FizzClientContext> clientContext): clientContext_(clientContext)
	{
		socket_ = AsyncSocket::UniquePtr(new AsyncSocket(evb));
		socket_->connect(this, "127.0.0.1", port, 1000);
	}

	void connectSuccess() noexcept override
	{
		transport_ = AsyncFizzClient::UniquePtr( new AsyncFizzClient(std::move(socket_), clientContext_));
		transport_->connect(this, nullptr, folly::none, std::string("resumption-id"));
		cerr<<"success"<<endl;
		auto buf = folly::IOBuf::copyBuffer("Maria Takigi, I love U.\n");
		transport_->writeChain(nullptr, move(buf));
	}

	void connectErr(const AsyncSocketException& ex) noexcept override
	{
		cerr << "TCP connect failed: " << ex.what();
		socket_.reset();
		success_ = false;
	}

	void fizzHandshakeSuccess(AsyncFizzClient*) noexcept override
	{
		success_ = true;
		transport_->setReadCB(this);
	}

	void fizzHandshakeError(AsyncFizzClient*, folly::exception_wrapper ex) noexcept override
	{
		cerr << "Handshake error: " << ex.what();
		transport_.reset();

		// If the server sent us a protocol_version alert assume that
		if (ex.what().find("received alert: protocol_version, in state ExpectingServerHello") != std::string::npos)
		{
			unimplemented_ = true;
		}
		success_ = false;
	}

	void getReadBuffer(void** /* bufReturn */, size_t* /* lenReturn */) override
	{
		throw std::runtime_error("getReadBuffer not implemented");
	}

	void readDataAvailable(size_t /* len */) noexcept override
	{
		cerr << "readDataAvailable not implemented";
	}

	bool isBufferMovable() noexcept override
	{
		return true;
	}

	void readBufferAvailable(std::unique_ptr<IOBuf> buf) noexcept override
	{
		io::Cursor cursor(buf.get());
		std::unique_ptr<IOBuf> write = IOBuf::create(0);
		io::Appender appender(write.get(), 50);
		while (!cursor.isAtEnd())
		{
			uint8_t byte;
			cursor.pull(&byte, 1);
			byte ^= 0xff;
			appender.push(&byte, 1);
		}
		transport_->writeChain(nullptr, std::move(write));
	}

	void readEOF() noexcept override {}

	void readErr(const AsyncSocketException&) noexcept override {}

	bool unimplemented() const {return unimplemented_;}

	bool success() const {return *success_;}
	
 private:
	AsyncSocket::UniquePtr socket_;
	std::shared_ptr<const FizzClientContext> clientContext_;
	AsyncFizzClient::UniquePtr transport_;

	bool unimplemented_{false};
	Optional<bool> success_;
};

int main()
{
	EventBase evb;
	auto ctx = make_shared<FizzClientContext>();
	//auto s = AsyncSocket::UniquePtr(new AsyncSocket(&evb)); 
	auto clt = make_unique<BogoTestClient>(&evb, 8443, ctx); 
	//ctx->setCompatibilityMode(true);
	evb.loop();

	if(!clt->success())
	{
		cerr<<"Connection failed."<<endl;
	}
	else
	{
		cout<<"Connect success!"<<endl;
	}
	return 0;
}
