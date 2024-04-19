/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sorting.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <7353718@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/14 19:03:08 by ggeorgie          #+#    #+#             */
/*   Updated: 2024/04/19 14:01:37 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	not_sorted(struct s_stack *stack_a)
{
	struct s_element	*current;

	current = (*(*stack_a).head).next;											// This must be the same as 	current = (*stack_a).head;	+	current = (*current).next;
	while (current != (*stack_a).head)
	{
//		printf("(*current).nbr: %i, (*(*current).next).nbr: %i\n", (*current).nbr, (*(*current).next).nbr);
		if ((*current).next && (*(*current).next).nbr != 0
			&& (*current).nbr > (*(*current).next).nbr)
			return (EXIT_FAILURE);
		current = (*current).next;
	}
	return (EXIT_SUCCESS);
}

void	reverse_rotate(struct s_stack *stack_a)
{
	struct s_element	*current;
	struct s_element	*rotated;

	if (!stack_a || (*stack_a).head == NULL
		|| (*(*stack_a).head).next == (*stack_a).head)							// Check that stack_a is not empty, has a valid sentinel node, which is no the only element.
	{
		exit(EXIT_FAILURE);														// Is this the correct way to handle this error, or we should go freeing all allocated memory?
//		return (write(2, "Error\n", 6), EXIT_FAILURE);							// This has to be changed, but function output is 'void'.
	}
	current = (*stack_a).head;
//	printf("(*current).nbr: %i\n", (*current).nbr);
	while (current && (*(*current).next).next != (*stack_a).head)
	{
		current = (*current).next;
//		printf("in loop (*current).nbr: %i\n", (*current).nbr);
	}
	rotated = (*current).next;
	// printf("(*rotated).nbr: %i\n", (*rotated).nbr);
	(*rotated).next = (*(*stack_a).head).next;
	// printf("(*rotated).next: %i\n", (*(*rotated).next).nbr);
	(*current).next = (*stack_a).head;
	// printf("(*(*current).next).nbr: %i\n", (*(*current).next).nbr);
	(*(*stack_a).head).next = rotated;
	// printf("(*(*(*stack_a).head).next).nbr: %i\n", (*(*(*stack_a).head).next).nbr);
	write(1, "rra\n", 4);
}

void	rotate(struct s_stack *stack_a)
{
	struct s_element	*current;
	struct s_element	*rotated;

	if (!stack_a || (*stack_a).head == NULL
		|| (*(*stack_a).head).next == (*stack_a).head)							// Check that stack_a is not empty, has a valid sentinel node, which is no the only element.
	{
		exit(EXIT_FAILURE);														// Is this the correct way to handle this error, or we should go freeing all allocated memory?
//		return (write(2, "Error\n", 6), EXIT_FAILURE);							// This has to be changed, but function output is 'void'.
	}
	current = (*stack_a).head;
//	printf("(*current).nbr: %i\n", (*current).nbr);
	rotated = (*current).next;
	// printf("(*rotated).nbr: %i\n", (*rotated).nbr);
	(*current).next = (*rotated).next;
	// printf("(*(*current).next).nbr: %i\n", (*(*current).next).nbr);
	(*rotated).next = (*stack_a).head;
	// printf("(*rotated).next: %i\n", (*(*rotated).next).nbr);
//	while ((*(*current).next).next != (*stack_a).head)
	while (current && (*current).next != (*stack_a).head)
	{
		current = (*current).next;
//		printf("in loop (*current).nbr: %i\n", (*current).nbr);
	}
	(*current).next = rotated;
	// printf("(*(*current).next).nbr: %i\n", (*(*current).next).nbr);
	write(1, "ra\n", 3);
}

void	push_2(struct s_stack *stack_1, struct s_stack *stack_2)
{
	struct s_element	*pushed;

	if (!stack_1 || (*stack_1).head == NULL
		|| (*(*stack_1).head).next == (*stack_1).head
		|| !stack_2 || (*stack_2).head == NULL)							// Check that stacks are not empty, have valid sentinel nodes, which are no their only element.
	{
		exit(EXIT_FAILURE);														// Is this the correct way to handle this error, or we should go freeing all allocated memory?
//		return (write(2, "Error\n", 6), EXIT_FAILURE);							// This has to be changed, but function output is 'void'.
	}
	pushed = (*(*stack_1).head).next;
//	printf("(*pushed).nbr: %i\n", (*pushed).nbr);
	(*(*stack_1).head).next = (*pushed).next;
//	printf("(*(*stack_1).head).next: %i\n", (*(*(*stack_1).head).next).nbr);
	(*pushed).next = (*(*stack_2).head).next;
//	printf("(*pushed).next: %i\n", (*(*pushed).next).nbr);
	(*(*stack_2).head).next = pushed;
//	printf("(*(*stack_2).head).next: %i\n", (*(*(*stack_2).head).next).nbr);
	(*stack_1).size--;
	(*stack_2).size++;
}

void	swap(struct s_stack *stack)
{
	struct s_element	*swapped_1;
	struct s_element	*swapped_2;

	if (!stack || (*stack).head == NULL
		|| (*(*stack).head).next == (*stack).head)								// Check that stack_a is not empty, has a valid sentinel node, which is no the only element.
	{
		exit(EXIT_FAILURE);														// Is this the correct way to handle this error, or we should go freeing all allocated memory?
//		return (write(2, "Error\n", 6), EXIT_FAILURE);							// This has to be changed, but function output is 'void'.
	}
	swapped_1 = (*(*stack).head).next;
	swapped_2 = (*swapped_1).next;
	(*(*stack).head).next = swapped_2;
	(*swapped_1).next = (*swapped_2).next;
	(*swapped_2).next = swapped_1;
}

void	sort_3(struct s_stack *stack_a)
{
	if (!stack_a || (*stack_a).head == NULL
		|| (*(*stack_a).head).next == (*stack_a).head)							// Check that stack_a is not empty, has a valid sentinel node, which is no the only element.
	{
		exit(EXIT_FAILURE);														// Is this the correct way to handle this error, or we should go freeing all allocated memory?
//		return (write(2, "Error\n", 6), EXIT_FAILURE);							// This has to be changed, but function output is 'void'.
	}
	if ((*(*(*(*stack_a).head).next).next).nbr > (*(*(*stack_a).head).next).nbr
		&& (*(*(*(*stack_a).head).next).next).nbr
		> (*(*(*(*(*stack_a).head).next).next).next).nbr)
	{
		reverse_rotate(stack_a);
	}
	if ((*(*(*stack_a).head).next).nbr > (*(*(*(*stack_a).head).next).next).nbr
		&& (*(*(*stack_a).head).next).nbr
		> (*(*(*(*(*stack_a).head).next).next).next).nbr)
	{
		rotate(stack_a);
	}	
	if ((*(*(*stack_a).head).next).nbr > (*(*(*(*stack_a).head).next).next).nbr)
	{
		swap(stack_a);
		write(1, "sa\n", 3);
	}	
}

void	sort_digit(struct s_stack *stack_a, struct s_stack *stack_b, int chkr)
{
	struct s_element	*current;
	int					i_a;
	int					stack_a_size;

	current = (*(*stack_a).head).next;
//	current = (*current).next;
	i_a = 1;
	stack_a_size = (*stack_a).size;
	while (i_a <= stack_a_size)
	{
		// printf("chkr: %i,\t number to be moved: %i\n", chkr, (*current).nbr);
		if ((*current).nbr & chkr)
		{
//			printf("The digit of %i-th number is 1 > rotate in stack_a\n", i_a);
			rotate(stack_a);
		}
		else
		{
//			printf("The digit of %i-th number is 0 > push to stack_b\n", i_a);
			push_2(stack_a, stack_b);
			write(1, "pb\n", 3);
		}
		current = (*(*stack_a).head).next;
		i_a++;
		// printf("\n");
		// printf("stack_a ");
		// print_stack(stack_a);
		// printf("stack_b ");
		// print_stack(stack_b);
	}
	//  printf("\nfunction to push stack_b[%d] to stack_a\n", (*stack_b).size);
	while ((*stack_b).size > 0)
	{
		push_2(stack_b, stack_a);
		write(1, "pa\n", 3);
	}
}

void	sort(struct s_stack *stack_a)
{
	struct s_stack		*stack_b;
	int					chkr;
	struct s_element	*current;
	int					stack_a_size;

	stack_b = initialize_stack();
	if (!stack_b)
	{
		printf("initialize_stack_b error\n");
//		return (write(2, "Error\n", 6), EXIT_FAILURE);							// This has to be changed, but function output is 'void'.
	}
	if ((*stack_a).size > 5)
	{
		chkr = 1;
		while (chkr < (*stack_a).size)
		{
			// printf("\n");
			// printf("stack_a ");
			// print_stack(stack_a);
			sort_digit(stack_a, stack_b, chkr);
			chkr = chkr * 2;
		}
		if (chkr == (*stack_a).size)
			rotate(stack_a);			
	}
	else
	{
		current = (*(*stack_a).head).next;
		stack_a_size = (*stack_a).size;
		while ((*stack_a).size > 3)
		{
			// printf("\n(*current).nbr: %i\n", (*current).nbr);
			if ((*current).nbr <= stack_a_size - 3)
			{
				current = (*current).next;
				push_2(stack_a, stack_b);
				write(1, "pb\n", 3);
			}
			else
			{
				rotate(stack_a);
				current = (*(*current).next).next;
			}
			// printf("stack_a ");
			// print_stack(stack_a);
		}
		sort_3(stack_a);
	}
	if ((*stack_b).size == 2 && (*(*(*stack_b).head).next).nbr
		< (*(*(*(*stack_b).head).next).next).nbr)
	{
		swap(stack_b);
		// printf("\n");
		// printf("stack_a ");
		// print_stack(stack_a);
		write(1, "sb\n", 3);
	}
	while ((*stack_b).size > 0)
	{
		push_2(stack_b, stack_a);
		// printf("\n");
		// printf("stack_a ");
		// print_stack(stack_a);
		write(1, "pa\n", 3);
	}
	//fn_free((char**)&stack_b);
	free_stack(stack_b);
}
