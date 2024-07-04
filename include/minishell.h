/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 21:08:44 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/04 11:10:42 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../include/libft/libft.h"
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <termios.h>
# include <unistd.h>

// # include "../libft/includes/libft.h"
// # include "defines.h"
// # include "structs.h"
// # include <dirent.h>
// # include <errno.h>
// # include <fcntl.h>
// # include <limits.h>
// # include <readline/history.h>
// # include <readline/readline.h>
// # include <signal.h>
// # include <stdio.h>
// # include <stdlib.h>
// # include <string.h>
// # include <sys/wait.h>
// # include <termios.h>
// # include <unistd.h>

/* Enumeration for token types.
   This enumeration contains various token types used in the program. */

/* PIPES: Represents a pipe token. */
/* GREAT: Represents a '>' token. output */
/* GREAT_GREAT: Represents a '>>' token. redirection */
/* LESS: Represents a '<' token. input */
/* LESS_LESS: Represents a '<<' token.  here document */
/* STRING: Represents a string token. */
/* UNKNOWN: Represents an unknown token type, default value is -1. */

typedef enum s_class
{
	PIPES,
	GREAT,
	GREAT_GREAT,
	LESS,
	LESS_LESS,
	STRING,
	UNKNOWN = -1,
}							t_class;

/* Define a structure for a doubly linked list node.
   This structure contains pointers to the next and previous nodes,
   an index, data, and a class. */

/* char *data: Pointer to a character array (string) containing the data. */
/* t_class class: Variable of type t_class representing the class of the node. */
/* int index: Integer representing the index of the node. */
/* struct s_node *next: Pointer to the next node in the doubly linked list. */
/* struct s_node *prev: Pointer to the previous node in the doubly linked list. */

typedef struct s_node
{
	char					*data;
	t_class					class;
	int						index;
	struct s_node			*next;
	struct s_node			*prev;
}							t_node;

/* Define a structure for representing an environment variable.
   This structure contains pointers to the next and previous environment variables,
   data and title strings. */

/* char *data: Pointer to a character array (string) containing the data. */
/* char *title: Pointer to a character array (string) containing the title. */
/* struct s_environment *next: Pointer to the next environment variable in the list. */
/* struct s_environment *prev: Pointer to the previous environment variable in the list. */

typedef struct s_environment
{
	char					*title;
	char					*data;
	struct s_environment	*next;
	struct s_environment	*prev;
}							t_environment;

/* Define a structure for representing a command and its arguments.
   This structure contains pointers to the next and previous commands,
   a string for the command, an array of strings for arguments,
	a file name for here document,
   an index, and lists of token arguments and redirections. */

/* char *str: Pointer to a character array (string) containing the command. */
/* char **arguments: Array of strings containing the arguments of the command. */
/* char *hd_file_name: Pointer to a character array (string) containing the here document file name. */
/* int count: Integer representing the index of the command in the table. */
/* t_node *token_arg: Pointer to the list of token arguments for the command. */
/* t_node *redirections: Pointer to the list of redirections for the command. */
/* struct s_simple_cmds *next: Pointer to the next command in the table. */
/* struct s_simple_cmds *prev: Pointer to the previous command in the table. */

typedef struct s_simple_cmds
{
	char					*command;
	char					**arguments;
	char					*hd_file_name;
	int						count;
	t_node					*token_arg;
	t_node					*redirections;
	struct s_simple_cmds	*next;
	struct s_simple_cmds	*prev;
}							t_simple_cmds;

/* Define a structure containing various tools and settings used in the program.
   This structure contains strings for user information, arrays for paths,
	pipes,
   environment variables, prompt messages,
	and flags indicating various statuses.
   It also includes file descriptors, process IDs, terminal settings,
	and lists of
   commands and environment variables. */

/* char *login_id: Pointer to a character array (string) containing the user name. */
/* char **paths: Array of strings containing paths to search for commands. */
/* char **pipes: Array of strings containing pipes for debugging. */
/* char **envp: Array of strings containing environment variables. */
/* char *message: Pointer to a character array (string) containing the current prompt message. */
/* char *content_hd: Pointer to a character array (string) containing the here document content. */
/* char *shell_string: Pointer to a character array (string) containing the prompt message. */
/* int last_status: Integer representing the exit code of the last executed command. */
/* int standard_input_fd: Integer representing the standard input file descriptor. */
/* int standard_output_fd: Integer representing the standard output file descriptor. */
/* int flag_envair: Integer flag indicating if the environment is absent. */
/* int flag_pipe: Integer flag indicating if the execution is on a pipe. */
/* int flag_hd: Integer flag indicating if here document expansion is required. */
/* int requires_expansion: Integer flag indicating if expansion is required. */
/* int flag_ready_to_execute: Integer flag indicating if the command should be executed. */
/* int flag_execution_completed: Integer flag indicating if the command has been executed. */
/* int flag_log: Integer flag indicating if logging is enabled. */
/* pid_t current_pid: Process ID of the current process. */
/* struct termios terminal_setting: Termios structure for terminal settings. */
/* t_simple_cmds *commands: Pointer to the table of commands. */
/* t_environment *envair: Pointer to the list of environment variables. */

typedef struct s_tools
{
	char					*login_id;
	char					**paths;
	char					**pipes;
	char					**envp;
	char					*message;
	char					*content_hd;
	char					*shell_string;
	int						last_status;
	int						standard_input_fd;
	int						standard_output_fd;
	int						flag_envair;
	int						flag_pipe;
	int						flag_hd;
	int						requires_expansion;
	int						flag_ready_to_execute;
	int						flag_execution_completed;
	int						flag_log;
	pid_t					current_pid;
	struct termios			terminal_setting;
	t_simple_cmds			*commands;
	t_environment			*envair;
}							t_tools;


# define TRUE 1
# define FALSE 0
# define FAIL 999
# define IS_DIGIT 1
# define IS_ALPHA 2
# define IS_ALNUM 4
# define IS_PRINTABLE 8
# define FLAG '-'
# define SPACE ' '
# define SPACES " \t\n\v\r\f"
# define MALLOC_FAIL "memory allocation failed"
# define CMD_NOT_FND "command not found"
# define FORK_ERR_MSG "Fork error"
# define SHELL "minishell: "
# define SQUOTE 39
# define DQUOTE 34
# define PIPE 124
# define OPERATORS "|><"
# define REDIRECTIONS "><"
# define GIVE_PERM_WTH_RW 0644
# define FORK_ERROR "fork error"
# define PIPE_ER "wrong pipe"
# define ISDIR "is a directory"


extern int					glob_c;

// envair
char						*get_current_directory(t_tools *tools);
t_environment				*create_envairm(char *environment);
void						append_env_node(t_environment *head,
								t_environment *new);
int							setup_environment(t_tools *tools,
								char **environment);
void						display_env(t_tools *tools, char **arg_list);
char						**convert_env_list_to_array(t_environment *environment);
void						refresh_environment(t_tools *tools);

// init
void						*smalloc(size_t size);
void						initialize_tools(t_tools *tools);
t_simple_cmds				*create_empty_structure(void);
t_node						*dublicate_list(t_node *original_token);
t_node						*append_token(t_node *token_list,
								t_node *new_token);
t_node						*process_redirection(t_simple_cmds *cmd_tbl,
								t_node *token, t_node *new_token);
t_node						*install_redirection(t_simple_cmds *cmd_tbl,
								t_node *token);
t_node						*handle_new_token(t_simple_cmds *cmd_table,
								t_node *tok);
int							check_token_validity(t_simple_cmds *cmd_table,
								t_node *tok);
t_node						*set_arg_to_command(t_simple_cmds *cmd_tbl,
								t_node *token);
t_simple_cmds				*extend_command_list(t_simple_cmds *command_list,
								t_simple_cmds *new_command);
int							validate_command(t_simple_cmds *cmd_table,
								t_node *tok);
t_node						*set_command(t_simple_cmds *cmd_table, t_node *tok);
t_node						*set_commands(t_simple_cmds *cmd_tbl,
								t_node *token);
t_node						*process_tokens_run(t_simple_cmds *cmd,
								t_node *tokens);
t_simple_cmds				*create_command_table(t_simple_cmds *command_tables,
								t_node *token_list);
void						loop_minishell(t_tools *tools);
// lexer

char						*duplicate_string_range(const char *source,
								int begin, int finish);
int							inside_double_quotes(const char *input, int index);
int							contains_quote(const char *input);
int							find_error_sint(char c);
int							find_error_sing_newline(void);
int							is_character_symbol(char c);
int							check_redir(const char *str);
int							count_escaped_chars(const char *str, int pos);
int							is_previous_char_redirection(const char *str,
								int index);
int							has_invalid_pipe(const char *str);
int							audit_simbol_bad(const char *str);
int							run_lexer(t_tools *tools);

// util
void						*smalloc(size_t size);
int							audit_intput(t_tools *tols);
int							find_character_type(int c);
int							validate_and_advance(char *str, int *index);
int							are_strings_equal(const char *s1, const char *s2);
void						optimize_commands(t_simple_cmds *tbl);
int							advance_past_whitespace(const char *s, int idx);
int							check_space(char ch);
int							is_uppercase(char ch);
int							convert_to_lowercase(char *s, int limit);
int							safe_mall(char ***ptr, size_t size);
int							count_strings(char **array);
char						*duplicate_string(const char *str);
char						**arr_dabl(char **array);
char						*trim_string(char const *s1, char const *set);
int							is_only_space(char *str);
int							run_strncmp(t_tools *tools, char *str, int s);

// parser
t_class						identify_redirection_type(char *str, int start,
								int end);
int							is_redirection_token(t_node *tkn);
void						free_single_token(t_node *tkn);
void						clear_token_list(t_node *tkn);
int							strings_equal(char *str1, char *str2);
char						*delite_quotes(char *input);
void						manage_process_tokens(t_node *tokens);
void						handle_command_table(t_simple_cmds *table,
								t_tools *tools);
void						delit_quotes_command(t_simple_cmds *table,
								t_tools *tools);
void						delite_quotes_tables(t_simple_cmds *tables,
								t_tools *tools);
int							calculate_token_size(t_node *token);
int							count_token_stack(t_node *token);
int							is_special_ascii(char c);
int							is_visible_char(char c);
t_node						*create_token(char *content, t_class type);
t_node						*create_new_token(char *str, t_class type);
t_node						*insert_token(t_node *head, char *str,
								t_class type);
void						raise_error(char *msg);
char						**allocate_cmd_args(char *cmd, t_node *token,
								int *size);
char						**extract_cmd_args(char *cmd, t_node *token);
void						init_commands(t_simple_cmds *tables);
t_node						*split_to_tokens(char *str, t_node *token);
int							count_escaped_characters(const char *str,
								int last_index);
t_node						*plus_quote(char *input_str, int *current_index,
								int *previous_index, t_node *token);
char						*duplicate_range(char *str, int start, int end);
t_node						*plus_string(char *str, int *current_index,
								int *previous_index, t_node *token);
int							find_char_in_str(const char *src, int character);
t_class						determine_redirection_type(char *input, int begin,
								int finish);
t_node						*plus_redire(char *input_str, int *current_index,
								int *previous_index, t_node *token_list);
t_node						*process_flag_token(char *str, int *idx,
								int *prev_idx, t_node *token_list);
t_node						*plus_token_f(char *input_str, int *current_index,
								int *previous_index, t_node *token_list);
int							parse_shell_input(t_tools *tools);
int							audit_command(t_simple_cmds *tables);
char						*process_token_segment(char *str, int start,
								int end);
char						**split_pipe_segments(char *str, int start, int end,
								int index);
int							skip_quoted_sections(char *str, int index);
int							count_pipe_segment(char *str);

// expander
char						*allocate_memory(size_t size);
int							expand_variables(char **input, t_tools *context);
int							run_process_command(char *cmd, t_tools *context);
void						process_tokens(struct s_node *tokens,
								t_tools *context);
int							multiple_tables_expand(struct s_simple_cmds *commands,
								t_tools *context);
char						*copy_variable(const char *var_val);
char						*handle_parentheses(char *trimmed_input,
								t_tools *context);
char						*run_dollar_expansion(char *trimmed_input,
								t_tools *context);
char						*handle_special_var(char *input, t_tools *context);
char						*finalize_expansion(char *expanded_var);
char						*expand_dollar_signs(char *input, t_tools *context);
int							validate_quotes(char *s, int i, int *single_quote,
								int *double_quote);
int							fail_res_expand(char *input_str, int pos, int sq,
								int dq);
int							skip_expans(char *input_str, int pos);
bool						has_dollar_sign(char *str, t_tools *tools);
void						duplicate_dollar_segment(char **dst, char **s,
								int index);
void						isolate_dollar_segment(char **s, t_tools *tools,
								char **bef_doll, char **rest);
char						*copy_variable_content(char *content);

// executor
int							check_invalid_redirection(t_simple_cmds *command);
void						run_heredoc_commands(t_simple_cmds *first,
								t_simple_cmds *last, t_tools *tools);
int							check_commands_for_invalid_redir(t_simple_cmds *commands,
								t_tools *tools);
void						execute_pipeline_commands(t_simple_cmds *commands,
								t_tools *tools);
int							initialize_pipe(int *descriptor_fd, t_tools *tools);
int							create_child_process(t_tools *tools,
								int *descriptor_fd);
void						execute_parent_process(t_simple_cmds *commands,
								t_tools *tools, int *descriptor_fd);
int							run_pipeline_child(t_simple_cmds *commands,
								t_tools *tools);
int							validate_redirections(t_simple_cmds *commands);
void						spawn_and_execute(t_simple_cmds *commands,
								t_tools *tools, int *is_redirect);
int							finalize_pipeline_execution(t_simple_cmds *commands,
								t_tools *tools);
int							reset_file_descriptors(t_tools *tools);
int							run_builtin_and_close(t_simple_cmds *commands,
								t_tools *tools);
int							handle_execution(t_simple_cmds *commands,
								t_tools *tools);
int							simple_execute(t_simple_cmds *commands,
								t_tools *tools);
void						error_exit(t_tools *tools, const char *msg);
void						handle_directory_command(char *cmd, t_tools *tools);
void						execute_final_command(char *command_path,
								t_simple_cmds *commands, t_tools *tools);
int							finalize_command_path(char *command_path,
								t_simple_cmds *table, t_tools *tools);
int							execute_command(t_simple_cmds *commands,
								t_tools *tools);
void						handle_redirect(t_tools *tools,
								t_simple_cmds *commands);
void						process_heredocs_run(t_tools *tools,
								t_simple_cmds *commands);
void						run_command_execution(t_tools *tools,
								t_simple_cmds *commands);
void						cleanup(t_tools *tools);
void						run_exec_steps(t_tools *tools,
								t_simple_cmds *commands);
int							find_invalid_redirection(t_node *start);
int							invalid_redirection(t_tools *tools, t_node *token,
								t_simple_cmds *table);
void						perform_only_hd(t_tools *tools, t_simple_cmds *tabl,
								t_node *end);
int							check_valid_redirection(t_node *curr);
void						handle_redirection_error(t_tools *tools);
int							manage_redirections(t_tools *tools,
								t_simple_cmds *table);
int							duplicate_and_close(int first_descriptor_fd,
								int second_descriptor_fd);
void						report_error(t_tools *tools);
int							process_heredoc_input(int descriptor_fd);
int							redirect_io(t_class type, int descriptor_fd,
								t_tools *tools);
t_node						*locate_heredoc_token(t_node *redirs_list);
int							restore_standard_input(t_tools *tools);
int							create_heredoc_file(t_simple_cmds *table,
								t_tools *tools, t_node *token);
int							open_file_for_reading(char *file_name);
int							open_file_for_writing(char *file_name, int flags);
int							handle_file_open_error(char *file_name,
								t_tools *tools);
int							descriptor_open_file(t_class type, char *file_name,
								t_tools *tools);
char						*trim_quotes(char *str);
int							validate_expansion(char *str, t_tools *tools);
char						*get_break_string(char *str, t_tools *tools);
int							execute_heredocs(t_simple_cmds *cmd_tbl,
								t_tools *tools);
int							clean_filename(char *filename);
char						*concatenate_strings(char *str1, char *str2);
char						*generate_temp_filename(t_simple_cmds *table);
int							write_to_file(int descriptor_fd, char *stop_word,
								t_tools *tools);
char						*generate_temp_heredoc(t_simple_cmds *cmd_tbl,
								char *stop_word, t_tools *tools);
char						*build_path(const char *base, const char *cmd);
int							report_error_and_exit(t_tools *tools,
								const char *cmd, const char *msg, int code);
char						*find_command_path(t_tools *tools, const char *cmd);
int							check_file_access(char *path, t_tools *tools);
int							audit_comand_path(char *path, t_tools *tools);

// free

void						free_string_array(char **array);
void						release_env_list(t_environment *environment_list);
void						cleanup_command_tables(t_simple_cmds *cmd_tables);
void						free_resources(t_tools *tools);
void						cleanup_before_exit(t_tools *tools);
void						cleanup_child_proc(t_tools *tools);
void						release_builtin_resources(t_tools *tools);
void						child_process_finish(t_tools *tools);

// BUILTINS
int						run_builtin(t_tools *tools, char *cmd,
								char **arg_list);

// SIGNALS
void						signals(struct termios *mirror_termios);
void						signal_ctrl_c(void);
void						signals_parent(void);
void						signal_ctrl_c_child(void);
void						signal_ctrl_c_parent(void);
void						signal_ctrl_backslash(void);
void						handle_sigint(int sig_num);
void						signal_ctrl_backslash_child(void);
void						signals(struct termios *terminal_sign);
void						signals_child(struct termios *mirror_termios);
void						save_settings_and_remove_c(struct termios *mirror_termios);

#endif
