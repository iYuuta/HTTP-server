CPP = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
SRC = src/main.cpp src/AServer.cpp
OBJS = ${SRC:.cpp=.o}
NAME = bin
HEADERS = includes/Config.hpp includes/Server.hpp includes/AServer.hpp

${NAME}: ${OBJS}
	${CPP} ${CFLAGS} ${OBJS} -o ${NAME}

all: ${NAME}

%.o: src/%.cpp ${HEADERS}
	${CPP} ${CFLAGS} -c $<

clean:
	rm -f ${OBJS}

fclean: clean
	rm -f ${NAME}

re: fclean all