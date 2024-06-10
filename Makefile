NAME = webserv

CC = c++
san = -fsanitize=address
CFLAGS = -Wall -Wextra -Werror --std=c++98 -g3 #$(san)

srcs = 	Reactor.cpp \
		EventHandlers/AcceptHandler.cpp \
		EventHandlers/EventHandler.cpp \
		EventHandlers/HttpHandler.cpp \
		Utils/Utils.cpp \
		configParser/ConfigParser.cpp \
		ServerConf.cpp Location.cpp \
		Server.cpp \
		main.cpp 

objs = $(srcs:.cpp=.o)

all : $(NAME)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME) : $(objs)
	$(CC) $(CFLAGS) $(objs) -o $(NAME)

clean :
	rm -rf $(objs)

fclean : clean
	rm -rf $(NAME)

re : fclean all

.PHONY : clean fclean all re
