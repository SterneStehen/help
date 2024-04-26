/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <7353718@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/06 15:44:21 by ggeorgie          #+#    #+#             */
/*   Updated: 2024/04/26 03:29:38 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* int_var[2] may be removed and all other variables shifted */

#include "push_swap.h"

/**
 * @brief	Counts the number of characters in 'str' string.
 * @return	The number of characters in 'str'.
 */
size_t	ft_strlen(const char *str)												// To be moved to another file, because there are too many functions on this one.
{
	size_t	i;

	i = 0;
	if (!str)
		return (0);
	while (str[i] != '\0')
		i++;
	return (i);
}

// /**
//  * @brief	Replaces the delimiter ’c’ with '\0'.
//  * @param	char	*s : the string being split.
//  * @param	char	*str : a copy of 's'.
//  * @param	int		int_var[0] : length of 's' and 'str'.
//  * @param	char	c : delimiter character.
//  * @return	A pointer to 'str' - the amended version of 's'. 
//  */
// char	*c_to_0(char const *s, char *str, int *int_var, char c)
// {
// 	int	i;

// 	i = 0;
// 	while (i < int_var[0])
// 	{
// 		if (s[i] == c)
// 			str[i] = '\0';
// 		else
// 			str[i] = s[i];
// 		i++;
// 	}
// 	str[i] = '\0';
// 	return (str);
// }

// /**
//  * @brief	Count the number of newly appeared sub-strings.
//  * @param	char	*str : a copy of 's' string being split.
//  * @param	int		int_var[0] : length of 's' and 'str'.
// // * @param	int		int_var[2] : total number of sub-strings.
//  * @return	The number of sub-strings/words. 
//  */
// int	sub_string_count(char *str, int *int_var, int *i_p)
// {
// 	int	i;

// 	i = 0;
// //	int_var[2] = 0;
// 	while (i < int_var[0])
// 	{
// //		printf("sub_string [%i]: i_p = %c\n", *i_p, str[i]);
// 		if (str[i] != '\0' && str[i + 1] == '\0')
// 			(*i_p)++;
// 		i++;
// 	}
// //	(*i_p)--;																	// No need to account for the last increment, because i_p = 0 gets omitted.
// 	return (*i_p);
// }

// /**
//  * @brief	Saves sub-strings into words, one at a time.
//  * @param	char	*str : a copy of 's' string being split.
//  * @param	int		int_var[1] : length of sub-string.
//  * @param	int		int_var[3] : index of sub-string.
//  * @param	int		int_var[4] : position within sub-string.
//  * @param	int		int_var[5] : position within 'str'.
//  * @param	char	**pointers : string of pointers to sub-strings.
//  * @return	A pointer to the new sub-string/word. If unsuccessful at any point,
//  * 			free all previously allocated spaces.
//  */
// char	*fill_word(char *str, int *int_var, char **pointers)
// {
// 	char	*word;

// 	int_var[4] = 0;
// //	printf("fill_word[%i]: str = '%s', int_var[1] = %i\n", int_var[3], str, int_var[1]);
// 	word = malloc(sizeof(char) * (int_var[1] + 1));
// 	if (word == NULL)
// 		fn_free_ptr(pointers);
// 	while (int_var[4] < int_var[1])
// 	{
// 		word[int_var[4]] = str[int_var[5] + int_var[4]];
// 		int_var[4]++;
// 	}
// 	word[int_var[4]] = '\0';
// 	return (word);
// }

// /**
//  * @brief	Assembles the '*pointers' string of pointers.
//  * @param	char	*str : a copy of 's' string being split.
//  * @param	int		int_var[1] : length of sub-string.
// // * @param	int		int_var[2] : total number of sub-strings.
//  * @param	int		int_var[3] : index of sub-string.
//  * @param	int		int_var[5] : position within 'str'.
//  * @param	char	**pointers : string of pointers to sub-strings.
//  * @return	A string of pointers to the new strings resulting from the split.
//  * 			If an allocation fails, free 'pointers' and return NULL.
//  */
// char	**fill_pointers(char *str, int *int_var, char **pointers, int *i_p)
// {
// 	char	*word;

// 	int_var[3] = 0;
// 	int_var[5] = 0;
// 	int_var[1] = 1;
// 	word = fill_word(" ", int_var, pointers);									// To account for argv[0] = file_name.
// 	pointers[0] = word;
// 	int_var[3]++;
// 	while (int_var[3] <= *i_p)
// 	{
// 		while (str[int_var[5]] == '\0')
// 			int_var[5]++;
// 		int_var[1] = ft_strlen(&str[int_var[5]]);
// 		word = fill_word(str, int_var, pointers);
// 		if (word == NULL)
// 			fn_free_ptr(pointers);
// 		pointers[int_var[3]] = word;
// 		int_var[5] = int_var[5] + int_var[1];
// 		int_var[3]++;
// 	}
// 	//pointers[*i_p+1] = NULL; 
// //	printf("fill_pointers: pointers[%d] = %s\n", int_var[3], pointers[int_var[3]]);
// //	pointers[int_var[3]] = NULL;
// //	pointers[int_var[3]] = (char *)'\0';
// 	return (pointers);
// }

// /**
//  * @brief	Splits string ’s’ into an array of sub-strings using ’c’ delimiter.
//  * @param	char	*s : string to be split.
//  * @param	char	c : delimiter character.
//  * @param	int		i_p : pointer to number of 'sub-strings'/'pointers'.
//  * @param	int		int_var : array for 6 counters.
//  * @param	int		int_var[0] : length of 's' and 'str'.
// // * @param	int		int_var[2] : total number of sub-strings.
//  * @return	A string of pointers to the new strings resulting from the split.
//  * 			If an allocation fails, free 'str'/'pointers' and return NULL.
//  */
// char	**ft_split(char const *s, char c, int *i_p)
// {
// 	char	*str;
// 	char	**pointers;
// 	int		int_var[6];

// 	if (s == NULL)
// 		return (NULL);
// 	int_var[0] = ft_strlen(s);
// //	printf("string length = %d, number of 'sub-strings'/'pointers' = %i\n", int_var[0], *i_p);
// 	str = malloc(sizeof(char) * (int_var[0] + 1));
// 	if (str != NULL)
// 		str = c_to_0(s, str, int_var, c);
// 	else
// 	{
// 		fn_free(&str);
// 		return (NULL);
// 	}
// 	(*i_p) = (*i_p) + sub_string_count(str, int_var, i_p);
// //	printf("string length = %d, number of 'sub-strings'/'pointers' = %i\n", int_var[0], *i_p);
// 	pointers = malloc(sizeof(char *) * ((*i_p) + 1));							// +1 for the NULL pointer at the end.
// 	if (pointers != NULL)
// 		pointers = fill_pointers(str, int_var, pointers, i_p);
// 	else
// 		fn_free_ptr(pointers);
// //		fn_free(*pointers);
// 	fn_free(&str);
// //	fn_free_ptr(pointers);														// This doesn't look like a good idea, but how to prevent leaks?
// 	int	i = 0;
// 	while (i < int_var[3])
// //	{
// //		printf("ft_split: pointer[%d] = %s\n", i, pointers[i]);
// 		i++;
// //	}
// 	return (pointers);
// }
