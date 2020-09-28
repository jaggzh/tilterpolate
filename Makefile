default: run

v:
	make 2>&1 | less

vi:
	vim \
		Makefile \
		main.c \
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

linfit: linfit.c main.c
	g++ -ggdb3 -Wall -pedantic -o linfit main.c linfit.c

eq-plot-perl-run:
	./eq-plot.pl