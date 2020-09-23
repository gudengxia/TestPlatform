#pragma once
/*#include <fizz/crypto/aead/AESGCM128.h>
#include <fizz/crypto/aead/OpenSSLEVPCipher.h>
#include <fizz/protocol/BrotliCertificateCompressor.h>
#include <fizz/protocol/DefaultCertificateVerifier.h>
#include <fizz/protocol/ZlibCertificateCompressor.h>
#include <fizz/protocol/ZstdCertificateCompressor.h>
#include <fizz/server/AsyncFizzServer.h>
#include <fizz/server/SlidingBloomReplayCache.h>
#include <fizz/server/TicketTypes.h>
#include <fizz/tool/FizzCommandCommon.h>
#include <fizz/util/Parse.h>
#include <folly/String.h>
#include <folly/io/async/AsyncSSLSocket.h>
#include <folly/io/async/SSLContext.h>
#include <folly/Format.h>
#include <folly/io/async/AsyncSSLSocket.h>
#include <folly/io/async/AsyncServerSocket.h>

#include <string>
#include <vector>*/
#include <fizz/server/AsyncFizzServer.h>
#include <folly/io/async/AsyncServerSocket.h>
#include <fizz/server/FizzServerContext.h>
#include <fizz/crypto/Utils.h>
#include <fizz/crypto/aead/AESGCM128.h>
#include <fizz/crypto/aead/OpenSSLEVPCipher.h>
#include <fizz/server/AsyncFizzServer.h>
#include <fizz/server/TicketTypes.h>
#include <folly/String.h>
/*#include <folly/io/async/AsyncSSLSocket.h>
#include <folly/io/async/SSLContext.h>
#include <folly/portability/GFlags.h*/

using namespace fizz;
using namespace fizz::server;
using namespace folly;
using namespace folly::ssl;

const int MAXCLIENTNUM = 10;
class FizzServerCB;

class FizzServerAcceptor : AsyncServerSocket::AcceptCallback 
{
 public:
    explicit FizzServerAcceptor( uint16_t port, std::shared_ptr<FizzServerContext> serverCtx, EventBase* evb);
  
    void connectionAccepted(folly::NetworkSocket fdNetworkSocket, const SocketAddress& clientAddr) noexcept override;

    void acceptError(const std::exception& ex) noexcept override;
  
    void done();

    void removeClient(AsyncFizzServer::HandshakeCallback *cb); 
 private:
    EventBase* evb_{nullptr};
    std::shared_ptr<FizzServerContext> ctx_;
    AsyncServerSocket::UniquePtr socket_;
    std::unique_ptr<AsyncFizzServer::HandshakeCallback> cbs_[MAXCLIENTNUM];
    int clt_num{0};
	//ServerProcesser dispatcher;
};


class FizzServerCB: public AsyncFizzServer::HandshakeCallback,
		    public AsyncTransportWrapper::ReadCallback
{
 public:
    explicit FizzServerCB(std::shared_ptr<AsyncFizzServer> transport, FizzServerAcceptor* acceptor, std::shared_ptr<FizzServerContext> serverCtx)
        : transport_(transport), acceptor_(acceptor)
    {
	    //client_num++;
    }
    
    void fizzHandshakeSuccess(AsyncFizzServer* server) noexcept override 
    {
        server->setReadCB(this);
        connected_ = true;
        cout << "TLS handshake succeed!" << endl;
	HandshakeSuccessLog();
    }

    void fizzHandshakeError( AsyncFizzServer* /*server*/, exception_wrapper ex) noexcept override 
    {
        cout << "Handshake error: " << ex.what() << endl;
        finish();
    }

    void getReadBuffer(void** bufReturn, size_t* lenReturn) override 
    {
        *bufReturn = readBuf_.data();
        *lenReturn = readBuf_.size();
    }

    void readDataAvailable(size_t len) noexcept override 
    {
        std::cout << std::string(readBuf_.data(), len);
    }

    bool isBufferMovable() noexcept override 
    {
        return true;
    }

    void readBufferAvailable(std::unique_ptr<IOBuf> buf) noexcept override 
    {
        std::cout << StringPiece(buf->coalesce()).str();
    }

    void readEOF() noexcept override 
    {
        cout << "Read EOF, TLS finishes." << endl;
        finish();
    }

    void readErr(const AsyncSocketException& ex) noexcept override 
    {
        cout << "Read error: " << ex.what() <<endl;
        finish();
    }

    void fizzHandshakeAttemptFallback(std::unique_ptr<IOBuf> clientHello) override
    {
        cerr<<"Unsupport Fallback."<< endl;
    }
    bool connected() const
    {
        return connected_;
    }

    void write(std::unique_ptr<IOBuf> msg)  
    {
        transport_->writeChain(nullptr, std::move(msg));
    }

    void close()
    {
        finish();
    }

 protected:
   void HandshakeSuccessLog();

    void finish() 
    {
        if (transport_ ) 
        {
        // Forcibly clean up connection
           transport_->closeNow();
        }
        transport_.reset();
        //acceptor_->done();
	acceptor_->removeClient(this);
    }

    std::shared_ptr<AsyncFizzServer> transport_;
    FizzServerAcceptor* acceptor_;
    std::array<char, 8192> readBuf_;
    bool connected_{false};
};

/*class ServerProcesser
{
public:
	void run(int fd, std::shared_ptr<FizzServerContext> ctx);
private:
	std::unique_ptr<AsyncFizzServer::HandshakeCallback> cbs[MAXCLIENTNUM];
	int cltnum{0};
	};*/
