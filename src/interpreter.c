/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpreter.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 15:08:58 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/09/12 15:08:59 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ycstr.h"
#include "ylib.h"
#include "ytypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "errno.h"

static t_bool	setup_redirect(t_string *path_str, int fd,
		int (*file_func)(t_string *), int *exit_code)
{
	int	ffd;

	if (!path_str || !file_func)
	{
		*exit_code = 1;
		return (FALSE);
	}
	ffd = file_func(path_str);
	if (ffd < 0 && *exit_code == 0)
	{
		perror("Couldn't open file for redirection");
		*exit_code = 1;
		return (FALSE);
	}
	if (dup2(ffd, fd) < 0 && *exit_code == 0)
	{
		*exit_code = 1;
		perror("Something's not good");
		return (FALSE);
	}
	return (TRUE);
}

static t_bool	setup_redirects(t_expression *exp, int *ec,
		t_redirection *rd, t_bool r)
{
	t_list_iter		it;
	int				pie[2];

	if (exp->redirections->size == 0)
		return (TRUE);
	it = list_iter(exp->redirections);
	while (list_iter_next(&it) && r)
	{
		rd = it.value;
		if (rd->type == REDIRECT_STDIN)
			r = setup_redirect(rd->path, STDIN_FILENO, open_file_read, ec);
		else if (rd->type == REDIRECT_STDOUT)
			r = setup_redirect(rd->path, STDOUT_FILENO, open_file_write, ec);
		else if (rd->type == REDIRECT_APPEND)
			r = setup_redirect(rd->path, STDOUT_FILENO, open_file_append, ec);
		else if (rd->type == REDIRECT_HEREDOC)
		{
			pipe(pie);
			handle_heredoc(rd->path, pie[1]);
			if (dup2(pie[0], STDIN_FILENO) < 0 && *ec == 0)
				(perror("Something's not good"), *ec = 1, r = FALSE);
			close(pie[1]);
		}
	}
	return (*ec == 0);
}

static t_cmd_result	invoke_command(t_pcall program, t_io_pipes pipes,
		t_expression *exp)
{
	int		fork_result;
	t_list	*arg_list;
	int		wstatus;

	(expand_program_envvars(&program), wstatus = 0);
	if (!program.pname || program.pname->size == 0)
		return (cleanup_program_envvars(&program),
			(t_cmd_result){RESULT_EXIT_CODE, 0});
	if (exp->op != OP_PIPE && try_run_sync_builtin(&program, &wstatus))
		return ((t_cmd_result){RESULT_EXIT_CODE, wstatus});
	fork_result = fork();
	if (fork_result == 0)
	{
		(set_pipes(pipes.in, pipes.out));
		(void)((!setup_redirects(exp, &wstatus, YNULL, TRUE)
			|| try_run_builtin(&program, &wstatus)) && (exit(wstatus), 1));
		arg_list = list_copy(program.params);
		list_insert_at(arg_list, 0, program.pname);
		wstatus = run_process(arg_list, get_paths(global().environment));
		(void)((wstatus && errno == EINVAL) && (exit(126), 1));
		exit(127);
	}
	cleanup_program_envvars(&program);
	return ((t_cmd_result){RESULT_PROC_ID, fork_result});
}

// The environment variable expansion should happen in this function
static void	eval_exp(t_expression *exp, t_itptr_ctx *ctx)
{
	int		pipes[2];

	if (ctx->prev_operator == OP_OR || ctx->prev_operator == OP_AND)
	{
		update_status_code(ctx, global().environment);
		if ((ctx->prev_operator == OP_OR && ctx->status_code == 0)
			|| (ctx->prev_operator == OP_AND && ctx->status_code != 0))
			return ;
	}
	if (exp->op == OP_PIPE)
		(pipe(pipes), set_write_pipe(ctx, pipes[1]));
	else
		set_write_pipe(ctx, ctx->og_pipes.out);
	if (exp->type == PROGRAM_CALL)
		ctx->prev_result = invoke_command(exp->pcall, ctx->pipes, exp);
	else if (exp->type == NESTED_EXPRESSION)
		ctx->prev_result = interpret_ast(exp->nested, ctx->pipes, TRUE, 0);
	if (exp->op == OP_PIPE)
		set_read_pipe(ctx, pipes[0]);
	(void)((exp->op != OP_PIPE) && (reset_pipes(ctx),
		ctx->prev_operator = exp->op));
}

t_cmd_result	interpret_ast(t_list *ast, t_io_pipes pipes, t_bool rq_fork,
		int wstatus)
{
	t_itptr_ctx	itptr_ctx;
	int			fork_result;

	itptr_ctx.prev_operator = OP_NONE;
	itptr_ctx.status_code = 0;
	itptr_ctx.og_pipes = pipes;
	itptr_ctx.pipes = pipes;
	itptr_ctx.prev_result = (t_cmd_result){RESULT_EXIT_CODE, 0};
	(void)(rq_fork && (fork_result = fork()));
	if (!rq_fork || fork_result == 0)
		list_capply(ast, (t_capply_lfn)eval_exp, &itptr_ctx);
	if (rq_fork)
	{
		if (fork_result == 0)
		{
			if (itptr_ctx.prev_result.type == RESULT_EXIT_CODE)
				exit(itptr_ctx.prev_result.data);
			waitpid(itptr_ctx.prev_result.data, &wstatus, 0);
			exit(WEXITSTATUS(wstatus));
		}
		waitpid(fork_result, &wstatus, 0);
		itptr_ctx.prev_result.type = RESULT_EXIT_CODE;
		itptr_ctx.prev_result.data = WEXITSTATUS(wstatus);
	}
	return (itptr_ctx.prev_result);
}
