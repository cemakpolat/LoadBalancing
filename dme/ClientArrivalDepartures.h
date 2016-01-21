/*
 * ClientArrivalDepartures.h
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#ifndef CLIENTARRIVALDEPARTURES_H_
#define CLIENTARRIVALDEPARTURES_H_
#include <string>
#include "writer.h"
#include "elements.h"
#include "reader.h"
#include "MainDME.h"
class ClientArrivalDepartures {
public:

	//MainDME* dme;
	static std::string className;
	std::vector<std::string> clientMACArray;
	int portNumber;
	std::string processedMAC,processesUserId,processedState,processedIP;
	static const int MAXITEMS = 4;
	static  int const Association=0, Authentication=1,Disassociation=2,Deauthentication=3;

	int totalNumberofClients;
	ClientArrivalDepartures();
	virtual ~ClientArrivalDepartures();
	static void* run(void*);
	int getNumberOfClients();
	json::Object getMessage(TCPSocket *sock);
	int getRelatedMessageNumber(std::string& message);

private:
	void deleteClientFromMACList(std::string& mac);
	void insertClientToMacList(std::string& mac);


};

#endif /* CLIENTARRIVALDEPARTURES_H_ */
