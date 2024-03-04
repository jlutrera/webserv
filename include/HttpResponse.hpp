/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 13:25:48 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/01 12:19:32 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "WebServer.hpp"

class HttpResponse
{
	private:
		int _statusCode;
		std::map<std::string, std::string> _headers;
		std::string _body;

	public:
		HttpResponse();
		~HttpResponse();
		HttpResponse(const HttpResponse& copy);
		HttpResponse& operator=(const HttpResponse& other);

		void 		setStatusCode(int code);
		void		setBody(const std::string& bodyContent);
		void		setHeader(const std::string& key, const std::string& value);

		std::string getBody();
		int 		getStatusCode();
		std::string getStatusMessage(int statusCode);

		std::string	buildResponse();
		void		printResponse(std::string responseStr);
};

#endif // HTTPRESPONSE_HPP
