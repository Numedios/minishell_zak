#include "minishell.h"

int	switch_dup2_fd_in(t_maillons *maillons, t_pipes *pipes, int i, int len)
{
	int	res;

	res = -2;
	if (find_if_have_output(maillons -> output, "<") == 1)
	{
		//dprintf(2, "Lecture %d est %s \n", i, find_name_sep(maillons -> output, "<"));
		res = open(find_name_sep(maillons -> output, "<"),  O_RDWR, O_DSYNC, !O_DIRECTORY);
		dup2(res, STDIN_FILENO);
	}
	else if (find_if_have_output(maillons -> output, "<<") == 1)
    {
		//dprintf(2, "Lecture %d  est heredoc %s\n", i ,find_name_sep(maillons -> output, "<<"));
		if (maillons -> heredoc != -1)
			dup2(maillons -> heredoc, STDIN_FILENO);
	}
	else if (!(maillons-> prev))
	{
		//dprintf(2, "Lecture %d  est  dev/stdin\n", i);
		//res = open("/dev/stdin",  O_RDWR, O_DSYNC, !O_DIRECTORY);
		//dup2(res, STDIN_FILENO);
		return (1);
	}
	else if (find_if_have_output(maillons -> prev -> output , ">") || !(maillons->prev->command))
	{
		//dprintf(2, "Lecture %d  est  dev/null\n", i);
		res = open("/dev/null",  O_RDWR, O_DSYNC, !O_DIRECTORY);
		dup2(res, STDIN_FILENO);
	}
	else
	{
		//dprintf(2, "Lecture %d  est  pipe[%d]\n", i,(i*2 -2));
		dup2(pipes->pipe[i * 2 - 2], STDIN_FILENO);
	}
	if (res != -2)
	{
		//dprintf(2, "free entrer / i = %d\n", i);
		close(res);
	}
	return (1);
}

int	switch_dup2_fd_out(t_maillons *maillons, t_pipes *pipes, int i, int len)
{
	int res;

	res = -2;
	if (find_if_have_output(maillons -> output, ">"))
    {
		//dprintf(2, "Ecriture %d est  %s \n", i, find_name_sep(maillons -> output, ">"));
		res = open(find_name_sep(maillons -> output, ">"), O_WRONLY | O_CREAT | O_TRUNC, 0644, !O_DIRECTORY);
		dup2(res, STDOUT_FILENO);
	}
	else if (find_if_have_output(maillons -> output, ">>"))
	{
		//dprintf(2, "Ecriture %d est %s \n", i, find_name_sep(maillons -> output, ">>"));
		res = open(find_name_sep(maillons -> output, ">>"),  O_WRONLY | O_CREAT | O_APPEND, 0644, !O_DIRECTORY);
		dup2(res, STDOUT_FILENO);
	}
	else if (!(maillons-> next))
	{
		//dprintf(2, "Ecriture %d est dev/stdout \n" , i);
		//dprintf(2, "Commande %s : \n", maillons->command);
		//res = open("/dev/stdout ",  O_RDWR, O_DSYNC, !O_DIRECTORY);
		//dprintf(2, "res = %d/ i = %d\n", res , i);
		//dup2(res , STDOUT_FILENO);
		return (1);
	}
	else
	{
		//dprintf(2, "Ecriture %d est pipes[%d] \n", i, (i*2 +1));
		dup2(pipes->pipe[i * 2 + 1], STDOUT_FILENO);
	}
	if (res != -2)
	{
		//dprintf(2, "free sortie / i = %d\n", i);
		close(res);
	}
	return (1);
}


int	pipex_multiple(t_maillons	*maillons, char **env, int len)
{
	t_pipes pipes;
	pid_t pid;
	int i;

	pipes = create_all_pipes(len - 1);
	//dprintf(2, "pipe[0] = %d  pipe[1] = %d\n", pipes.pipe[0], pipes.pipe[1]);
	//dprintf(2, "pipe[2] = %d  pipe[3] = %d\n", pipes.pipe[2], pipes.pipe[3]);
	i = 0;
	while (maillons)
	{
		pid = fork();
		if (pid == -1)
			return (perror("fork"), 1);
		if (pid == 0)
		{
			switch_dup2_fd_in(maillons, &pipes, i, len);
			switch_dup2_fd_out(maillons, &pipes, i, len);
			free_all_pipes((len-1) * 2, pipes);
			if (maillons->heredoc !=-1)
				close(maillons->heredoc);
			if (execve(maillons ->command, maillons -> args , env) == -1)
			{
				perror("execve");
				exit (1);
			}
		}
		if (i > 0 && pipes.pipe[i * 2 - 2])
		{
			//dprintf(2, "tour %d close pipe[%d]\n", i, i * 2 - 2);
			close(pipes.pipe[i * 2 - 2]);
		}
		if (i != (len -1) && pipes.pipe[i * 2 + 1])
		{
			//dprintf(2, "tour %d close pipe[%d]\n", i, i * 2 +1);
			close(pipes.pipe[i * 2 + 1]);
		}
		i++;
		maillons = maillons -> next;
	}
	i = 0;
	while (i < len)
	{
		waitpid(-1, NULL, 0);
		i++;
	}
	free(pipes.pipe);
	return (1);
}