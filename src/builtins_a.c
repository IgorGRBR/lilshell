/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_a.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 15:16:46 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/09/19 15:16:47 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"
#include <unistd.h>

t_bool	try_run_builtin(t_pcall *program, int *exit_code)
{
	t_program_func	program_function;

	program_function = map_get(global().builtin_programs, program->pname);
	if (program_function)
	{
		*exit_code = program_function(program->params, YNULL);
		cleanup_program_envvars(program);
		return (TRUE);
	}
	return (try_run_sync_builtin(program, exit_code));
}

t_bool	try_run_sync_builtin(t_pcall *program, int *exit_code)
{
	t_program_func	program_function;

	program_function = map_get(global().builtin_sync_programs, program->pname);
	if (program_function)
	{
		*exit_code = program_function(program->params, YNULL);
		return (TRUE);
	}
	return (FALSE);
}

t_bool	register_builtin(t_string *name, t_program_func builtin)
{
	return (map_insert(global().builtin_programs, name, builtin));
}

t_bool	register_sync_builtin(t_string *name, t_program_func builtin)
{
	return (map_insert(global().builtin_sync_programs, name, builtin));
}
