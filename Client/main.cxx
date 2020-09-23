#include "MyClient.h"
#include <folly/FileUtil.h>
#include <iostream>
#include <fstream>
int main()
{
	//EventBase evb;
	const int n = 1;
	const char logfile[]="Result/tm.txt";
	//const string s_ip = "10.1.3.3";
	const string s_ip = "192.168.72.106";
	uint16_t s_port = 6666;
	
	auto caFilePtr = "../Certs/ca.crt";
	folly::ssl::X509StoreUniquePtr storePtr;
	std::shared_ptr<const CertificateVerifier> verifier;
	
        storePtr.reset(X509_STORE_new());
	if (X509_STORE_load_locations(storePtr.get(), caFilePtr, nullptr) ==0)
	{
		std::cerr << "Failed to load CA certificates" << std::endl;
		return 1;
	}
	verifier = std::make_shared<const DefaultCertificateVerifier>(VerificationContext::Client, std::move(storePtr));
	
	auto ctx = make_shared<FizzClientContext>();
	//ctx->setCompatibilityMode(true);
	ctx->setSupportedCiphers({{CipherSuite::TLS_AES_128_GCM_SHA256}});
	ctx->setSupportedGroups({NamedGroup::p384_kyber768});
	ctx->setDefaultShares({NamedGroup::p384_kyber768});
	ctx->setSupportedSigSchemes({SignatureScheme::p256_dilithium3});
	//ctx->setSupportedCompressionAlgorithms({});
	/*auto certManager = std::make_unique<CertManager>();
	std::string certData;
	std::string keyData;
	folly::readFile("maria.crt", certData);
	folly::readFile("maria.key", keyData);
	if(certData.empty() || keyData.empty())
		std::cerr<<"Read file error!"<<std::endl;
	auto cert = CertUtils::makeSelfCert(certData, keyData);
	//ctx->setClientCertificate(std::move(cert));*/

  	int i = 0;
	uint64_t total_time = 0;
	uint64_t t, tm[n];
	memset(tm, 0, sizeof(tm));

	int cnt = 0;

	//EventBase evb;
	//auto clt = make_unique<MyClient>(&evb, ctx, verifier);
	for(i = 0; i < n; i++)
	{
		EventBase evb;
		auto clt = make_unique<MyClient>(&evb, ctx, verifier);
		clt->connect(s_ip, s_port);
		evb.loop();
		//cout << clt->getHandshakeTime() << endl;
		t = clt->getHandshakeTime();
		if(t != 0)
		{
			total_time += t;
			tm[cnt] = t;
			cnt++;
		}
		//usleep(100);
		//auto msg = folly::IOBuf::copyBuffer("av\r\n");
		//clt->write(std::move(msg));
	}

	if(cnt > 0)
	{
		std::cout<<"cnt="<<cnt<<std::endl;
		std::cout<<"average handshake time:"<<total_time/(long double)cnt<<std::endl;
		ofstream out(logfile, ios::out);
		for(i = 0; i < cnt; i++)
			out<<tm[i]<<std::endl;
		out.close();
	}
	else
		std::cout<<"cnt=0"<<std::endl;
	
	return 0;
}
