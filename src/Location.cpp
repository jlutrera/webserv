/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 12:39:09 by adpachec          #+#    #+#             */
/*   Updated: 2024/01/22 12:39:09 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

//*******************************************************************
// Constructores y destructor de la clase canónica
//*******************************************************************
Location::Location() { }
Location::Location(const Location &other)
{
	_path = other._path;
	_root = other._root;
	_autoindex = other._autoindex;
	_index = other._index;
	_cgiPath = other._cgiPath;
	_cgiExt = other._cgiExt;
	_return = other._return;
    _methods = other._methods;
	_modifier = other._modifier;
	_clientMaxBodySize = other._clientMaxBodySize;
}

Location &Location::operator=(const Location &other)
{
	if (this != &other)
	{
		_path = other._path;
		_root = other._root;
		_autoindex = other._autoindex;
		_index = other._index;
		_cgiPath = other._cgiPath;
		_cgiExt = other._cgiExt;
		_return = other._return;
		_methods = other._methods;
		_modifier = other._modifier;
		_clientMaxBodySize = other._clientMaxBodySize;
    }
	return (*this);
}

Location::~Location() { }

Location::Location(std::string &path, std::string &modifier, std::vector<std::string> &parametr)
{
	_path = path;
	_root = "";
	_autoindex = false;
	_index = "";
	_return.reserve(2);
	_return.push_back("");
	_return.push_back("");
	_modifier = modifier;
	_clientMaxBodySize = MAX_CONTENT_LENGTH;

	//Métodos implementados
	_methods.reserve(3);
	_methods.push_back(1); // GET
	_methods.push_back(1); // POST
	_methods.push_back(1); // DELETE

	// Configuración de los parámetros de la localización
	configureLocation(path, parametr);
}

//*******************************************************************
// Getters
//*******************************************************************
const std::string &Location::getModifier() const { return (_modifier); }

const std::string &Location::getPath() const { return (_path); }

const std::string &Location::getRootLocation() const { return (_root); }

const std::string &Location::getIndexLocation() const { return (_index); }

const std::vector<short> &Location::getMethods() const { return (_methods); }

const std::vector<std::string> &Location::getCgiPath() const { return (_cgiPath); }

const std::vector<std::string> &Location::getCgiExtension() const { return (_cgiExt); }

const bool &Location::getAutoindex() const { return (_autoindex); }

const std::vector<std::string> &Location::getReturn() const { return (_return); }

const std::string Location::getExtensionCgiPath(std::string &ext) const 
{
	std::cout << "    Looking for " << ext << std::endl;
	std::vector<std::string>::const_iterator it;

	for (it = _cgiPath.begin(); it != _cgiPath.end(); ++it)
	{
		if ((ext.find(".py") != std::string::npos && it->find("python") != std::string::npos) ||
			(ext == ".php" && it->find("php") != std::string::npos)   ||
			(ext == ".pl" && it->find("perl") != std::string::npos)   ||
			(ext == ".rb" && it->find("ruby") != std::string::npos)   ||
			(ext == ".sh" && it->find("bash") != std::string::npos) ) 
			break;
	}
	if (it != _cgiPath.end())
		return (*it);
	return ("");
}

const unsigned long &Location::getMaxBodySize() const { return (_clientMaxBodySize); }

const std::string Location::getErrorPage(short i) const
{
	std::map<short, std::string>::const_iterator it = _errorPages.find(i);
	if (it != _errorPages.end())
		return (it->second);
	return ("");
 }

const std::map<short, std::string> &Location::getErrorPages() const { return (_errorPages); }

 //*******************************************************************
 // Setters
 //*******************************************************************
void Location::setPath(std::string parametr) { _path = parametr; }

void Location::setRootLocation(std::string parametr)
{
	checkToken(parametr);
	parametr = ConfigFile::prefixPath(parametr);
	_root = parametr;
}

void Location::setMethods(std::vector<std::string> methods)
{
	_methods[GET_METHOD] = 0;
	_methods[POST_METHOD] = 0;
	_methods[DELETE_METHOD] = 0;

	for (size_t i = 0; i < methods.size(); i++)
	{
		if (methods[i] == "GET")
			_methods[GET_METHOD] = 1;
		else if (methods[i] == "POST")
			_methods[POST_METHOD] = 1;
		else if (methods[i] == "DELETE")
			_methods[DELETE_METHOD] = 1;
		else
			throw ErrorException("Method not supported " + methods[i]);
	}
}

void Location::setAutoindex(std::string parametr) { _autoindex = (parametr == "on"); }

void Location::setIndexLocation(std::string parametr)
{
	checkToken(parametr);
	if (getIndexLocation().empty())
		_index = ConfigFile::adjustName(parametr);
}

void Location::setReturn(std::string parametr1, std::string parametr2)
{

	if (!parametr2.empty())
		checkToken(parametr2);
	else
	{
		checkToken(parametr1);
		int code = ft_stoi(parametr1);
		if (code < 100 || code > 599)
			throw ErrorException("Invalid return code");
	}
	_return[0] = parametr1; // Código de redirección	
	_return[1] = parametr2; // Ruta de redirección
}

void Location::setCgiPath(std::vector<std::string> &path) { _cgiPath = path; }

void Location::setCgiExtension(std::vector<std::string> &extension) { _cgiExt = extension; }

void Location::setMaxBodySize(std::string parametr) { _clientMaxBodySize = ft_stoi(parametr); }

void Location::setMaxBodySize(unsigned long parametr) { _clientMaxBodySize = parametr; }

void Location::setModifier(std::string parametr) { _modifier = parametr; }

void Location::setErrorPage(short i, std::string parametr)
{
	_errorPages[i] = ConfigFile::prefixPath(parametr);
}

void Location::setErrorPages(std::map<short, std::string> errorPages) { _errorPages = errorPages; }

//*******************************************************************
// Métodos de la clase
//*******************************************************************
void Location::configureLocation(std::string &path, std::vector<std::string> &parametr)
{
	std::vector<std::string>	methods;
	std::string errorCodePath;
	short errorCode;
	bool flag_methods = false;
	bool flag_autoindex = false;

	for (size_t i = 0; i < parametr.size(); i++)
	{
		if (parametr[i] == "error_page" && (i + 1) < parametr.size())
		{
			errorCode = ft_stoi(parametr[++i]);
			checkToken(parametr[++i]);	
			errorCodePath = parametr[i];
			setErrorPage(errorCode, errorCodePath);
		}
		else if (parametr[i] == "root" && (i + 1) < parametr.size())
		{
			++i;
			if (getRootLocation().empty())
				setRootLocation(parametr[i]);
		}
		
		else if ((parametr[i] == "allow_methods" || parametr[i] == "methods") && (i + 1) < parametr.size())
		{
			if (flag_methods)
				std::cerr << "Allow_methods of location is duplicated" << std::endl;
			std::vector<std::string> methods;
			while (++i < parametr.size())
			{
				if (parametr[i].find(";") != std::string::npos)
				{
					checkToken(parametr[i]);
					methods.push_back(parametr[i]);
					break ;
				}
				else
				{
					methods.push_back(parametr[i]);
					if (i + 1 >= parametr.size())
						throw ErrorException("Token is invalid");
				}
			}
			setMethods(methods);
			flag_methods = true;
		}
		else if (parametr[i] == "autoindex" && (i + 1) < parametr.size())
		{
			if (path == "/cgi-bin")
				throw ErrorException("Parametr autoindex not allow for CGI");
			if (flag_autoindex)
				std::cerr << "Autoindex of location is duplicated" << std::endl;
			checkToken(parametr[++i]);
			if (parametr[i] != "on" && parametr[i] != "off")
				throw ErrorException("Index of location is invalid");
			setAutoindex(parametr[i]);
			flag_autoindex = true;
		}
		else if (parametr[i] == "index" && (i + 1) < parametr.size())
		{
			++i;
			setIndexLocation(parametr[i]);
		}
		else if (parametr[i] == "return" && (i + 1) < parametr.size())
		{
			if (path == "/cgi-bin")
				throw ErrorException("Parametr return not allow for CGI");
			std::string codeString = parametr[++i];
			if (codeString.find(";") != std::string::npos)
				setReturn(codeString, "");
			else
				setReturn(codeString, parametr[++i]);
			break;
		}
		else if (parametr[i] == "cgi_ext" && (i + 1) < parametr.size())
		{
			std::vector<std::string> extension;
			while (++i < parametr.size())
			{
				if (parametr[i].find(";") != std::string::npos)
				{
					checkToken(parametr[i]);
					extension.push_back(parametr[i]);
					break ;
				}
				else
				{
					extension.push_back(parametr[i]);
					if (i + 1 >= parametr.size())
						throw ErrorException("Token is invalid");
				}
			}
			setCgiExtension(extension);
		}
		else if (parametr[i] == "cgi_path" && (i + 1) < parametr.size())
		{
			std::vector<std::string> path;
			while (++i < parametr.size())
			{
				if (parametr[i].find(";") != std::string::npos)
				{
					checkToken(parametr[i]);
					path.push_back(parametr[i]);
					break ;
				}
				else
				{
					path.push_back(parametr[i]);
					if (i + 1 >= parametr.size())
						throw ErrorException("Token is invalid");
				}
			}
			setCgiPath(path);
		}
		else
			throw ErrorException("Invalid location directive --> " + parametr[i]);
	}
}

void Location::checkToken(std::string &parametr)
{
	size_t pos = parametr.rfind(';');
	if (pos != parametr.size() - 1)
		throw ErrorException("Token is invalid");
	parametr.erase(pos);
}

int Location::ft_stoi(std::string str)
{
    std::stringstream ss(str);
    if (str.length() > 10)
        throw ErrorException("Number is too big");
    for (size_t i = 0; i < str.length(); ++i)
    {
        if(!isdigit(str[i]))
            throw ErrorException("Invalid number");
    }
    int res;
    ss >> res;
    return (res);
}

int Location::checkLocation(Location &location, std::string serverRoot, std::string serverIndex)
{
	std::string path = location.getPath();
	std::cout << "location " << path << std::endl;
	
	if (path == "/")
		path = "";
	
	if (location.getRootLocation().empty())
		location.setRootLocation(serverRoot + ";");

	serverIndex = " ";
	std::string root =  location.getRootLocation();
	
	if (path != "/cgi-bin")
	{
		if (!location.getReturn()[1].empty())
		{
			std::string newpath = location.getReturn()[1];	
			if (newpath[0] == '/')
			{
				std::cout << "         root from the " << path << std::endl;
				std::cout << "         new path  = " << newpath << std::endl;
			}	
		}
		else
		{
			std::string fullpath = root + path + location.getIndexLocation();
			std::cout << "         root  = " << root << std::endl;
			std::cout << "         index = " << fullpath << std::endl;
			if (!ConfigFile::fileExistsAndReadable(fullpath))
				return (5);
		}
	}
	else
	{
		if (location.getCgiPath().empty() || location.getCgiExtension().empty() ||
			location.getCgiPath().size() != location.getCgiExtension().size())
			return (1);
		//index is not neccessary for cgi-bin, so we don't check it
		std::vector<std::string>::const_iterator it_ext;
		for (it_ext = location.getCgiExtension().begin(); it_ext != location.getCgiExtension().end(); ++it_ext)
		{
			std::cout << " 	  cgi extension = " << *it_ext << " --> ";
			if (*it_ext != ".py" && *it_ext != ".php" && *it_ext != ".sh" && *it_ext != ".rb" && *it_ext != ".pl")
			{
					std::cout << RED << "extension not supported" << RESET << std::endl;
					return (1);
			}
			std::vector<std::string>::const_iterator it_path;
			for (it_path = location.getCgiPath().begin(); it_path != location.getCgiPath().end(); ++it_path)
			{		
				if (it_path->find("python") != std::string::npos && *it_ext ==".py" && ConfigFile::checkPath(*it_path) == IS_FILE)
				{
					std::cout << "python path = " << *it_path << GREEN << " OK !" << RESET << std::endl;
					break;
				}
				else if (it_path->find("php") != std::string::npos && *it_ext == ".php" && ConfigFile::checkPath(*it_path) == IS_FILE)
				{
					std::cout << "php path = " << *it_path << GREEN << " OK !" << RESET << std::endl;
					break;
				}
				else if (it_path->find("bash") != std::string::npos && *it_ext == ".sh" && ConfigFile::checkPath(*it_path) == IS_FILE)
				{
					std::cout << "bash path = " << *it_path << GREEN << " OK !" << RESET << std::endl;
					break;
				}
				else if (it_path->find("ruby") != std::string::npos && *it_ext == ".rb" && ConfigFile::checkPath(*it_path) == IS_FILE)
				{
					std::cout << "ruby path = " << *it_path << GREEN << " OK !" << RESET << std::endl;
					break;
				}
				else if (it_path->find("perl") != std::string::npos && *it_ext == ".pl" && ConfigFile::checkPath(*it_path) == IS_FILE)
				{
					std::cout << "perl path = " << *it_path << GREEN << " OK !" << RESET << std::endl;
					break;
				}
			}
			if (it_path == location.getCgiPath().end())
			{
				std::cout << RED << "path not found" << RESET << std::endl;
				return (1);
			}
		}
	}
	return (0);
}

const Location* Location::selectLocation(const std::string& requestURI,
	const std::vector<Location>& locations)
{
	const Location* exactMatch = findExactMatch(requestURI, locations);
	if (exactMatch)
		return exactMatch;

	const Location* longestPrefixMatch = findLongestPrefixMatch(requestURI, locations);
	if (longestPrefixMatch && longestPrefixMatch->getModifier() == "^~")
		return longestPrefixMatch;

	return longestPrefixMatch;
}

const Location* Location::findExactMatch(const std::string& requestURI,
	const std::vector<Location>& locations)
{
	for (size_t i = 0; i < locations.size(); ++i)
	{
		if ((locations[i].getModifier() == "=") && locations[i].getPath() == requestURI)
			return &locations[i];
	}
	return NULL;
}

const Location* Location::findLongestPrefixMatch(const std::string& requestURI,
	const std::vector<Location>& locations)
{
	const Location* longestMatch = NULL;
	size_t longestLength = 0;

	for (size_t i = 0; i < locations.size(); ++i)
	{
		if ((locations[i].getModifier() == "^~" || locations[i].getModifier().empty())
			&& startsWith(requestURI, locations[i].getPath()) &&
				locations[i].getPath().length() > longestLength)
		{
			longestMatch = &locations[i];
			longestLength = locations[i].getPath().length();
			if (!locations[i].getReturn()[0].empty())
				return &locations[i];
		}	
	}
	return longestMatch;
}

bool Location::startsWith(const std::string& str, const std::string& prefix)
{
    return str.substr(0, prefix.size()) == prefix;
}

std::string Location::printMethods() const
{
	std::string res;
	if (_methods[DELETE_METHOD])
	{
		if (!res.empty())
			res.insert(0, ", ");
		res.insert(0, "DELETE");
	}
	if (_methods[POST_METHOD])
	{
		if (!res.empty())
			res.insert(0, ", ");
		res.insert(0, "POST");
	}
	if (_methods[GET_METHOD])
	{
		if (!res.empty())
			res.insert(0, ", ");
		res.insert(0, "GET");
	}
	return (res);
}
