/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   global.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/17 11:32:02 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/11/17 11:32:03 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ydefines.h"
#include "ylib.h"
#include "ytypes.h"
#include <stdlib.h>

static t_global	_init_global(char *envp[], t_bool exit)
{
	static t_global	lmfao = {
		._initialized = FALSE,
	};

	if (!lmfao._initialized && envp != YNULL)
	{
		lmfao.history = list_new();
		lmfao._initialized = TRUE;
		lmfao.itptr_ast = malloc(sizeof (t_ast_refs));
		lmfao.builtin_programs = map_new((t_hash_func)string_hash,
				(t_equals_func)string_equal);
		lmfao.builtin_sync_programs = map_new((t_hash_func)string_hash,
				(t_equals_func)string_equal);
		lmfao.environment = envmap_from_env(envp);
		map_insert(lmfao.environment, string_from_cstr("?"),
			string_from_cstr("0"));
	}
	if (exit)
		lmfao.exit = TRUE;
	return (lmfao);
}

void	init_global(char **envp)
{
	(void)_init_global(envp, FALSE);
}

t_global	global(void)
{
	return (_init_global(YNULL, FALSE));
}

void	clear_global(void)
{
	t_map_iter	it;

	it = map_iter(global().builtin_programs);
	while (map_iter_next(&it))
		string_delete(it.key);
	it = map_iter(global().builtin_sync_programs);
	while (map_iter_next(&it))
		string_delete(it.key);
	map_delete(global().builtin_programs);
	map_delete(global().builtin_sync_programs);
	list_apply(global().history, (t_apply_lfn)string_delete);
	list_delete(global().history);
	envmap_delete(global().environment);
	free(global().itptr_ast);
}

void	global_exit(void)
{
	(void)(_init_global(YNULL, TRUE));
}
