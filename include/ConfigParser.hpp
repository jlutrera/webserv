/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 12:40:57 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/01 11:15:45 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "WebServer.hpp"
# include "VirtualServers.hpp"
# include "ConfigFile.hpp"

class ConfigParser
{
	private:
		std::vector<VirtualServers>	_servers;
		std::vector<std::string>	_serverConfig;
		size_t						_nbServer;
		
	public:
		ConfigParser();
		~ConfigParser();
		ConfigParser(const ConfigParser &other);
		ConfigParser &operator=(const ConfigParser &other);

		std::vector<VirtualServers> getServers();
		
		std::string loadFile(const std::string &configFile);
		int			initParser(std::string &configFile);
		void		splitServers(std::string &content);
		
		class ErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ErrorException(std::string message) throw()
				{
					_message = "CONFIG PARSER ERROR: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};
};

#endif //CONFIGPARSER_HPP