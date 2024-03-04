/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 11:33:24 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/04 12:04:37 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

//*******************************************************************
// Constructores y destructor
//*******************************************************************
Socket::Socket() : _socketFd(-1) {}

Socket::~Socket() {}

Socket::Socket(Socket& other) : _socketFd(other._socketFd)
{
	other._socketFd = -1;
}

Socket& Socket::operator=(Socket& other)
{
	if (this != &other)
	{
		if (_socketFd != -1)
			::close(_socketFd);
		_socketFd = other._socketFd;
		other._socketFd = -1;
	}
	return *this;
}

//*******************************************************************
// Getters
//*******************************************************************
int	Socket::getSocketFd() { return (this->_socketFd); }

sockaddr_in Socket::getSocketAddr() { return (this->_address); }

int	Socket::getListenPort() { return (this->_listenPort); };

//*******************************************************************
// Métodos de la clase
//*******************************************************************
bool Socket::open(int port, in_addr addr)
{

	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	std::cout << "Server ip adress: " << addr.s_addr << std::endl;
	if (addr.s_addr == 0)
		_address.sin_addr.s_addr = INADDR_ANY;
	else
		_address.sin_addr.s_addr = addr.s_addr;
	_address.sin_port = htons(port);
	_listenPort = ntohs(_address.sin_port);
	std::cout << "OPEN puerto: " << ntohs(_address.sin_port) << std::endl;
	
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd == -1)
		return false;
	
	int opt = 1;
	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error configuring SO_REUSEADDR" << std::endl;
		return false;
	}
	
	fcntl(_socketFd, F_SETFL, O_NONBLOCK);

	if (bind(_socketFd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
		return false;

	if (listen(_socketFd, 50) < 0)
		return false;

	return true;
}

bool Socket::accept(Socket& newSocket, int port) const
{
	socklen_t addressLen = sizeof(_address);

	int new_sockfd = ::accept(this->_socketFd, (sockaddr *)&_address, &addressLen);
	if (new_sockfd < 0)
		return false;

	// std::cout << "Nueva conexion: " << new_sockfd << std::endl;
	newSocket._socketFd = new_sockfd;
	newSocket._address = _address;
	newSocket._listenPort = port;

	int opt = 1;
	setsockopt(newSocket._socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	fcntl(newSocket._socketFd, F_SETFL, O_NONBLOCK);
	
	return true;
}

int Socket::send(const char* buffer, int length) const
{
	int totalSent = 0;
	int n;

	n = ::send(_socketFd, buffer, length, 0);
	totalSent += n;
	return (n == -1) ? -1 : totalSent;
}

int Socket::receive(char* buffer, int maxLength, size_t startOffset) const
{
	int n = ::recv(_socketFd, buffer + startOffset, maxLength - startOffset, 0);
	if (n == -1)
	{
		std::cerr << "Error: receive" << std::endl;
		return -1;
	}
	if (n == 0)
		return 0;

	return (n <= 0) ? -1 : n;
}

void Socket::close()
{
	// std::cout << "Socket closed FD: " << this->getSocketFd() << std::endl;
	if (_socketFd != -1)
	{
		::close(_socketFd);
		_socketFd = -1;
	}
}
