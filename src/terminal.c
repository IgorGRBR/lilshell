/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/17 11:44:48 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/11/17 11:44:49 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"
#include "stdlib.h"
#include "termios.h"
#include "ylist.h"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

void	setup_terminal(char *envp[])
{
	struct termios	t;

	init_global(envp);
	setup_signal_handlers();
	tcgetattr(0, &t);
	t.c_lflag &= ~ECHOCTL;
	tcsetattr(0, TCSANOW, &t);
}

#ifdef LINUX

void	cleanup_terminal(void)
{
	if (global().itptr_ast->tokens)
	{
		list_apply(global().itptr_ast->tokens, (t_apply_lfn)string_delete);
		list_delete(global().itptr_ast->tokens);
	}
	if (global().itptr_ast->ast)
	{
		list_apply(global().itptr_ast->ast, (t_apply_lfn)expression_delete);
		list_delete(global().itptr_ast->ast);
	}
	clear_global();
	rl_clear_history();
}

#endif

#ifdef OSX

void	cleanup_terminal(void)
{
	if (global().itptr_ast->tokens)
	{
		list_apply(global().itptr_ast->tokens, (t_apply_lfn)string_delete);
		list_delete(global().itptr_ast->tokens);
	}
	if (global().itptr_ast->ast)
	{
		list_apply(global().itptr_ast->ast, (t_apply_lfn)expression_delete);
		list_delete(global().itptr_ast->ast);
	}
	clear_global();
	clear_history();
}

#endif
