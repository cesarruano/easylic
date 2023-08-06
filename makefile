CXX = g++

CXXFLAGS = -std=c++11 -Wall

LDFLAGS = -lssl -lcrypto

OBJS = *.o

test_easylic.exe: test_easylic.o easylic.o handy_crypto.o
	$(CXX) $(CXXFLAGS) -o test_easylic.exe test_easylic.o easylic.o handy_crypto.o $(LDFLAGS)

test: clean test_easylic.exe
	./test_easylic.exe

test_easylic.o: ./test/test_easylic.cpp
	echo $(DEVLIB)
	$(CXX) $(CXXFLAGS) -I./src -I$(HANDY_CRYPTO)/src -I$(HANDY_CRYPTO)/test -c ./test/test_easylic.cpp -o test_easylic.o

easylic.o: ./src/easylic.cpp ./src/easylic.hpp
	$(CXX) $(CXXFLAGS) -I$(HANDY_CRYPTO)/src -I$(HANDY_CRYPTO)/test -c ./src/easylic.cpp -o easylic.o

handy_crypto.o: $(HANDY_CRYPTO)/src/handy_crypto.cpp $(HANDY_CRYPTO)/src/handy_crypto.hpp
	$(CXX) $(CXXFLAGS) -c $(HANDY_CRYPTO)/src/handy_crypto.cpp -o handy_crypto.o

clean:
	rm -f $(OBJS) test_easylic.exe
	rm -f *.csv

