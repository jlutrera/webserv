/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 12:37:38 by adpachec          #+#    #+#             */
/*   Updated: 2024/02/28 17:32:55 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "WebServer.hpp"
# include "Socket.hpp"
# include "ConnectionManager.hpp"
# include "VirtualServers.hpp"
# include "ConfigFile.hpp"
# include "ServerUtils.hpp"
# include "CgiHandler.hpp"

class Server
{
	private:
		std::vector<Socket*> _serverSockets;
		std::vector<Socket*> _clientSockets;
		ConnectionManager _connectionManager;
		std::vector<struct pollfd> _pollFds;
		std::map<int, HttpResponse> _responsesToSend;
		short _errorCode;
	
	public:
		Server();
		Server(std::vector<VirtualServers> servers);
		~Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		
		void 		run(std::vector<VirtualServers> servers);
		void 		processRequest(HttpRequest request, VirtualServers server, Socket* socket);
		void 		processReturnDirective(const Location& locationRequest,
					HttpResponse& processResponse);
		void 		processGet(std::string resourcePath, const Location* locationRequest,
					Socket* socket, VirtualServers server);
		void		processGetCGI(std::string resourcePath, const Location* locationRequest,
					Socket* socket, VirtualServers server, HttpRequest request);
		void		processPost(HttpRequest request, VirtualServers server, Socket* socket,
					const Location* locationRequest);
		void 		processPostCGI(HttpRequest request, VirtualServers server, Socket* socket,
					const Location* locationRequest);
		void 		processDelete(std::string resourcePath, VirtualServers server, Socket* socket);
		bool 		postFileCGI(const std::string& httpBody, const std::string& filename, 
					VirtualServers server, Socket* socket);
		bool 		postFile(std::string resourcePath, HttpRequest request, VirtualServers server, 
					Socket* socket);
		std::string checkGetPath(std::string resourcePath, const Location* locationRequest,
					Socket* socket, VirtualServers server);
		Socket* 	handleNewConnection(int i);
		void		createErrorPage(short errorCode, VirtualServers &server,
					Socket* socket);
	
		class ErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ErrorException(std::string message) throw()
				{
					_message = "SERVER ERROR: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};
};

#endif // SERVER_HPP