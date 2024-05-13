NAME = webserv

CC = c++
CFLAGS = -Wall -Wextra -Werror --std=c++98 -g3

srcs =	main.cpp \
		Reactor.cpp \
		EventHandlers/AcceptHandler.cpp \
		EventHandlers/EventHandler.cpp \
		EventHandlers/HttpHandler.cpp

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
