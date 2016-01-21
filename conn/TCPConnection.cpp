/*
 * TCPConnection.cpp
 *
 *  Created on: Mar 7, 2013
 *      Author: cem
 */

#include "TCPConnection.h"
#include <iostream>           // For cerr and cout
//#include <cstdlib>            // For atoi()
//#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
//#include <vector>
//#include <sstream>
#include "Utilities.h"

#include <netinet/in.h>
#include "CommLang.h"

//#include "tinyxml.h"



TCPConnection::TCPConnection() {
	// TODO Auto-generated constructor stub

}

TCPConnection::~TCPConnection() {
	// TODO Auto-generated destructor stub
}

void TCPConnection::sendMessageInSmallChunks(string ip, char* text, int echoServPort) {
	string servAddress = ip;

	char *echoString = text;
	int echoStringLen = strlen(echoString);   // Determine input length
	try {
		// Establish connection with the echo server
		TCPSocket sock(servAddress, echoServPort);
		// Send the string to the echo server
		sock.send(echoString, echoStringLen);

	} catch (SocketException &e) {
		cerr << e.what() << endl;

	}
}
std::string TCPConnection::getMessageInSmallChunks(TCPSocket *sock) { //,string servAddress,int echoServPort
	std::string str;
	try {
		// Establish connection with the echo server
		//	TCPSocket sock(servAddress, echoServPort);
		cout << sock->getForeignAddress() << ":";cout << sock->getForeignPort();
		char echoBuffer[RCVBUFSIZE];
		int recvMsgSize;
		while ((recvMsgSize = sock->recv(echoBuffer, RCVBUFSIZE)) > 0) { // Zero means// end of transmission

			// Echo message back to client
			//cout<<":received Message Size "<<recvMsgSize<<endl;
			echoBuffer[RCVBUFSIZE] = '\0';
			//cout << ":echo " << echoBuffer << endl;
			str.append(echoBuffer);
			//sock->send(echoBuffer, recvMsgSize);
		}
		//cout << ":all " << str << endl;
		delete sock;

	} catch (SocketException &e) {
		cerr << e.what() << endl;

	}
	return str;
}
std::string TCPConnection::getMessageFirstSizeInInt(TCPSocket *sock) {
	int val[1];
	try {
		//get packet size
		sock->recv(val, sizeof(val));
		uint32_t size = ntohl(val[0]);
		char echoBuffer[size];
		//read packet itself
		sock->recv(echoBuffer, size);
		delete sock;
		return echoBuffer;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
	}

}
bool TCPConnection::sendMessageFirstSizeInInt(string ip, string text,
		int port) {
	try {
		TCPSocket *sock = new TCPSocket(ip, port);
		uint32_t size = htonl(strlen(text.c_str()));
		//sent packet size
		sock->send(&size, sizeof(size));
		//cout<<"val "<<ntohl(htonl(size))<<endl;   cout<<"val "<<htons(val[0])<<endl;
		//cout<<"val "<<ntohs(htons(val[0]))<<endl;   cout<<"val "<<htonl(val[0])<<endl;
		//sent packet itself
		sock->send(text.c_str(), size);
		delete sock;
		return true;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		return false;
	}
	return false;
}

std::string TCPConnection::getMessageLocal(TCPSocket *sock) {
	int val[1];
	try {
		//get packet size
		sock->recv(val, sizeof(val));
		//uint32_t size = ntohl(val[0]);
		char echoBuffer[val[0]];
		//read packet itself
		sock->recv(echoBuffer, val[0]);
		delete sock;

		return echoBuffer;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
	}
	return "";
}
bool TCPConnection::sendMessageLocal(string text,int port) {
	try {
		TCPSocket *sock = new TCPSocket("127.0.0.1", port);
		const char *echoString =text.c_str();
		int val[1];
		//val[0]=strlen(echoString);
		val[0]=text.length();
		sock->send(val, sizeof(val));
		sock->send(text.c_str(),val[0]);
		//sock->send(echoString,val[0]);
		delete sock;
		return true;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		return false;
	}
	return false;
}

double TCPConnection::sendReqToUBMRecvMessageFromUBM(int& reqNumber) {
	json::Object incomingMessage;
	//json::Object requestMessage;

	int measurementPlaneServer = 14131;
	int loadBalancingServerPort = 14555;
	string localAddress = "127.0.0.1";
	TCPServerSocket *serv = new TCPServerSocket(loadBalancingServerPort);
	TCPConnection *senReqToMP = new TCPConnection();

	// SEND REQUEST MESSAGE TO MEASUREMENT PLANE
	CommLang *cm=new CommLang();
	senReqToMP->sendMessageFirstSizeInInt(localAddress,
			cm->createRequestMesObj(measurementPlaneServer,localAddress,reqNumber),
			measurementPlaneServer);

	//GET RESPONSE FROM UBM
	string mes=senReqToMP->getMessageFirstSizeInInt(serv->accept());
	incomingMessage = Utilities::convertStringToJSON(mes);
	delete senReqToMP;
	string ipAddress = json::String(incomingMessage["ip"]);
	double value =json:: Number( incomingMessage["response"]);
	delete cm;
	return value;
}
////TODO: Does not work properly
//string TCPConnection::getMessageFirstSizeInChar(TCPSocket *sock) {
//	char val[BUFFIRSTSIZEINCHAR];
//	sock->recv(val, sizeof(val));
//	//cout << " val " << &val[0] << endl;
//	//val[BUFFIRSTSIZEINCHAR] = '\0';
//	int a = atoi(&val[0]);
//	//cout << ":a " << a << endl;
//
//
//	char echoBuffer2[a];
//	sock->recv(echoBuffer2, a);
//	cout << ":echo " << echoBuffer2 << endl;
//	echoBuffer2[a] = '\0'; //TODO: you should add '\0' end of the line so as to prevent the additional data.
//	cout << ":echo " << echoBuffer2 << endl;
//	delete sock;
//	return echoBuffer2;
//}
////TODO: Does not work properly
//void TCPConnection::sendMessageFirstSizeInChar(string ip, char* text, int port) {
//	//TCPSocket sock("192.168.1.1", 12121);
//	TCPSocket *sock=new TCPSocket(ip, port);
//	char *echoString =text;
//
//	// Send the string to the echo server
//	int Number = strlen(echoString);       // number to be converted to a string
//	string Result = echoString;          	// string which will contain the result
//	ostringstream convert;   				// stream used for the conversion
//	convert << Number; // insert the textual representation of 'Number' in the characters in the stream
//	Result = convert.str();
//	cout<<"REsult"<<Result<<endl;
//	//send firstly the size of the packet
//	sock->send(Result.c_str(), sizeof(Result));
//	//send message itself
//	cout<<"REsult"<<echoString<<endl;
//	sock->send(echoString, Number);
//	delete sock;
//}
/*
 *A SIMPLE EXAMPLE FOR CLIENT AND SERVER
//CLIENT
int main(int argc, char *argv[]) {

	string servAddress = argv[1]; // First arg: server address
	char *echoString = argv[2];   // Second arg: string to echo

	try {
		TCPConnection *con = new TCPConnection();
		for (;;) {
			//con->sendMessageInSmallChunks("192.168.1.1",echoString,12121);

			con->sendMessageFirstSizeInInt("192.168.1.1", echoString, 12121);
			//sendMessage();
			sleep(2);
		}
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}

	return 0;
}
//SERVER
int main(int argc, char *argv[]) {

	try {
		TCPConnection *con = new TCPConnection();
		// Server Socket object
		TCPServerSocket servSock(12121);
		for (;;) {   // Run forever
			//cout<<con->getMessageInSmallChunks(servSock.accept())<<endl;
			cout << con->getMessageFirstSizeInInt(servSock.accept()) << endl;

		}
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	// NOT REACHED

	return 0;
}
*/

