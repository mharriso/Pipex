#include "pipex.h"

static void	save_args(t_info *info, int argc, char **argv)
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
