
NAME		= ircserv
	
CC			= c++
FLAGS		= -Wall -Wextra -Werror -std=c++98 -g3
RM			= rm -rf

OBJDIR = obj

FILES		= main Client Server Channel Command rpl Utils Mode

SRC			= $(FILES:=.cpp)
OBJ			= $(addprefix $(OBJDIR)/, $(FILES:=.o))
HEADER		= Client.hpp Server.hpp Channel.hpp rpl.hpp
#OPTS = -g

all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	@$(CC) $(OBJ) $(OPTS) -o $(NAME)

$(OBJDIR)/%.o: %.cpp $(HEADER)
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) $(OPTS) -c $< -o $@

clean:
	@$(RM) $(OBJDIR) $(OBJ)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus norm