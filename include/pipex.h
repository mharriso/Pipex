#ifndef PIPEX_H
# define PIPEX_H

# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>

# include "libft.h"


# define RESET   "\033[0m"  //все атрибуты по умолчанию
# define BLACK   "\033[30m"  //30-37 цвет текста
# define RED     "\033[31m"
# define GREEN   "\033[32m"
# define YELLOW  "\033[33m"
# define BLUE    "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN    "\033[36m"
# define WHITE   "\033[37m"

# define PROMT "\033[33m->\033[0m pipex: "

# define STDOUT 1
# define STDIN 0
# define SIDE_OUT 1
# define SIDE_IN 0

# define R_LEFT 1
# define R_RIGHT 2
# define R_DRIGHT 3
# define R_DLEFT 4
# define PIPE 5

typedef struct s_cmd
{
	char	**arg;
	char	*path;
	int		type;
	int		pipe[2];
	size_t	size;
} t_cmd;

typedef struct s_info
{
	t_cmd	*cmd;
	char	*file_in;
	char	*file_out;
	char	*limiter;
	size_t	size;
} t_info;

char	**split_path(char const *s, char c);
void	ft_free(char **str);
char	**free_array(char **array);
void	exit_error(char *s);
void	print_error(char *msg, char *arg);




#endif
