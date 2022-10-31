CC = gcc
CFLAG = -Wall -Wextra -Werror
NAME = client

SRCS = client.c
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAG) $(OBJS) -o $(NAME)

.PHONY: clean
clean:
	rm -f $(OBJS)

.PHONY: fclean
fclean: clean
	rm -f $(NAME)

.PHONY: re
re: fclean all
