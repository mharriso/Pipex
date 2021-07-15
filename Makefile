NAME	=	pipex
SRCDIR	=	src
OBJDIR	=	.obj
FT		=	libft
INC		=	include
HEADER	=	pipex.h \
			libft.h
HFILES	=	$(addprefix $(INC)/, $(HEADER))

SRCS	= main.c \
		  utils.c \
		  split_path.c

SRCS	:=	$(addprefix $(SRCDIR)/, $(SRCS))
FLAGS	=	-Wall -Wextra -Werror
OBJECTS	=	$(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

GREEN      = [1;32m
RESET      = [0m

all: ${NAME}

$(NAME): ${OBJECTS}
	make bonus -C ${FT}
	gcc -I ${INC} $(OBJECTS) -l ft -L ${FT} -o $(NAME)
	@echo "$(GREEN)Built target $(NAME)$(RESET)"
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	gcc ${FLAGS} -I ${INC} -c $< -o $@
$(OBJDIR):
	@mkdir -p $@

clean:
	make clean -C ${FT}
	rm -rf ${OBJDIR}
fclean: clean
	make fclean -C ${FT}
	rm -f ${NAME}
re: fclean all

bonus: ${NAME}

norm:
	norminette src/* inc/*

.PHONY: all clean fclean re bonus
