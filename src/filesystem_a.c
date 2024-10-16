/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filesystem_a.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/25 16:09:58 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/09/25 16:09:59 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ylib.h"
#include "minishell.h"
#include <unistd.h>
#include <stdlib.h>

t_list	*get_path(t_string *path_str)
{
	t_list		*result;
	t_string	*trimmed_str;
	t_list_iter	it;

	trimmed_str = string_trim_cstr(path_str, "/");
	result = string_split(trimmed_str, '/');
	string_delete(trimmed_str);
	it = list_iter(result);
	while (list_iter_next(&it))
	{
		if (((t_string *)it.value)->size == 0)
		{
			list_apply(result, (t_apply_lfn)string_delete);
			list_delete(result);
			return (YNULL);
		}
	}
	return (result);
}

t_string	*get_working_dir_str(void)
{
	char		*cdir_cstr;
	t_string	*cdir_str;

	cdir_cstr = getcwd(YNULL, 0);
	cdir_str = string_from_cstr(cdir_cstr);
	free(cdir_cstr);
	return (cdir_str);
}

t_list	*get_working_dir(void)
{
	t_string	*cwd_str;
	t_list		*path;

	cwd_str = get_working_dir_str();
	path = get_path(cwd_str);
	string_delete(cwd_str);
	return (path);
}

t_string	*get_path_string(t_list *path, t_bool relative)
{
	t_string	*str;
	t_list_iter	it;

	str = string_from_cstr("");
	it = list_iter(path);
	while (list_iter_next(&it))
	{
		if (relative)
			relative = FALSE;
		else
			string_append_cstr(str, "/");
		string_append(str, it.value);
	}
	return (str);
}

t_list	*get_absolute_path(t_list *where)
{
	t_list		*new_path;
	t_list_iter	it;
	t_string	*str;

	new_path = get_working_dir();
	it = list_iter(where);
	while (list_iter_next(&it))
	{
		str = it.value;
		if (string_equal_cstr(str, ".."))
		{
			str = list_pop(new_path);
			if (!str)
				return (list_apply(new_path, (t_apply_lfn)string_delete),
					list_delete(new_path), nil());
			string_delete(str);
		}
		else if (!string_equal_cstr(str, "."))
			list_insert(new_path, string_copy(str));
		if (!path_exists(new_path))
			return (list_apply(new_path, (t_apply_lfn)string_delete),
				list_delete(new_path), nil());
	}
	return (new_path);
}
