#include "MyServer.h"
#include <folly/FileUtil.h>
#include <fizz/crypto/Utils.h>
int main()
{
	const uint16_t s_port = 6666;
	EventBase evb;
	auto ctx = make_shared<FizzServerContext>();
	//ctx->setCompatibilityMode(true);
	//ctx->setSupportedGroups({NamedGroup::x25519_kyber768});

	auto certManager = std::make_unique<CertManager>();
	std::string certData;
	std::string keyData;
	folly::readFile("../Certs/certchain.crt", certData);
	folly::readFile("../Certs/p256_dilithium3.key", keyData);
	if(certData.empty() || keyData.empty())
		std::cerr<<"Read file error!"<<std::endl;
	auto cert = CertUtils::makeSelfCert(certData, keyData);
	certManager->addCert(std::move(cert), true);
	ctx->setCertManager(std::move(certManager));
	ctx->setSupportedCiphers({{CipherSuite::TLS_AES_128_GCM_SHA256}});
	ctx->setSupportedGroups({NamedGroup::p384_kyber768});
	//ctx->setSupportedSigSchemes({SignatureScheme::p521_falcon1024});
        //ctx->setSupportedCompressionAlgorithms({});
	auto srv = new std::unique_ptr<FizzServerAcceptor>(new FizzServerAcceptor(s_port, ctx, &evb));

    	evb.loop();

	return 0;
}
