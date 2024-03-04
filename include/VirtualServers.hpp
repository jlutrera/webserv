/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServers.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 12:41:20 by adpachec          #+#    #+#             */
/*   Updated: 2024/01/22 12:41:20 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VIRTUALSERVERS_HPP
# define VIRTUALSERVERS_HPP

# include "WebServer.hpp"
# include "Location.hpp"
# include "ConfigFile.hpp"

class VirtualServers
{
	private:
		uint16_t				_port;
		in_addr					_ipAddress;
		std::string				_serverName;
		std::string				_root;
		std::string				_index;
		bool					_autoindex;
		std::vector<Location>	_locations;
		unsigned long			_clientMaxBodySize;
		bool					_defaultServer;
		std::map<short, std::string>	_errorPages;
		
		void _createServer(std::string &config, VirtualServers &server);
		void _checkServer(VirtualServers &server);
	
	public:
		VirtualServers();
		VirtualServers(std::string &config);
		VirtualServers(const VirtualServers &other);
		~VirtualServers();

		VirtualServers &operator=(const VirtualServers &other);	

		const uint16_t 			&getPort();
		const std::string 		&getServerName();
		const std::string 		&getRoot();
		const std::string 		&getIndex();
		const bool 				&getAutoindex();
		const std::vector<Location>	&getLocations();
		const std::string 		getErrorPage(short i);
		const unsigned long 	&getClientMaxBodySize();
		const in_addr 			&getIpAddress();
		const bool 				&getDefaultServer();
		const std::map<short, std::string> &getErrorPages();

		void setPort(std::string parametr);
		void setServerName(std::string parametr);
		void setRoot(std::string parametr);
		void setIndex(std::string parametr);
		void setAutoindex(std::string parametr);
		void setErrorPage(short i, std::string parametr);
		void setClientMaxBodySize(std::string parametr);
		void setIpAddress(std::string parametr);
		void setLocation(std::vector<std::string> &parametrs, long unsigned &i);
	
		class ErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ErrorException(std::string message) throw()
				{
					_message = "CONFIG SERVER ERROR: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};
};

#endif // VIRTUALSERVERS_HPP