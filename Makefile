NAME = minirt

OBJ_BOTH = bin/main.o

BIN = bin
FLAGS = -Wall -Wextra -Werror -O3 -Ofast -fno-strict-aliasing -ffast-math -march=native -funsafe-math-optimizations -fomit-frame-pointer -funroll-loops -mtune=native -msse4.2 -mfpmath=sse 

OS = $$(uname -s)

ECHO = echo
RED = \033[31m
GRE = \033[32m
GRA = \033[37m
BLU = \033[34m
EOC = \033[0m

all: OBJ := $(OBJ_BOTH) $(OBJ_MANDATORY)
all: OBJ_UNUSED := $(OBJ_BONUS)
all: $(OBJ_MANDATORY) $(NAME)

bonus: OBJ := $(OBJ_BOTH) $(OBJ_BONUS)
bonus: OBJ_UNUSED := $(OBJ_MANDATORY)
bonus: $(OBJ_BONUS) $(NAME)

%.fdf:
	@$(ECHO) "$(GRE)● Using $@ map to run $(NAME) 🎨$(EOC)"
	@./$(NAME) maps/$@

run: all
	@$(ECHO) "$(GRE)● Launching $(NAME) 💪$(EOC)"
	@./$(NAME) maps/$(DEFAULT_MAP)

bin/%.o: %.c
	@$(ECHO) "$(BLU)● Compiling $^ 🔧$(EOC)"
	@rm -rf $(NAME) $(OBJ_UNUSED)
	@mkdir -p $(BIN)
	@gcc $(FLAGS) -c -I/usr/local/include $^ -o $@

$(NAME): $(OBJ_BOTH)
	@if [ $(OS) = 'Linux' ]; then \
		$(ECHO) "$(GRE)● Compiling to binary ⚙️  $(GRA)(Linux 🐧 mode)$(EOC)"; \
		gcc $(FLAGS) -L/usr/local/lib $(OBJ) -o $(NAME) -l m -l mlx -l Xext -l X11 -l z; \
	else \
		$(ECHO) "$(GRE)● Compiling to binary ⚙️  $(GRA)(macOS 🍎 mode)$(EOC)"; \
		gcc $(FLAGS) -l mlx -framework OpenGL -framework AppKit $(OBJ) -o $(NAME) -lm; \
	fi

clean:
	@$(ECHO) "$(RED)● Removing /$(BIN) 📁$(EOC)"
	@rm -rf $(BIN)

fclean: clean
	@$(ECHO) "$(RED)● Removing binary ⚙️ $(EOC)"
	@rm -rf $(NAME)

re: fclean all

.PHONY: all bonus run clean fclean re
