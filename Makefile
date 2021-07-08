NAME	=	pipex
SRC		=	src
FT		=	libft
INC		=	include
HEADER	=	pipex.h
HFILES	=	$(addprefix $(INC)/, $(HEADER))

SRC_F	= main.c \
          utils.c

SOURCE	= $(addprefix $(SRC)/, $(SRC_F))
FLAGS	= #-Wall -Wextra -Werror
OBJECTS	= ${SOURCE:.c=.o}


all: ${NAME}

$(NAME): ${OBJECTS}
	make bonus -C ${FT}
	gcc -I ${INC} $(OBJECTS) -l ft -L ${FT} -o $(NAME)

%.o: %.c ${HFILES}
	gcc ${FLAGS} -I ${INC} -c $< -o $@

clean:
	make clean -C ${FT}
	rm -f ${OBJECTS}
fclean: clean
	make fclean -C ${FT}
	rm -f ${NAME}
re: fclean all

bonus: ${NAME}

norm:
	norminette src/* inc/*

.PHONY: all clean fclean re bonus
