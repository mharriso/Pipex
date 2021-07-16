#include "pipex.h"
#include <fcntl.h>
#include "libft.h"
#include <sys/types.h>
#include <sys/wait.h>

t_cmd	*init_cmd(char **argv, size_t size)
{
	t_cmd		*cmd;
	size_t		i;

	cmd = malloc(size * sizeof(t_cmd));
	if (!cmd)
		exit_error("Can not allocate cmd");
	i = 0;
	while (i < size)
	{
		cmd[i].arg = ft_split(argv[i + 2], ' ');
		if (!cmd[i].arg)
			exit_error("Can not allocate cmd arguments");
		if (i == 0)
			cmd[i].type = R_LEFT;
		else if (i == size - 1)
			cmd[i].type = R_RIGHT;
		else
			cmd[i].type = PIPE;
		i++;
	}
	return (cmd);
}

void	save_args(t_info *info, int argc, char **argv)
{
	if (argc != 5)
		exit_error("Invalid arguments: file1 cmd1 cmd2 file2");
	info->limiter = NULL;
	info->size = 2;
	info->file_in = argv[1];
	info->file_out = argv[4];
	info->cmd = malloc(2 * sizeof(t_cmd));
	info->cmd[0].arg = ft_split(argv[2], ' ');
	info->cmd[0].type = R_LEFT;
	info->cmd[1].arg = ft_split(argv[3], ' ');
	info->cmd[1].type = R_RIGHT;
}

void	save_args_bonus(t_info *info, int argc, char **argv)
{
	if(argc > 1 && !ft_strcmp(argv[1], "here_doc"))
	{
		if(argc != 6)
			exit_error("Invalid arguments: here_doc LIMITER cmd cmd1 file");
		info->limiter = argv[2];
		info->size = 2;
		info->cmd = malloc(2 * sizeof(t_cmd));
		info->cmd[0].arg = ft_split(argv[3], ' ');
		info->cmd[0].type = R_LEFT;
		info->cmd[1].arg = ft_split(argv[4], ' ');
		info->cmd[1].type = R_DRIGHT;
		info->file_in = argv[1];
		info->file_out = argv[5];
	}
	else
	{
		if (argc < 5)
			exit_error("Invalid arguments: file1 cmd1 cmd2 ... cmdn file2");
		info->limiter = NULL;
		info->size = argc - 3;
		info->file_in = argv[1];
		info->file_out = argv[argc - 1];
		info->cmd = init_cmd(argv, info->size);
	}
}

char	*get_env_value(char *name, char **env)
{
	int		i;
	char	*eq;
	char	*value;

	i = 0;
	while (env[i])
	{
		if (!ft_strncmp(env[i], name, ft_strlen(name)))
		{
			eq = ft_strchr(env[i], '=');
			if (eq)
			{
				value = ft_strdup(eq + 1);
				if (!value)
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

void	get_path_to_cmd(t_info *info, char **env)
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

int	open_file(char *fname, int red_type)
{
	int		fd;
	int		flags;

	flags = 0;
	if (red_type == R_LEFT)
		flags = O_RDONLY;
	else if (red_type == R_RIGHT)
		flags = O_CREAT | O_WRONLY | O_TRUNC;
	else if (red_type == R_DRIGHT)
		flags = O_CREAT | O_WRONLY | O_APPEND;
	fd = open(fname, flags, S_IREAD | S_IWRITE);
	return (fd);
}

int	here_doc(t_info *info)
{
	int		doc;
	int		ret;
	char	*line;

	doc = open(info->file_in, O_CREAT | O_RDWR | O_TRUNC, S_IREAD | S_IRWXU | S_IRWXG | S_IRWXO);
	if (doc == -1)
		exit_error(info->file_in);
	write(1, "> ", 2);
	ret = get_next_line(1, &line);
	while (ret == 1)
	{
		if(!ft_strcmp(line, info->limiter))
			break ;
		write(doc, line, ft_strlen(line));
		write(doc, "\n", 1);
		write(1, "> ", 2);
		ret = get_next_line(1, &line);
	}
	close(doc);
	doc = open(info->file_in, O_RDONLY);
	return (doc);
}

void	execute_commands(t_info *info, char **env)
{
	size_t	i;
	//int		status;
	int		fd;

	get_path_to_cmd(info, env);
	i = -1;
	while (++i < info->size)
	{
		if (info->cmd[i].path == NULL)
		{
			print_error(info->cmd[i].arg[0], ": command not found");
			continue ;
		}
		if (pipe(info->cmd[i].pipe) == -1)
			exit_error("Pipe");
		info->cmd[i].pid = fork();
		if (info->cmd[i].pid == -1)
			exit_error("Fork");
		else if (info->cmd[i].pid == 0)
		{
			if (info->cmd[i].type == R_LEFT)
			{
				if(info->limiter)
					fd = here_doc(info);
				else
					fd = open_file(info->file_in, R_LEFT);
				if (fd == -1)
					exit_error(info->file_in);
				if (dup2(fd, STDIN) == -1)
					exit_error("dup2");
				if (dup2(info->cmd[i].pipe[SIDE_OUT], STDOUT) == -1)
					exit_error("dup2");
				close(info->cmd[i].pipe[SIDE_OUT]);
				close(info->cmd[i].pipe[SIDE_IN]);
			}
			else if (info->cmd[i].type == R_RIGHT || info->cmd[i].type == R_DRIGHT)
			{
				fd = open_file(info->file_out, info->cmd[i].type);
				if (fd == -1)
					exit_error(info->file_out);
				if (dup2(fd, STDOUT) == -1)
					exit_error("dup2");
				if (dup2(info->cmd[i - 1].pipe[SIDE_IN], STDIN) == -1)
					exit_error("dup2");
				close(info->cmd[i].pipe[SIDE_OUT]);
				close(info->cmd[i].pipe[SIDE_IN]);
			}
			else if (info->cmd[i].type == PIPE)
			{
				if (dup2(info->cmd[i].pipe[SIDE_OUT], STDOUT) == -1)
					exit_error("dup2");
				if (dup2(info->cmd[i - 1].pipe[SIDE_IN], STDIN) == -1)
					exit_error("dup2");
				close(info->cmd[i].pipe[SIDE_OUT]);
				close(info->cmd[i].pipe[SIDE_IN]);
			}
			execve(info->cmd[i].path, info->cmd[i].arg, env);
			exit_error("execve");
		}
		else
		{

			close(info->cmd[i].pipe[SIDE_OUT]);
			if(info->cmd[i].type != R_LEFT)
				close(info->cmd[i - 1].pipe[SIDE_IN]);
			if (info->cmd[i].type == R_RIGHT)
				close(info->cmd[i].pipe[SIDE_IN]);
		}

		for(size_t k = 0; k < info->size; k++)
		{
			close(info->cmd[i].pipe[SIDE_IN]);
			close(info->cmd[i].pipe[SIDE_OUT]);
		}

		//waitpid(info->cmd[info->size - 1].pid, NULL, 0);
		for(size_t k = 0; k < info->size; k++)
			wait(NULL);
	}
}

void	free_info(t_info *info)
{
	size_t	i;

	i = 0;
	while (i < info->size)
	{
		free_array(info->cmd[i].arg);
		ft_free(&info->cmd[i].path);
		i++;
	}
	free(info->cmd);
	free(info);
}

int	main(int argc, char const **argv, char **env)
{
	t_info	*info;

	info = malloc(sizeof(t_info));
	if (!info)
		exit_error("Can not allocate info");
	save_args_bonus(info, argc, (char **)argv);
	//printf("file out %s\n", info->file_out);
	execute_commands(info, env);
		if(info->limiter)
		unlink(info->file_in);
	free_info(info);
	//sleep(10);

	return (0);
}
