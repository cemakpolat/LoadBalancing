/*
 * MigrateWorstQoEClient.h
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#ifndef MIGRATEWORSTQOECLIENT_H_
#define MIGRATEWORSTQOECLIENT_H_
#include <string>
class MigrateWorstQoEClient {
public:
	std::string apSSID;
	std::string otherSSID;
	std::string ipAddr;
	int clientPortOfUser;
	int measurementPlaneServer;
	int loadBalancingServerPort;
	std::string localAddress;
	//QoETracker qoETracker;
	MigrateWorstQoEClient();
	virtual ~MigrateWorstQoEClient();
	int migrateWorstCongestedClient(std::string& apSSID);
	std::string getIPAddr();
};

#endif /* MIGRATEWORSTQOECLIENT_H_ */
