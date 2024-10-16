/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 14:20:43 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/07/21 14:20:48 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ylib.h"
#include "minishell.h"
#include "ytypes.h"

void	builtins(void)
{
	register_sync_builtin(string_from_cstr("exit"), exit_builtin);
	register_builtin(string_from_cstr("echo"), echo_builtin);
	register_sync_builtin(string_from_cstr("cd"), cd_builtin);
	register_sync_builtin(string_from_cstr("pwd"), pwd_builtin);
	register_sync_builtin(string_from_cstr("export"), export_builtin);
	register_sync_builtin(string_from_cstr("unset"), unset_builtin);
	register_builtin(string_from_cstr("env"), env_builtin);
}

int	main(int argc, char **argv, char *envp[])
{
	t_list_iter	it;
	t_string	*cmd_string;

	cmd_string = YNULL;
	setup_terminal(envp);
	builtins();
	if (argc > 1)
		cmd_string = string_from_cstr(argv[1]);
	while (io_loop(cmd_string))
		;
	it = list_iter(global().history);
	while (list_iter_next(&it))
		yprintf("history entry: %s\n", ((t_string *)it.value)->cstr);
	cleanup_terminal();
	return (0);
}
