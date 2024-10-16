/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_a.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 15:01:28 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/09/12 15:01:29 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"

t_bool	is_operator(t_string *token)
{
	return (get_op(token) != OP_NONE);
}

t_operator	get_op(t_string *token)
{
	if (cstr_cmp(token->cstr, "&&") == 0)
		return (OP_AND);
	else if (cstr_cmp(token->cstr, "||") == 0)
		return (OP_OR);
	else if (cstr_cmp(token->cstr, "|") == 0)
		return (OP_PIPE);
	else if (cstr_cmp(token->cstr, ">") == 0)
		return (OP_STDOUT);
	else if (cstr_cmp(token->cstr, "<") == 0)
		return (OP_STDIN);
	else if (cstr_cmp(token->cstr, ">>") == 0)
		return (OP_APPEND);
	else if (cstr_cmp(token->cstr, "<<") == 0)
		return (OP_DELIM);
	return (OP_NONE);
}

t_bool	is_parens_op(t_string *token)
{
	return (cstr_cmp(token->cstr, "(") == 0);
}

t_bool	is_parens_cl(t_string *token)
{
	return (cstr_cmp(token->cstr, ")") == 0);
}

t_bool	is_parens(t_string *token)
{
	return (is_parens_op(token) || is_parens_cl(token));
}
