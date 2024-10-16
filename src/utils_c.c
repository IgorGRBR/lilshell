/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_c.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 13:26:53 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/11/28 13:26:55 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"

static const t_string	g_path_str = {"PATH", 4};

t_list	*get_paths(t_map *envmap)
{
	t_string	*env_str;
	t_list		*result;

	env_str = map_get(envmap, (void *)&g_path_str);
	if (!env_str)
		return (YNULL);
	result = string_split(env_str, ':');
	return (result);
}

void	str_del_if_exists(t_string *str)
{
	if (str)
		string_delete(str);
}

t_bool	null_filter(void *ptr)
{
	return (ptr == YNULL);
}
