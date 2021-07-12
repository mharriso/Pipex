#include "pipex.h"
#include <stdio.h>
#include "libft.h"

#define STDOUT 1
#define STDIN 0

typedef struct s_commands
{
	char ***array;
	char **path;
	char *file_in;
	char *file_out;
} t_commands;

void	ft_free(char **str)
{
	if(!*str)
		return ;
	free(*str);
	*str = NULL;
}

char	**free_array(char **array)
{
	size_t		i;

	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
	return (NULL);
}

void	exit_error(char *s)
{
	if (errno)
		perror(s);
	else
	{
		ft_putstr_fd(PROMT, STDOUT);
		ft_putendl_fd(s, STDOUT);
	}
	exit(EXIT_FAILURE);
}

void		save_args(t_commands *cmd, int argc, char **argv)
{
	if(argc != 5)
		exit_error("Invalid arguments: file1 cmd1 cmd2 file2");
	cmd->array = malloc(3 * sizeof(char **));
	cmd->path = ft_calloc(3, sizeof(char *));
	if(!cmd->array || !cmd->path)
		exit_error("Can not allocate cmd array");
	cmd->array[0] = ft_split(argv[2], ' ');
	cmd->array[1] = ft_split(argv[3], ' ');
	cmd->array[2] = NULL;
	if(!cmd->array[0] || !cmd->array[1])
		exit_error("Can not allocate cmd array element");
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
char	*get_cmd_name(char *src)
{
	char	*cmd;
	char	*space;
	size_t	size;

	space = ft_strchr(src, ' ');
	if(space)
	{
		size = space - src;
		cmd = malloc(size + 1);
		if(!cmd)
			exit_error("Can not allocate cmd");
		ft_memcpy(cmd, src, size);
		cmd[size] = '\0';
	}
	else
	{
		cmd = ft_strdup(src);
		if(!cmd)
			exit_error("Can not allocate cmd");
	}
	return (cmd);
}

char	*set_path(char *path, char *cmd)
{
	char *res;

	if(cmd[0] == '.' || cmd[0] == '/')
		res = ft_strdup(cmd);
	else
		res = ft_strjoin(path, cmd);
	if(!res)
		exit_error("Can not allocate res");
	if(!access(res, 1))
		return (res);
	else
		free(res);
	return (NULL);
}

void get_path_to_cmd(t_commands *cmd, char **env)
{
	char	*tmp;
	char	**path_array;
	int		i;
	int		j;

	tmp = get_env_value("PATH", env);
	path_array = split_path(tmp, ':');
	if(!path_array)
		exit_error("Can not allocate path_array");
	ft_free(&tmp);
	i = -1;
	while(cmd->array[++i])
	{
		j = -1;
		while(path_array[++j])
		{
			cmd->path[i] = set_path(path_array[j], cmd->array[i][0]);
			if(cmd->path[i])
				break ;
		}
		ft_free(&tmp);
	}
	free_array(path_array);
}

void execute_commands(t_commands *cmd, char **env)
{
	int	i;
	char **args;

	get_path_to_cmd(cmd, env);
	for (size_t i = 0; cmd->array[i]; i++)
		printf("path = |%s|\n", cmd->path[i]);
	while(cmd->array[i])
	{
		printf("cmd  = |%s|\n", cmd->array[i][0]);
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

	free_array(cmd->path);
	free(cmd->array);
	free(cmd);

	//sleep(20);
	return (0);
}

