OQSLIBS_DIR=/home/fzhang/fizz/huawei
obj=main.o MyServer.o
all: ${obj}
	g++ -o srv.exe $^ -lasan -lfizz -lfolly -lgflags -pthread -lakcn -lssl -lcrypto -lglog -ldl -loqs -lsodium -lfolly_test_util -ldouble-conversion -levent -lboost_context -lboost_filesystem -lboost_system -lboost_program_options -liberty -L${OQSLIBS_DIR}/lib -laigis
	rm ${obj}
${obj} : %.o: %.cxx
	g++ $< -o $@ -std=c++17 -c -I${OQSLIBS_DIR}/include
.PHONY: clean   
clean: 
	rm -rf ${obj} srv.exe
