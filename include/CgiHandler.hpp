/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 12:40:51 by adpachec          #+#    #+#             */
/*   Updated: 2024/02/26 12:07:25 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "WebServer.hpp"
# include "VirtualServers.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

class CgiHandler
{
	private:
		std::map<std::string, std::string>	_env;
		std::string							_body;
		void								_initEnv( HttpRequest &request,
												const Location &config, 
													  VirtualServers &server);
		char**								_getEnvAsCstrArray() const;

	public:
		CgiHandler(void);
		// sets up env according to the request
		CgiHandler(HttpRequest &request, const Location &config, VirtualServers &server);
		CgiHandler(CgiHandler const &src);
		virtual ~CgiHandler();
		CgiHandler	&operator=(CgiHandler const &src);
		
		// executes cgi and returns body or error
		std::string	executeCgi(std::string const scriptName, std::string const pathCGI);
};

#endif
