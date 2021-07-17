#include "pipex.h"

static char	*set_path(char *path, char *cmd)
{
	char	*res;

	if (cmd[0] == '.' || cmd[0] == '/')
		res = ft_strdup(cmd);
	else
		res = ft_strjoin(path, cmd);
	if (!res)
		exit_error("Can not allocate res");
	if (!access(res, 1))
		return (res);
	else
		free(res);
	return (NULL);
}

static void	get_path_to_cmd(t_info *info, char **env)
{
	char	*path;
	char	**path_array;
	size_t	i;
	int		j;

	path = get_env_value("PATH", env);
	path_array = split_path(path, ':');
	if (!path_array)
		exit_error("Can not allocate path_array");
	ft_free(&path);
	i = -1;
	while (++i < info->size)
	{
		j = -1;
		while (path_array[++j])
		{
			info->cmd[i].path = set_path(path_array[j], info->cmd[i].arg[0]);
			if (info->cmd[i].path)
				break ;
		}
	}
	free_array(path_array);
}

static void	close_fd(t_info *info, int i)
{
	close(info->cmd[i].pipe[SIDE_OUT]);
	if (info->cmd[i].type != R_LEFT)
		close(info->cmd[i - 1].pipe[SIDE_IN]);
	if (info->cmd[i].type == R_RIGHT || info->cmd[i].type == R_DRIGHT)
	{
		close(info->cmd[i].pipe[SIDE_IN]);
		if (info->cmd[i].type == R_DRIGHT)
			unlink(info->file_in);
	}
}

void	execute_commands(t_info *info, char **env)
{
	size_t	i;

	get_path_to_cmd(info, env);
	i = -1;
	while (++i < info->size)
	{
		if (pipe(info->cmd[i].pipe) == -1)
			exit_error("Pipe");
		info->cmd[i].pid = fork();
		if (info->cmd[i].pid == -1)
			exit_error("Fork");
		else if (info->cmd[i].pid == 0)
		{
			pipex_handler(info, i);
			if (info->cmd[i].path == NULL)
				exit_error_arg(info->cmd[i].arg[0], ": command not found");
			execve(info->cmd[i].path, info->cmd[i].arg, env);
			exit_error("execve");
		}
		else
			close_fd(info, i);
	}
	while (i--)
		wait(NULL);
}
