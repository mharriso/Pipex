#include "pipex.h"
#include <errno.h>


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
	ft_putstr_fd(PROMT, STDERR);
	if (errno)
		perror(s);
	else
		ft_putendl_fd(s, STDERR);
	exit(EXIT_FAILURE);
}

void	exit_error_arg(char *msg, char *arg)
{
	ft_putstr_fd(PROMT, STDERR);
	ft_putstr_fd(msg, STDERR);
	ft_putendl_fd(arg, STDERR);
	exit(EXIT_FAILURE);
}
