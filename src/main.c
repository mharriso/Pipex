#include "pipex.h"

static t_cmd	*init_cmd(char **argv, size_t size)
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

static void	save_args(t_info *info, int argc, char **argv)
{
	if (argc > 1 && !ft_strcmp(argv[1], "here_doc"))
	{
		if (argc != 6)
			exit_error("Invalid arguments: here_doc LIMITER cmd cmd1 file");
		info->limiter = argv[2];
		info->size = 2;
		info->cmd = malloc(2 * sizeof(t_cmd));
		info->cmd[0].arg = ft_split(argv[3], ' ');
		info->cmd[0].type = R_DLEFT;
		info->cmd[1].arg = ft_split(argv[4], ' ');
		info->cmd[1].type = R_DRIGHT;
		info->file_in = argv[1];
		info->file_out = argv[5];
	}
	else
	{
		if (argc < 5)
			exit_error("Invalid arguments");
		info->limiter = NULL;
		info->size = argc - 3;
		info->file_in = argv[1];
		info->file_out = argv[argc - 1];
		info->cmd = init_cmd(argv, info->size);
	}
}

static void	free_info(t_info *info)
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
	save_args(info, argc, (char **)argv);
	execute_commands(info, env);
	if (info->limiter)
		unlink("here_doc");
	free_info(info);
	return (0);
}
