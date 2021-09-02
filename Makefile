SRCS = main.c
OUTPUTS = $(patsubst %.c,%.o,$(SRCS))
NAME = ray-tracing
cc = gcc -Wall -Wextra -Werror

%.o: %.c
	$(cc) -c $*.c -o $*.o

all: $(NAME)
$(NAME): $(OUTPUTS)
	$(cc) -o $(NAME) $(OUTPUTS)

clean:
	rm -rf $(OUTPUTS)
fclean: clean
	rm -rf $(NAME)
re: fclean all