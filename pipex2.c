#include "minishell.h"

int	dup_fd(int new_stdin, int new_stdout)
{
	if (dup2(new_stdin, STDIN_FILENO) == -1
		|| dup2(new_stdout, STDOUT_FILENO) == -1)
		return (0);
	return (1);
}

void	create_pipe(int *pipes, int i)
{
	int	pipe_fd[2];

	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		exit (1);
	}
	pipes[i] = pipe_fd[0];
	pipes[i + 1] = pipe_fd[1];
}

int	*create_pipes(int len)
{
	int	*pipes;
	int	i;

	i = 0;
	pipes = malloc(sizeof(int *) * (len * 2));
	while (i < len * 2)
	{
		create_pipe(pipes, i);
		if (!pipes[i] || !pipes[i + 1])
			return (NULL);
		i = i + 2;
	}
	i = 0;
	return (pipes);
}

int find_stdin(t_maillons *maillons)
{
	int	fd;

    if (find_if_have_output(maillons, '<'))
    {
		fd = open(find_name_sep(maillons, '<'),  O_RDWR, O_DSYNC, !O_DIRECTORY);
		return (fd);
	}
	else if (!(maillons-> prev))
	{
		printf("stdin trouver\n");
		return (STDIN_FILENO);
	}
	else if (find_if_have_output(maillons -> prev, '>') || !(maillons->prev->command))
	{
		fd = open("/dev/null",  O_RDWR, O_DSYNC, !O_DIRECTORY);
		return (fd);
	}
	else
	{
		// return pipes[0] du maillons d'avant
	}
	return (-1);
}

int find_stdout(t_maillons *maillons)
{
	int	fd;

    if (find_if_have_output(maillons, '>'))
    {
		fd = open(find_name_sep(maillons, '>'),  O_RDWR, O_DSYNC, !O_DIRECTORY);
		return (fd);
	}
	else if (!(maillons-> next))
	{
		printf("stdout trouver\n");
		return (STDOUT_FILENO);
	}
	else
	{
		// return pipes[1] du maillons d'avant
	}
	return (-1);
}


int pipex_one(t_maillons  *maillons, char **env)
{
    pid_t	pid;
	char *a[2];
	int fd_in;
	int	fd_out;

	fd_in = -1; 
	a[0] = ft_strdup("cat");
	a[1] = 0;
    pid = fork();
    if (pid == -1)
			return (perror("fork"), 1);
	if (pid == 0)
	{
			
		dup_fd(find_stdin(maillons, &fd_in), find_stdout(maillons, &fd_out));
		if (execve("/usr/bin/cat", a , env) == -1)
		{
			perror("execve");
			exit (1);
		}
		return (1);
	}
	waitpid(-1 , NULL, 0);
	free(a[0]);
	return(0);
}



int pipex(t_maillons *maillons, char **env)
{
    int *pipes;
    int len;

    len = ft_strlen_maillons(maillons); // nombre de maillons
    if (len == 1)
	{
        pipex_one(maillons, env);
	}
    //pipes = create_pipes(len); // crer (len -1) pipe
    return (0);


}