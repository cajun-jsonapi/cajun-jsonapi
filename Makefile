EXE_NAME = ./example.out

$(EXE_NAME): example.cpp
	g++ -Iinclude -o $@ $^

clean:
	-rm $(EXE_NAME)
