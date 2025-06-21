CPP = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
SRC = src/main.cpp src/configs/Server.cpp
OBJS = ${SRC:.cpp=.o}
NAME = bin
HEADERS = includes/Config.hpp includes/Server.hpp includes/Server.hpp

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