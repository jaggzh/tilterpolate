default: run

v:
	make 2>&1 | less

vi:
	vim \
		Makefile \
		main.c \
		termstuff.c \
		termstuff.h \
		linfit.c \
		eq-plot.pl \
		data-samples/sample-test.txt \
		data-samples/sample1.txt \
		data-samples/sample2.txt

run: linfit-run

linfit-run: linfit
	./linfit

clean:
	rm linfit

linfit: linfit.c main.c termstuff.c
	g++ -ggdb3 -Wall -pedantic -o linfit main.c linfit.c termstuff.c

eq-plot-perl-run:
	./eq-plot.pl
