#include "pipex.h"
#include <stdio.h>
#include "libft.h"

#define STDOUT 1
#define STDIN 0

typedef struct s_commands
{
	char **array;
	char **path;
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

void		save_args(t_commands *cmd, int argc, char **argv)
{
	if(argc != 5)
		exit_error("Invalid arguments: file1 cmd1 cmd2 file2");
	cmd->array = malloc(3 * sizeof(char *));
	cmd->path = ft_calloc(3, sizeof(char *));
	if(!cmd->array || !cmd->path)
		exit_error("Can not allocate cmd array");
	cmd->array[0] = argv[2];
	cmd->array[1] = argv[3];
	cmd->array[2] = NULL;
	cmd->file_in = argv[1];
	cmd->file_out = argv[4];
}

char *get_env_value(char *name, char **env)
{
	int		i;
	char	*eq;
	char	*value;

	i = 0;
	while(env[i])
	{
		if(!ft_strncmp(env[i], name, ft_strlen(name)))
		{
			eq = ft_strchr(env[i], '=');
			if(eq)
			{
				value = ft_strdup(eq + 1);
				if(!value)
					exit_error("Can not allocate value");
				return (value);
			}
		}
		i++;
	}
	return (NULL);
}

void get_path_to_cmd(t_commands *cmd, char **env)
{
	char	*path;
	char	**path_array;
	int		i;
	int		j;

	path = get_env_value("PATH", env);
	path_array = split_path(path, ':');
	if(!path_array)
		exit_error("Can not allocate path_array");
	i = 0;
	while(cmd->array[i])
	{
		j = 0;
		while(path_array[j])
		{
			cmd->path[i] = ft_strjoin(path_array[j], cmd->array[i]);
			printf("%s\n", cmd->path[i]);
			if(!access(cmd->path[i], 1))
				break ;
			else
				{
					free(cmd->path[i]);
					cmd->path[i] = NULL;
				}
			j++;
		}
		i++;
	}
}

int	execute_cmd(char *cmd_path, char *arg)
{
	pid_t	pid;
	int		ret;
	int		status;

	pid = fork();
	if (pid < 0)
		exit_error("Fork rror");
	else if (pid == 0)
	{

		if ((ret = execve(cmd->args[0], cmd->args, env)) < 0)
		{
			show_error("error: cannot execute ");
			show_error(cmd->args[0]);
			show_error("\n");
		}
		exit(ret);
	}
	else
	{
		waitpid(pid, &status, 0);
		if (pipe_open)
		{
			close(cmd->pipes[SIDE_IN]);
			if (!cmd->next || cmd->type == TYPE_BREAK)
				close(cmd->pipes[SIDE_OUT]);
		}
		if (cmd->previous && cmd->previous->type == TYPE_PIPE)
			close(cmd->previous->pipes[SIDE_OUT]);
		if (WIFEXITED(status))
			ret = WEXITSTATUS(status);
	}

}

void execute_commands(t_commands *cmd, char **env)
{
	int	i;

	get_path_to_cmd(cmd, env);
	for (size_t i = 0; cmd->array[i]; i++)
		printf("%s\n", cmd->path[i]);
	while(cmd->array[i])
	{
		execve(cmd->path[i], cmd->array[i], env);
		i++;
	}

}


int main(int argc, char const **argv, char **env)
{
	t_commands *cmd;

	cmd = malloc(sizeof(t_commands));
	if(!cmd)
		exit_error("Can not allocate cmd");
	save_args(cmd, argc, (char **)argv);
	execute_commands(cmd, env);
	return (0);
}

