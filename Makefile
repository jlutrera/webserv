# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/12 11:30:21 by adpachec          #+#    #+#              #
#    Updated: 2024/03/05 10:19:18 by adpachec         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Program name
NAME			=	webserv

# Folders
SRC_DIR			= 	src/
INC_DIR			= 	include/
OBJ_DIR			= 	obj/

# Sources 
SRC				=	ConfigFile.cpp \
					ConfigParser.cpp \
					ConnectionData.cpp \
					ConnectionManager.cpp \
					HttpRequest.cpp \
					HttpResponse.cpp \
					Location.cpp \
					main.cpp \
					Server.cpp \
					Socket.cpp \
					VirtualServers.cpp \
					CgiHandler.cpp \
					ServerUtils.cpp

SRCS 			= 	$(addprefix $(SRC_DIR), $(SRC))

# Headers
HEADERS 		=	$(INC_DIR)HttpRequest.hpp \
					$(INC_DIR)HttpResponse.hpp \
					$(INC_DIR)Server.hpp \
					$(INC_DIR)Socket.hpp \
					$(INC_DIR)ConnectionData.hpp \
					$(INC_DIR)ConnectionManager.hpp \
					$(INC_DIR)WebServer.hpp \
					$(INC_DIR)CgiHandler.hpp \
					$(INC_DIR)ServerUtils.hpp

# Objects
OBJ				= 	$(SRC:.cpp=.o)
OBJS			= 	$(addprefix $(OBJ_DIR), $(OBJ))

# Flags
CXX				= 	c++
CXXFLAGS		= 	-Wall -Werror -Wextra -std=c++98

# Rules
all				: 	create_dir $(NAME)

$(NAME)			: 	$(OBJS) $(HEADERS)
					printf "\033[0;33mALL objects files created !\033[0m\n"
					printf "\n\033[0;32mLinking ...\033[0m $@"
					$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
					printf " \033[0;32mOK!\033[0m\n"

create_dir		:
					mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o	:	$(SRC_DIR)%.cpp
					printf "\033[0;33mGenerating objects ... \033[0m$@\r"
					$(CXX) $(CXXFLAGS) -I$(INC_DIR) -o $@ -c $<
					printf "                                               \r"

clean			:
					printf "\n\033[0;31mDeleting objects\033[0m"
					rm -rf $(OBJ_DIR)
					printf " \033[0;32mOK!\033[0m\n"

fclean			: 	clean
					printf "\033[0;31mDeleting \033[0m $(NAME)"
					rm -rf $(NAME)
					printf " \033[0;32mOK!\033[0m\n"

re				: 	fclean all

.PHONY			: 	all re clean fclean create_dir

.SILENT			: 	create_dir clean fclean re $(OBJS) $(NAME) $(OBJ_DIR)%.o
