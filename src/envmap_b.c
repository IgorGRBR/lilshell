/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envmap_b.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 12:49:46 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/11/13 12:49:54 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"
#include "ylist.h"
#include "ystring.h"
#include "ytypes.h"

static t_string	*expander(t_string *str, t_map *envmap)
{
	return (envmap_expand(string_copy(str), envmap));
}

// static t_bool	empty_filter(t_string *str)
// {
// 	if (!str)
// 		return (TRUE);
// 	if (str->size == 0)
// 	{
// 		string_delete(str);
// 		return (TRUE);
// 	}
// 	return (FALSE);
// }

void	expand_program_envvars(t_pcall *program)
{
	program->pname = envmap_expand(string_copy(program->pname),
			global().environment);
	program->params = list_cmap(program->params, (t_cmap_lfn)expander,
			global().environment);
	list_filter(program->params, (t_filter_lfn)null_filter);
	if (!program->pname || program->pname->size == 0)
	{
		if (program->pname)
			string_delete(program->pname);
		program->pname = list_get(program->params, 0);
		list_remove_at(program->params, 0);
	}
}

void	cleanup_program_envvars(t_pcall *program)
{
	if (program->pname)
		string_delete(program->pname);
	list_apply(program->params, (t_apply_lfn)string_delete);
	list_delete(program->params);
}
