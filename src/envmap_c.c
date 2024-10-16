/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envmap_c.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 14:55:22 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/11/28 14:55:23 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ycstr.h"
#include "ylib.h"
#include "ymap.h"
#include "ytypes.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

char	**make_envp(t_map *envmap)
{
	char		**envp;
	t_map_iter	it;
	t_uint		i;

	envp = malloc(sizeof (char *) * (envmap->size + 1));
	it = map_iter(envmap);
	i = 0;
	while (map_iter_next(&it))
	{
		envp[i] = cstr_format("%s=%s", ((t_string *)it.key)->cstr,
				((t_string *)it.value)->cstr);
		i++;
	}
	envp[i] = YNULL;
	return (envp);
}

void	f_env(char **envp)
{
	t_uint		i;

	i = 0;
	while (envp[i])
	{
		free(envp[i]);
		i++;
	}
	free(envp);
}

void	update_status_code(t_itptr_ctx *ctx, t_map *envmap)
{
	t_string				*str;
	static const t_string	g_key = {"?", 1};
	int						wstatus;

	if (ctx->prev_result.type == RESULT_PROC_ID)
	{
		while (waitpid(ctx->prev_result.data, &wstatus, WNOHANG)
				!= ctx->prev_result.data)
			;
		if (WIFEXITED(wstatus))
			ctx->status_code = WEXITSTATUS(wstatus);
		else if (WIFSIGNALED(wstatus))
			(void)(WIFSIGNALED(wstatus) && (WTERMSIG(wstatus) == SIGTERM
				|| WTERMSIG(wstatus) == SIGINT));
	}
	else if (ctx->prev_result.type == RESULT_EXIT_CODE)
		ctx->status_code = ctx->prev_result.data;
	ctx->prev_result.type = RESULT_EXIT_CODE;
	ctx->prev_result.data = ctx->status_code;
	str = map_get(envmap, (void *)&g_key);
	if (str)
		(string_delete(str), map_set_value(envmap, (void *)&g_key,
			string_format_cstr("%d", ctx->status_code)));
	else
		map_set(envmap, string_copy(&g_key), string_format_cstr("%d",
				ctx->status_code));
}

t_map	*envmap_new(void)
{
	return (map_new((t_hash_func)string_hash, (t_equals_func) string_equal));
}
