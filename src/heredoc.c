/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/06 15:53:50 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/10/06 15:53:51 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"
#include <stdio.h>
#include <readline/readline.h>
#include <stdlib.h>

void	handle_heredoc(t_string *delim, int out_pipe)
{
	char		*inp_cstr;
	t_string	inp_str;
	char		*prompt_cstr;

	string_init(&inp_str);
	prompt_cstr = cstr_format("minish heredoc (%s)> ", delim->cstr);
	while (TRUE)
	{
		string_deinit(&inp_str);
		inp_cstr = readline(prompt_cstr);
		string_init_from_cstr(&inp_str, inp_cstr);
		free(inp_cstr);
		if (!string_equal(&inp_str, delim))
		{
			string_append_cstr(&inp_str, "\n");
			write_cstr_fd(out_pipe, inp_str.cstr);
		}
		else
			break ;
	}
	string_deinit(&inp_str);
	free(prompt_cstr);
}
