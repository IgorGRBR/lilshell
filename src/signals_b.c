/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_b.c                                        :+:      :+:    :+:   */
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

void	reset_signal_handlers(void)
{
	struct sigaction	sgaction;

	sgaction.sa_handler = rt_sigint_handler;
	sgaction.sa_flags = SA_NODEFER;
	sgaction.sa_mask = (__sigset_t){{SIGINT}};
	sgaction.sa_restorer = YNULL;
	sigaction(SIGINT, &sgaction, YNULL);
	sgaction.sa_handler = exit_handler;
	sigaction(SIGTERM, &sgaction, YNULL);
	sgaction.sa_handler = rt_sigint_handler;
	sigaction(SIGQUIT, &sgaction, YNULL);
}

#endif

#ifdef OSX

void	reset_signal_handlers(void)
{
	struct sigaction	sgaction;

	sgaction.sa_handler = SIG_DFL;
	sgaction.sa_flags = SA_NODEFER;
	sgaction.sa_mask = (sigset_t){SIGINT};
	sigaction(SIGINT, &sgaction, YNULL);
	sgaction.sa_handler = SIG_DFL;
	sigaction(SIGTERM, &sgaction, YNULL);
	sgaction.sa_handler = SIG_DFL;
	sigaction(SIGQUIT, &sgaction, YNULL);
}

#endif

void	rt_sigint_handler(int c)
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
}
