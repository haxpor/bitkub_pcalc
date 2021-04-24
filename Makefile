.PHONY: all clean

OUT=pcalc

all: ${OUT}

${OUT}: main.o
	g++ -std=c++17 -Wall -Wextra -O3 $< -o $@

main.o: main.cpp
	g++ -std=c++17 -Wall -Wextra -O3 -c $<

clean:
	rm -rf ${OUT} *.o
