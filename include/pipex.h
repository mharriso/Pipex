#ifndef PIPEX_H
# define PIPEX_H

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define PROMT "-> pipex: "

char	**split_path(char const *s, char c);


#endif
