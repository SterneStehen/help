/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggeorgie <ggeorgie@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/17 03:02:31 by ggeorgie          #+#    #+#             */
/*   Updated: 2024/04/18 16:13:29 by ggeorgie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include <stdlib.h>	/* to use 'exit', 'free', 'malloc' */
# include <unistd.h>	/* to use 'read', 'write' */
# include <limits.h>	/* to use 'INT_MIN', 'INT_MAX' */						// i.e. between -2,147,483,648 and +2,147,483,647.
# include <stdio.h>		/* to use 'printf' */									// For debugging purposes only.

# ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS 0
# endif

# ifndef EXIT_FAILURE
#  define EXIT_FAILURE 1
# endif

struct s_stack
{
	struct s_element	*head;													// This is a pointer to stack's head.
	int					size;													// This is the number of elements in the stack.
};

struct s_element
{
	int					nbr;													// This is the value of the element.
	struct s_element	*next;													// This is a pointer to the next element in the stack.
};

struct s_element	*create_element(int nbr);
void				*ft_calloc(size_t count, size_t size);
struct s_stack		*fill_stack(int num_count, int **array, struct s_stack *stack_a);
char				*free_element(struct s_element *element);
char				*fn_free(char *variable);
char				*fn_free_ptr(char **ptr);
void				free_stack(struct s_stack *stack);
int					**index_array(int *num_count, int **array);
int					**initialize_2d_array(int num_count);
struct s_stack		*initialize_stack(void);
struct s_element	*insert_element(int nbr, struct s_stack *stack_a);
int					**make_index_array(int *num_count, char **argv);
int					not_sorted(struct s_stack *stack_a);
int					**parse_input(int num_count, char **input_str, int **array);
void				print_array(int num_count, int **array, int z);
void 				print_stack(struct s_stack *stack_a);
void				sort(struct s_stack *stack_a);
char				**ft_split(char const *s, char c, int *i_p);
#endif

//struct s_element	*fill_stack_a(int nbr);
//void				free_element(struct s_element *element);
//void				free_last(struct s_element *element);
//char				*free_struct(struct s_element *element);
