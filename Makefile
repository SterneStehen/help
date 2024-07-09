NAME = minishell

# Компилятор и флаги компиляции
CC = gcc
CFLAGS = -Wall 
# -Wextra -Werror

# Директории
SRCDIR = src
INCDIR = include
LIBFTDIR = $(INCDIR)/libft
BUILDDIR = build

# Поиск всех файлов .c в директории src и её подпапках
SRC = $(shell find $(SRCDIR) -type f -name "*.c")

# Объектные файлы
OBJ = $(SRC:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

# Заголовочные файлы
INC = -I$(INCDIR) -I$(LIBFTDIR)

# Библиотеки
LIBFT = $(LIBFTDIR)/libft.a

# Цель по умолчанию
all: $(NAME)

# Сборка исполняемого файла
$(NAME): $(OBJ) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJ) -L$(LIBFTDIR) -lft -lreadline -o $(NAME)

# Сборка объектных файлов
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Сборка библиотеки libft
$(LIBFT):
	make -C $(LIBFTDIR)

# Очистка объектных файлов и исполняемого файла
clean:
	rm -rf $(BUILDDIR)
	make -C $(LIBFTDIR) clean

fclean: clean
	rm -f $(NAME)
	make -C $(LIBFTDIR) fclean

# Пересборка
re: fclean all

# Не является файлом
.PHONY: all clean fclean re
