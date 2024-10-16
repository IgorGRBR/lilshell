/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 15:00:22 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/09/12 15:00:23 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"
#include "ylist.h"
#include "ytypes.h"
#include <unistd.h>
#include <stdlib.h>

static void	*ast_cleanup(t_list *exp_list)
{
	list_apply(exp_list, (t_apply_lfn)expression_delete);
	list_delete(exp_list);
	return (nil());
}

static t_bool	handle_redirect(t_list_iter	*it, t_expression *exp)
{
	t_operator		redirect_op;
	t_red_type		type;

	redirect_op = get_op(it->value);
	if (redirect_op == OP_STDIN)
		type = REDIRECT_STDIN;
	else if (redirect_op == OP_STDOUT)
		type = REDIRECT_STDOUT;
	else if (redirect_op == OP_APPEND)
		type = REDIRECT_APPEND;
	else if (redirect_op == OP_DELIM)
		type = REDIRECT_HEREDOC;
	if (!list_iter_next(it))
		return (FALSE);
	if (!is_identifier(it->value))
		return (FALSE);
	list_insert(exp->redirections, redirection_new(type, it->value));
	return (TRUE);
}

static t_bool	make_expression(t_list_iter *it, t_expression *exp)
{
	while (is_redirect(it->value) && handle_redirect(it, exp))
		list_iter_next(it);
	if (is_operator(it->value))
		return (FALSE);
	if (is_parens_op(it->value))
	{
		exp->type = NESTED_EXPRESSION;
		exp->nested = build_exp_list(it, TRUE);
		if (!exp->nested)
			return (FALSE);
	}
	else
		exp->pcall.pname = string_copy(it->value);
	while (list_iter_next(it) && (is_identifier(it->value)
			|| is_redirect(it->value)))
	{
		if (is_identifier(it->value))
			list_insert(exp->pcall.params, string_copy(it->value));
		if (is_redirect(it->value) && !handle_redirect(it, exp))
			return (expression_delete(exp), FALSE);
	}
	return (TRUE);
}

t_list	*build_exp_list(t_list_iter *it, t_bool nested)
{
	t_list			*elist;
	t_expression	*exp;

	elist = list_new();
	while (list_iter_next(it) && !is_parens_cl(it->value))
	{
		exp = expression_new(PROGRAM_CALL, YNULL);
		if (!exp || !make_expression(it, exp))
			return (expression_delete(exp), ast_cleanup(elist));
		if (it->value && is_operator(it->value))
			exp->op = get_op(it->value);
		list_insert(elist, exp);
		if (it->value)
		{
			if ((nested && !it->value) || (!nested && is_parens_cl(it->value)))
				return (ast_cleanup(elist));
			if (nested && is_parens_cl(it->value))
				return (elist);
		}
	}
	if (nested && !it->value)
		return (ast_cleanup(elist));
	return (elist);
}
