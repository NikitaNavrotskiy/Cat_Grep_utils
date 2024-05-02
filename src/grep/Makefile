CC=gcc
FLAGS=-std=c11 -Wall -Werror -Wextra

NAME=my_grep

all: $(NAME)

$(NAME):
		$(CC) $(FLAGS) main.c grep.c -o $(NAME)

clean:
		rm -f $(NAME) grep_out.txt out.txt

rebuild: clean all
