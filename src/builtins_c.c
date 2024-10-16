/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_c.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/17 12:37:48 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/11/17 12:37:49 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"
#include "ymap.h"
#include "yprint.h"
#include "ystring.h"
#include "ytypes.h"
#include <stdio.h>
#include <unistd.h>

int	export_builtin(t_list *args, void *context)
{
	t_list		*pair;
	t_string	*key;
	t_list_iter	it;

	if (args->size < 1)
		return (env_builtin(args, context));
	it = list_iter(args);
	while (list_iter_next(&it))
	{
		pair = string_split(it.value, '=');
		key = list_get(pair, 0);
		if (!key || !string_is_valid_var_name(key))
			return (write_cstr_fd(STDERR_FILENO,
					"export: not a valid identifier\n"), 1);
		context = string_new();
		if (pair->size > 1)
			string_copy_from_string(context, list_get(pair, 1));
		if (map_has(global().environment, key))
			(string_delete(map_get(global().environment, key)),
				map_set_value(global().environment, key, context));
		else
			map_set(global().environment, string_copy(key), context);
		(list_apply(pair, (t_apply_lfn)string_delete), list_delete(pair));
	}
	return (0);
}

int	unset_builtin(t_list *args, void *context)
{
	t_kv_pair	pair;
	t_list_iter	it;

	(void)context;
	if (args->size < 1)
		return (0);
	it = list_iter(args);
	while (list_iter_next(&it))
	{
		pair = map_get_pair(global().environment, it.value);
		if (pair.key)
		{
			map_unset(global().environment, pair.key);
			string_delete(pair.key);
			if (pair.value)
				string_delete(pair.value);
		}
	}
	return (0);
}

static const t_string	g_empty = (t_string){"''", 2};
static const t_string	g_qmark = (t_string){"?", 1};

// if (string_find_char(value, ' ') > 0)
// 	yprintf("%s='%s'\n", key->cstr, value->cstr);
// else
int	env_builtin(t_list *args, void *context)
{
	t_string	*key;
	t_string	*value;
	t_map_iter	it;
	t_kv_pair	question_mark;

	(void)args;
	(void)context;
	question_mark = map_get_pair(global().environment, (void *)&g_qmark);
	map_unset(global().environment, (void *)&g_qmark);
	it = map_iter(global().environment);
	while (map_iter_next(&it))
	{
		key = it.key;
		value = it.value;
		if (!value)
			value = (t_string *)&g_empty;
		yprintf("%s=%s\n", key->cstr, value->cstr);
	}
	if (question_mark.key)
		map_set(global().environment, question_mark.key, question_mark.value);
	return (0);
}

// int	argprint_builtin(t_list *args, void *context)
// {
// 	(void)context;
// 	t_list_iter it = list_iter(args);
// 	while (list_iter_next(&it))
// 	{
// 		t_string *arg = it.value;
// 		yprintf("arg[%d]: %s\n", it.i - 1, arg->cstr);
// 	}
// 	return (0);
// }
