#ifndef PIPEX_H
# define PIPEX_H

# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include "libft.h"

# define PROMT "\033[33m->\033[0m pipex: "

# define STDOUT 1
# define STDIN 0
# define STDERR 2
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
	pid_t	pid;
	size_t	size;
}	t_cmd;

typedef struct s_info
{
	t_cmd	*cmd;
	char	*file_in;
	char	*file_out;
	char	*limiter;
	size_t	size;
}	t_info;

char	**split_path(char const *s, char c);
void	ft_free(char **str);
char	**free_array(char **array);
void	exit_error(char *s);
void	exit_error_arg(char *msg, char *arg);
void	execute_commands(t_info *info, char **env);
char	*get_env_value(char *name, char **env);
void	pipex_handler(t_info *info, int i);

#endif
