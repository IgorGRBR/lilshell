/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_b.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 15:36:26 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/09/19 15:36:27 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ycstr.h"
#include "yio.h"
#include "ylib.h"
#include "minishell.h"
#include "ymap.h"
#include "ystring.h"
#include "ytypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int	echo_builtin(t_list *args, void *context)
{
	t_list_iter	it;
	t_string	*arg;
	t_bool		print_newline;

	(void)context;
	print_newline = TRUE;
	it = list_iter(args);
	list_iter_next(&it);
	arg = it.value;
	if (arg)
	{
		if (string_equal_cstr(arg, "-n"))
			print_newline = FALSE;
		else
			yprintf("%s", arg->cstr);
	}
	while (list_iter_next(&it))
	{
		arg = it.value;
		if (arg)
			yprintf(" %s", arg->cstr);
	}
	if (print_newline)
		yprintf("\n");
	return (0);
}

int	exit_builtin(t_list *args, void *context)
{
	int			exit_code;
	t_string	*num_str;

	(void)context;
	if (args->size < 1)
		exit_code = 0;
	else if (args->size > 1)
	{
		write_cstr_fd(STDERR_FILENO, "exit: too many arguments\n");
		exit_code = 1;
	}
	else
	{
		num_str = list_get(args, 0);
		if (!cstr_try_atoi(num_str->cstr, &exit_code))
		{
			write_cstr_fd(STDERR_FILENO, "exit: numeric argument required\n");
			exit_code = 2;
		}
	}
	cleanup_terminal();
	exit(exit_code);
	return (exit_code);
}

static t_bool	cd_helper(t_list *path, t_bool absolute)
{
	t_bool	result;
	t_list	*abs_path;

	if (!absolute)
	{
		abs_path = get_absolute_path(path);
		(list_apply(path, (t_apply_lfn)string_delete), list_delete(path));
	}
	else
		abs_path = path;
	if (!abs_path)
		return (write_cstr_fd(STDERR_FILENO,
				"cd: No such file or directory\n"), FALSE);
	result = change_working_dir_abs(abs_path);
	if (!result)
		write_cstr_fd(STDERR_FILENO, "cd: No such file or directory\n");
	(list_apply(abs_path, (t_apply_lfn)string_delete), list_delete(abs_path));
	return (result);
}

static const t_string	g_home_var = {"HOME", 4};
// static const t_string	g_pwd_var = {"PWD", 3};

int	cd_builtin(t_list *args, void *context)
{
	t_string	*path_str;
	t_list		*path;
	t_bool		result;

	if (args->size > 1)
		return (write_cstr_fd(STDERR_FILENO, "cd: too many arguments\n"), 1);
	path_str = list_get(args, 0);
	if (!path_str)
		path_str = map_get(global().environment, (void *)&g_home_var);
	if (!path_str)
		return (1);
	context = path_str;
	path_str = string_trim_cstr(context, " \t");
	string_delete(context);
	path = get_path(path_str);
	result = cd_helper(path, path_str->cstr[0] == '/');
	if (result)
	{
		return (string_delete(path_str), 0);
	}
	return (string_delete(path_str), 1);
}

int	pwd_builtin(t_list *args, void *context)
{
	t_string	*path_str;

	(void)args;
	(void)context;
	path_str = get_working_dir_str();
	yprintf("%s\n", path_str->cstr);
	string_delete(path_str);
	return (0);
}
