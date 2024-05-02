CC=gcc
FLAGS=-std=c11 -Wall -Werror -Wextra

NAME=my_cat


all: $(NAME)


$(NAME):
		$(CC) -std=c11 -Wall -Werror -Wextra main.c -o $(NAME)

clean:
		rm -f my_out.txt cat_out.txt out.txt $(NAME)


rebuild: clean all
