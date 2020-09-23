sigAlg=p256_dilithium3
openssl_oqs=/home/fzhang/fizz/huawei/bin/openssl
#Create root CA certificate
echo "**********************Make CA*************************"
mkdir CA
cd CA && mkdir certs private
echo 01 > serial && touch index.txt 
cp /usr/local/ssl/${openssl_oqs}.cnf ./

${openssl_oqs} genpkey -algorithm ${sigAlg} -out private/ca.key
${openssl_oqs} req -new -newkey ${sigAlg} -x509 -days 3650 -extensions v3_ca -key private/ca.key -out certs/ca.crt
${openssl_oqs} x509 -in certs/ca.crt -out certs/ca.crt -outform PEM

#Create intermediate CA
echo "\n\n*****************Make inermediate CA**************"
mkdir ICA
cd ICA
mkdir certs csr private
touch index.txt && echo 01 > serial && echo 01 > crlnumber
${openssl_oqs} genpkey -algorithm ${sigAlg} -out private/ica.key
${openssl_oqs} req -new -newkey ${sigAlg} -key private/ica.key -out csr/ica.csr

cd ..
${openssl_oqs} ca -extensions v3_intermediate_ca -days 2650 -notext -batch -in ICA/csr/ica.csr -out ICA/certs/ica.crt -keyfile private/ca.key
${openssl_oqs} x509 -in ICA/certs/ica.crt -out ICA/certs/ica.crt -outform PEM

${openssl_oqs} verify -CAfile certs/ca.crt ICA/certs/ica.crt  

#Create server certifivate using Intermediate CA
echo "\n\n****************Make server certificate************"
cd ICA
${openssl_oqs} genpkey -algorithm ${sigAlg} -out private/${sigAlg}.key
${openssl_oqs} req -new -newkey ${sigAlg} -days 365 -key private/${sigAlg}.key -out csr/${sigAlg}.csr
${openssl_oqs} ca -in csr/${sigAlg}.csr -days 365 -out certs/${sigAlg}.crt -cert certs/ica.crt -keyfile private/ica.key

#make ca bundle
echo "\n\n***************copy certificate********************"
cd ../../
cp CA/certs/ca.crt ./
cp CA/ICA/certs/ica.crt ./
cp CA/ICA/certs/${sigAlg}.crt ./
cp CA/ICA/private/${sigAlg}.key ./

echo "\n\n************Make certificate chain*****************"
cat ica.crt ca.crt >cabundle.crt
${openssl_oqs} verify -CAfile cabundle.crt ${sigAlg}.crt
cat ${sigAlg}.crt ica.crt > certchain.crt
