/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 12:40:51 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/01 11:14:58 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

# include "WebServer.hpp"

class ConfigFile
{
	private:
		std::string	_path;

	public:
		ConfigFile();
		ConfigFile(std::string const path);
		ConfigFile(const ConfigFile &other);
		~ConfigFile();
		ConfigFile &operator=(const ConfigFile &other);
		
		std::string			getPath();

		static int			checkPath(std::string path);
		static std::string	readFile(std::string path);
		static bool 		fileExistsAndReadable(const std::string& filePath);
		static std::string 	prefixPath(std::string const path);
		static std::string 	adjustName(std::string const name);
};

#endif // CONFIGFILE_HPP