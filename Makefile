# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: smoreron <7353718@gmail.com>               +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/10 23:14:27 by ggeorgie          #+#    #+#              #
#    Updated: 2024/04/19 15:13:58 by smoreron         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = push_swap
CC = cc
#CFLAGS = -Wall -Wextra -Werror													# To be uncommented before submission
#CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address							# To be removed/commented out before submission
CFLAGS = -Wall -Wextra -Werror -g												# To be removed/commented out before submission

SOURCES = push_swap.c \
		array_utils.c \
		memory_control.c \
		sorting.c \
		ft_split.c \
		stack_utils.c

OBJECTS = $(SOURCES:.c=.o)

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)

clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

test: fclean all
#test: 
#	Replace the number after '-r' with the desired number of random numbers to generate
	$(eval ARG = $(shell jot -r 5 -2147483648 2147483647 | tr '\n' ' '))
	valgrind --leak-check=full ./push_swap $(ARG) | ./checker_original $(ARG)
#	valgrind --leak-check=full ./push_swap '$(ARG)' | ./checker_original '$(ARG)'
#	valgrind --leak-check=full ./push_swap "$(ARG)" | ./checker_original "$(ARG)"
#	./push_swap $(ARG) 															# In case there's additional printing for debugging
	@echo "Instructions:"
	@./push_swap $(ARG) | wc -l
# <https://manpages.ubuntu.com/manpages/noble/en/man1/jot.1.html>

#parse: fclean all
parse: 
#	./push_swap 1 2 3 4 5 6														# Already sorted stack
#	./push_swap 0 02 -0 +0002													# Stack with duplicates
#	./push_swap -1 2f s3 +4														# Stack with non-numerical values
#	./push_swap 0 -2147483649 2147483648 1										# Stack with too high and too low values
	./push_swap 6 5 4 3 2 1														# Reverse sorted stack
#	./push_swap 1 2 3 4 -00 5 -69  +9    -9 -1234567890 | ./checker_original 1 2 3 4 -00 5 -69  +9    -9 -1234567890
#	./push_swap ' 1 2 3 4 -00 5 -69  +9    -9 -1234567890 ' | ./checker_original '1 2 3 4 -00 5 -69  +9    -9 -1234567890 '
#	./push_swap 10 61 44 54 68 
#	valgrind --leak-check=full ./push_swap  1 2 3 4 -00 5 -69  +9    -9 -1234567890
#	valgrind --leak-check=full --track-origins=yes ./push_swap  1 2 3 4 5  6 -1234567890 7 8 9  9
#	valgrind --leak-check=full ./push_swap ' 1 2 3 4 5 6 -1234567890 7 8 9   '
#	@echo "Instructions:"
#	@./push_swap  1 2 3 4 -00 5 -69  +9    -9 -1234567890  | wc -l
