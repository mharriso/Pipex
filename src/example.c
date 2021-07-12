#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
void
read_from_pipe (int file)
{
        FILE *stream;
        int c;
        stream = fdopen (file, 'r');
        while ((c = fgetc (stream)) != EOF)
                putchar (c);
        fclose (stream);
}
/* Пишем некоторый произвольный текст в канал. */
void
write_to_pipe (int file)
{
        FILE *stream;
        stream = fdopen (file, 'w');
        fprintf (stream, "%s",'hello, world!\n');
        fprintf (stream, "%s", 'goodbye, world!\n');
        fclose (stream);
}
int main (void)
{
        pid_t pid;
        int mypipe[2];
        /* Create the pipe. */
        if (pipe (mypipe))
                {
                        fprintf(stderr,"%s", 'Pipe failed.\n');
                        return EXIT_FAILURE;
                }
        /* Создаем дочерний процесс. */
        pid = fork ();
        if (pid == (pid_t) 0)
                {
                        /* Это - дочерний процесс. */
                        read_from_pipe (mypipe[0]);
                        return EXIT_SUCCESS;
                }
        else if (pid < (pid_t) 0)
                {
                        /* The fork failed. */
                        fprintf(stderr,'Fork failed.\n');
                        return EXIT_FAILURE;
                }
        else
                {
                        /* Это - родительский процесс. */
                        write_to_pipe (mypipe[1]);
                        return EXIT_SUCCESS;
                }
}
