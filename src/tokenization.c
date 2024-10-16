/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenization.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 14:58:41 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/09/12 14:58:42 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"
#include "ystring.h"
#include "ytypes.h"
#include <stdlib.h>

static void	parse_str(const char *cstr, t_uint *i, char quote,
	t_string *p_str)
{
	t_string	*str;
	char		*mcstr;
	t_uint		start;

	(*i)++;
	start = *i;
	while (cstr[*i] && cstr[*i] != quote)
		(*i)++;
	mcstr = cstr_sub(cstr, start, *i - start);
	str = string_from_cstr(mcstr);
	if (quote == '"')
		string_char_to_mask(str, TRUE);
	(free(mcstr), string_append(p_str, str));
	string_delete(str);
}

static t_string	*parse_operator(const char *cstr, t_uint *i)
{
	t_string	*str;
	char		*mcstr;
	t_uint		start;
	char		op_char;

	start = *i;
	op_char = cstr[*i];
	while (cstr[*i] == op_char)
		(*i)++;
	if (*i == start)
		return (*i = 0, nil());
	mcstr = cstr_sub(cstr, start, *i - start);
	str = string_from_cstr(mcstr);
	free(mcstr);
	return (str);
}

static const char	g_wc = WILDCARD_MASK;

static void	parse_token(t_list *tokens, const char *cstr, t_uint *i,
		t_uint start)
{
	t_string	*str;
	t_string	*temp;
	char		*mcstr;

	str = string_new();
	while (cstr[*i] && cstr[*i] != ' ' && cstr[*i] != '(' && cstr[*i] != ')'
		&& cstr[*i] != '&' && cstr[*i] != '|' && cstr[*i] != '<'
		&& cstr[*i] != '>')
	{
		if (cstr[*i] == '"' || cstr[*i] == '\'')
		{
			mcstr = cstr_sub(cstr, start, *i - start);
			temp = string_from_cstr(mcstr);
			string_char_to_mask(temp, FALSE);
			(string_append(str, temp), free(mcstr), string_delete(temp));
			(parse_str(cstr, i, cstr[*i], str), start = *i + 1);
		}
		(*i)++;
	}
	mcstr = cstr_sub(cstr, start, *i - start);
	temp = string_from_cstr(mcstr);
	string_char_to_mask(temp, FALSE);
	(string_append(str, temp), free(mcstr), string_delete(temp));
	if (!(string_find_char(str, g_wc) >= 0 && parse_wildcard(tokens, str)))
		list_insert(tokens, str);
}

static t_string	*parse_parens(const char *cstr, t_uint *i)
{
	if (cstr[*i] == '(')
		return (++*i, string_from_cstr("("));
	else
		return (++*i, string_from_cstr(")"));
}

t_list	*tokenize(t_string *cmd_str)
{
	t_list		*tokens;
	t_uint		i;
	t_string	*str;

	tokens = list_new();
	i = 0;
	while (cmd_str->cstr[i])
	{
		str = YNULL;
		i += cmd_str->cstr[i] == ' ';
		if (cmd_str->cstr[i] == '(' || cmd_str->cstr[i] == ')')
			str = parse_parens(cmd_str->cstr, &i);
		else if (cmd_str->cstr[i] == '&' || cmd_str->cstr[i] == '|'
			|| cmd_str->cstr[i] == '<' || cmd_str->cstr[i] == '>')
			str = parse_operator(cmd_str->cstr, &i);
		else if (cmd_str->cstr[i] != ' ')
			parse_token(tokens, cmd_str->cstr, &i, i);
		if (i == 0)
			return (list_apply(tokens, (t_apply_lfn)string_delete),
				list_delete(tokens), nil());
		(void)((str) && (list_insert(tokens, str), 1));
	}
	return (tokens);
}
