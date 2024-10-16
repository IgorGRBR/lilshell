/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 13:25:41 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/09/13 13:25:42 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "yio.h"
#include "ylib.h"
#include "ystring.h"
#include "ytypes.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

static void	*to_cstr(void *str)
{
	if (((t_string *)str)->size == 0)
		return (YNULL);
	return (((t_string *)str)->cstr);
}

static void	append_slash(void *vstr)
{
	t_string	*str;

	str = vstr;
	if (str && string_get(str, str->size - 1) != '/')
		string_append_cstr(str, "/");
}

static t_bool	try_run_relative_program(t_list *arg_list, char **envp)
{
	t_list		*abs_p;
	t_list		*rel_p;
	t_string	*cmd;
	t_list		*cstr_args;

	if (((t_string *)list_get(arg_list, 0))->cstr[0] != '/')
	{
		rel_p = get_path(list_get(arg_list, 0));
		abs_p = get_absolute_path(rel_p);
		(list_apply(rel_p, (t_apply_lfn)string_delete), list_delete(rel_p));
	}
	else
		abs_p = get_path(list_get(arg_list, 0));
	if (!abs_p || !path_exists(abs_p))
		return (perror("minish"), f_env(envp), FALSE);
	if (path_is_directory(abs_p))
		(write_cstr_fd(STDERR_FILENO, "minish: Is a directory"), exit(126));
	cmd = get_path_string(abs_p, FALSE);
	cstr_args = list_map(arg_list, (t_map_lfn)to_cstr);
	(execve(cmd->cstr, (char **)(cstr_args->data), envp), f_env(envp));
	(perror(cmd->cstr), string_delete(cmd), list_delete(cstr_args));
	(list_apply(abs_p, (t_apply_lfn)string_delete), list_delete(abs_p));
	return (TRUE);
}

t_bool	run_process(t_list *arg_list, t_list *path_list)
{
	t_string	*cmd;
	t_uint		i;
	t_list		*cstr_args;
	char		**envp;

	envp = make_envp(global().environment);
	if (string_is_path_string(list_get(arg_list, 0)))
		return (try_run_relative_program(arg_list, envp));
	(list_insert(arg_list, string_from_cstr("")), i = 0);
	cstr_args = list_map(arg_list, (t_map_lfn)to_cstr);
	(void)(path_list && (list_apply(path_list, (t_apply_lfn)append_slash), 1));
	while (path_list && i < path_list->size)
	{
		cmd = string_concat(list_get(path_list, i), list_get(arg_list, 0));
		execve(cmd->cstr, (char **)(cstr_args->data), envp);
		string_delete(cmd);
		i++;
	}
	(list_apply(cstr_args, (t_apply_lfn)free), list_delete(cstr_args));
	(void)(path_list && (list_apply(path_list, (t_apply_lfn)string_delete),
		list_delete(path_list), 1));
	(f_env(envp), write_cstr_fd(STDERR_FILENO, "minish: command not found\n"));
	return (TRUE);
}
