/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ihhrabar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 14:30:08 by ihhrabar          #+#    #+#             */
/*   Updated: 2023/07/25 14:30:10 by ihhrabar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# include "ytypes.h"
# define ENVVAR_CHAR '$'
# define ENVVAR_MASK 26
# define WILDCARD_CHAR '*'
# define WILDCARD_MASK 5
# define SHELL_LABEL "minish> "

typedef enum e_exp_type {
	PROGRAM_CALL,
	NESTED_EXPRESSION,
}	t_exp_type;

typedef enum s_operator {
	OP_NONE,
	OP_OR,
	OP_AND,
	OP_PIPE,
	OP_STDIN,
	OP_STDOUT,
	OP_DELIM,
	OP_APPEND,
}	t_operator;

typedef struct s_pcall {
	t_string	*pname;
	t_list		*params;
}	t_pcall;

typedef enum e_redirection_type {
	REDIRECT_STDIN,
	REDIRECT_STDOUT,
	REDIRECT_APPEND,
	REDIRECT_HEREDOC,
}	t_red_type;

typedef struct s_redirection {
	t_red_type		type;
	t_string		*path;
}	t_redirection;

typedef struct s_expression {
	t_exp_type			type;
	t_list				*nested;
	t_operator			op;
	t_pcall				pcall;
	t_list				*redirections;
}	t_expression;

typedef struct s_io_pipes {
	int	in;
	int	out;
}	t_io_pipes;

typedef enum e_cmd_result_type {
	RESULT_PROC_ID,
	RESULT_EXIT_CODE
}	t_cmd_result_type;

typedef struct s_command_result {
	t_cmd_result_type	type;
	int					data;
}	t_cmd_result;

typedef struct s_interpreter_context {
	t_operator		prev_operator;
	int				status_code;
	t_io_pipes		og_pipes;
	t_io_pipes		pipes;
	t_cmd_result	prev_result;
}	t_itptr_ctx;

typedef struct s_ast_refs {
	t_list	*ast;
	t_list	*tokens;
}	t_ast_refs;

typedef struct s_global {
	t_list		*history;
	t_map		*builtin_programs;
	t_map		*builtin_sync_programs;
	t_map		*environment;
	t_bool		exit;
	t_ast_refs	*itptr_ast;
	t_bool		_initialized;
}	t_global;

// Utilities
t_bool			is_operator(t_string *token);
t_operator		get_op(t_string *token);
t_bool			is_parens_op(t_string *token);
t_bool			is_parens_cl(t_string *token);
t_bool			is_parens(t_string *token);
t_bool			is_redirect(t_string *token);
t_bool			is_redirect_op(t_operator op);
t_bool			is_identifier(t_string *token);
t_expression	*expression_new(t_exp_type type, t_string *pname);
void			expression_delete(t_expression *exp);
t_list			*get_paths(t_map *envmap);
void			string_char_to_mask(t_string *str, t_bool ignore_wildcards);
t_bool			string_is_valid_var_name(t_string *str);
void			str_del_if_exists(t_string *str);
t_bool			null_filter(void *ptr);

// Filesystem stuff
t_bool			path_exists(t_list *path);
t_bool			path_is_directory(t_list *path);
t_list			*get_path(t_string *path_str);
t_string		*get_path_string(t_list *path, t_bool relative);
t_string		*get_working_dir_str(void);
t_list			*get_absolute_path(t_list *where);
t_bool			change_working_dir_abs(t_list *where);
t_bool			string_is_path_string(t_string *str);
int				open_file_read(t_string *path_str);
int				open_file_write(t_string *path_str);
int				open_file_append(t_string *path_str);
t_list			*get_all_files_from_relative_path_str(t_string *str);

// Built-in programs
typedef int		(*t_program_func)(t_list *, void *);
t_bool			register_builtin(t_string *name, t_program_func builtin);
t_bool			register_sync_builtin(t_string *name, t_program_func builtin);
t_bool			try_run_builtin(t_pcall *program, int *exit_code);
t_bool			try_run_sync_builtin(t_pcall *program, int *exit_code);

int				exit_builtin(t_list *args, void *context);
int				echo_builtin(t_list *args, void *context);
int				cd_builtin(t_list *args, void *context);
int				pwd_builtin(t_list *args, void *context);
int				export_builtin(t_list *args, void *context);
int				unset_builtin(t_list *args, void *context);
int				env_builtin(t_list *args, void *context);
// TODO: remove these:
int				argprint_builtin(t_list *args, void *context);

// Pipe operations
void			set_pipes(int rpipe, int wpipe);
void			set_read_pipe(t_itptr_ctx *ctx, int rpipe);
void			set_write_pipe(t_itptr_ctx *ctx, int wpipe);
void			reset_pipes(t_itptr_ctx *ctx);

// Redirection operations
t_redirection	*redirection_new(t_red_type type, t_string *path);
void			redirection_delete(t_redirection *rd);
void			redirect_stdin(t_string *file_path);
void			redirect_stdin_delimeter(t_string *delimeter);
void			redirect_stdout(t_string *file_path, t_bool append_mode);
void			handle_heredoc(t_string *delim, int out_pipe);

// Wildcards
t_list			*unwrap_wildcard(t_string *wcstr, t_string *path);
t_bool			parse_wildcard(t_list *tokens, t_string *wcstr);

// Environment variables
t_map			*envmap_from_env(char **env);
t_map			*envmap_new(void);
void			envmap_delete(t_map *envmap);
t_string		*expand_get_string(t_string *str, t_map *envmap);
t_string		*envmap_expand(t_string *str, t_map *envmap);
void			expand_program_envvars(t_pcall *program);
void			cleanup_program_envvars(t_pcall *program);
void			update_status_code(t_itptr_ctx *ctx, t_map *envmap);
char			**make_envp(t_map *envmap);
void			f_env(char **envp);

// Signals
void			setup_signal_handlers(void);
void			reset_signal_handlers(void);
void			sigint_handler(int c);
void			rt_sigint_handler(int c);
void			exit_handler(int c);

// Main stuff (TODO: implement cleanup_terminal)
void			setup_terminal(char *envp[]);
void			cleanup_terminal(void);
t_global		global(void);
void			init_global(char **envp);
void			global_exit(void);
void			clear_global(void);
t_list			*tokenize(t_string *cmd_str);
t_list			*build_exp_list(t_list_iter *it, t_bool nested);
t_cmd_result	interpret_ast(t_list *ast, t_io_pipes pipes, t_bool rq_fork,
					int wstatus);
t_bool			io_loop(t_string *cmd_str);
t_bool			run_process(t_list *arg_list, t_list *path_list);

#endif
