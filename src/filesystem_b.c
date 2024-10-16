/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filesystem_b.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/25 17:21:55 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/09/25 17:21:56 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ylib.h"
#include "minishell.h"
#include "ystring.h"
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

t_bool	path_exists(t_list *path)
{
	t_string	*path_str;
	struct stat	stat_buf;

	path_str = get_path_string(path, FALSE);
	if (stat(path_str->cstr, &stat_buf) == 0)
	{
		string_delete(path_str);
		return (TRUE);
	}
	string_delete(path_str);
	return (FALSE);
}

t_bool	path_is_directory(t_list *path)
{
	t_string	*path_str;
	DIR			*dir_stream;

	if (!path_exists(path))
		return (FALSE);
	path_str = get_path_string(path, FALSE);
	dir_stream = opendir(path_str->cstr);
	if (dir_stream)
	{
		closedir(dir_stream);
		return (TRUE);
	}
	return (FALSE);
}

// return (string_equal_cstr(str, "..")
// 		|| string_equal_cstr(str, ".")
// 		|| string_find_char(str, '/'));
t_bool	string_is_path_string(t_string *str)
{
	return (string_find_char(str, '/') != -1);
}

t_list	*get_all_files_from_relative_path_str(t_string *str)
{
	t_list			*result;
	DIR				*dir_ptr;
	struct dirent	*dirent_ptr;
	t_string		*file_name;

	result = list_new();
	dir_ptr = opendir(str->cstr);
	dirent_ptr = readdir(dir_ptr);
	while (dirent_ptr)
	{
		file_name = string_from_cstr(dirent_ptr->d_name);
		if (!string_equal_cstr(file_name, ".")
			&& !string_equal_cstr(file_name, ".."))
			list_insert(result, file_name);
		dirent_ptr = readdir(dir_ptr);
	}
	closedir(dir_ptr);
	return (result);
}

t_bool	change_working_dir_abs(t_list *where)
{
	t_string	*path_str;
	t_string	*temp;
	t_bool		success;

	path_str = get_path_string(where, FALSE);
	temp = string_from_cstr("/");
	string_append(temp, path_str);
	string_delete(path_str);
	success = chdir(temp->cstr) == 0;
	string_delete(temp);
	return (success);
}
