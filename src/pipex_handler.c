#include "pipex.h"
#include <fcntl.h>

static int	open_file(char *fname, int red_type)
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

static int	here_doc(t_info *info)
{
	int		doc;
	int		ret;
	char	*line;

	doc = open(info->file_in, O_CREAT | O_RDWR | O_TRUNC, S_IREAD | S_IRWXU);
	if (doc == -1)
		exit_error(info->file_in);
	write(1, "> ", 2);
	ret = get_next_line(1, &line);
	while (ret == 1)
	{
		if (!ft_strcmp(line, info->limiter))
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

static void	redirect_left(t_info *info, int i)
{
	int		fd;

	if (info->cmd[i].type == R_DLEFT)
		fd = here_doc(info);
	else
		fd = open_file(info->file_in, R_LEFT);
	if (fd == -1)
		exit_error(info->file_in);
	if (dup2(fd, STDIN) == -1)
		exit_error("dup2");
	if (dup2(info->cmd[i].pipe[SIDE_OUT], STDOUT) == -1)
		exit_error("dup2");
}

static void	redirect_right(t_info *info, int i)
{
	int		fd;

	fd = open_file(info->file_out, info->cmd[i].type);
	if (fd == -1)
		exit_error(info->file_out);
	if (dup2(fd, STDOUT) == -1)
		exit_error("dup2");
	if (dup2(info->cmd[i - 1].pipe[SIDE_IN], STDIN) == -1)
		exit_error("dup2");
}

void	pipex_handler(t_info *info, int i)
{
	int		type;

	type = info->cmd[i].type;
	if (type == R_LEFT || type == R_DLEFT)
		redirect_left(info, i);
	else if (type == R_RIGHT || type == R_DRIGHT)
		redirect_right(info, i);
	else if (type == PIPE)
	{
		if (dup2(info->cmd[i].pipe[SIDE_OUT], STDOUT) == -1)
			exit_error("dup2");
		if (dup2(info->cmd[i - 1].pipe[SIDE_IN], STDIN) == -1)
			exit_error("dup2");
	}
	close(info->cmd[i].pipe[SIDE_OUT]);
	close(info->cmd[i].pipe[SIDE_IN]);
}
