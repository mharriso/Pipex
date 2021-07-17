#include "pipex.h"
#include <errno.h>

void	ft_free(char **str)
{
	if (!*str)
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
