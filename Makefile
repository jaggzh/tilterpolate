BIN=radterp

default: run

v:
	make 2>&1 | less

vi:
	vim \
		Makefile \
		README.md \
		main.cpp \
		radterpolate.cpp \
		radterpolate.h \
		termstuff.c \
		termstuff.h \
		linfit.c \
		eq-plot.pl \
		data-samples/sample-test.txt \
		data-samples/sample1.txt \
		data-samples/sample2.txt

run: $(BIN)-run

$(BIN)-run: $(BIN)
	./$(BIN)
	#./$(BIN) & echo "attach $!; break main.cpp 70" > .gdbinit; fg

clean:
	rm $(BIN)

$(BIN): main.cpp radterpolate.cpp radterpolate.h termstuff.c termstuff.h
	g++ -ggdb3 -Wall -pedantic -o $(BIN) main.cpp radterpolate.cpp termstuff.c

field_build:
	g++ -ggdb3 -D SHOW_FS_FIELD -Wall -pedantic -o $(BIN) main.cpp radterpolate.cpp termstuff.c

field: field_build
	./radterp

debug: main.cpp radterpolate.cpp radterpolate.h termstuff.c termstuff.h
	g++ -ggdb3 -D RAD_VERBOSE=2 -Wall -pedantic -o $(BIN) main.cpp radterpolate.cpp termstuff.c
	./radterp


eq-plot-perl-run:
	./eq-plot.pl
