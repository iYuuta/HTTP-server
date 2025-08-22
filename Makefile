CPP = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
SRC = src/main.cpp src/server/Server.cpp src/configs/Config.cpp \
	src/utils/utils.cpp src/parsing/parser.cpp src/parsing/optionsValidator.cpp \
	src/configs/Size.cpp src/parsing/Token.cpp src/parsing/optionsParser.cpp src/configs/Location.cpp \
	src/parsing/optionsParser2.cpp  src/client/Client.cpp \
	src/client/request/Request.cpp src/client/response/Response.cpp \
	src/client/validRequest.cpp src/server/HttpServer.cpp src/parsing/validator.cpp
	
OBJS = ${SRC:.cpp=.o}
NAME = webserv
HEADERS = includes/Client.hpp \
			includes/Config.hpp \
			includes/HttpRequestMethod.hpp \
			includes/HttpServer.hpp \
			includes/Location.hpp \
			includes/Request.hpp \
			includes/Response.hpp \
			includes/Server.hpp \
			includes/Size.hpp \
			includes/Token.hpp \
			includes/parser.hpp \
			includes/utils.hpp

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