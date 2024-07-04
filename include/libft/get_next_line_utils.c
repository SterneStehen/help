/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 14:44:19 by ggeorgie          #+#    #+#             */
/*   Updated: 2024/07/03 22:20:21 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include "libft.h"

/* Count and return the number of characters in a string preceding the '\0'. */
// int	ft_strlen(const char *str)
// {
// 	int	i;

// 	i = 0;
// 	if (!str)
// 		return (0);
// 	while (str[i] != '\0')
// 		i++;
// 	return (i);
// }

/* Allocate contiguous memory with size 'count' * 'size' bytes.
 * (Which is to be used by 'count' (number of) objects of 'size' bytes each.)
 * Fill the allocated memory with '0's.
 * Return a pointer to the allocated memory. */
// void	*ft_calloc(int count, int size)
// {
// 	int		total_size;
// 	char	*string;
// 	int		i;

// 	i = 0;
// 	total_size = count * size;
// 	string = malloc(total_size);
// 	if (string)
// 	{
// 		while (i < total_size)
// 		{
// 			string[i] = '\0';
// 			i++;
// 		}
// 	}
// 	return (string);
// }

/* Allocate in heap memory a slot sufficient to fit the concatenation of
 * ’s1’ and ’s2’. Fill it up.
 * Return the new string or NULL if the allocation fails. */
char	*ft_gnl_strjoin(char *s1, char *s2)
{
	char	*str;
	int		i;

	if (!s1 || !s2)
		return (NULL);
	str = ft_calloc(ft_strlen(s1) + ft_strlen(s2) + 1, sizeof(char));
	if (str)
	{
		i = 0;
		while (s1[i])
		{
			str[i] = s1[i];
			i++;
		}
		i = 0;
		while (s2[i])
		{
			str[ft_strlen(s1) + i] = s2[i];
			i++;
		}
		str[ft_strlen(s1) + ft_strlen(s2)] = '\0';
	}
	return (str);
}

/* Copy up to 'dstsize'-1 characters from 'src' string to 'dst' string,
 * add '\0' to the end if 'dstsize' is not 0.
 * Calculate and return the total length of 'src' string.
 */
// int	ft_strlcpy(char *dst, const char *src, int dstsize)
// {
// 	int	len_src;
// 	int	i;

// 	len_src = ft_strlen(src);
// 	if (dstsize < 1)
// 		return (len_src);
// 	i = 0;
// 	while (src[i] && i < (dstsize - 1))
// 	{
// 		dst[i] = src[i];
// 		i++;
// 	}
// 	dst[i] = '\0';
// 	return (len_src);
// }

/* Return a pointer of the first occurrence of 'c'
 * in the string pointed to by 's', or, if not found, NULL. */
char	*ft_strchr(const char *s, int c)
{
	int		i;
	int		len_str;
	char	*output;

	i = 0;
	len_str = ft_strlen(s);
	if (!s)
		return (NULL);
	while (i < len_str && s[i] != (unsigned char) c)
		i++;
	if (s[i] == (unsigned char) c)
		output = (char *)&s[i];
	else
		output = NULL;
	return (output);
}
