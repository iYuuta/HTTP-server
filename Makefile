CPP = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
SRC = src/main.cpp src/configs/Server.cpp src/configs/Config.cpp src/utils/ioutils.cpp \
	src/utils/strutils.cpp src/configs/parser.cpp src/utils/arrutils.cpp
OBJS = ${SRC:.cpp=.o}
NAME = bin
HEADERS = includes/Config.hpp includes/Server.hpp includes/Size.hpp \
			includes/Route.hpp includes/methods/HttpRequestMethod.hpp includes/methods/Get.hpp \
			includes/methods/Delete.hpp includes/methods/Post.hpp includes/utils.hpp

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