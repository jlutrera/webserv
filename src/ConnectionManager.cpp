/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionManager.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 12:42:54 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/01 11:31:14 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionManager.hpp"

//*******************************************************************
// Constructores y destructor de la clase canónica
//*******************************************************************
ConnectionManager::ConnectionManager() {}

ConnectionManager::~ConnectionManager() {}

ConnectionManager::ConnectionManager(const ConnectionManager& other)
{
	connections = other.connections;
}

ConnectionManager& ConnectionManager::operator=(const ConnectionManager& other)
{
	if (this != &other)
		connections = other.connections;
	return *this;
}

//*******************************************************************
// Métodos de la clase
//*******************************************************************
void ConnectionManager::addConnection(Socket& socket)
{
	int socketFd = socket.getSocketFd();
	ConnectionData connData;

	connections.insert(std::make_pair(socketFd, connData));
	// std::cout << "    Connection added. Socket FD: " << socketFd << std::endl;
}

void ConnectionManager::removeConnection(Socket& socket, int i,
			std::vector<struct pollfd>& _pollFds, std::vector<Socket *>& _clientSockets, std::map<int, HttpResponse>& _responsesToSend)
{
	int socketFd = socket.getSocketFd();

	for (size_t j = 0; j < _clientSockets.size(); ++j)
	{
		if (_clientSockets[j]->getSocketFd() == socketFd)
			_clientSockets.erase(_clientSockets.begin() + j);
	}
	
	if (_pollFds[i].fd == socketFd)
		_pollFds.erase(_pollFds.begin() + i);
	
	std::map<int, ConnectionData>::iterator it = connections.find(socketFd);
	if (it != connections.end())
	{
		connections.erase(it);
		// std::cout << "Connection deleted. Socket FD = " << socketFd << std::endl;
	}
	else
		std::cout << "Connection not found. Socket FD = " << socketFd << std::endl;
	
	std::map<int, HttpResponse>::iterator it2 = _responsesToSend.find(socketFd);
	if (it2 != _responsesToSend.end())
		_responsesToSend.erase(socketFd);

	if (socketFd != -1)
		socket.close();
}

HttpRequest ConnectionManager::readData(Socket& socket, int i,
			std::vector<struct pollfd> &_pollFds, std::vector<Socket *> &_clientSockets, std::map<int, HttpResponse>& _responsesToSend)
{
	ConnectionData* data(&connections[socket.getSocketFd()]);

	//Si no hay hueco en el buffer aumentamos tamaño
	if (data->readBuffer.size() - data->accumulatedBytes == 0)
		data->readBuffer.resize(data->readBuffer.size() + 1024);
	if (!data->readBuffer.empty())
	{
		std::string headers(data->readBuffer.begin(), data->readBuffer.end());
		size_t contentLength = getContentLength(headers);
		if (contentLength > 0)
			data->readBuffer.reserve(data->readBuffer.size() + contentLength);
	}

	// Leer datos del socket
	int bytesRead = socket.receive(&data->readBuffer[0], data->readBuffer.size(), data->accumulatedBytes);

	if (bytesRead > 0)
	{
		data->accumulatedBytes += bytesRead; // Añadir a la cuenta de bytes acumulados

		if (isHttpRequestComplete(data->readBuffer, data->accumulatedBytes))
		{
			// Procesar la solicitud completa
			HttpRequest request(std::string(data->readBuffer.begin(), data->readBuffer.end()));

			if (request.getIsValidRequest())
			{
				request.setValidRequest(true);
				request.setCompleteRequest(true);
				data->readBuffer.clear();
				data->readBuffer.resize(1024);
				data->accumulatedBytes = 0;
				data->headerReceived = false;
				connections[socket.getSocketFd()] = *data;

				request.printRequest();

				return request;
			}
			else
			{
				std::cerr << "    Invalid request" << std::endl;
				data->readBuffer.clear();
				data->readBuffer.resize(1024);
				data->accumulatedBytes = 0;
				data->headerReceived = false;
				request.setValidRequest(false);
				request.setCompleteRequest(true);
				return request;
			}
		}
		connections[socket.getSocketFd()] = *data;
	}
	else
	{
		this->removeConnection(socket, i, _pollFds, _clientSockets, _responsesToSend);
		HttpRequest invalidRequest;
		invalidRequest.setValidRequest(false);
		return invalidRequest;
	}
	HttpRequest incompleteRequest;
	return incompleteRequest;
}

void ConnectionManager::writeData(Socket& socket, HttpResponse &response) 
{	
	ConnectionData data(connections[socket.getSocketFd()]);

	std::string responseStr = response.buildResponse();
	data.accumulatedBytes = responseStr.size();
	data.writeBuffer = new char[data.accumulatedBytes];

	std::copy(responseStr.begin(), responseStr.end(), data.writeBuffer);
	
	while (data.writeBuffer && data.accumulatedBytes > 0)
	{
		int bytesSent = socket.send(data.writeBuffer, data.accumulatedBytes);
		
		// response.printResponse(responseStr);
		if (bytesSent > 0)
		{
			data.accumulatedBytes -= bytesSent;
			std::memmove(data.writeBuffer, data.writeBuffer + bytesSent, data.accumulatedBytes);
		}
		else if (bytesSent < 0)
		{
			if (data.writeBuffer)
				delete[] data.writeBuffer;
			data.writeBuffer = NULL;
			std::cout << "Error sending the response" << std::endl;
		}
		else if (bytesSent == 0)
		{
			std::cout << "0 BytesSent response size: " << responseStr.size() << std::endl;
		}
		if (data.accumulatedBytes == 0)
		{
			if (data.writeBuffer)
				delete[] data.writeBuffer;
			data.writeBuffer = NULL;
			response.setBody("");
		}
	}
}

bool ConnectionManager::isHttpRequestComplete(const std::vector<char>& buffer, size_t accumulatedBytes)
{
	const std::string endOfHeader = "\r\n\r\n";

	std::vector<char>::const_iterator endOfHeaderPos =
		std::search(buffer.begin(), buffer.end(), endOfHeader.begin(), endOfHeader.end());

	if (endOfHeaderPos != buffer.end())
	{
		// Encabezados completos recibidos, calcular el tamaño total esperado de la solicitud
		std::string header(buffer.begin(), endOfHeaderPos + endOfHeader.length());
		size_t contentLength = getContentLength(header);
		// Calcular el total de bytes recibidos hasta ahora y actualizar accumulatedBytes
		size_t headersLength = endOfHeaderPos + endOfHeader.length() - buffer.begin();
		accumulatedBytes = headersLength + contentLength;

		// Comprobar si hemos recibido todo el cuerpo de la solicitud
		return buffer.size() >= accumulatedBytes;
	}
	return false;
}

int ConnectionManager::getContentLength(const std::string& header)
{
	// Convertir el buffer actual a string para buscar el Content-Length
	std::size_t startPos = header.find("Content-Length: ");
	
	if (startPos != std::string::npos)
	{
		startPos += std::string("Content-Length: ").length();
		std::size_t endPos = header.find("\r\n", startPos);
		if (endPos != std::string::npos)
		{
			std::string contentLengthValue = header.substr(startPos, endPos - startPos);
			std::istringstream iss(contentLengthValue);
			int contentLength;
			if (iss >> contentLength)
				return contentLength;
		}
	}
	return 0;
}
