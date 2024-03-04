/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 12:41:07 by adpachec          #+#    #+#             */
/*   Updated: 2024/01/22 12:41:07 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "WebServer.hpp"
# include "ConfigFile.hpp"


class Location
{
	private:
		std::string					_path;
		std::string					_root;
		bool						_autoindex;
		std::string					_index;
		std::vector<short>			_methods; // GET, POST, DELETE
		std::vector<std::string>	_return; 
		std::map<short, std::string>_errorPages;
		unsigned long				_clientMaxBodySize;
		std::string					_modifier;
		std::vector<std::string>	_cgiPath;
		std::vector<std::string>	_cgiExt;

	public:
		Location();
		Location(std::string &path, std::string &modifier, std::vector<std::string> &paramtr);
		Location(const Location &other);
		~Location();
		Location &operator=(const Location &other);

		const std::string 				&getPath() const;
		const std::string 				&getRootLocation() const;
		const std::vector<short> 		&getMethods() const;
		const bool 						&getAutoindex() const;
		const std::string 				&getIndexLocation() const;
		const std::vector<std::string> 	&getReturn() const;
		const std::vector<std::string> 	&getCgiPath() const;
		const std::vector<std::string> 	&getCgiExtension() const;
		const std::string 				getExtensionCgiPath(std::string &ext) const;
		const unsigned long 			&getMaxBodySize() const;
		const std::string 				&getModifier() const;
		const std::string 				getErrorPage(short i) const;
		const std::map<short, std::string> &getErrorPages() const;
	
		void setPath(std::string parametr);
		void setRootLocation(std::string parametr);
		void setMethods(std::vector<std::string> methods);
		void setAutoindex(std::string parametr);
		void setIndexLocation(std::string parametr);
		void setReturn(std::string parametr1, std::string parametr2);
		void setCgiPath(std::vector<std::string> &path);
		void setCgiExtension(std::vector<std::string> &extension);
		void setMaxBodySize(std::string parametr);
		void setMaxBodySize(unsigned long parametr);
		void setModifier(std::string parametr);
		void setErrorPage(short i, std::string parametr);
		void setErrorPages(std::map<short, std::string> errorPages);
	
		void 			configureLocation(std::string &path, std::vector<std::string> &parametr);
		const Location* selectLocation(const std::string& requestURL,
						const std::vector<Location>& locations);
		const Location* findExactMatch(const std::string& requestURL,
						const std::vector<Location>& locations);
		const Location* findLongestPrefixMatch(const std::string& requestURI,
						const std::vector<Location>& locations);
		static bool 	startsWith(const std::string& str, const std::string& prefix);
		static void 	checkToken(std::string &parametr);
		static int 		ft_stoi(std::string str);
		int				checkLocation(Location &location, std::string serverRoot, std::string serverIndex);

		std::string 	printMethods() const;
		
		class ErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ErrorException(std::string message) throw()
				{
					_message = "CONFIG LOCATION ERROR: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};
};

#endif // LOCATION_HPP