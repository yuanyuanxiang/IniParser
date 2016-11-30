cc=g++

test:test.o IniParser.o
		cc -o test test.o IniParser.o -lstdc++
test.o:test.cpp
		cc -g -c test.cpp -o test.o
IniParser.o:IniParser.cpp
		cc -g -c IniParser.cpp -o IniParser.o
clean:
	rm -rf *.o

