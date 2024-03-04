/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 11:50:52 by adpachec          #+#    #+#             */
/*   Updated: 2024/02/16 12:26:29 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_UTILS_HPP
#define SERVER_UTILS_HPP

#include "WebServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "VirtualServers.hpp"
#include "Socket.hpp"
#include "Location.hpp"

std::string 	getMimeType(const std::string& filePath);
bool 			isValidPath(const std::string& basePath, const std::string& path);
bool 			isCGIScript(const std::string& resourcePath);
std::string 	getFilename(HttpRequest request, std::string resourcePath);
std::string 	getFilenameCGI(HttpRequest request);
std::string 	generateDirectoryIndex(const std::string& directoryPath);
std::string 	buildResourcePathForPost(HttpRequest& request, 
				const Location& location, VirtualServers& server);
std::string 	adjustPathForDirectory(const std::string& requestURL, const std::string& basePath,
				const Location& location, VirtualServers& server);
std::string 	buildResourcePath(HttpRequest& request, const Location& location,
				 VirtualServers& server);
std::string 	bodyReturn(const std::string cad, const std::string& url, int statusCode);
bool 			checkOpenPorts(std::vector<Socket*> _serverSockets, VirtualServers server);
std::string 	createBodyErrorPage(short &errorCode);
bool 			areAddressesEqual(const sockaddr_in& addr1, const sockaddr_in& addr2);
VirtualServers 	getBestServer(HttpRequest &request, size_t i, std::vector<VirtualServers> servers,
				std::vector<Socket*> _clientSockets, std::vector<struct pollfd> _pollFds);



#endif // SERVER_UTILS_HPP