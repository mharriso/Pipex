NAME	=	pipex
NAME_BONUS = pipex_bonus
SRCDIR	=	src
OBJDIR	=	.obj
FT		=	libft
INC		=	include
HEADER	=	pipex.h \
			libft.h
HFILES	=	$(addprefix $(INC)/, $(HEADER))

SRCS	= main.c \
		  utils.c \
		  split_path.c \
		  execute.c \
		  pipex_handler.c
SRCS_BONUS	= main_bonus.c \
			  utils.c \
			  split_path.c \
			  execute.c \
			  pipex_handler.c
SRCS	:=	$(addprefix $(SRCDIR)/, $(SRCS))
FLAGS	=	-g -Wall -Wextra -Werror
OBJS	=	$(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJS_BONUS	=	$(SRCS_BONUS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

GREEN      = [1;32m
RESET      = [0m

all: ${NAME}

$(NAME): ${OBJECTS}
	make bonus -C ${FT}
	gcc -I ${INC} $(OBJS) -l ft -L ${FT} -o $(NAME)
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

bonus: ${NAME_BONUS}

$(NAME_BONUS): ${OBJECTS}
	make bonus -C ${FT}
	gcc -I ${INC} $(OBJS_BONUS) -l ft -L ${FT} -o $(NAME_BONUS)
	@echo "$(GREEN)Built target $(NAME_BONUS)$(RESET)"
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	gcc ${FLAGS} -I ${INC} -c $< -o $@
$(OBJDIR):
	@mkdir -p ${NAME_BONUS}

norm:
	norminette src/* inc/*

.PHONY: all clean fclean re bonus
