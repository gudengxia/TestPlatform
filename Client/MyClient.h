#pragma once
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
#include <chrono>
using namespace fizz;
using namespace fizz::client;
using namespace fizz::server;
using namespace folly;
using namespace folly::ssl;

class MyClient : public AsyncSocket::ConnectCallback,
                 public AsyncFizzClient::HandshakeCallback,
		 public AsyncTransportWrapper::ReadCallback
		 //public InputHandlerCallback
//public AsyncTransport::ReplaySafetyCallback
{
public:
	MyClient(EventBase* evb, std::shared_ptr<FizzClientContext> clientContext, std::shared_ptr<const CertificateVerifier> verifier);

	void connect(const string ip, uint16_t port);

	void connectErr(const AsyncSocketException& ex) noexcept override;

	void connectSuccess() noexcept override;

	void fizzHandshakeSuccess(AsyncFizzClient* /*client*/) noexcept override;

	void fizzHandshakeError(AsyncFizzClient* /*client*/, exception_wrapper ex) noexcept override;
	
	void close();

	void getReadBuffer(void** bufReturn, size_t* lenReturn) override;

	void readDataAvailable(size_t len) noexcept override;
	
	bool isBufferMovable() noexcept override;

	void readBufferAvailable(std::unique_ptr<IOBuf> buf) noexcept override;

	void readEOF() noexcept override;

	void readErr(const AsyncSocketException& ex) noexcept override;

	bool connected() const;

	void write(std::unique_ptr<IOBuf> msg);
	
	uint64_t getHandshakeTime()
	{
		return hstime;
	}

protected:
	void doHandshake();
private:
	EventBase* evb_;
	std::shared_ptr<FizzClientContext> clientContext_;
	std::shared_ptr<const CertificateVerifier> verifier_;
	AsyncSocket::UniquePtr sock_;
	AsyncFizzClient::UniquePtr transport_;
	std::array<char, 8192> readBuf_;
	std::chrono::time_point<std::chrono::system_clock> start, end;
	uint64_t hstime;
};
