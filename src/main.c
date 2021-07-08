#include "pipex.h"
#include <stdio.h>

#define STDOUT 1
#define STDIN 0

typedef struct s_commands
{
	char **array;
	char *file_in;
	char *file_out;
} t_commands;

void	exit_error(char *s)
{
	if (errno)
		perror(s);
	else
		ft_putendl_fd(s, STDOUT);
	exit(EXIT_FAILURE);
}

int main(int argc, char const **argv, char **env)
{
	t_commands *cmd;

	cmd = malloc(sizeof(t_commands));
	if(!cmd)
	{
		perror("Can not allocate cmd\n");
		return (1);
	}
	//save_args(cmd, argc, argv);
	return (0);
}

