/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <7353718@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 16:08:16 by ggeorgie          #+#    #+#             */
/*   Updated: 2024/04/19 15:08:36 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*	Sort the linked list using an auxiliary linked list.
	Print the operations for sorting the linked list.

Create an 'argc-1 * 2' array of integers.
Fill the first half of the array with 'argv'.
Check for duplicates and invalid arguments (< INT_MIN, > INT_MAX, non-digits).
Full the second half of the array with the indexes of the first half.
Make a linked list of the second half of the array.

<https://en.wikipedia.org/wiki/Stack_(abstract_data_type)#Linked_list>
When a stack is implemented as a singly linked list,
the 'top' of the stack is the 'head' of the list, and
the bottom of the stack is the tail of the list.
Overflow is not possible in this implementation.

Write code to execute the permited operations:
sa	(swap a)			:	Swap the first 2 elements at the top of stack 'a'.
							Do nothing if there is only one or no elements.
sb	(swap b)			:	Swap the first 2 elements at the top of stack 'b'.
							Do nothing if there is only one or no elements.
ss 						:	'sa' and 'sb' at the same time.

pa	(push a)			:	Take the first element at the top of 'b' and put
							it at the top of 'a'. Do nothing if 'b' is empty.
pb 	(push b)			:	Take the first element at the top of 'a' and put
							it at the top of 'b'. Do nothing if 'a' is empty.

ra 	(rotate a)			:	The first element of stack 'a' becomes the last one.
							All elements of stack 'a' shift up by 1. 
rb 	(rotate b)			:	The first element of stack 'b' becomes the last one.
							All elements of stack 'b' shift up by 1.
rr 						:	'ra' and 'rb' at the same time.

rra	(reverse rotate a)	:	The last element of stack 'a' becomes the first one.
							All elements of stack 'a' shift down by 1.
rrb	(reverse rotate b)	:	The last element of stack 'b' becomes the first one.
							All elements of stack 'b' shift down by 1.
rrr						:	'rra' and 'rrb' at the same time.

Examples:
$>./push_swap 2 1 3 5 4
stack_a:			stack_a:		stack_a:		stack_b:		stack_a:
original input:		sa:				pb, pb:			pb, pb:			end result:
sentinel node		sentinel node	sentinel node	sentinel node	sentinel
2					1				3				2				1
1					2				5				1				2
3					3				4								3
5					5												4
4					4												5

1	1	2	2	3	3
2	3	3	1	1	2
3	2	1	3	2	1
	rra	rra	sa	ra	ra
	sa				sa

Memory allocations:
main (**array) > make_index_array (**array) > malloc
 * Anytime there is 'exit' or 'return (EXIT_FAILURE)' in the code, these above should be 'free'd before it.

*/

#include "push_swap.h"

/**
 * Checks if there are duplicate elements in the array.
 */
int	duplicates_check(int num_count, int *array[0])
{
	int	i;
	int	j;

//	printf("Checking for duplicates.\n");
	if (!array)
	{
		// print error, free allocated memory, of let calling function handle this.
		printf("!inputs @ duplicates_check\n");
		return EXIT_FAILURE;
	}
	i = 0;
//	while (array[0][i + 1])
	while (i < num_count - 2)
	{
		j = i + 1;
		while (j < num_count - 1)
		{
//			printf("array[0][%d] = %d, array[0][%d] = %d\n", i, array[0][i], j, array[0][j]);
			if (array[0][i] == array[0][j])
			{
				printf("Hello, duplicate!\n");
				// print error, free allocated memory, of let calling function handle this.
				return (EXIT_FAILURE);
			}
			j++;
//			printf("i = %d, array[0][%d] = %d.\n", i, j, array[0][j]);
		}
		i++;
	}
//	printf("No duplicates.\n");
	return (EXIT_SUCCESS);
}	

int	push_swap_atoi(char input_str[], int *i, int sign)
{
	signed long long	nbr;

	nbr = 0;
	while (input_str[*i] != '\0')
	{
//		printf("input_str[%i] = %c\n", *i, input_str[*i]);
		if (input_str[*i] < '0' || input_str[*i] > '9'
		|| ((nbr * 10 + (input_str[*i] - '0')) * sign) < INT_MIN
		|| ((nbr * 10 + (input_str[*i] - '0')) * sign) > INT_MAX)
		{
//			fn_free_ptr((char **)array);										// Causes 'pointer being freed was not allocated' error.
			printf("Non-numerical symbol or out of integer limits.\n");
//			fn_free_ptr(input_str);
			write(2, "Error\n", 6);
			exit (EXIT_FAILURE);												// Maybe we should go freeing all allocated memory too?
//			return (NULL);														// This is not correct way to handle this error - it gives segmentation fault.
		}
//			free_exit(stack_a, integer);										// Think how to handle this.
		else
			nbr = nbr * 10 + (input_str[*i] - '0');
		(*i)++;
	}
//	printf("nbr = %lld\n", nbr);
	return (nbr * sign);
}

int	ascii_sign(char input_str, int *i)
{
	int	sign;

	sign = 1;
	if (input_str == '+' || input_str == '-')
	{
		if (input_str == '-')
			sign = -1;
		(*i)++;
	}
//	printf("sign = %d\n", sign);
	return (sign);
}

/**
 * Converts the input ASCII arguments to integers and checks if they are valid.
 * Assumed there are no white spaces in argv.
 * @param	int		argc : Number of arguments passed to the function.
 * @param	char	**input_str : Pointer to strings passed to the fn.
 * @param	int 	**array : Pointer to the 2D array to be created.
 * @param	int		i_nbr : Index of the current number/substring.
 * @param	int		i_char : Index of current character of current substring.
 * @param	int		sign : Sign of the current number/substring.
 * @param	int		nbr : The current number/substring.
 * @return	A pointer to the newly created array. Throws errors in case of
 * non-numerical symbols in argv (except +/-), or integers exceeding limits.
 */
int	**parse_input(int num_count, char **input_str, int **array)
{
	int	i_nbr;
	int	i_char;
	int	sign;
	int	nbr;

	i_nbr = 1;
	while (i_nbr < num_count)														// The 0-th argument is the program name, so it is skipped, but the loop should also go though argv[num_count - 1].
	{
//		printf("\ninput_str[%i][%i] = %c, num_count = %i\n", i_nbr, i_char, input_str[i_nbr][i_char], num_count);
		i_char = 0;
		nbr = 0;
//		printf("\nNext number:\n");
		sign = ascii_sign(input_str[i_nbr][i_char], &i_char);
//		printf("sign = %d, i_char = %i\n", sign, i_char);
		nbr = push_swap_atoi(input_str[i_nbr], &i_char, sign);
//		printf("nbr = %d with %i digits.\n", nbr, i_char);
//		printf("nbr = %d.\n", nbr);
// 		if (!nbr)
// 		{
// //			fn_free_ptr((char **)array);										// Causes 'pointer being freed was not allocated' error.
// 			printf("Non-numerical symbol or out of integer limits.\n");
// //			fn_free_ptr(input_str);
// 			write(2, "Error\n", 6);
// 			exit (EXIT_FAILURE);												// Maybe we should go freeing all allocated memory too?
// //			return (NULL);														// This is not correct way to handle this error - it gives segmentation fault.
// 		}
		array[0][i_nbr - 1] = nbr;
		i_nbr++;
//		printf("input_str[i_nbr = %i][i_char = %i] = %c, num_count = %i\n", i_nbr, i_char, input_str[i_nbr][i_char], num_count);
//		printf("(*stack_a).size : %d\n", (*stack_a).size);
	}
	return (array);
}

/**
 * Sorts a stack of integers with the smallest number being at the top, 
 * using a minimum number of operations.
 * @param	int				argc : number of arguments passed by the user.
 * @param	char			*argv[] : ptr to each argument passed by the user.
 * @param	int				**array : 2D array of integers and their indexes.
 * @param	struct s_stack	*stack_a : Pointer to the stack to be created.
 * @param	int				num_count : number of arguments.
 * @return	0 if the stack is sorted, 1 if the stack is not sorted.
 */
int	main(int argc, char **argv)
{
	int				**array;
	struct s_stack	*stack_a;
	int				num_count;
	int				i;

	if (argc < 2)
	{
		printf("Too few arguments.\n");
		return (EXIT_FAILURE);
	}
	else
	{
		num_count = argc;
		array = make_index_array(&num_count, argv);
		if (duplicates_check(num_count, array))
		{
//	Free each sub-array of array and the array itself							// This leads to no leaks in case of duplicates.
			i = 0;
			while (i < 2)														// Number of rows in the array.
			{
				fn_free((char **)&array[i]);
				i++;
			}
			free(array);
//			fn_free_ptr((char **)array);										// Causes 'pointer being freed was not allocated' error.
			return (write(2, "Error\n", 6), EXIT_FAILURE);
		}
		array = index_array(&num_count, array);
		// printf("\nArray:\n");
		// print_array(num_count, array, 0);
		// print_array(num_count, array, 1);
		stack_a = initialize_stack();
		if (!stack_a)
		{
			printf("initialize_stack_a\n");
			return (write(2, "Error\n", 6), EXIT_FAILURE);
		}
		stack_a = fill_stack(num_count, array, stack_a);
		// printf("(*stack_a).size = %d\n", (*stack_a).size);
		if(argc == 2)
			free_int_array(array);
		while (not_sorted(stack_a))
		{
			sort(stack_a);
		}
		free_stack(stack_a);
	}
//	Free each sub-array of array and the array itself
	i = 0;
	while (i < 2)																// Number of rows in the array.
	{
		fn_free((char **)&array[i]);
		i++;
	}
	//free_int_array(array);
	//fn_free((char**)&array);
//	fn_free_ptr((char **)array);
	free(array);											// Causes 'pointer being freed was not allocated' error.
	return (EXIT_SUCCESS);
}
