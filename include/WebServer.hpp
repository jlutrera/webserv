/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 12:41:27 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/01 12:40:10 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

// Libraries
# include <algorithm>
# include <arpa/inet.h>
# include <cstdlib>
# include <cstdio>
# include <cstring>
# include <dirent.h>
# include <errno.h>
# include <fstream>
# include <fcntl.h>
# include <iostream>
# include <map>
# include <unordered_map>
# include <netinet/in.h>
# include <poll.h>
# include <string>
# include <sstream>
# include <stdint.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <sys/select.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# include <vector>
# include <chrono>

// Colours
# define RESET          "\x1B[0m"
# define RED            "\x1B[31m"
# define LIGHTRED      	"\x1B[91m"
# define YELLOW         "\x1B[33m"
# define LIGHTBLUE     	"\x1B[94m"
# define CYAN           "\x1B[36m"
# define DARKGREY      	"\x1B[90m"
# define LIGHTMAGENTA   "\x1B[95m"
# define GREEN 			"\x1B[32m"

// HTTP Methods
# define GET_METHOD 	0
# define POST_METHOD 	1
# define DELETE_METHOD 	2

// Types
# define IS_DIR 		2
# define IS_FILE 		1

// Macros
# define MAX_CONTENT_LENGTH 1000000000
# define CGI_BUFSIZE 		1000000

# define UPLOAD "/uploads"

#endif // WEBSERVER_HPP