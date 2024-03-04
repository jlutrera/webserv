/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 12:38:56 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/01 11:15:58 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

//*******************************************************************
// Constructores y destructor de la clase canónica
//*******************************************************************
ConfigParser::ConfigParser() { _nbServer = 0; }

ConfigParser::ConfigParser(const ConfigParser &other)
{
	_nbServer = other._nbServer;
	_serverConfig = other._serverConfig;
}

ConfigParser &ConfigParser::operator=(const ConfigParser &other)
{
	if (this == &other)
		return (*this);
	_nbServer = other._nbServer;
	_serverConfig = other._serverConfig;
	return (*this);
}

ConfigParser::~ConfigParser() { }

//*******************************************************************
// Getters
//*******************************************************************
std::vector<VirtualServers> ConfigParser::getServers() { return _servers; }

//*******************************************************************
// Métodos de la clase
//*******************************************************************
// Remove comments from char # to \n and empty lines
void removeCommentsAndEmptyLines(std::string &content)
{
    std::string result;
    std::istringstream iss(content);
    std::string line;

    while (std::getline(iss, line))
	{
        size_t firstCharPos = line.find_first_not_of(" \t");
        if (firstCharPos != std::string::npos)
		{
			int comment = line.find('#');
			if (comment != -1)
				line = line.substr(firstCharPos, comment - firstCharPos);
			else
				line = line.substr(firstCharPos);
			result += line;
			if (line != "")
				result += '\n';	
        }
    }
	if (!result.empty() && result[result.length() - 1] == '\n')
		result.erase(result.length() - 1);
    content = result;
}

// Finding the keyword "server" and returning the index of "{" (start of server)
size_t findStartServer (size_t start, std::string &content)
{
	size_t i;

	for (i = start; content[i]; i++)
	{
		if (content[i] == 's')
			break ;
		if (!isspace(content[i]))
			throw ConfigParser::ErrorException("Wrong character out of server scope{}");
	}
	if (!content[i])
		return (start);
	if (content.compare(i, 6, "server") != 0)
		throw ConfigParser::ErrorException("Wrong character out of server scope{}");
	i += 6;
	while (content[i] && isspace(content[i]))
		i++;
	if (content[i] == '{')
		return (i);
	else
		throw  ConfigParser::ErrorException("Wrong character out of server scope{}");
}

// Finding "}" (the end of a server) and returning its index
size_t findEndServer (size_t start, std::string &content)
{
	size_t	i;
	size_t	scope;
	
	scope = 0;
	for (i = start + 1; content[i]; i++)
	{
		if (content[i] == '{')
			scope++;
		if (content[i] == '}')
		{
			if (!scope)
				return (i);
			scope--;
		}
	}
	return (start);
}

// Spliting servers on separetly strings in vector
void ConfigParser::splitServers(std::string &content)
{
	size_t start = 0;
	size_t end = 1;

	removeCommentsAndEmptyLines(content);
	if (content.find("server", 0) == std::string::npos)
		throw ErrorException("Server did not find");
	while (start != end && start < content.length())
	{
		start = findStartServer(start, content);
		end = findEndServer(start, content);
		if (start == end)
			throw ErrorException("There is a problem with scope");
		_serverConfig.push_back(content.substr(start, end - start + 1));
		_nbServer++;
		start = end + 1;
	}
}

// The main function
std::string ConfigParser::loadFile(const std::string &configFile)
{
	std::string content;
	ConfigFile	file(configFile);

	// Checking and read config file
	if (file.checkPath(file.getPath()) == -1)
		throw ErrorException("File is invalid");
	if (!file.fileExistsAndReadable(file.getPath()))
		throw ErrorException("File is not accessible");
	content =  file.readFile(configFile);
	if (content.empty())
		throw ErrorException("File is empty");
	return (content);
}

int ConfigParser::initParser(std::string &content)
{
	//Splitting servers to strings
	splitServers(content);

	if (this->_serverConfig.size() != this->_nbServer)
		throw ErrorException("There is a problem with server configuration");
	// Creating vector of servers
	for (size_t i = 0; i < this->_nbServer; i++)
	{
		VirtualServers server(_serverConfig[i]);
		this->_servers.push_back(server);
	}
	return (0);
}
