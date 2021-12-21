# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: acoezard <acoezard@student.42nice.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/05 11:42:41 by acoezard          #+#    #+#              #
#    Updated: 2021/12/03 18:07:33 by acoezard         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		:=	libft.a

OBJECTS		:=	./bin
SOURCES		:=	./sources
INCLUDES	:=	./includes

FILES 		:=	ft_isalnum.c ft_isalpha.c ft_isascii.c ft_isdigit.c \
				ft_islower.c ft_isupper.c ft_isprint.c ft_isspace.c \
				ft_tolower.c ft_toupper.c \
				ft_putchar_fd.c ft_putendl_fd.c ft_putnbr_fd.c \
				ft_putstr_fd.c ft_putunbr_fd.c ft_putunbr_base_fd.c \
				ft_bzero.c ft_calloc.c ft_memccpy.c ft_memchr.c \
				ft_memcmp.c ft_memcpy.c ft_memmove.c ft_memset.c  \
				ft_itoa.c ft_atoi.c ft_substr.c get_next_line.c \
				str/str_split.c \
				str/str_cat.c \
				str/str_chr.c \
				str/str_cpy.c \
				str/str_dup.c \
				str/str_join.c \
				str/str_lcat.c \
				str/str_lcpy.c \
				str/str_len.c \
				str/str_mapi.c \
				str/str_cmp.c \
				str/str_ncmp.c \
				str/str_nstr.c \
				str/str_rchr.c \
				str/str_trim.c \
				str/str_iteri.c \
				list/list_create.c \
				list/list_create_node.c \
				list/list_add_front.c \
				list/list_add_back.c \
				list/list_remove.c \
				list/list_clear.c

SRCS		:=	$(addprefix ${SOURCES}/, ${FILES})
OBJS		:=	$(addprefix ${OBJECTS}/, $(FILES:.c=.o))

CC			:=	gcc
CINCLUDES	:=	-I${INCLUDES}
CFLAGS		:=	-Wall -Wextra -Werror

BLACK		:=	"\033[1;30m"
RED			:=	"\033[1;31m"
GREEN		:=	"\033[1;32m"
CYAN		:=	"\033[1;35m"
BLUE		:=	"\033[1;36m"
WHITE		:=	"\033[1;37m"
EOC			:=	"\033[0;0m"

${OBJECTS}/%.o : ${SOURCES}/%.c
	@echo "● Compilation de "$(BLUE)"${notdir $<}"$(EOC)"."
	@mkdir -p $(dir $@)
	@${CC} ${CFLAGS} ${CINCLUDES} -c $^ -o $@

all: ${NAME}

${NAME}: ${OBJS}
	@echo $(GREEN)"● Generation de la librairie ${NAME}..."$(EOC)
	@ar rcs ${NAME} ${OBJS}

clean:
	@rm -rf ${OBJECTS}

fclean: clean
	@rm -f ${NAME}

re:	fclean all

.PHONY: all clean fclean re
