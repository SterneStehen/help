/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <7353718@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 13:44:55 by smoreron          #+#    #+#             */
/*   Updated: 2023/10/31 16:59:56 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_word_count(char const *str, char c)
{
	int	count;
	int	in_word;

	count = 0;
	in_word = 0;
	while (*str)
	{
		if (*str == c)
		{
			in_word = 0;
		}
		else if (!in_word)
		{
			in_word = 1;
			count++;
		}
		str++;
	}
	return (count);
}

static int	find_start(const char *s, char c, int i)
{
	while (s[i] == c && s[i] != '\0')
		i++;
	return (i);
}

static int	find_end(const char *s, char c, int i)
{
	while (s[i] != c && s[i] != '\0')
		i++;
	return (i);
}

static void	ft_clean(char ***arr, int index)
{
	while (index > 0)
		free((*arr)[--index]);
	free(*arr);
	*arr = NULL;
}

char	**ft_split(char const *s, char c)
{
	int		arr[4];
	char	**result;

	arr[0] = 0;
	arr[3] = 0;
	result = malloc(sizeof(char *) * (ft_word_count(s, c) + 1));
	if (!result || !s)
		return (NULL);
	while (s[arr[0]])
	{
		arr[1] = find_start(s, c, arr[0]);
		arr[2] = find_end(s, c, arr[1]);
		if (arr[2] > arr[1])
		{
			result[arr[3]] = ft_substr(s, arr[1], arr[2] - arr[1]);
			if (result[arr[3]] == NULL)
				return (ft_clean(&result, arr[3]), NULL);
			arr[3]++;
		}
		arr[0] = arr[2];
	}
	result[arr[3]] = NULL;
	return (result);
}

// arr[0] = i, arr[1] = start, arr[2] = end, arr[3] = j
// char **ft_split(char const *s, char c) {
// 	int i;
// 	int j; 
// 	int start;
// 	int end;
// 	char **result;
//     i = 0;
// 	j = 0;
// 	result = malloc(sizeof(char *) * (ft_word_count(s, c) + 1));
//     if (!result || !s) return NULL;
//     while (s[i]) {
//         start = find_start(s, c, i);
//         end = find_end(s, c, start);
//         if (end > start) {
//             result[j] = ft_substr(s, start, end - start);
//             if (result[j] == NULL) {
//                 while (j > 0) free(result[--j]);
//                 free(result);
//                 return NULL;
//             }
//             j++;
//         }
//         i = end;
//     }
//     result[j] = NULL;
//     return result;
// }
