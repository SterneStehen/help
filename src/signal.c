/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 22:50:00 by smoreron          #+#    #+#             */
/*   Updated: 2024/07/09 18:17:37 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int		g_global_flag = 0;

/* Ignores the specified signal.
 * This function configures the signal handler to ignore the given signal.
 * This is useful to prevent the default handling of certain signals,
 * which might be disruptive to the program's flow. */
void	ignore_signal(int signum)
{
	struct sigaction	ctrl_back_slash;

	ctrl_back_slash.sa_handler = SIG_IGN;
	ctrl_back_slash.sa_flags = SA_RESTART;
	sigemptyset(&ctrl_back_slash.sa_mask);
	sigaction(signum, &ctrl_back_slash, NULL);
}

/* Signal handler for SIGINT in the parent process.

	* This handler writes "^C\n" to the standard
	output and resets the readline buffer
	* to handle an interrupt signal (Ctrl+C)
	gracefully without exiting the program. */
void	sigint_handler_parent(int sig_num)
{
	if (sig_num == SIGINT)
	{
		write(1, "^C\n", 3);
		rl_on_new_line();
		rl_replace_line("", 0);
	}
}

/* Signal handler for SIGINT in the child process.
 * This handler sets the global flag to 1 and resets the readline buffer
 * to handle an interrupt signal (Ctrl+C) in the child process. */
void	sigint_handler_child(int sig_num)
{
	if (sig_num == SIGINT)
	{
		g_global_flag = 1;
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

/* Sets up a signal handler for SIGINT using the specified handler function.

	* This function configures the signal handler
	for SIGINT to use the given handler function,
	* allowing for custom behavior when the
	interrupt signal (Ctrl+C) is received. */
void	setup_sigint_handler(void (*handler)(int))
{
	struct sigaction	ctrl_c;

	ctrl_c.sa_handler = handler;
	ctrl_c.sa_flags = SA_RESTART;
	sigemptyset(&ctrl_c.sa_mask);
	sigaction(SIGINT, &ctrl_c, NULL);
}

/* Configures the terminal settings to disable the ECHOCTL flag.

	* This function modifies the terminal
	settings to prevent the echoing of control characters,
 * such as Ctrl+C, to make the terminal interaction cleaner. */
void	configure_termios(struct termios *term_settings)
{
	struct termios	new_settings;

	tcgetattr(1, term_settings);
	tcgetattr(1, &new_settings);
	new_settings.c_lflag &= ~ECHOCTL;
	tcsetattr(1, TCSAFLUSH, &new_settings);
}

/* Configures signals for the child process.
 * Restores the terminal settings, ignores SIGQUIT,
	and sets the SIGINT handler to default.

	* This ensures that the child process behaves correctly
	with respect to terminal signals. */
void	config_signals_child(struct termios *term_settings)
{
	tcsetattr(1, TCSAFLUSH, term_settings);
	ignore_signal(SIGQUIT);
	setup_sigint_handler(SIG_DFL);
}

/* Configures signals for the parent process.
 * Sets up the SIGINT handler and ignores SIGQUIT.

	* This ensures that the parent process handles
	signals appropriately during its execution. */
void	configure_signals_parent(void)
{
	setup_sigint_handler(sigint_handler_parent);
	ignore_signal(SIGQUIT);
}

/* Configures signals for the child process including terminal settings.
 * Sets up the SIGINT handler, ignores SIGQUIT, and resets the global flag.
 * This function is used to prepare the child process for signal handling. */
void	configure_signals(struct termios *term_settings)
{
	g_global_flag = 0;
	configure_termios(term_settings);
	ignore_signal(SIGQUIT);
	setup_sigint_handler(sigint_handler_child);
}
