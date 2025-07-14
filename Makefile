CPP = c++
CFLAGS =-std=c++98  # -Wall -Wextra -Werror -g -fsanitize=address
SRC = src/main.cpp src/configs/Server.cpp src/configs/Config.cpp \
	src/utils/strUtils.cpp src/parsing/parser.cpp src/parsing/optionsValidator.cpp \
	src/configs/Size.cpp src/parsing/Token.cpp src/parsing/optionsParser.cpp
OBJS = ${SRC:.cpp=.o}
NAME = bin
HEADERS = includes/Config.hpp includes/Server.hpp includes/Size.hpp \
			includes/Route.hpp includes/methods/HttpRequestMethod.hpp includes/methods/Get.hpp \
			includes/methods/Delete.hpp includes/methods/Post.hpp includes/utils.hpp includes/Token.hpp

${NAME}: ${OBJS}
	${CPP} ${CFLAGS} ${OBJS} -o ${NAME}

all: ${NAME}

%.o: %.cpp ${HEADERS}
	${CPP} ${CFLAGS} -c $< -o $@

clean:
	rm -f ${OBJS}

fclean: clean
	rm -f ${NAME}

re: fclean all