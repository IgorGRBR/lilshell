/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   piping.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/22 15:12:14 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/09/22 15:12:16 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ylib.h"
#include "minishell.h"
#include <stdio.h>
#include <unistd.h>

void	set_pipes(int rpipe, int wpipe)
{
	// yprintf("Setting RW pipes: %d, %d\n", rpipe, wpipe);
	if (dup2(rpipe, STDIN_FILENO) < 0)
		perror("RPipe error");
	if (dup2(wpipe, STDOUT_FILENO) < 0)
		perror("WPipe error");
}

void	set_read_pipe(t_itptr_ctx *ctx, int rpipe)
{
	if (ctx->pipes.in == rpipe)
		return ;
	if (ctx->pipes.in != ctx->og_pipes.in)
	{
		// yprintf("Closing read pipe %d\n", ctx->pipes.in);
		close(ctx->pipes.in);
	}
	ctx->pipes.in = rpipe;
}

void	set_write_pipe(t_itptr_ctx *ctx, int wpipe)
{
	if (ctx->pipes.out == wpipe)
		return ;
	if (ctx->pipes.out != ctx->og_pipes.out)
	{
		// yprintf("Closing write pipe %d\n", ctx->pipes.out);
		close(ctx->pipes.out);
	}
	ctx->pipes.out = wpipe;
}

void	reset_pipes(t_itptr_ctx *ctx)
{
	if (ctx->pipes.in != ctx->og_pipes.in)
	{
		// yprintf("Resetting read pipe %d\n", ctx->pipes.in);
		close(ctx->pipes.in);
	}
	if (ctx->pipes.out != ctx->og_pipes.out)
	{
		// yprintf("Resetting write pipe %d\n", ctx->pipes.in);
		close(ctx->pipes.out);
	}
	ctx->pipes = ctx->og_pipes;
}
