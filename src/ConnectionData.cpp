/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionData.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adpachec <adpachec@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 11:32:20 by adpachec          #+#    #+#             */
/*   Updated: 2024/03/01 11:16:35 by adpachec         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionData.hpp"

//*******************************************************************
// Constructores y destructor de la clase canónica
//*******************************************************************
ConnectionData::ConnectionData() : readBuffer(1024),
	writeBuffer(NULL), headerReceived(false), accumulatedBytes(0) {}

ConnectionData::~ConnectionData() { delete[] writeBuffer; }

ConnectionData::ConnectionData(const ConnectionData& other)
{
	readBuffer = other.readBuffer;
	writeBuffer = other.writeBuffer;
	headerReceived = other.headerReceived;
	accumulatedBytes = other.accumulatedBytes;
}

ConnectionData& ConnectionData::operator=(const ConnectionData& other)
{
	if (this != &other)
	{
		readBuffer = other.readBuffer;
		writeBuffer = other.writeBuffer;
		headerReceived = other.headerReceived;
		accumulatedBytes = other.accumulatedBytes;
	}
	return *this;
}
