/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 13:27:01 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/01 12:20:54 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

//*******************************************************************
// Constructores y destructor de la clase canónica
//*******************************************************************
HttpResponse::HttpResponse(void) { _statusCode = 200; }

HttpResponse::~HttpResponse() {}

HttpResponse::HttpResponse(const HttpResponse& copy)
{
	_statusCode = copy._statusCode;
	_headers = copy._headers;
	_body = copy._body;
}

HttpResponse& HttpResponse::operator=(const HttpResponse& other)
{
	if (this != &other)
	{
		_statusCode = other._statusCode;
		_headers = other._headers;
		_body = other._body;
	}
	return *this;
}

//*******************************************************************
// Setters
//*******************************************************************
void HttpResponse::setHeader(const std::string& key, const std::string& value)
{
	_headers[key] = value;
}

void HttpResponse::setBody(const std::string& bodyContent)
{
	_body = bodyContent;
}

void HttpResponse::setStatusCode(int code) { _statusCode = code; }

//*******************************************************************
// Getters
//*******************************************************************
std::string HttpResponse::getStatusMessage(int statusCode)
{
	switch (statusCode)
	{
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 202:
			return "Accepted";
		case 204:
			return "No Content";

		//in case of redirection
		case 301:
			return "Moved Permanently";
		case 302:
			return "Found";
		case 303:
			return "See Other";
		case 307:
			return "Temporary Redirect";
		case 308:
			return "Permanent Redirect";

		//in case of client error
		case 400:
			return "Bad Request";
		case 401:
			return "Unauthorized";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";
		case 408:
			return "Request Timeout";
		case 413:
			return "Payload Too Large";
			
		//in case of server error
		case 415:
			return "Unsupported Media Type";
		case 500:
			return "Internal Server Error";
		default:
			return "Unknown Server Error";
	}
}

std::string 	HttpResponse::getBody() { return _body; }

int 			HttpResponse::getStatusCode() { return _statusCode; }

//*******************************************************************
// Métodos de la clase
//*******************************************************************
std::string HttpResponse::buildResponse()
{
	std::stringstream response;

	// Obtiene el mensaje de estado correspondiente al código de estado
	std::string statusMessage = getStatusMessage(_statusCode);

	// Agrega la línea de estado (código de estado y mensaje)
	response << "HTTP/1.1 " << _statusCode << " " << statusMessage << "\r\n";
	
	// Agrega las cabeceras
	for (std::map<std::string, std::string>::const_iterator it =	
		_headers.begin(); it != _headers.end(); ++it)
		response << it->first << ": " << it->second << "\r\n";

	// Agrega la longitud del cuerpo y la línea en blanco que indica el final de las cabeceras
	response << "Content-Length: " << _body.size() << "\r\n";
	
	// Agrega el cuerpo
	response << "\r\n" << _body;

	return response.str();
}

void HttpResponse::printResponse(std::string responseStr)
{
	std::cout << "\n***** RESPONSE *****" << std::endl;

	std::cout << CYAN << responseStr << RESET << std::endl;

	std::cout << "*******************" << std::endl;
}