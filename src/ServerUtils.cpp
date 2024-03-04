/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 11:49:23 by adpachec          #+#    #+#             */
/*   Updated: 2024/02/20 12:07:35 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerUtils.hpp"

std::string getMimeType(const std::string& filePath)
{
	size_t dotPos = filePath.rfind('.');
	static std::unordered_map<std::string, std::string> mimeTypes =
	{
		{".html", "text/html"},
		{".css",  "text/css"},
		{".txt",  "text/plain"},
		{".csv",  "text/csv"},
		{".htm",  "text/html"},
		
		{".jpg",  "image/jpeg"},
		{".jpeg", "image/jpeg"},
		{".png",  "image/png"},
		{".gif",  "image/gif"},
		{".svg",  "image/svg+xml"},
		{".ico",  "image/x-icon"},

		{".pdf",  "application/pdf"},
		{".zip",  "application/zip"},
		{".tar",  "application/x-tar"},
		{".gz",   "application/gzip"},
		{".js",   "application/javascript"},
		{".json", "application/json"},
		{".xml",  "application/xml"},
		{".doc",  "application/msword"},

		{".mp3",  "audio/mpeg"},
		{".mp4",  "video/mp4"},
		{".avi",  "video/x-msvideo"},
		{".mpeg", "video/mpeg"},
		{".webm", "video/webm"},
	};

	if (dotPos != std::string::npos)
	{
		std::string ext = filePath.substr(dotPos);
		if (mimeTypes.count(ext))
			return mimeTypes[ext];
	}
	return "text/plain"; // Tipo MIME por defecto si no se reconoce la extensión
}

bool isValidPath(const std::string& basePath, const std::string& path)
{
	std::string fullPath = path;

	// Asegurarse de que el path no salga del directorio base
	if (fullPath.find(basePath) != 0)
		return false;

	return true;
}

bool isCGIScript(const std::string& resourcePath)
{
    // Determine if the resource is a CGI script based on some criteria
    // For example, check the file extension or any other condition
    // Return true if it's a CGI script, false otherwise

    // Example: Check if the file extension is ".cgi"
	if (resourcePath.empty())
		return false;

    if (resourcePath.size() >= 4)
	{
		if ((resourcePath.substr(resourcePath.size() - 3) == ".py")  ||
			(resourcePath.substr(resourcePath.size() - 3) == ".sh") ||
			(resourcePath.substr(resourcePath.size() - 3) == ".php") ||
			(resourcePath.substr(resourcePath.size() - 3) == ".pl") ||
			(resourcePath.substr(resourcePath.size() - 3) == ".rb"))
			{
				std::cout << "    CGI script detected" << std::endl;
				return true;
			}
	}
	return false;
}

std::string getFilename(HttpRequest request, std::string resourcePath)
{
	std::string filename;
	size_t lastSlashPos = request.getURL().find_last_of('/');
	
	if (lastSlashPos != std::string::npos)
		filename = request.getURL().substr(lastSlashPos + 1);

	if (!filename.empty() )
	{
		if (resourcePath.size() < filename.size() ||
			resourcePath.substr(resourcePath.size() - filename.size()) != filename)
			{
				resourcePath += UPLOAD;
				resourcePath += "/" + filename;
			}
	}

	if (resourcePath.size() >= 2 && resourcePath[0] == '/')
		resourcePath = resourcePath.substr(1);
	return resourcePath;
}

std::string getFilenameCGI(HttpRequest request)
{
	size_t initFilename = request.getBody().find("filename=");
	if (initFilename == std::string::npos)
		return "";
	
	size_t sizeFilename = request.getBody().find("\r\n", initFilename);
	if (sizeFilename == std::string::npos)
		return "";
		
	std::string filename = request.getBody().substr(initFilename + 10,
		sizeFilename - initFilename - 11);

	return ("/" + filename);
}

std::string generateDirectoryIndex(const std::string& directoryPath)
{
	std::stringstream html;
	html << "<html>\n<head>\n<title>Index of " << directoryPath << "</title>\n</head>\n";
	html << "<body>\n<h1>Index of " << directoryPath << "</h1>\n";
	html << "<ul>\n";

	DIR* dir = opendir(directoryPath.c_str());
	if (dir != NULL)
	{
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
		{
			// Filtra "." y ".."
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			// Construye el enlace al archivo/directorio
			html << "<li><a href=\"" << directoryPath + "/"+entry->d_name << "\">" << entry->d_name << "</a></li>\n";
		}
		closedir(dir);
	}
	else
		html << "<p>Error opening directory.</p>\n";

	html << "</ul>\n</body>\n</html>";
	return html.str();
}

std::string buildResourcePathForPost(HttpRequest& request,
	const Location& location, VirtualServers& server)
{
	std::string requestURL = request.getURL();

	// Eliminar parámetros de consulta
	size_t queryPos = requestURL.find('?');
	if (queryPos != std::string::npos)
		requestURL = requestURL.substr(0, queryPos);

	std::string basePath = location.getRootLocation().empty() ? server.getRoot() : location.getRootLocation();

	if (basePath != "/" && !basePath.empty() && basePath[basePath.length() - 1] == '/')
		basePath.erase(basePath.length() - 1);

	if (!requestURL.empty() && requestURL[0] != '/' && basePath != "/")
		requestURL = "/" + requestURL;
	
	std::string resourcePath = basePath + requestURL;

	// Prevenir la creación de archivos fuera del directorio raíz
	if (!request.startsWith(resourcePath, basePath))
		return "";

	return resourcePath;
}

std::string adjustPathForDirectory(const std::string& requestURL, const std::string& basePath,
										const Location& location, VirtualServers& server)
{
	if (ConfigFile::fileExistsAndReadable(requestURL))
			return requestURL;

	std::string fullPath = basePath;
	if (requestURL != "/")
		fullPath += requestURL;
	
	if (ConfigFile::checkPath(fullPath) == IS_DIR)
	{
		if (location.getAutoindex())
			return fullPath;
		std::string indexFile;
		if (location.getIndexLocation().empty() && requestURL == "/")
		{
			indexFile = server.getIndex();
			fullPath = server.getRoot() + indexFile;
		}
		else
		{
			indexFile = location.getIndexLocation();
			fullPath += indexFile;
		}
		if (ConfigFile::fileExistsAndReadable(fullPath))
			return fullPath;
		else
			return requestURL;
	}
	return fullPath;
}

std::string buildResourcePath(HttpRequest& request,
	const Location& location, VirtualServers& server)
{
	// Extraer la URL de la solicitud
	std::string requestURL = request.getURL();

	// Eliminar cualquier parámetro de consulta de la URL
	size_t queryPos = requestURL.find('?');
	if (queryPos != std::string::npos)
		requestURL = requestURL.substr(0, queryPos);

	std::string basePath = location.getRootLocation().empty() ? server.getRoot() : location.getRootLocation();

	// Ajustar la ruta del recurso para manejo de directorios
	std::string resourcePath =
		adjustPathForDirectory(requestURL, basePath, location, server);
	
	return resourcePath;
}

std::string bodyReturn(const std::string cad, const std::string& url, int statusCode)
{
	HttpResponse r;
	std::stringstream ss;
	ss << statusCode;

	std::string body = "<html>\n<head>\n<title>";
	body += ss.str() + " " +r.getStatusMessage(statusCode) + "</title>\n</head>\n";
	body += "<body>\n<h1>" + ss.str() + " " + r.getStatusMessage(statusCode) + "</h1>\n";
	if (url != "")
		body += "<p><h2><font color=\"green\">Redirecting to <a href=\"" + url + "\">" + url + "</a></font></h2></p>\n";
	else
		body += "<p><h2><font color=\"red\">" + cad + "</font></h2></p>\n";
	body += "</body>\n</html>";
	return body;
}

bool checkOpenPorts(std::vector<Socket*> _serverSockets, VirtualServers server)
{
	if (_serverSockets.empty())
		return true;
	for (size_t i = 0; i < _serverSockets.size(); ++i)
	{
		if (htons(_serverSockets[i]->getSocketAddr().sin_port) == server.getPort())
			return false;
	}
	return true;
}

std::string createBodyErrorPage(short &errorCode)
{
	HttpResponse msg;
	std::ostringstream errorCodeS;
	errorCodeS << errorCode;
	std::string errorPage = "<html>\n<head>\n<title>Error " + errorCodeS.str() + "</title>\n</head>\n";
		errorPage += "<body>\n<h1>Error " + errorCodeS.str() + "</h1>\n";
		errorPage += "<p>" + errorCodeS.str() + " " + msg.getStatusMessage(errorCode) + "</p>\n";
		errorPage += "</body>\n</html>";
	return errorPage;
}

bool areAddressesEqual(const sockaddr_in& addr1, const sockaddr_in& addr2)
{
	return (addr1.sin_addr.s_addr == addr2.sin_addr.s_addr) &&
		(addr1.sin_port == addr2.sin_port);
}

std::string removePortFromHost(const std::string& hostName)
{
	size_t colonPosition = hostName.find(':');
	if (colonPosition != std::string::npos)
		return hostName.substr(0, colonPosition);
	return hostName;
}

VirtualServers getBestServer(HttpRequest &request, size_t i, std::vector<VirtualServers> servers
	, std::vector<Socket*> _clientSockets, std::vector<struct pollfd> _pollFds)
{
	size_t j = 0;

	while (j < _clientSockets.size() && _clientSockets[j]->getSocketFd() != _pollFds[i].fd)
	{
		j++;
	}
	
	if (j == _clientSockets.size())
	{
		VirtualServers aServer;
		return (aServer);
	}
	
	int nbServer = 0;
	std::vector<int> candidates(servers.size(), 0);
	for (long unsigned k = 0; k < servers.size(); k++)
	{
		if (inet_ntoa(servers[k].getIpAddress()) == inet_ntoa(_clientSockets[j]->getSocketAddr().sin_addr)
			&& servers[k].getPort() == _clientSockets[j]->getListenPort())
		{
			nbServer++;
			candidates[k] = 1;
		}
	}
	if (nbServer == 1)
	{
		for (long unsigned k = 0; k < servers.size(); k++)
		{
			if (candidates[k] == 1)
				return servers[k];
		}
	}
	if (nbServer > 1)
	{
		for (long unsigned k = 0; k < servers.size(); k++)
		{
			if (servers[k].getPort() == _clientSockets[j]->getListenPort() && candidates[k] != 1)
			{
				nbServer++;
				candidates[k] = 1;
			}
		}
		if (nbServer == 1)
		{
			for (long unsigned k = 0; k < servers.size(); k++)
			{
				if (candidates[k] == 1)
					return servers[k];
			}
		}
		if (nbServer > 1)
		{
			std::string requestHostName = removePortFromHost(request.getHost());
			requestHostName.erase(std::remove(requestHostName.begin(), requestHostName.end(), '\n'), requestHostName.end());
			requestHostName.erase(std::remove(requestHostName.begin(), requestHostName.end(), '\r'), requestHostName.end());
			for (long unsigned k = 0; k < servers.size(); k++)
			{
				std::string serverName = servers[k].getServerName();
				if (candidates[k] == 1 && serverName == requestHostName)
					return servers[k];
			}
			for (long unsigned k = 0; k < servers.size(); k++)
			{
				std::string serverName = servers[k].getServerName();
				if (candidates[k] == 1 && serverName == requestHostName)
					return servers[k];
			}
			for (long unsigned k = 0; k < servers.size(); k++)
			{
				if (candidates[k] == 1)
						return servers[k];
			}
		}
	}
	for (long unsigned k = 0; k < servers.size(); k++)
	{
		if (candidates[k] == 1)
		{
			if (servers[k].getDefaultServer())
				return servers[k];
		}
	}
	return servers[0];
}
