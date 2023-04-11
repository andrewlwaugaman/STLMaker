generator: generator.c 3d.o
	gcc -Wall -Werror -std=c11 -o generator generator.c 3d.o -lm -g
3d.o: 3d.h 3d.c
	gcc -Wall -Werror -std=c11 3d.c -c -g
clean:
	rm -f 3d.o; rm -f generator;
