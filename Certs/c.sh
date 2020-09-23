FizzDir=/home/fzhang/fizz/huawei/bin
#ip netns exec ns1 ${FizzDir}/fizz s_client -kex akcn_hybrid -connect 10.1.3.3:6666 -verify -cafile ca.crt
${FizzDir}/fizz s_client -kex p521_kyber1024 -connect 127.0.0.1:6666 -verify -cafile ca.crt
#sudo ip netns exec ns1 ../Client/clt.exe
#sudo ip netns exec ns1 ${FizzDir}/fizz s_client -connect 10.1.3.3:6666
