/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 10:49:30 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/01 12:18:59 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

//*******************************************************************
// Constructores y destructor de la clase canónica
//*******************************************************************
HttpRequest::HttpRequest(const std::string& rawRequest)
{
	this->_isValid = true;
	this->_isComplete = false;
	this->_isKeepAlive = false;
	if (rawRequest.empty())
		invalidRequest();
	else
		_parseRequest(rawRequest);
}

HttpRequest::HttpRequest() { this->_isValid = false; this->_isComplete = false; }

HttpRequest::HttpRequest(const HttpRequest& copy) {	*this = copy; }

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	if (this != &other)
	{
		this->_method = other._method;
		this->_url = other._url;
		this->_httpVersion = other._httpVersion;
		this->_headers = other._headers;
		this->_body = other._body;
		this->_errorMessage = other._errorMessage;
		this->_isValid = other._isValid;
		this->_isComplete = other._isComplete;
		this->_isKeepAlive = other._isKeepAlive;
	}
	return (*this);
}

HttpRequest::~HttpRequest() {};

//*******************************************************************
// Getters
//*******************************************************************
std::string HttpRequest::getMethod() { return (this->_method); }

std::string HttpRequest::getURL() {	return (this->_url); }

std::string HttpRequest::getHttpVersion() {	return (this->_httpVersion); }

std::map<std::string, std::string> HttpRequest::getHeaders() { return (this->_headers); }

std::string HttpRequest::getHeader(const std::string& header_name) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(header_name);
	if (it != _headers.end())
		return it->second;
	return "";
}

std::string HttpRequest::getBody() { return (this->_body); }

std::string HttpRequest::getHost()
{
	std::map<std::string, std::string>::iterator it = _headers.find("Host");
	if (it != _headers.end())
		return it->second;
	return "";
}
bool HttpRequest::getIsValidRequest() { return (this->_isValid); }

bool HttpRequest::getIsCompleteRequest() { return (this->_isComplete); }

bool HttpRequest::getIsKeepAlive() { return (this->_isKeepAlive); }

std::string HttpRequest::getErrorMessage() { return (this->_errorMessage); }

//*******************************************************************
// Setters
//*******************************************************************
void	HttpRequest::setValidRequest(bool validity) { this->_isValid = validity; }

void	HttpRequest::setCompleteRequest(bool complete) { this->_isComplete = complete; }

//*******************************************************************
// Métodos de la clase
//*******************************************************************
void HttpRequest::_parseRequest(const std::string& rawRequest)
{
	std::istringstream requestStream(rawRequest);
	std::string line;

	// Parsear la primera línea
	getline(requestStream, line);
	_parseFirstLine(line);
	if (!this->_isValid)
		return ;

	// Parsear las cabeceras
	std::string headersStr;
	while (getline(requestStream, line) && line[0] && line != "\r")
		headersStr += line + "\n";
	_parseHeaders(headersStr);
	if (!this->_isValid)
		return ;

	// Parsear el cuerpo (si existe)
	if (this->_method == "POST")
	{
		std::string bodyStr;
		while (getline(requestStream, line) && line[0])
			_body += line + "\n";
	}
}

void HttpRequest::_parseFirstLine(const std::string& line)
{
	std::istringstream lineStream(line);
	lineStream >> _method >> _url >> _httpVersion;
}

void HttpRequest::_parseHeaders(const std::string& headersStr)
{
	std::istringstream headersStream(headersStr);
	std::string line;
	while (getline(headersStream, line))
	{
		std::size_t colonPos = line.find(':');
		if (colonPos != std::string::npos && colonPos > 0)
		{
			std::string headerName = line.substr(0, colonPos);
			std::string headerValue = line.substr(colonPos + 2); // +2 para saltar el espacio después de los dos puntos
			if (headerName == "" || headerValue == "")
				return invalidRequest();
			if (headerValue.find("keep-alive") != std::string::npos)
				this->_isKeepAlive = true;
				
			_headers[headerName] = headerValue;
		}
		else
			return invalidRequest();
	}
}

void HttpRequest::invalidRequest()
{
	this->_isValid = false;
	this->_errorMessage = "Bad Request";
}

bool HttpRequest::startsWith(const std::string& str, const std::string& prefix)
{
    return str.substr(0, prefix.size()) == prefix;
}

void HttpRequest::printRequest()
{
	std::cout << "\n***** REQUEST *****" << std::endl;
	std::cout << YELLOW << "Method: " <<_method << std::endl;
	std::cout << "URL: " << _url << std::endl;
	std::cout << "HTTP Version: " << _httpVersion << std::endl;
	std::cout << "Headers: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		std::cout << "    " << it->first << ": " << it->second << std::endl;
	std::cout << "Body: [body content]" << std::endl;
	std::cout << _body << std::endl;
	std::cout << RESET << "********************" << std::endl;
}