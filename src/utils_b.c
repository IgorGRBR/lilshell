/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_b.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 15:01:33 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/09/12 15:01:34 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ychar.h"
#include "ylib.h"
#include "minishell.h"
#include "ytypes.h"

t_bool	is_redirect(t_string *token)
{
	return (string_equal_cstr(token, ">")
		|| string_equal_cstr(token, "<")
		|| string_equal_cstr(token, ">>")
		|| string_equal_cstr(token, "<<"));
}

t_bool	is_redirect_op(t_operator op)
{
	return (op == OP_STDIN
		|| op == OP_STDOUT
		|| op == OP_APPEND
		|| op == OP_DELIM);
}

t_bool	is_identifier(t_string *token)
{
	return (!is_parens(token) && !is_operator(token));
}

void	string_char_to_mask(t_string *str, t_bool ignore_wildcards)
{
	t_uint	i;

	i = 0;
	while (i < str->size)
	{
		if (str->cstr[i] == ENVVAR_CHAR && (is_alpha(str->cstr[i + 1])
				|| str->cstr[i + 1] == '_' || str->cstr[i + 1] == '?'))
			str->cstr[i] = ENVVAR_MASK;
		else if (str->cstr[i] == WILDCARD_CHAR && !ignore_wildcards)
			str->cstr[i] = WILDCARD_MASK;
		i++;
	}
}

t_bool	string_is_valid_var_name(t_string *str)
{
	t_uint	i;

	i = 0;
	if (!is_alpha(str->cstr[i]) && str->cstr[i] != '_')
		return (FALSE);
	while (i < str->size)
	{
		if (!is_alnum(str->cstr[i]) && str->cstr[i] != '_')
			return (FALSE);
		i++;
	}
	return (TRUE);
}
