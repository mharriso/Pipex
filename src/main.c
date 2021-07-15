#include "pipex.h"
#include <fcntl.h>
#include "libft.h"
// #include <sys/types.h>
// #include <sys/wait.h>

t_cmd	*init_cmd(char **argv, size_t size)
{
	t_cmd		*cmd;
	size_t		i;

	cmd = malloc(size * sizeof(t_cmd));
	if(!cmd)
		exit_error("Can not allocate cmd");
	i = 0;
	while(i < size)
	{
		cmd[i].arg = ft_split(argv[i + 2], ' ');
		if(!cmd[i].arg)
			exit_error("Can not allocate cmd arguments");
		if(i == 0)
			cmd[i].type = R_LEFT;
		else if(i == size - 1)
			cmd[i].type = R_RIGHT;
		else
			cmd[i].type = PIPE;
		i++;
	}
	return (cmd);
}

void		save_args(t_info *info, int argc, char **argv)
{
	if(argc != 5)
		exit_error("Invalid arguments: file1 cmd1 cmd2 file2");
	info->limiter = NULL;
	info->size = 2;
	info->file_in = argv[1];
	info->file_out = argv[4];
	info->cmd = init_cmd(argv, info->size);
}

void		save_args_bonus(t_info *info, int argc, char **argv)
{
	if(argc < 5)
		exit_error("Invalid arguments: file1 cmd1 cmd2 ... cmdn file2");
	info->limiter = NULL;
	info->size = argc - 3;
	info->file_in = argv[1];
	info->file_out = argv[argc - 1];
	info->cmd = init_cmd(argv, info->size);
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
	size_t		i;
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

// 	if (pipe(info->pipes) == -1)
// 		exit_error("Error: ");
// 	pid = fork();
// 	if (pid < 0)
// 		exit_error("Error: ");
// 	else if (pid == 0)
// 	{
// 		if (i > 0 && dup2(info->pipes[SIDE_IN], STDOUT) < 0)
// 			return (exit_fatal());
// 		if (i > 0 && dup2(info->previous->pipes[SIDE_OUT], STDIN) < 0)
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
// 	}
// 	return (0);
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
	return (0);
}

int	set_redirect(t_cmd *cmd, t_info *info)
{
	int	fd;

	if (cmd->type == R_LEFT)
	{
		fd = redirect(info->file_in, R_LEFT);
		if(fd == -1)
		{
			print_error("Error: ", info->file_in);
			return (-1);
		}
	}
	else if (cmd->type == R_RIGHT)
	{
		fd = redirect(info->file_out, R_RIGHT);
		if(fd == -1)
		{
			print_error("Error: ", info->file_out);
			return (-1);
		}
	}
	return (0);
}

void execute_commands(t_info *info, char **env)
{
	size_t	i;
	int		status;
	pid_t	pid;

	get_path_to_cmd(info, env);
	i = -1;
	// for(int j = 0; j < info->size; j++)
	// 	if(pipe(info->cmd[j].pipe) == -1)
	// 		exit_error("Pipe");
	//printf("path %s\n", info->cmd[0].path);
	while(++i < info->size)
	{
		if(info->cmd[i].path == NULL)
		{
			print_error(info->cmd[i].arg[0], ": command not found");
			continue ;
		}
		if(pipe(info->cmd[i].pipe) == -1)
			exit_error("Pipe");
		pid = fork();
		if(pid == -1)
			exit_error("Fork");
		else if (pid == 0)
		{
			if(set_redirect(&info->cmd[i], info) == -1)
				exit(EXIT_FAILURE);
			if(info->cmd[i].type == R_LEFT)
			{
				if(dup2(info->cmd[i].pipe[SIDE_OUT], STDOUT) == -1)
					exit_error("dup2");
				close(info->cmd[i].pipe[SIDE_OUT]);
				close(info->cmd[i].pipe[SIDE_IN]);
			}
			else if(info->cmd[i].type == R_RIGHT)
			{
				if(dup2(info->cmd[i - 1].pipe[SIDE_IN], STDIN) == -1)
					exit_error("dup2");
				close(info->cmd[i].pipe[SIDE_OUT]);
				close(info->cmd[i].pipe[SIDE_IN]);
			}
			else if(info->cmd[i].type == PIPE)
			{
				if(dup2(info->cmd[i].pipe[SIDE_OUT], STDOUT) == -1)
					exit_error("dup2");
				if(dup2(info->cmd[i - 1].pipe[SIDE_IN], STDIN) == -1)
					exit_error("dup2");
				close(info->cmd[i].pipe[SIDE_OUT]);
				close(info->cmd[i].pipe[SIDE_IN]);
			}

			execve(info->cmd[i].path, info->cmd[i].arg, env);
			exit_error("execve");
		}
		else
		{
			waitpid(pid, &status, 0);
			close(info->cmd[i].pipe[SIDE_OUT]);
			// if(info->cmd[i].type != R_LEFT)
			// 	close(info->cmd[i - 1].pipe[SIDE_OUT]);
			if(info->cmd[i].type == R_RIGHT)
			{
				close(info->cmd[i].pipe[SIDE_OUT]);
				close(info->cmd[i].pipe[SIDE_IN]);
			}
		}

	}
}

void	free_info(t_info *info)
{
	size_t	i;

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

