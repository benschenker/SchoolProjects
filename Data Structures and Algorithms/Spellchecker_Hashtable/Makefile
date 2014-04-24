spellcheck.exe: spellcheck.o hash.o
	g++ -o spellcheck.exe spellcheck.o hash.o
	
spellcheck.o: spellcheck.cpp hash.h
	g++ -c spellcheck.cpp
	
hash.o:hash.cpp hash.h
	g++ -c hash.cpp
debug:
	g++ -g -o spellDebug.exe spellcheck.cpp hash.cpp
