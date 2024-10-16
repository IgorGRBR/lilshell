/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_a.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 16:27:05 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/11/27 16:27:07 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"
#include "ystring.h"
#include <stdlib.h>

t_redirection	*redirection_new(t_red_type type, t_string *path)
{
	t_redirection	*rd;

	rd = (t_redirection *)malloc(sizeof (t_redirection));
	rd->type = type;
	rd->path = string_copy(path);
	return (rd);
}

void	redirection_delete(t_redirection *rd)
{
	string_delete(rd->path);
	free(rd);
}
