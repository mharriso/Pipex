#include "pipex.h"
#include <stdio.h>
#include <fcntl.h>
#include "libft.h"

#define STDOUT 1
#define STDIN 0

#define SIDE_OUT 1
#define SIDE_IN 0

# define R_LEFT 1
# define R_RIGHT 2
# define R_DRIGHT 3
# define R_DLEFT 4

typedef struct s_cmd
{
	char	**arg;
	char	*path;
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

void	print_error(char *s)
{
	ft_putstr_fd(PROMT, STDOUT);
	if (errno)
		perror(s);
	else
		ft_putendl_fd(s, STDOUT);
}

void		save_args(t_info *info, int argc, char **argv)
{
	if(argc != 5)
		exit_error("Invalid arguments: file1 info1 info2 file2");
	info->limiter = NULL;
	info->size = 2;
	info->cmd = malloc(info->size * sizeof(t_cmd));
	info->cmd->arg = malloc(info->size * sizeof(char **));
	if(!info->cmd || !info->cmd->arg)
		exit_error("Can not allocate cmd");
	info->cmd[0].arg = ft_split(argv[2], ' ');
	info->cmd[1].arg = ft_split(argv[3], ' ');
	if(!info->cmd[0].arg || !info->cmd[1].arg)
		exit_error("Can not allocate cmd array arguments");
	info->file_in = argv[1];
	info->file_out = argv[4];
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

void get_path_to_cmd(t_info *info, char **env)
{
	char	*path;
	char	**path_array;
	int		i;
	int		j;

	path = get_env_value("PATH", env);
	path_array = split_path(path, ':');
	if(!path_array)
		exit_error("Can not allocate path_array");
	ft_free(&path);
	i = -1;
	while(++i < info->size)
	{
		j = -1;
		while(path_array[++j])
		{
			info->cmd[i].path = set_path(path_array[j], info->cmd[i].arg[0]);
			if(info->cmd[i].path)
				break ;
		}
	}
	free_array(path_array);
}

// int exec_info(t_list *info, char **env)
// {
// 	pid_t	pid;
// 	int		ret;
// 	int		status;
// 	int		pipe_open;

// 	ret = EXIT_FAILURE;
// 	pipe_open = 0;
// 	if (info->type == TYPE_PIPE || (info->previous && info->previous->type == TYPE_PIPE))
// 	{
// 		pipe_open = 1;
// 		if (pipe(info->pipes))
// 			return (exit_fatal());
// 	}
// 	pid = fork();
// 	if (pid < 0)
// 		return (exit_fatal());
// 	else if (pid == 0)
// 	{
// 		if (info->type == TYPE_PIPE
// 			&& dup2(info->pipes[SIDE_IN], STDOUT) < 0)
// 			return (exit_fatal());
// 		if (info->previous && info->previous->type == TYPE_PIPE
// 			&& dup2(info->previous->pipes[SIDE_OUT], STDIN) < 0)
// 			return (exit_fatal());
// 		if ((ret = execve(info->args[0], info->args, env)) < 0)
// 			pritnf("error: cannot execute %s\n", info->args[0]);
// 		exit(ret);
// 	}
// 	else
// 	{
// 		waitpid(pid, &status, 0);
// 		if (pipe_open)
// 		{
// 			close(info->pipes[SIDE_IN]);
// 			if (!info->next || info->type == TYPE_BREAK)
// 				close(info->pipes[SIDE_OUT]);
// 		}
// 		if (info->previous && info->previous->type == TYPE_PIPE)
// 			close(info->previous->pipes[SIDE_OUT]);
// 		if (WIFEXITED(status))
// 			ret = WEXITSTATUS(status);
// 	}
// 	return (ret);
// }

static int	redirect(char *fname, int red_type)
{
	int		fd;
	int		flags;
	int		dup_fd;

	dup_fd = STDOUT;
	if (red_type == R_LEFT)
	{
		flags = O_RDONLY;
		dup_fd = STDIN;
	}
	else if (red_type == R_RIGHT)
		flags = O_CREAT | O_WRONLY | O_TRUNC;
	else if (red_type == R_DRIGHT)
		flags = O_CREAT | O_WRONLY | O_APPEND;
	else
		return (-2);
	fd = open(fname, flags, S_IREAD | S_IWRITE);
	if (fd == -1)
		return (-1);
	dup2(fd, dup_fd);
	return (fd);
}

int	execute_cmd(t_info *info, char **env)
{

}

void execute_commands(t_info *info, char **env)
{
	int		i;
	int		fd;
	char	**args;

	get_path_to_cmd(info, env);

	i = -1;
	while(++i < info->size)
	{
		//printf("path = |%s|\n", info->cmd[i].path);
		//printf("cmd  = |%s|\n", info->cmd[i].arg[0]);
		if (i == 0)
		{
			fd = redirect(info->file_in, R_LEFT);
			if(fd == -1)
			{
				print_error(info->file_in);
				continue ;
			}

		}
		else if (i == info->size - 1)
		{
			fd = redirect(info->file_out, R_RIGHT);
			if(fd == -1)
			{
				print_error(info->file_out);
				continue ;

			}
		}
		execve(info->cmd[i].path, info->cmd[i].arg, env);
		print_error("Error");
	}


}

void	free_info(t_info *info)
{
	int	i;

	i = 0;
	while(i < info->size)
	{
		free_array(info->cmd[i].arg);
		ft_free(&info->cmd[i].path);
		i++;
	}
	free(info);
}
int main(int argc, char const **argv, char **env)
{
	t_info *info;

	info = malloc(sizeof(t_info));
	if(!info)
		exit_error("Can not allocate info");
	save_args(info, argc, (char **)argv);
	execute_commands(info, env);


	free_info(info);

	//sleep(20);
	return (0);
}

