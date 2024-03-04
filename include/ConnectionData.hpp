/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionData.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 11:31:40 by adpachec          #+#    #+#             */
/*   Updated: 2024/02/20 11:19:29 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTIONDATA_HPP
# define CONNECTIONDATA_HPP

# include "WebServer.hpp"

class ConnectionData
{
	public:
		std::vector<char> readBuffer;
		char*	writeBuffer;
		bool	headerReceived;
		size_t	accumulatedBytes;

		ConnectionData();
		~ConnectionData();
		ConnectionData(const ConnectionData& other);
		ConnectionData& operator=(const ConnectionData& other);
};

#endif // CONNECTIONDATA_HPP