/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filesystem_c.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/02 15:56:24 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/10/02 15:56:25 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ylib.h"
#include "fcntl.h"

int	open_file_read(t_string *path_str)
{
	int			fd;

	if (!path_str)
		return (0);
	if (!try_open_file(path_str, O_RDONLY, &fd))
		fd = -1;
	string_delete(path_str);
	return (fd);
}

int	open_file_write(t_string *path_str)
{
	int			fd;

	if (!path_str)
		return (0);
	if (!try_open_file(path_str, O_WRONLY | O_CREAT, &fd))
		fd = -1;
	string_delete(path_str);
	return (fd);
}

int	open_file_append(t_string *path_str)
{
	int			fd;

	if (!path_str)
		return (0);
	if (!try_open_file(path_str, O_WRONLY | O_CREAT | O_APPEND, &fd))
		fd = -1;
	string_delete(path_str);
	return (fd);
}
