/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 14:45:24 by ggeorgie          #+#    #+#             */
/*   Updated: 2024/07/03 22:22:12 by smoreron         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE
# define GET_NEXT_LINE
# include <limits.h>	/* to limit the values of various variable types */
# include <stdlib.h>	/* to use 'malloc', 'free' */
# include <unistd.h>	/* to use 'read' */
//# include <stdio.h>		/* to use 'print' */

# define BUFFER_SIZE 1000

// get_next_line_bonus.c
char	*fn_free(char **variable);
char	*process_carry_over(char *carry_over);
char	*read_buffer(int fd, char *carry_over);
char	*get_next_line(int fd);

// get_next_line_utils_bonus.c
//int		ft_strlen(const char *str);
//void	*ft_calloc(int count, int size);
//char	*ft_strjoin(char *s1, char *s2);
//int		ft_strlcpy(char *dst, const char *src, int dstsize);
//char	*ft_gnl_strjoin(const char *s, int c);
char	*ft_gnl_strjoin(char *s1, char *s2);

#endif
