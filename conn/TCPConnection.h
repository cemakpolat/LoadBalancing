/*
 * TCPConnection.h
 *
 *  Created on: Mar 7, 2013
 *      Author: cem
 */

#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_
#include <string>
#include "PracticalSocket.h"
class TCPConnection {
public:
	static const unsigned int RCVBUFSIZE = 32;
	static const unsigned int BUFFIRSTSIZEINCHAR=10;
	TCPConnection();
	virtual ~TCPConnection();
	std::string getMessageInSmallChunks(TCPSocket* sock);
	void sendMessageInSmallChunks(string ip, char* text,int echoServPort);
	std::string getMessageFirstSizeInInt(TCPSocket *sock);//For hostpad
	bool sendMessageFirstSizeInInt(string ip,string text,int port);
	bool sendMessageLocal(string text,int port);
	std::string getMessageLocal(TCPSocket *sock);
	double sendReqToUBMRecvMessageFromUBM(int& reqNumber);
};

#endif /* TCPCONNECTION_H_ */
