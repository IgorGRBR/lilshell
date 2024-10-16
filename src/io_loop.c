/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io_loop.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 14:30:01 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/07/25 14:30:02 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>

static void	wait_for_all(void)
{
	int	wstatus;

	while (wait(&wstatus) > 0)
		;
}

static void	evaluate_command(t_string *cmd_str)
{
	t_list			*tokens;
	t_list_iter		it;
	t_list			*ast;
	t_itptr_ctx		dummy;

	tokens = tokenize(cmd_str);
	if (!tokens && (yprintf("Tokenization error\n"), TRUE))
		return ;
	it = list_iter(tokens);
	ast = build_exp_list(&it, FALSE);
	global().itptr_ast->tokens = tokens;
	global().itptr_ast->ast = ast;
	if (!ast && (yprintf("Parsing error\n"), TRUE))
		return ;
	reset_signal_handlers();
	dummy.prev_result = interpret_ast(ast,
			(t_io_pipes){STDIN_FILENO, STDOUT_FILENO}, FALSE, 0);
	update_status_code(&dummy, global().environment);
	wait_for_all();
	setup_signal_handlers();
	(list_apply(ast, (t_apply_lfn)expression_delete), list_delete(ast));
	(list_apply(tokens, (t_apply_lfn)string_delete), list_delete(tokens));
	global().itptr_ast->tokens = YNULL;
	global().itptr_ast->ast = YNULL;
}

t_bool	io_loop(t_string *cmd_str)
{
	char		*cstr;
	t_string	*temp_str;
	t_bool		should_continue;

	should_continue = !cmd_str;
	if (!cmd_str)
	{
		cstr = readline(SHELL_LABEL);
		if (!cstr)
			return (FALSE);
		cmd_str = string_from_cstr(cstr);
		free(cstr);
	}
	temp_str = string_trim_cstr(cmd_str, " \t");
	(string_delete(cmd_str), cmd_str = temp_str);
	add_history(cmd_str->cstr);
	evaluate_command(cmd_str);
	string_delete(cmd_str);
	should_continue = should_continue && !global().exit;
	return (should_continue);
}
