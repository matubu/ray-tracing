NAME			:=	minirt

OBJECTS			:=	./bin
INCLUDES		:=	./includes
SOURCES			:=	./srcs

LIBFT			:=	./libft

SRCS			:=	main.c \
					core/ray.c \
					core/vector.c

OBJS			:=	$(addprefix ${OBJECTS}/, $(SRCS:.c=.o))

CC				:=	gcc
CFLAGS			:=	-Wall -Wextra -Werror -O3 -Ofast -fno-strict-aliasing -fomit-frame-pointer -mtune=native -msse4.2 -mfpmath=sse -march=native -funsafe-math-optimizations -funroll-loops -ffast-math
CINCLUDES		:=	-I${INCLUDES} -I${LIBFT}/includes -I/usr/local/include


OS = $$(uname -s)

ECHO = echo
RED = \033[31m
GRE = \033[32m
GRA = \033[37m
BLU = \033[34m
EOC = \033[0m

all: $(NAME)

run: all
	@$(ECHO) "$(GRE)● Launching $(NAME) 💪$(EOC)"
	@./$(NAME)

${OBJECTS}/%.o: ${SOURCES}/%.c
	@$(ECHO) "$(BLU)● Compiling $^ 🔧$(EOC)"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ -c $< ${CINCLUDES}

$(NAME): $(OBJS)
	@make -C ${LIBFT}
	@if [ $(OS) = 'Linux' ]; then \
		$(ECHO) "$(GRE)● Compiling to binary ⚙️  $(GRA)(Linux 🐧 mode)$(EOC)"; \
		${CC} $(CFLAGS) -L/usr/local/lib $(OBJS) -o $(NAME) -l m -l mlx -l Xext -l X11 -l z; \
	else \
		$(ECHO) "$(GRE)● Compiling to binary ⚙️  $(GRA)(macOS 🍎 mode)$(EOC)"; \
		${CC} $(CFLAGS) -l mlx -framework OpenGL -framework AppKit $(OBJS) -o $(NAME) -lm; \
	fi

clean:
	@$(ECHO) "$(RED)● Removing $(OBJECTS) 📁$(EOC)"
	@make -C ${LIBFT} clean
	@rm -rf $(OBJECTS)

fclean: clean
	@$(ECHO) "$(RED)● Removing binary ⚙️ $(EOC)"
	@make -C ${LIBFT} fclean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all run clean fclean re
