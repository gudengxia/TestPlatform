#include "MyClient.h"
#include <stdio.h>
#include <iostream>
using namespace std;
const int TIMEOUT = 10000;
MyClient::MyClient(EventBase* evb, std::shared_ptr<FizzClientContext> clientContext, std::shared_ptr<const CertificateVerifier> verifier):
  evb_(evb), clientContext_(clientContext), verifier_(move(verifier)), hstime(0)
{
}

void MyClient::connect(const std::string ip, uint16_t port)
{
	sock_ = AsyncSocket::UniquePtr(new AsyncSocket(evb_));
	sock_->connect(this, ip, port, TIMEOUT);
}

void MyClient::connectErr(const AsyncSocketException& ex) noexcept
{
	 cout << "Connect error: " << ex.what() << endl;
	 this->close();
}

void MyClient::connectSuccess() noexcept 
{
  //cout << "Connection established." << endl;
	start = std::chrono::system_clock::now();
	doHandshake();
}

void MyClient::fizzHandshakeSuccess(AsyncFizzClient* /*client*/) noexcept 
{
	/*
	if (transport_->isReplaySafe())
	{
		printHandshakeSuccess();
	}
	else
	{
		LOG(INFO) << "Early handshake success.";
		transport_->setReplaySafetyCallback(this);
	}
	*/
	end = std::chrono::system_clock::now();
	auto d = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	//cout << "Finished handshake," << "handshake consumes:" << d.count() << "ns." << endl;
	//cout<<d.count()<<endl;
	hstime = d.count();
	transport_->setReadCB(this);
	//transport_->writeChain(nullptr, folly::IOBuf::copyBuffer("I am Client\r\n"));
	usleep(1);
	this->close();
	usleep(1);
}

void MyClient::fizzHandshakeError(AsyncFizzClient* /*client*/, exception_wrapper ex) noexcept 
{
	cout << "Handshake error: " << ex.what() << endl;
	hstime = 0;
	this->close();
}

void MyClient::close()
{
	evb_->terminateLoopSoon();
	if (transport_)
	{
		transport_->close();
	}
        if (sock_)
	{
		sock_->close();
	}
}


void MyClient::doHandshake()
{
	transport_ = AsyncFizzClient::UniquePtr(new AsyncFizzClient(std::move(sock_), clientContext_));
	transport_->connect(this, verifier_, std::string("maria takigi"), folly::none, std::chrono::milliseconds(TIMEOUT));
}

void MyClient::getReadBuffer(void** bufReturn, size_t* lenReturn) 
{
	*bufReturn = readBuf_.data();
	*lenReturn = readBuf_.size();
}

void MyClient::readDataAvailable(size_t len) noexcept 
{
	readBufferAvailable(IOBuf::copyBuffer(readBuf_.data(), len));
}

bool MyClient::isBufferMovable() noexcept 
{
	return true;
}

void MyClient::readBufferAvailable(std::unique_ptr<IOBuf> buf) noexcept 
{
	cout << "Received Data:" << StringPiece(buf->coalesce()).str() <<endl;
}

void MyClient::readEOF() noexcept 
{
	this->close();
}

void MyClient::readErr(const AsyncSocketException& ex) noexcept 
{
	cout << "Read error: " << ex.what() << endl;
	this->close();
}

bool MyClient::connected() const
{
	return transport_ && !transport_->connecting() && transport_->good();
}

void MyClient::write(std::unique_ptr<IOBuf> msg)
{
	if (transport_)
	{
		transport_->writeChain(nullptr, std::move(msg));
	}

}
