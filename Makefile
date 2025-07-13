CPP = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
SRC = src/main.cpp src/configs/Server.cpp src/configs/Config.cpp \
	src/utils/str_utils.cpp src/parsing/parser.cpp src/utils/arr_utils.cpp \
	src/configs/Size.cpp src/parsing/Token.cpp
OBJS = ${SRC:.cpp=.o}
NAME = bin
HEADERS = includes/Config.hpp includes/Server.hpp includes/Size.hpp \
			includes/Route.hpp includes/methods/HttpRequestMethod.hpp includes/methods/Get.hpp \
			includes/methods/Delete.hpp includes/methods/Post.hpp includes/utils.hpp includes/Token.hpp

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