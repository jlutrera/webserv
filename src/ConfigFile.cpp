/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 12:38:47 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/01 11:15:23 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

//*******************************************************************
// Constructores y destructor de la clase canónica
//*******************************************************************
ConfigFile::ConfigFile() { }

ConfigFile::ConfigFile(const ConfigFile &other) { _path = other._path; }

ConfigFile &ConfigFile::operator=(const ConfigFile &other)
{
	if (this == &other)
		return (*this);
	_path = other._path;
	return (*this);
}
ConfigFile::ConfigFile(std::string const path) : _path(path) { }

ConfigFile::~ConfigFile() { }

//*******************************************************************
// Getters
//*******************************************************************
std::string ConfigFile::getPath() {	return (_path); }

//*******************************************************************
// Métodos de la clase
//*******************************************************************
std::string ConfigFile::prefixPath(std::string const path)
{
	std::string name = path;
	if (path[path.length() - 1] == '/')
		name = path.substr(0, path.length() - 1);
	if (path[0] == '.')
		return (name);
	else if (path[0] == '/')
	{
		std::string newPath = '.' + path;
		return (newPath);
	}
	return ("./" + name);
}

// Check if path is a file(1) or a folder (2)
// or something else (3) or doesn't exist (-1)
int ConfigFile::checkPath(std::string path)
{
	struct stat	buffer;

	if (stat(path.c_str(), &buffer) != 0)
		return (-1);
	
	if (buffer.st_mode & S_IFREG)
		return (IS_FILE); //is file

	if (buffer.st_mode & S_IFDIR)
		return (IS_DIR); //is folder

	return (3); //is something else
}

// Read from file to string
std::string	ConfigFile::readFile(std::string path)
{
	if (path.empty() || path.length() == 0)
		return (NULL);

	std::string expath = prefixPath(path);
	std::ifstream config_file(expath.c_str());
	if (!config_file || !config_file.is_open())
		return (NULL);
	
	std::stringstream stream_binding;
	stream_binding << config_file.rdbuf();
	return (stream_binding.str());
}

// Check if file exists and is readable
bool ConfigFile::fileExistsAndReadable(const std::string& filePath)
{
	struct stat buffer;
	std::string path;

	if (filePath[0] != '.' && filePath[0] == '/')
		path = "." + filePath;
	else
		path = filePath;

	std::string exFilePath = prefixPath(path);

	if (stat(exFilePath.c_str(), &buffer) == 0)
		return S_ISREG(buffer.st_mode) && (access(exFilePath.c_str(), R_OK) == 0);
	return false;
}

// Adjust name to be used in the server
// it will be /name for files
std::string ConfigFile::adjustName(std::string const name)
{
	std::string exName = name;

	if (name == "/")
		return (name);
	//Quito / del final, si lo hubiera
	if (name[name.length() - 1] == '/')
		exName = name.substr(0, name.length() - 1);
	//Añado / del principio, si no lo hubiera
	if (name[0] != '/')
		exName = "/" + exName;

	return (exName);
}