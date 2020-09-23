ASANFLAGS=
OQSLIBS_DIR=/home/fzhang/fizz/huawei
obj = main.o MyClient.o
all: ${obj}
	g++ -o clt.exe $^ ${ASANFIAGS} -lasan -lfizz -lfolly -lgflags -pthread -lssl -lcrypto -lglog -ldl -loqs -lsodium -lfolly_test_util -ldouble-conversion -levent -lboost_context -lboost_filesystem -lboost_system -lboost_program_options -liberty -lakcn -L${OQSLIBS_DIR}/lib -laigis
	rm ${obj}

${obj} : %.o: %.cxx
	g++ $< -o $@ -std=c++17 -c -I${OQSLIBS_DIR}/include ${ASANFLAGS}
.PHONY: clean  
clean:	
	rm -rf ${obj} clt.exe
