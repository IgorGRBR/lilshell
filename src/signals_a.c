/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_a.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/17 11:31:55 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/11/17 11:31:57 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/wait.h>

#ifdef LINUX

static void	sigpipe_handler(int c)
{
	(void)c;

	yprintf("ALARM!!!!! SIGPIPE HAPPENED!!!!!!!\n");
}


void	setup_signal_handlers(void)
{
	struct sigaction	sgaction;

	sgaction.sa_handler = sigint_handler;
	sgaction.sa_flags = SA_NODEFER;
	sgaction.sa_mask = (__sigset_t){{SIGINT}};
	sgaction.sa_restorer = YNULL;
	sigaction(SIGINT, &sgaction, YNULL);
	sgaction.sa_handler = exit_handler;
	sigaction(SIGTERM, &sgaction, YNULL);
	sgaction.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sgaction, YNULL);
	sgaction.sa_handler = sigpipe_handler;
	sigaction(SIGPIPE, &sgaction, YNULL);
}

#endif

#ifdef OSX

void	setup_signal_handlers(void)
{
	struct sigaction	sgaction;

	sgaction.sa_handler = sigint_handler;
	sgaction.sa_flags = SA_NODEFER;
	sgaction.sa_mask = (sigset_t){SIGINT};
	sigaction(SIGINT, &sgaction, YNULL);
	sgaction.sa_handler = exit_handler;
	sigaction(SIGTERM, &sgaction, YNULL);
	sgaction.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sgaction, YNULL);
}

#endif

void	exit_handler(int c)
{
	int	wstatus;

	c = waitpid(-1, &wstatus, WNOHANG);
	while (c > 0)
	{
		kill(c, SIGTERM);
		c = waitpid(-1, &wstatus, WNOHANG);
	}
	global_exit();
}

#ifdef LINUX

void	sigint_handler(int c)
{
	int	wstatus;
	int	k;

	k = waitpid(-1, &wstatus, WNOHANG);
	while (k > 0)
	{
		kill(k, c);
		k = waitpid(-1, &wstatus, WNOHANG);
	}
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

#endif

#ifdef OSX

void	sigint_handler(int c)
{
	(void)c;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_redisplay();
}

#endif
