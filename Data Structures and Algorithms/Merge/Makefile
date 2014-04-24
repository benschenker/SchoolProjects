merge.exe: merge.cpp
	g++ merge.cpp

debug:
	g++ -g -o MergeDebug.exe merge.cpp

clean:
	rm -f *.exe *.o *.stackdump *~

backup:
	test -d backups || mkdir backups
	cp *.cpp backups
	cp *.h backups
	cp Makefile backups
