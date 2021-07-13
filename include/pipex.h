#ifndef PIPEX_H
# define PIPEX_H

# include <errno.h>
# include <stdlib.h>
# include <unistd.h>

# define RESET   "\033[0m"  //все атрибуты по умолчанию
# define BLACK   "\033[30m"  //30-37 цвет текста
# define RED     "\033[31m"
# define GREEN   "\033[32m"
# define YELLOW  "\033[33m"
# define BLUE    "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN    "\033[36m"
# define WHITE   "\033[37m"

# define PROMT "\033[33m->\033[0m pipex: "

char	**split_path(char const *s, char c);

#endif
