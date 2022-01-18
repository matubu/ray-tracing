NAME			:=	minirt

OBJECTS			:=	./bin
INCLUDES		:=	./includes
SOURCES			:=	./srcs

SRCS			:=	main.c \
					libs/err.c \
					parsing/parse.c \
					parsing/line.c \
					parsing/push.c \
					libs/gnl.c \
					libs/split.c
SRCS_MANDATORY	:=	controls.c hook.c
SRCS_BONUS		:=	controls_bonus.c hook_bonus.c

MAP_MANDATORY	:=	./assets/minimal.rt
MAP_BONUS		:=	./assets/minimal_bonus.rt

OBJS			:=	$(addprefix ${OBJECTS}/, $(SRCS:.c=.o))
OBJS_MANDATORY	:=	$(addprefix ${OBJECTS}/, $(SRCS_MANDATORY:.c=.o))
OBJS_BONUS		:=	$(addprefix ${OBJECTS}/, $(SRCS_BONUS:.c=.o))

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

all: OBJS += $(OBJS_MANDATORY)
all: OBJS_UNUSED := $(OBJS_BONUS)
all: $(OBJS_MANDATORY) $(NAME)

bonus: OBJS += $(OBJS_BONUS)
bonus: OBJS_UNUSED := $(OBJS_MANDATORY)
bonus: $(OBJS_BONUS) $(NAME)

run:
	@$(ECHO) "💪 $(GRE)Execution de $(NAME)$(EOC)"
	./$(NAME) $(MAP_MANDATORY)

${OBJECTS}/%.o: ${SOURCES}/%.c
	@$(ECHO) "🔧 Compilation de $(BLU)${notdir $<}$(EOC)."
	@rm -rf $(NAME) $(OBJS_UNUSED)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ -c $^ -I${INCLUDES}

$(NAME): $(OBJS)
	@if [ $(OS) = 'Linux' ]; then \
		$(ECHO) "⚙️  $(GRE)Compilation de ${NAME}...$(EOC)"; \
		${CC} $(CFLAGS) -L/usr/local/lib $(OBJS) -o $(NAME) -lm -lmlx -lXext -lX11 -lz; \
	else \
		$(ECHO) "⚙️  $(GRE)Compilation de ${NAME}...$(EOC)"; \
		${CC} $(CFLAGS) -lmlx -framework OpenGL -framework AppKit $(OBJS) -o $(NAME) -lm; \
	fi

clean:
	@echo "$(RED)📁 Supression des fichiers de $(OBJECTS)...$(EOC)"
	@rm -rf $(OBJECTS)

fclean: clean
	@echo "$(RED)⚙️  Supression de l'executable...$(EOC)"
	@rm -rf $(NAME)

re: fclean all

.PHONY: all bonus run clean fclean re
