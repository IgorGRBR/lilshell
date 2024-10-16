/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envmap_a.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 12:29:16 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/10/18 12:29:17 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ylib.h"
#include "minishell.h"
#include "ystring.h"

t_map	*envmap_from_env(char **env)
{
	t_map		*map;
	t_uint		i;
	t_string	*str;
	int			eq;

	map = envmap_new();
	i = 0;
	while (env[i])
	{
		str = string_from_cstr(env[i]);
		eq = string_find_char(str, '=');
		if (eq > 0)
			map_insert(map, string_substring(str, 0, eq),
				string_substring(str, eq + 1, str->size));
		string_delete(str);
		i++;
	}
	return (map);
}

static void	delete_strings(t_string *key, t_string *value)
{
	string_delete(key);
	string_delete(value);
}

void	envmap_delete(t_map *envmap)
{
	map_apply(envmap, (t_apply_mfn)delete_strings);
	map_delete(envmap);
}

static const t_string	g_key = {"?", 1};

t_string	*expand_get_string(t_string *str, t_map *envmap)
{
	t_string	*postfix;
	t_string	*key;
	t_uint		i;
	t_string	*value;

	if (str->cstr[0] == '?')
	{
		postfix = string_substring(str, 1, str->size);
		value = string_concat(map_get(envmap, (void *)&g_key), postfix);
		return (string_delete(postfix), value);
	}
	else if (!is_alpha(str->cstr[0]))
		return (YNULL);
	i = 1;
	while (is_alnum(str->cstr[i]) || str->cstr[i] == '_')
		i++;
	key = string_substring(str, 0, i);
	value = map_get(envmap, key);
	(string_delete(key), postfix = string_substring(str, i, str->size));
	if (!value && postfix->size == 0)
		return (string_delete(postfix), nil());
	else if (!value)
		return (postfix);
	value = string_concat(value, postfix);
	return (string_delete(postfix), value);
}

t_string	*envmap_expand(t_string *str, t_map *envmap)
{
	t_string	*result;
	t_list		*vars;
	t_list		*expanded;

	vars = string_split(str, ENVVAR_MASK);
	result = YNULL;
	if (str->size == 0)
		return (str);
	if (str->cstr[0] != ENVVAR_MASK)
	{
		result = list_get(vars, 0);
		list_remove_at(vars, 0);
	}
	string_delete(str);
	expanded = list_cmap(vars, (t_cmap_lfn)expand_get_string, envmap);
	(list_apply(vars, (t_apply_lfn)string_delete), list_delete(vars));
	list_filter(expanded, null_filter);
	if (result)
		list_insert_at(expanded, 0, result);
	if (expanded->size == 0)
		return (list_delete(expanded), nil());
	result = string_join_cstr(expanded, "");
	list_apply(expanded, (t_apply_lfn)string_delete);
	list_delete(expanded);
	return (result);
}
