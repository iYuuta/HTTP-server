CPP = c++
CFLAGS = -std=c++98 -Wall -Wextra -Werror -g -fsanitize=address
SRC = src/main.cpp src/server/Server.cpp src/configs/Config.cpp \
	src/utils/utils.cpp src/parsing/parser.cpp src/parsing/optionsValidator.cpp \
	src/configs/Size.cpp src/parsing/Token.cpp src/parsing/optionsParser.cpp src/configs/Location.cpp \
	src/parsing/optionsParser2.cpp  src/client/Client.cpp \
	src/client/request/Request.cpp src/client/response/Multipart.cpp src/client/response/Response.cpp \
	 src/client/validRequest.cpp src/server/HttpServer.cpp 
	
OBJS = ${SRC:.cpp=.o}
NAME = webserv
HEADERS = includes/Config.hpp includes/Server.hpp includes/Size.hpp \
			includes/HttpRequestMethod.hpp includes/utils.hpp includes/Token.hpp \
			includes/Location.hpp includes/Request.hpp includes/Response.hpp \
			includes/Client.hpp includes/HttpServer.hpp

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