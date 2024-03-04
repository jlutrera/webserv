/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 11:32:14 by adpachec          #+#    #+#             */
/*   Updated: 2024/01/22 17:01:59 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "WebServer.hpp"

class Socket
{
	private:
		int 		_socketFd;
		sockaddr_in _address;
		int			_listenPort;

	public:
		Socket();
		~Socket();
		Socket(Socket& other);
		Socket& operator=(Socket& other);

		int			getSocketFd();
		sockaddr_in	getSocketAddr();
		int			getListenPort();

		bool	open(int port, in_addr addr);
		bool	accept(Socket& newSocket, int port) const;
		int		send(const char* buffer, int length) const;
		int		receive(char* buffer, int maxLength, size_t startOffset) const;
		void	close();

};

#endif // SOCKET_HPP