#include "MyServer.h"
#include <iostream>
using namespace std;
FizzServerAcceptor::FizzServerAcceptor(uint16_t port, std::shared_ptr<FizzServerContext> serverCtx, EventBase* evb)
    : evb_(evb), ctx_(serverCtx) 
{
    socket_ = AsyncServerSocket::UniquePtr(new AsyncServerSocket(evb_));
    socket_->bind(port);
    socket_->listen(MAXCLIENTNUM);
    socket_->addAcceptCallback(this, evb_);
    socket_->startAccepting();
    cout << "Started listening on " << socket_->getAddress() << endl;
}

void FizzServerAcceptor::connectionAccepted(folly::NetworkSocket fdNetworkSocket, const SocketAddress& clientAddr) noexcept 
{
    int fd = fdNetworkSocket.toFd();

    cout << "Client " << clt_num << ": Connection accepted from " << clientAddr <<endl;
    auto sock = new AsyncSocket(evb_, folly::NetworkSocket::fromFd(fd));
    std::shared_ptr<AsyncFizzServer> transport = AsyncFizzServer::UniquePtr(new AsyncFizzServer(AsyncSocket::UniquePtr(sock), ctx_));
    socket_->pauseAccepting();
    auto serverCb = std::make_unique<FizzServerCB>(transport, this, ctx_);
    //inputHandler_ = std::make_unique<TerminalInputHandler>(evb_, serverCb.get());
    //transport->setSecretCallback(serverCb.get());
    cbs_[clt_num] = std::move(serverCb);
    transport->accept(cbs_[clt_num].get());
    clt_num++;
    socket_->startAccepting();
}

void FizzServerAcceptor::acceptError(const std::exception& ex) noexcept 
{
    cerr << "Failed to accept connection: " << ex.what() << endl;
    evb_->terminateLoopSoon();
}

void FizzServerAcceptor::done() 
{
    int i;
    for(i = 0; i < clt_num; i++)
        cbs_[i].reset();
    //socket_->startAccepting();
}

void FizzServerAcceptor::removeClient(AsyncFizzServer::HandshakeCallback *eb)
{
	int i;
	
	for(i = 0; i < clt_num; i++)
	{
		if(cbs_[i].get() == eb)
		{
			break;
		}
	}
	cbs_[i].reset();
	for(; i < clt_num - 1; i++)
		cbs_[i] = std::move(cbs_[i+1]);
	clt_num--;
	std::cout<<"There are "<<clt_num<<" clients connecting."<<std::endl;
}
/***********************************************************************************/
/***********************************************************************************/

void FizzServerCB::HandshakeSuccessLog() 
{
    auto& state = transport_->getState();
    //auto serverCert = state.serverCert();
    //auto clientCert = state.clientCert();
    
    cout << folly::to<std::string>("TLS Version: ", toString(*state.version())) << endl;
    cout << folly::to<std::string>("Cipher Suite: ", toString(*state.cipher())) <<endl;
    cout << folly::to<std::string>("Named Group: ", (state.group() ? toString(*state.group()) : "(none)")) << endl;
    cout << folly::to<std::string>("Signature Scheme: ", (state.sigScheme() ? toString(*state.sigScheme()) : "(none)")) << endl;
    /*    folly::to<std::string>("  PSK: ", toString(*state.pskType())),
        folly::to<std::string>(
            "  PSK Mode: ",
            (state.pskMode() ? toString(*state.pskMode()) : "(none)")),*/
    cout << folly::to<std::string>("Key Exchange Type: ", toString(*state.keyExchangeType())) << endl;
    /*    folly::to<std::string>("  Early: ", toString(*state.earlyDataType())),
        folly::to<std::string>(
            "  Server identity: ",
            (serverCert ? serverCert->getIdentity() : "(none)")),
        folly::to<std::string>(
            "  Client Identity: ",
            (clientCert ? clientCert->getIdentity() : "(none)")),
        folly::to<std::string>(
            "  Server Certificate Compression: ",
            (state.serverCertCompAlgo() ? toString(*state.serverCertCompAlgo())
                                        : "(none)")),
        folly::to<std::string>("  ALPN: ", state.alpn().value_or("(none)")),
        folly::to<std::string>("  Secrets:"),
        folly::to<std::string>(
            "    External PSK Binder: ", secretStr(externalPskBinder_)),
        folly::to<std::string>(
            "    Resumption PSK Binder: ", secretStr(resumptionPskBinder_)),
        folly::to<std::string>(
            "    Early Exporter: ", secretStr(earlyExporterSecret_)),
        folly::to<std::string>(
            "    Early Client Data: ", secretStr(clientEarlyTrafficSecret_)),
        folly::to<std::string>(
            "    Client Handshake: ", secretStr(clientHandshakeTrafficSecret_)),
        folly::to<std::string>(
            "    Server Handshake: ", secretStr(serverHandshakeTrafficSecret_)),
        folly::to<std::string>(
            "    Exporter Master: ", secretStr(exporterMasterSecret_)),
        folly::to<std::string>(
            "    Resumption Master: ", secretStr(resumptionMasterSecret_)),
        folly::to<std::string>(
            "    Client Traffic: ", secretStr(clientAppTrafficSecret_)),
        folly::to<std::string>(
            "    Server Traffic: ", secretStr(serverAppTrafficSecret_))};*/
  }

/*void ServerProcesser::run(FizzServerAcceptor* acceptor, std::shared_ptr<FizzServerContext> ctx_, int fd)
{
	auto sock = new AsyncSocket(evb_, folly::NetworkSocket::fromFd(fd));
	std::shared_ptr<AsyncFizzServer> transport = AsyncFizzServer::UniquePtr(new AsyncFizzServer(AsyncSocket::UniquePtr(sock), ctx_));
	auto serverCb = std::make_unique<FizzServerCB>(transport, this, ctx_);
	transport->setSecretCallback(serverCb.get());
	cbs[cltnum] = std::move(serverCb);
	transport->accept(cbs[cltnum].get());
	cltnum++;
	}*/
