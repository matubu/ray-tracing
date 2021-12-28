NAME			:=	minirt

OBJECTS			:=	./bin
INCLUDES		:=	./includes
SOURCES			:=	./srcs

SRCS			:=	main.c \
					hook.c \
					libs/err.c \
					parsing/parse.c \
					parsing/line.c \
					parsing/push.c \
					libs/gnl.c \
					libs/split.c

OBJS			:=	$(addprefix ${OBJECTS}/, $(SRCS:.c=.o))

CC				:=	gcc
CFLAGS			:=	-Wall -Wextra -Werror \
					-Ofast -fno-strict-aliasing \
					-fomit-frame-pointer -mtune=native \
					-msse4.2 -mfpmath=sse -march=native \
					-funsafe-math-optimizations -funroll-loops \
					-ffast-math -flto -finline-functions

OS = $$(uname -s)

ECHO = echo
RED = \033[31m
GRE = \033[32m
GRA = \033[37m
BLU = \033[34m
EOC = \033[0m

all: $(NAME)

bonus: all

run: all
	@$(ECHO) "💪 $(GRE)Execution de $(NAME)$(EOC)"
	@./$(NAME) assets/minimal.rt

${OBJECTS}/%.o: ${SOURCES}/%.c
	@$(ECHO) "🔧 Compilation de $(BLU)${notdir $<}$(EOC)."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ -c $^ -I${INCLUDES}

$(NAME): $(OBJS)
	@if [ $(OS) = 'Linux' ]; then \
		$(ECHO) "⚙️  $(GRE)Compilation de ${NAME}... $(GRA)(Linux 🐧 mode)$(EOC)"; \
		${CC} $(CFLAGS) -L/usr/local/lib $(OBJS) -o $(NAME) -lm -lmlx -lXext -lX11 -lz; \
	else \
		$(ECHO) "⚙️  $(GRE)Compilation de ${NAME}... $(GRA)(macOS 🍎 mode)$(EOC)"; \
		${CC} $(CFLAGS) -lmlx -framework OpenGL -framework AppKit $(OBJS) -o $(NAME) -lm; \
	fi

clean:
	@echo "$(RED)📁 Supression des fichiers de $(OBJECTS)...$(EOC)"
	@rm -rf $(OBJECTS)

fclean: clean
	@echo "$(RED)⚙️  Supression de l'executable...$(EOC)"
	@rm -rf $(NAME)

re: fclean all

.PHONY: all run clean fclean re
