/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 12:34:17 by adpachec          #+#    #+#             */
/*   Updated: 2024/01/22 16:44:17 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ConfigParser.hpp"
#include "WebServer.hpp"

std::string buildDefault()
{
   std::ostringstream configStream;

    configStream << 
        "# Default Server Configuration File\n"
        "server {\n"
        "listen 127.0.0.1:9000;\n"
        "root docs/www;\n"
		"location / {\n"
		"autoindex off;\n"
		"}\n"
		"}";
    return configStream.str();
}

int main(int argc, char **argv) 
{
	//Check the number of arguments
	if (argc != 1 && argc != 2)
	{
		std::cout << "Error: wrong arguments" << std::endl;
		return (1);
	}
		
	ConfigParser	cluster;
	try 
	{
		std::string		config;
		// Configuration file as argument or default path
		config = (argc == 1 ? buildDefault() : cluster.loadFile(argv[1]));
		cluster.initParser(config);
		//Print the configuration file parsed
		//cluster.printConfig();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	//Initialize the servers
	Server myServer(cluster.getServers());
	//Run the servers
	myServer.run(cluster.getServers());
	return (0);
}
