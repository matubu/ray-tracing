all:
	gcc -Wall -Wextra -Werror -o ray-tracing main.c -lm
run: all
	./ray-tracing
