/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_a.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 16:52:18 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/10/11 16:52:19 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"

static t_bool	match_pattern(t_string *pattern, t_string *str)
{
	t_list		*substr_list;
	t_list_iter	it;
	int			ch[2];
	int			old_size;

	substr_list = string_split(pattern, WILDCARD_MASK);
	if (!substr_list)
		return (FALSE);
	it = list_iter(substr_list);
	ch[0] = 0;
	old_size = 0;
	while (list_iter_next(&it))
	{
		ch[1] = string_find(str, it.value);
		if (!(ch[1] != -1 && ch[1] >= ch[0] + old_size))
			break ;
	}
	list_apply(substr_list, (t_apply_lfn)string_delete);
	list_delete(substr_list);
	return (!it.value);
}

t_list	*unwrap_wildcard(t_string *wcstr, t_string *path)
{
	t_list		*files;
	t_list_iter	it;
	t_list		*result;

	files = get_all_files_from_relative_path_str(path);
	if (!files)
		return (nil());
	it = list_iter(files);
	result = list_new();
	while (list_iter_next(&it))
	{
		if (match_pattern(wcstr, it.value))
			list_insert(result, string_copy(it.value));
	}
	list_apply(files, (t_apply_lfn)string_delete);
	list_delete(files);
	return (result);
}

static t_list	*pre_unwrap_wildcard(t_string *wcstr)
{
	t_list		*path;
	t_list		*result;
	t_string	*path_str;
	t_bool		is_path;

	is_path = FALSE;
	if (string_is_path_string(wcstr))
	{
		path = get_path(wcstr);
		wcstr = list_pop(path);
		is_path = TRUE;
	}
	else
	{
		path = list_new();
		list_insert(path, string_from_cstr("."));
	}
	path_str = get_path_string(path, TRUE);
	result = unwrap_wildcard(wcstr, path_str);
	list_apply(path, (t_apply_lfn)string_delete);
	list_delete(path);
	string_delete(path_str);
	if (is_path)
		string_delete(wcstr);
	return (result);
}

t_bool	parse_wildcard(t_list *tokens, t_string *wcstr)
{
	t_list		*matches;
	t_string	*op;
	t_list_iter	it;

	matches = pre_unwrap_wildcard(wcstr);
	if (matches && matches->size == 0)
		(list_delete(matches), matches = YNULL);
	if (!matches)
		return (yprintf("Unable to find any matches\n"), FALSE);
	op = list_get(tokens, tokens->size - 1);
	it = list_iter(matches);
	if (op && is_redirect(op))
	{
		list_iter_next(&it);
		list_insert(tokens, string_copy(op));
		list_insert(tokens, it.value);
	}
	while (list_iter_next(&it))
	{
		if (op && is_redirect(op))
			list_insert(tokens, string_copy(op));
		list_insert(tokens, it.value);
	}
	return (TRUE);
}
