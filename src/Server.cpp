/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 12:38:27 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/05 10:26:46 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//******************************************************************************
// Constructores, destructor y operador de asignación
//******************************************************************************
Server::Server() { }
Server::Server(const Server& other)
{
	_serverSockets = other._serverSockets;
	_clientSockets = other._clientSockets;
	_connectionManager = other._connectionManager;
	_pollFds = other._pollFds;
	_responseToSend = other._responseToSend;
}
Server& Server::operator=(const Server& other)
{
	if (this != &other)
	{
		_serverSockets = other._serverSockets;
		_clientSockets = other._clientSockets;
		_connectionManager = other._connectionManager;
		_pollFds = other._pollFds;
		_responseToSend = other._responseToSend;
	}
	return *this;
}
Server::Server(std::vector<VirtualServers>	servers)
{
	std::cout << "\nInitializing server..." << std::endl;
	std::cout << "    Num. of servers: " << servers.size() << std::endl;
	_serverSockets.reserve(servers.size());

	// Crear sockets
	for (size_t i = 0; i < servers.size(); ++i)
	{
		if (checkOpenPorts(_serverSockets, servers[i]))
		{
			Socket* newSocket = new Socket();
			if (newSocket->open((int) servers[i].getPort(), servers[i].getIpAddress()) == false)
			{
				delete newSocket;
				throw ErrorException("Error opening the socket");
			}
				
			_serverSockets.push_back(newSocket);
			struct pollfd serverPollFd;

			serverPollFd.fd = _serverSockets.back()->getSocketFd();
			serverPollFd.events = POLLIN; // Establecer para leer
			this->_pollFds.push_back(serverPollFd);
			std::cout << "    Listening on port:  " <<
				servers[i].getPort() << std::endl;
		}
	}
}
Server::~Server()
{
	for (size_t i = 0; i < _serverSockets.size(); ++i)
	{
		_serverSockets[i]->close();
		delete _serverSockets[i];
	}
	
	for (size_t i = 0; i < _clientSockets.size(); ++i)
	{
		_clientSockets[i]->close();
		delete _clientSockets[i];
	}
}

//******************************************************************************
// Métodos de la clase
//******************************************************************************
void Server::run(std::vector<VirtualServers> servers)
{
	std::cout << "\nServer running..." << std::endl;

	while (true)
	{
		// Llamar a poll con la lista de file descriptors y un tiempo de espera
		int ret = poll(&_pollFds[0], _pollFds.size(), -1); // -1 para tiempo de espera indefinido
		if (ret < 0)
		{
			std::cerr << "    Poll error !" << std::endl;
			createErrorPage(500, servers[0]);
			break;
		}

		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			HttpRequest requestReceive;
			VirtualServers bestServer;
			int currentFd = _pollFds[i].fd;
			Socket* dataSocket = NULL;
			
			if (_pollFds[i].revents & POLLIN)
			{
				dataSocket = handleNewConnection(i);
				if (dataSocket && dataSocket->getSocketFd() != -1 &&
					currentFd == dataSocket->getSocketFd())
				{
					requestReceive = _connectionManager.readData(*dataSocket, i, _pollFds, _clientSockets);
					if (requestReceive.getIsValidRequest() && requestReceive.getIsCompleteRequest())
					{
						bestServer = getBestServer(requestReceive, i, servers, _clientSockets, _pollFds);
						// std::cout << "Server: " << bestServer.getServerName() << std::endl;
						processRequest(requestReceive, bestServer);
						_connectionManager.writeData(*dataSocket, _responseToSend);
						if (!requestReceive.getIsKeepAlive() && dataSocket)
							_connectionManager.removeConnection(*dataSocket, i, _pollFds, _clientSockets);
					}
					else if (!requestReceive.getIsValidRequest() && requestReceive.getIsCompleteRequest())
					{
						if (_pollFds.size() > i - 1)
							--i;
						delete dataSocket;
						createErrorPage(400, bestServer);
					}
					else
						delete dataSocket;
					
					break ;
				}
			}
			else if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				// Manejar desconexiones o errores
				std::cout << "    Connection closed or error in socket FD: " << currentFd << std::endl;
				for (size_t j = 0; j < _clientSockets.size(); ++j)
				{
					if (_clientSockets[j]->getSocketFd() == currentFd)
					{
						std::cout << "Client socket deleted: " << _clientSockets[j]->getSocketFd() << std::endl;
						_connectionManager.removeConnection(*(_clientSockets[j]), i, _pollFds, _clientSockets);
						--i;
						break ;
					}
				}
			}
		}
	}
}

void Server::processRequest(HttpRequest request, VirtualServers server)
{
	// Configurar la respuesta
	// std::cout << "\nProcessing REQUEST... " << std::endl;
	// std::cout << "    Method: " << request.getMethod() << std::endl;
	// std::cout << "    Requested URL: " << request.getURL() << std::endl;
	if (server.getPort() == 0)
	{
		std::cout << "    Server not found" << std::endl;
		createErrorPage(_errorCode, server);
		return ;
	}

	const Location*	locationRequest = NULL;
	Location tmp;
	
	locationRequest = tmp.selectLocation(request.getURL(), server.getLocations());
	
	if (locationRequest == NULL)
	{
		std::cout << "    Location not found" << std::endl;
		createErrorPage(404, server);
		return ;
	}
	// std::cout << "    Location found: " << locationRequest->getPath() << std::endl;
	if (locationRequest->getReturn()[0] != "")
	{
		processReturnDirective(*locationRequest, _responseToSend);
		return ;
	}
	
	std::string resourcePath = buildResourcePath(request, *locationRequest, server);
	//****************************GET Method****************************
	if (request.getMethod() == "GET")
	{
		if (!locationRequest->getMethods()[GET_METHOD])
		{
			createErrorPage(405, server);
			return ;
		}
		if (isCGIScript(resourcePath))
			processGetCGI(resourcePath, locationRequest, server, request);
		else
			processGet(resourcePath, locationRequest, server);
	}

	//****************************POST Method****************************
	else if (request.getMethod() == "POST")
	{
		// Verificar si el método POST está permitido
		if (!locationRequest->getMethods()[POST_METHOD])
		{
			createErrorPage(405, server);
			return ;
		}
		if (request.getBody().size() > (locationRequest->getMaxBodySize() > 0 ?
			locationRequest->getMaxBodySize() : server.getClientMaxBodySize()))
		{
			createErrorPage(413, server);
			return ;
		}
		if (isCGIScript(resourcePath))
			processPostCGI(request, server, locationRequest);
		else
			processPost(request, server, locationRequest);
	}
	//****************************DELETE Method****************************
	else if (request.getMethod() == "DELETE")
	{
		// Verificar si el método DELETE está permitido
		if (!locationRequest->getMethods()[DELETE_METHOD])
		{
			createErrorPage(405, server);
			return ;
		}
		processDelete(resourcePath, server);
	}
	//****************************Unknown Method****************************
	else
	{
		// Método no soportado
		createErrorPage(405, server);
		return ;
	}
}

void Server::processGetCGI(std::string resourcePath, const Location* locationRequest,
	VirtualServers server, HttpRequest request)
{
	CgiHandler cgi(request, *locationRequest, server);

	std::string extension = resourcePath.substr(resourcePath.find_last_of("."));
	
	std::string pathCGI = locationRequest->getExtensionCgiPath(extension);
	
	if (pathCGI.empty())
	{
		createErrorPage(404, server);
		return ;
	}

	if (!ConfigFile::fileExistsAndReadable(resourcePath))
	{
		createErrorPage(404, server);
		return ;
	}

	std::string buffer = cgi.executeCgi(resourcePath, pathCGI);
	if (buffer == "Status: 500\r\n\r\n")
		createErrorPage(500, server);
	else
	{
		_responseToSend.setStatusCode(200);
		_responseToSend.setHeader("Content-Type", "text/html");
		_responseToSend.setBody(buffer);
	}
}

void Server::processGet(std::string resourcePath, const Location* locationRequest,
	VirtualServers server)
{

	HttpResponse processResponse;

	resourcePath = checkGetPath(resourcePath, locationRequest, server);
	
	if (resourcePath.empty())
		return ;

	
	std::string buffer = ConfigFile::readFile(resourcePath);
	
	if (buffer.empty())
	{
		//Error si el archivo está vacío o no se pudo abrir
		createErrorPage(204, server);
		return;
	}
	// Si se leyó con éxito, construir la respuesta
	_responseToSend.setStatusCode(200);
	_responseToSend.setHeader("Content-Type", getMimeType(resourcePath));
	_responseToSend.setBody(buffer);
}

void Server::processPostCGI(HttpRequest request, VirtualServers server,
	const Location* locationRequest)
{
	std::string contentLengthHeader = request.getHeader("Content-Length");
	unsigned long contentLength;
	if (contentLengthHeader.empty())
		contentLength = 0;
	else
		contentLength = std::strtoul(contentLengthHeader.c_str(), NULL, 10);
	if (contentLength > server.getClientMaxBodySize())
	{
		createErrorPage(413, server);
		return;
	}

	std::string resourcePath = buildResourcePathForPost(request, *locationRequest, server);

	std::string root;
	if (locationRequest->getRootLocation().empty())
	{
		root = server.getRoot() + UPLOAD;
		std::cout << "    Root from server: " << server.getRoot() << std::endl;
	}
	else
	{
		root = locationRequest->getRootLocation();
		std::cout << "    Root from location: " << locationRequest->getRootLocation() << std::endl;
	}
	std::string fullResourcePath = root + getFilenameCGI(request);

	if (!postFileCGI(request.getBody(), fullResourcePath, server))
		return ;

	// Guardar el cuerpo de la solicitud en el archivo especificado por la ruta	
	_responseToSend.setStatusCode(200);
	size_t contentTypeIni = request.getBody().find("Content-Type:");
	if (contentTypeIni == std::string::npos)
	{
		createErrorPage(500, server);
		return;
	}
	size_t contentTypeEnd = request.getBody().find("\n", contentTypeIni);
	if (contentTypeEnd == std::string::npos)
	{
		createErrorPage(500, server);
		return;
	}
	std::string contentType = request.getBody().substr(contentTypeIni + 14,
			contentTypeEnd - contentTypeIni -14);

	_responseToSend.setHeader("Content-Type", contentType);
	_responseToSend.setBody("Content uploaded successfully.");
}

void Server::processPost(HttpRequest request, VirtualServers server,
	const Location* locationRequest)
{
	// Verificar si el tipo de contenido es soportado (ejemplo: no se soporta multipart/form-data o chunked)
	std::string contentTypeHeader = request.getHeader("Content-Type");
	if (contentTypeHeader.find("multipart/form-data") != std::string::npos ||
		contentTypeHeader.find("chunked") != std::string::npos)
	{
		createErrorPage(501, server);
		return ;
	}

	// Determinar la ruta absoluta donde se guardará el contenido de la solicitud POST
	// Error si la ruta es inválida o no se puede escribir
	std::string resourcePath = buildResourcePathForPost(request, *locationRequest, server);

	if (resourcePath.empty() || !isValidPath(locationRequest->getRootLocation().empty() ? server.getRoot()
		: locationRequest->getRootLocation(), resourcePath))
	{
		createErrorPage(400, server);
		return ;
	}
	// Guardar el cuerpo de la solicitud en el archivo especificado por la ruta
	// Error si no se puede abrir el archivo
	std::string	fullResourcePath = getFilename(request, resourcePath);
	if (!postFile(fullResourcePath, request, server))
		return ;

	// Guardar el cuerpo de la solicitud en el archivo especificado por la ruta	
	_responseToSend.setStatusCode(200);
	_responseToSend.setHeader("Content-Type", "text/plain");
	_responseToSend.setBody("Content uploaded successfully.");
}

bool Server::postFileCGI(const std::string& httpBody, const std::string& filename, 
		VirtualServers server)
{
    // Find the position of "Content-Type: text/plain" in the HTTP body
    size_t contentStartPos = httpBody.find("Content-Type:");
    
    // If "Content-Type:" is found
	if (contentStartPos == std::string::npos)
	{
		createErrorPage(500, server);
		return false;
	}
    // Find the position of the newline character after "Content-Type: text/plain"
    size_t newlinePos = httpBody.find("\n", contentStartPos);

    // If the newline character is found
	if (newlinePos == std::string::npos)
	{
		createErrorPage(500, server);
		return false;
	}
    // Extract the file content starting from the newline character
    std::string fileContent = httpBody.substr(newlinePos + 1);

    // Find the position of the boundary line
    size_t boundaryPos = fileContent.find("------WebKitFormBoundary");
	if (boundaryPos != std::string::npos)
	    fileContent = fileContent.substr(0, boundaryPos);
    
	// Save the file content to a file with the provided filename
    std::ofstream outputFile(filename.c_str());
	if (!outputFile.is_open())
	{
		createErrorPage(500, server);
		return false;
	}
    outputFile << fileContent;
    outputFile.close();
	return true;
}

void Server::processDelete(std::string resourcePath, VirtualServers server)
{	
	if (!ConfigFile::fileExistsAndReadable(resourcePath))
	{
		createErrorPage(404, server);
		return ;
	}
	// Eliminar el recurso
	if (remove(resourcePath.c_str()) != 0)
	{
		createErrorPage(500, server);
		return ;
	}
	// Construir la respuesta
	_responseToSend.setStatusCode(204);
	_responseToSend.setBody("Delete successful");
}

void Server::createErrorPage(short errorCode, VirtualServers &server)
{
	_responseToSend.setStatusCode(errorCode);
	
	std::string errorPage1 = server.getRoot();
	std::string errorPage2 = server.getErrorPage(errorCode);

	if (errorPage2 == "")
		_responseToSend.setBody(createBodyErrorPage(errorCode));
	else if (errorPage2[0] != '/')
		errorPage2 = "/" + errorPage2;
	errorPage2 = errorPage1 + errorPage2;
	if (ConfigFile::fileExistsAndReadable(errorPage2))
	{
		std::string bodyFromFile = ConfigFile::readFile(errorPage2);
		_responseToSend.setBody(bodyFromFile);
	}
	else
		_responseToSend.setBody(createBodyErrorPage(errorCode));
	if (_responseToSend.getBody().empty())
		_responseToSend.setBody("Error Page Undefined");
}

void Server::processReturnDirective(const Location& locationRequest,
	HttpResponse& processResponse)
{
	std::vector<std::string> ret = locationRequest.getReturn();
	int statusCode = Location::ft_stoi(ret[0]);
	std::string urlOrText = locationRequest.getReturn()[1];

	if (urlOrText[0] == '/')
	{
		// Si la cadena comienza con '/', es una redirección interna
		// La URL se construye a partir de la raíz del servidor
		// Si es un código de redirección, añadir la URL a la cabecera 'Location'
		processResponse.setHeader("Location", urlOrText);
		if (statusCode == 301 || statusCode == 302 || statusCode == 303 || statusCode == 307)
			processResponse.setBody("return successful");
		else
			processResponse.setBody(bodyReturn(locationRequest.getPath(), urlOrText, statusCode));
	}
	else
	{	
		// si no, se envía el texto de la redirección
		processResponse.setHeader("Location", locationRequest.getPath());
		if (statusCode == 301 || statusCode == 302 || statusCode == 303 || statusCode == 307)
			processResponse.setBody("return successful");
		else
			processResponse.setBody(bodyReturn(urlOrText, "", statusCode));
	}
	processResponse.setStatusCode(statusCode);
}

Socket* Server::handleNewConnection(int i)
{
	Socket* existingSocket;
	if (i < (int) _serverSockets.size())
	{
		Socket* newSocket = new Socket();
		if (_serverSockets[i]->accept(*newSocket, _serverSockets[i]->getListenPort()))
		{
			std::vector<Socket*>::iterator it;
			for (it = _clientSockets.begin(); it != _clientSockets.end(); ++it)
			{
				existingSocket = *it;
				if (areAddressesEqual(newSocket->getSocketAddr(), existingSocket->getSocketAddr()))
				{
					delete newSocket;
					return existingSocket;
				}
			}
			struct pollfd newPollFd;
			newPollFd.fd = newSocket->getSocketFd();
			newPollFd.events = POLLIN | POLLOUT;
			_connectionManager.addConnection(*newSocket);
			_pollFds.push_back(newPollFd);
			_clientSockets.push_back(newSocket);
			return newSocket;
		}
		else
		{
			delete newSocket;
			std::cerr << "    Error accepting new connection" << std::endl;
			Socket *errorSocket = NULL;
			return errorSocket;
		}
	}
	else
	{
		for (size_t j = 0; j < _clientSockets.size(); ++j)
		{
			if (_clientSockets[j]->getSocketFd() == _pollFds[i].fd)
				return (_clientSockets[j]);
		}
	}
	return _serverSockets[0];
}

bool Server::postFile(std::string resourcePath, HttpRequest request, VirtualServers server)
{
	HttpResponse processResponse;
	if (resourcePath[0] == '.')
		resourcePath = resourcePath.substr(2);
	std::ofstream outputFile(resourcePath.c_str(), std::ios::out | std::ios::binary);
	if (!outputFile.is_open())
	{
		createErrorPage(500, server);
		return false;
	}
	outputFile.write(request.getBody().c_str(), request.getBody().size());
	outputFile.close();
	return true;
}

std::string Server::checkGetPath(std::string resourcePath, const Location* locationRequest,
		VirtualServers server)
{
	if (ConfigFile::checkPath(resourcePath) == IS_DIR)
	{
		if (locationRequest->getAutoindex())
		{
			// Autoindex activado: generar y enviar página de índice
			std::string directoryIndexHTML = generateDirectoryIndex(resourcePath);		
			_responseToSend.setStatusCode(200);
			_responseToSend.setHeader("Content-Type:", "text/html");
			if (!directoryIndexHTML.empty())
				_responseToSend.setBody(directoryIndexHTML);
			else
				_responseToSend.setBody("Empty directory");
			return "";
		}
		else
		{
			// Autoindex desactivado: buscar archivo index por defecto
			std::string indexPath = resourcePath;
			if (ConfigFile::fileExistsAndReadable(indexPath))
			{
				std::string buffer = ConfigFile::readFile(indexPath);
				
				_responseToSend.setStatusCode(200);
				_responseToSend.setHeader("Content-Type:", getMimeType(indexPath));
				if (buffer.empty())
					_responseToSend.setBody("Empty file");
				else
					_responseToSend.setBody(buffer);
				return "";
			}
			else
			{
				// Directorio sin archivo index y autoindex desactivado
				createErrorPage(403, server);
				return "";
			}
		}
	}
	else if (!ConfigFile::fileExistsAndReadable(resourcePath))
	{
		// Si no existe, intenta enviar página de error personalizada o respuesta 404 genérica
		createErrorPage(404, server);
		return "";
	}

	return resourcePath;
}
