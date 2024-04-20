/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <7353718@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 20:04:03 by ggeorgie          #+#    #+#             */
/*   Updated: 2024/04/20 09:58:32 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void free_string_array(char **array)
{
    if (array == NULL)
	{
        return;
    }

    int i = 0;
    while (array[i] != NULL)
	{
        free(array[i]);
        array[i] = NULL;
        i++;
    }

    free(array);
    array = NULL;
}


void free_int_array(int **array) {
    if (array == NULL) {
        return;
    }

    int i = 0;
    while (array[i] != NULL) { 
        free(array[i]);
        array[i] = NULL;
        i++;
    }

    free(array);
    array = NULL;
}


void	print_array(int num_count, int **array, int row)
{
	int	i;

	if (row == 0)
		printf("original input:\t");
	if (row == 1)
		printf("   stack input:\t");
	i = 0;
	while (i < num_count - 1)
	{
//		printf("array[0][%d] = %d, array[1][%d] = %d\n", i, array[0][i], i, array[1][i]);
//		printf("array[0][%d] = %d\n", i, array[0][i]);
		printf("%d,\t", array[row][i]);
		i++;
	}
		printf("\n");
}

/**
 * 2D array initialization.
 * @param	int	cols : the number of columns in the array 
 * 			= num_count: the number of elements in the stack.
 * @param	int	rows : the number of rows in the array = 2:
 *			- 0-th - the original values of the elements in the stack,
 *			- 1-st - the index of the elements in the stack after sorting.
 * @return	int	**array : A double pointer to the array.
 */
int	**initialize_2d_array(int cols)
{
	int	**array;
	int	rows;
	int	i;

	rows = 2;
	array = malloc(rows * sizeof(int));
	if (!array)
	{
		printf("Memory allocation for array failed.\n");
//		return (write(2, "Error\n", 6), (int **)EXIT_FAILURE);
		return ((int **)EXIT_FAILURE);
	}
	i = 0;
	while (i < rows)
	{
		array[i] = malloc(cols * sizeof(int));									// 'cols + 1' doesn't seem to make any difference regarding 'Invalid write of size 8'.
		if (!array[i])
		{
//	Free each sub-array of array and the array itself							// This doesn't seem to make any difference.
			int i = 0;
			while (i < rows)
			{
				fn_free((char **)&array[i]);

				i++;
			}
			free(array);
//			fn_free_ptr((char **)array);										// This doesn't seem to make any difference.
			printf("Memory allocation for *array failed.\n");
//			return (write(2, "Error\n", 6), (int **)EXIT_FAILURE);
			return ((int **)EXIT_FAILURE);
		}
		i++;
	}
	return (array);
}

int	**fill_array1(int **array, int *array_copy, int *num_count)
{
	int	i;
	int	j;

	i = 0;
	while (i < *num_count - 1)
	{
//		printf("array[0][%i]: %i\t", i, array[0][i]);
		j = 0;
		while (j < *num_count)
		{
//			printf("\tarray_copy[%i]: %i", j, array_copy[j]);
			if (array[0][i] == array_copy[j])
			{
				array[1][i] = j + 1;											// The '+ 1' is necessary to avoid the '0' index, which conincides with the sentinel node value.
//				printf("\tarray[1][%i]: %i\n", i, array[1][i]);
				break;
			}
			j++;
		}
		i++;
	}
	return (array);
}

int	*bubble_sort(int *array_copy, int *num_count)
{
	int	i;
	int	j;
	int	temp;

	j = 0;
	while (j < *num_count - 1)
	{
		i = 0;
//		printf("\nloop to: %i\n", *num_count - j - 2);
		while (i < *num_count - j - 2)											// 'while (i < *num_count - 2)' is wrong.
		{
//			printf("j[%i]i[%i]: %i\t", j, i, array_copy[i]);
			if (array_copy[i] > array_copy[i + 1])
			{
				temp = array_copy[i];
				array_copy[i] = array_copy[i + 1];
				array_copy[i + 1] = temp;
			}
//			printf("\tis now: %i\t", array_copy[i]);
			i++;
//			printf("\tarray_copy[%i]: %i\n", i, array_copy[i]);
		}
		j++;
	}
	return (array_copy);
}

int	**index_array(int *num_count, int **array)
{
	int	*array_copy;
	int	i;

	array_copy = malloc(*num_count * sizeof(int));
	if (!array_copy)
	{
		printf("Memory allocation for array_copy\n");
		return (write(2, "Error\n", 6), (int **)EXIT_FAILURE);
	}
	i = 0;
	while (i < *num_count)
	{
		array_copy[i] = array[0][i];
		i++;
	}
	array_copy = bubble_sort(array_copy, num_count);
//	printf("array_copy\n");
//	print_array(*num_count, &array_copy, 1);
	array = fill_array1(array, array_copy, num_count);
//	print_array(array, 1);
	fn_free((char **)&array_copy);
	return (array);
}

/**
 * Takes the user input, parses it, and fills the array with the values.
 * @param	int		*num_count: number of arguments passed by the user.
 * @param	char	*argv[] : pointer to each argument passed by the user.
 * @param	int		**array : 2D array of integers and their indexes.
 * @param	char	**input_str : double pointer to parsed input,
 * 					which was provided in quotation marks.
 * @return	int	**array : double pointer to the array.
 */
int	**make_index_array(int *num_count, char **argv)
{
	int		**array;
	char	**input_str;

	if (*num_count == 2)
	{
		*num_count = *num_count - 2;
		input_str = ft_split(argv[1], ' ', num_count);
		if (!input_str)
		{
			fn_free_ptr(input_str);												// This doesn't seem to make any difference.
			printf("No output from split\n");
			return ((int **)EXIT_FAILURE);
		}
		array = initialize_2d_array(*num_count);
		if (!array)
		{
//	Free each sub-array of array and the array itself							// This doesn't seem to make any difference.
			int i = 0;
			while (i < 2)
			{
				fn_free((char **)&array[i]);
				i++;
			}
			free(array);
			// fn_free_ptr((char **)array);										// This doesn't seem to make any difference.
			fn_free_ptr(input_str);												// This doesn't seem to make any difference.
			printf("initialize_2d_array in quotation marks\n");
			return (write(2, "Error\n", 6), (int **)EXIT_FAILURE);
		}
		array = parse_input(*num_count + 1, input_str, array);
		free_string_array(input_str);				
		if (!array)
		{
//	Free each sub-array of array and the array itself							// This doesn't seem to make any difference.
			int i = 0;
			while (i < 2)
			{
				fn_free((char **)&array[i]);
				i++;
			}
			free(array);
//			fn_free_ptr((char **)array);										// This doesn't seem to make any difference.
			fn_free_ptr(input_str);												// This doesn't seem to make any difference.
			printf("parse_input in quotation marks\n");
			return (write(2, "Error\n", 6), (int **)EXIT_FAILURE);
//			return ((int **)EXIT_FAILURE);										// Why is this different from argc != 2?
		}
		*num_count = *num_count + 1;
	}
	else
	{
		array = initialize_2d_array(*num_count);								// Why was there a +/-1 here?
		if (!array)
		{
//	Free each sub-array of array and the array itself							// This doesn't seem to make any difference.
			int i = 0;
			while (i < 2)
			{
				fn_free((char **)&array[i]);
				i++;
			}
			free(array);
//			fn_free_ptr((char **)array);										// This doesn't seem to make any difference.
			printf("initialize_2d_array from argv\n");
			return (write(2, "Error\n", 6), (int **)EXIT_FAILURE);
		}
		array = parse_input(*num_count, argv, array);
		if (!array)
		{
//	Free each sub-array of array and the array itself							// This doesn't seem to make any difference.
			int i = 0;
			while (i < 2)
			{
				fn_free((char **)&array[i]);
				//fn_free((char **)&array[i]);
				i++;
			}
			free(array);
//			fn_free_ptr((char **)array);										// This doesn't seem to make any difference.
			printf("parse_input from argv\n");
			return (write(2, "Error\n", 6), (int **)EXIT_FAILURE);
		}
	}
	return (array);
}
