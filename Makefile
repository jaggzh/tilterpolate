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

clean:
	rm $(BIN)

$(BIN): main.cpp radterpolate.cpp termstuff.c
	g++ -ggdb3 -Wall -pedantic -o $(BIN) main.cpp radterpolate.cpp termstuff.c

eq-plot-perl-run:
	./eq-plot.pl
