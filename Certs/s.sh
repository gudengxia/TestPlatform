FizzDir=/home/fzhang/fizz/huawei/bin
#../Server/srv.exe
#sudo ip netns exec ns2 ${FizzDir}/fizz s_server -key p521_falcon1024.key -cert certchain.crt -accept 6666 #1>/dev/null 2>/dev/null
${FizzDir}/fizz s_server -key p256_falcon512.key -cert certchain.crt -accept 6666 #1>/dev/null 2>/dev/null
