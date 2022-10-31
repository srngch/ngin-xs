CC = c++
INCLUDE_FLAGS = -I srcs -I includes
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic $(INCLUDE_FLAGS)
TARGET = webserv

SRCS_FILES = main.cpp utils.cpp \
			Nginxs.cpp Master.cpp Worker.cpp Request.cpp Response.cpp Cgi.cpp Uri.cpp \
			autoindex/AutoindexData.cpp autoindex/Autoindex.cpp \
			config/Config.cpp config/Block.cpp
SRCS = $(addprefix ./srcs/, $(SRCS_FILES))
OBJS = $(SRCS:.cpp=.o)


.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o $(TARGET)

.PHONY: clean
clean:
	rm -f $(OBJS)

.PHONY: fclean
fclean: clean
	rm -f $(TARGET)

.PHONY: re
re: fclean
	make all
