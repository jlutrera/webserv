/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 10:44:37 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/01 12:18:55 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "WebServer.hpp"

class HttpRequest
{
	private:
		std::string _method;
		std::string _url;
		std::string _httpVersion;
		std::string _body;
		std::string _errorMessage;
		bool		_isValid;
		bool		_isComplete;
		bool		_isKeepAlive;
		std::map<std::string, std::string> _headers;

		void _parseRequest(const std::string& rawRequest);
		void _parseFirstLine(const std::string& line);
		void _parseHeaders(const std::string& headersStr);
	
	public:
		HttpRequest();
		HttpRequest(const std::string& rawRequest);
		~HttpRequest();
		HttpRequest(const HttpRequest& copy);
		HttpRequest& operator=(const HttpRequest& other);

		std::string getMethod();
		std::string getHost();
		std::string getURL();
		std::string getHttpVersion();
		std::string getBody();
		std::map<std::string, std::string> getHeaders();
		std::string getHeader(const std::string& header_name) const;
		bool		getIsValidRequest();
		bool 		getIsCompleteRequest();
		bool 		getIsKeepAlive();
		std::string	getErrorMessage();

		void		setValidRequest(bool validity);
		void		setCompleteRequest(bool complete);
		
		void		invalidRequest();
		bool 		startsWith(const std::string& str, const std::string& prefix);
		void		printRequest();
};

#endif // HTTPREQUEST_HPP