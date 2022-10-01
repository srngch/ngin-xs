CC = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic
INCLUDE_FLAGS = -I srcs
TARGET = webserv

SRCS_FILES = main.cpp Master.cpp Worker.cpp Request.cpp Response.cpp utils.cpp
SRCS = $(addprefix ./srcs/, $(SRCS_FILES))
OBJS = $(SRCS:.cpp=.o)


.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) $(INCLUDE_FLAGS) $(OBJS) -o $(TARGET) 

.PHONY: clean
clean:
	rm -f $(OBJS)

.PHONY: fclean
fclean: clean
	rm -f $(TARGET)

.PHONY: re
re: fclean
	make all
