/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionManager.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 12:42:04 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/01 11:31:21 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTIONMANAGER_HPP
# define CONNECTIONMANAGER_HPP

# include "WebServer.hpp"
# include "Socket.hpp"
# include "VirtualServers.hpp"
# include "ConnectionData.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

class ConnectionManager
{
	public:
		std::map<int, ConnectionData> connections;

		ConnectionManager();
		~ConnectionManager();
		ConnectionManager(const ConnectionManager& other);
		ConnectionManager& operator=(const ConnectionManager& other);

		void	addConnection(Socket& socket);
		void	removeConnection(Socket& socket, int i,
				std::vector<struct pollfd> &_pollFds, std::vector<Socket *> &_clientSockets,
					std::map<int, HttpResponse>& _responsesToSend);
		HttpRequest	readData(Socket& socket, int i,
				std::vector<struct pollfd> &_pollFds, std::vector<Socket *> &_clientSockets,
					std::map<int, HttpResponse>& _responsesToSend);
		void	writeData(Socket& socket, HttpResponse &response);
		bool	isHttpRequestComplete(const std::vector<char>& buffer, size_t accumulatedBytes);
		int		getContentLength(const std::string& header);
		
		
};

#endif // CONNECTIONMANAGER_HPP
