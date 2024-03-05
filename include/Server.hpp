/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 12:37:38 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/04 13:35:13 by adpachec         ###   ########.fr       */
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
		HttpResponse _responseToSend;
		short _errorCode;
	
	public:
		Server();
		Server(std::vector<VirtualServers> servers);
		~Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		
		void 		run(std::vector<VirtualServers> servers);
		void 		processRequest(HttpRequest request, VirtualServers server);
		void 		processReturnDirective(const Location& locationRequest,
						HttpResponse& processResponse);
		void 		processGet(std::string resourcePath, const Location* locationRequest,
						VirtualServers server);
		void		processGetCGI(std::string resourcePath, const Location* locationRequest,
						VirtualServers server, HttpRequest request);
		void		processPost(HttpRequest request, VirtualServers server,
						const Location* locationRequest);
		void 		processPostCGI(HttpRequest request, VirtualServers server,
						const Location* locationRequest);
		void 		processDelete(std::string resourcePath, VirtualServers server);
		bool 		postFileCGI(const std::string& httpBody, const std::string& filename, 
						VirtualServers server);
		bool 		postFile(std::string resourcePath, HttpRequest request, VirtualServers server);
		std::string checkGetPath(std::string resourcePath, const Location* locationRequest,
						VirtualServers server);
		Socket* 	handleNewConnection(int i);
		void		createErrorPage(short errorCode, VirtualServers &server);
	
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