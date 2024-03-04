/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 12:40:51 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/01 11:14:06 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

//*******************************************************************
// Constructores y destructor de la clase canónica
//*******************************************************************
CgiHandler::CgiHandler(void) { }
CgiHandler::~CgiHandler(void) { }
CgiHandler::CgiHandler(HttpRequest &request, const Location &config, VirtualServers &server)
{
	_body = request.getBody();
	this->_initEnv(request, config, server);
}
CgiHandler::CgiHandler(CgiHandler const	&src)
{
	if (this != &src)
	{
		this->_body = src._body;
		this->_env = src._env;
	}
	return ;
}
CgiHandler&	CgiHandler::operator=(CgiHandler const	&src)
{
	if (this != &src)
	{
		this->_body = src._body;
		this->_env = src._env;
	}
	return *this;
}

//*******************************************************************
// Métodos de la clase
//*******************************************************************
std::string ft_itoa(int n)
{
	std::string str;
	std::stringstream ss;
	ss << n;
	ss >> str;
	return str;
}

void	CgiHandler::_initEnv(HttpRequest &request, const Location &config, VirtualServers &server)
{
/*
CONTENT_TYPE: 	The data type of the content, used when the client is sending attached content to the server. 
				For example file upload etc.
CONTENT_LENGTH: The length of the query information that is available only for POST requests.
HTTP_USER_AGENT:The User-Agent request-header field contains information about the user agent originating the request. 
				It is a name of the web browser.
PATH_INFO: 		The path for the CGI script.
QUERY_STRING: 	The URL-encoded information that is sent with GET method request.
REMOTE_ADDR: 	The IP address of the remote host making the request. 
				This can be useful for logging or for authentication purpose.
REMOTE_HOST: 	The fully qualified name of the host making the request. 
				If this information is not available then REMOTE_ADDR can be used to get IR address.
REQUEST_METHOD:	The method used to make the request. The most common methods are GET and POST.
SCRIPT_FILENAME:The full path to the CGI script.
SCRIPT_NAME: 	The name of the CGI script.
SERVER_NAME: 	The server's hostname or IP Address.
SERVER_SOFTWARE:The name and version of the software the server is running.*/
	std::map<std::string, std::string>	headers = request.getHeaders();
	std::string url = request.getURL();
	std::string contentType = request.getBody();

	if (headers.find("Content-Type") != headers.end())
		this->_env["CONTENT_TYPE"] = headers["Content-Type"];
	else if (!contentType.empty())
		this->_env["CONTENT_TYPE"] = contentType.substr(contentType.find("Content-Type") + 13);
	else
		this->_env["CONTENT_TYPE"] = "text/plain";
	this->_env["CONTENT_LENGTH"] = ft_itoa(request.getMethod() == "GET" ? 0 : this->_body.length());
	this->_env["HTTP_USER_AGENT"] = headers["User-Agent"]; 
	this->_env["PATH_INFO"] = url.substr(0, url.find("cgi-bin") + 7);
	this->_env["QUERY_STRING"] = url.substr(url.find("?") + 1);	
	this->_env["REMOTE_ADDR"] =  inet_ntoa(server.getIpAddress());
	if (headers.find("Host") != headers.end())
		this->_env["SERVER_HOST"] = headers["Host"].substr(0, headers["Host"].find(":"));
	else
		this->_env["SERVER_HOST"] = this->_env["REMOTE_ADDR"];
	this->_env["REQUEST_METHOD"] = request.getMethod();
	this->_env["SCRIPT_FILENAME"] =  config.getRootLocation() + config.getPath();
	this->_env["SCRIPT_NAME"] =  url.substr(url.find("cgi-bin") + 7, url.find("?") - 8);
	this->_env["SERVER_NAME"] = inet_ntoa(server.getIpAddress());
	this->_env["SERVER_SOFTWARE"] = "Webserver/1.0";
}

char	**CgiHandler::_getEnvAsCstrArray() const
{
	char	**env = new char*[this->_env.size() + 1];
	int	j = 0;
	for (std::map<std::string, std::string>::const_iterator i = this->_env.begin(); i != this->_env.end(); i++) {
		std::string	element = i->first + "=" + i->second;
		env[j] = new char[element.size() + 1];
		env[j] = strcpy(env[j], (const char*)element.c_str());
		j++;
	}
	env[j] = NULL;
	return env;
}

std::string CgiHandler::executeCgi(std::string const scriptName, std::string const pathCGI)
{
	pid_t		pid;
	int			saveStdin;
	int			saveStdout;
	char		**env;
	std::string	newBody;

	try {
		env = this->_getEnvAsCstrArray();
	}
	catch (std::bad_alloc &e) {
		std::cerr << RED << e.what() << RESET << std::endl;
	}

	// SAVING STDIN AND STDOUT IN ORDER TO TURN THEM BACK TO NORMAL LATER
	saveStdin = dup(STDIN_FILENO);
	saveStdout = dup(STDOUT_FILENO);

	FILE	*fIn = tmpfile();
	FILE	*fOut = tmpfile();
	long	fdIn = fileno(fIn);
	long	fdOut = fileno(fOut);
	int		ret = 1;

	write(fdIn, _body.c_str(), _body.size());
	lseek(fdIn, 0, SEEK_SET);

	pid = fork();

	if (pid == -1)
	{
		std::cerr << RED << "Fork crashed." << RESET << std::endl;
		return ("Status: 500\r\n\r\n");
	}
	else if (!pid) // Child process
	{
		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);

		const char* argv[] = { pathCGI.c_str(), scriptName.c_str()};
		int err = access(pathCGI.c_str(), X_OK);
		if (err < 0)
		{
			std::cerr << RED << pathCGI << " don't found" << RESET << std::endl;  
			write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
		}
		execve(pathCGI.c_str(), const_cast<char* const*>(argv), env);

		// If execve fails, it will return here and print an error message
		std::cerr << RED << "Execve crashed." << RESET << std::endl;
		write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
	}
	else // Parent process
	{
		char	buffer[CGI_BUFSIZE] = {0};

		waitpid(-1, NULL, 0);
		lseek(fdOut, 0, SEEK_SET);
		ret = 1;
		while (ret > 0)
		{
			memset(buffer, 0, CGI_BUFSIZE);
			ret = read(fdOut, buffer, CGI_BUFSIZE - 1);
			newBody += buffer;
		}
	}

	// TURNING STDIN AND STDOUT BACK TO NORMAL
	dup2(saveStdin, STDIN_FILENO);
	dup2(saveStdout, STDOUT_FILENO);

	fclose(fIn);
	fclose(fOut);
	close(fdIn);
	close(fdOut);
	close(saveStdin);
	close(saveStdout);
	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;
	if (!pid)
		exit(0);

	return (newBody);
}
