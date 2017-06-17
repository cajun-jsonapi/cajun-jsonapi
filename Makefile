EXE_NAME = ./example.out

$(EXE_NAME): example.cpp
	g++ -Iinclude -o $@ $^

clean:
	-rm $(EXE_NAME)

get_boost:
	wget https://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.tar.bz2
	tar -xf boost_1_64_0.tar.bz2

make_test:
	g++ -o test_prog test/*.cpp -I include/cajun/ -I boost_1_64_0/ -std=c++11

test: make_test
	./test_prog
