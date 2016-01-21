/*
 * QoETracker.h
 *
 *  Created on: 08.08.2012
 *      Author: cem
 */

#ifndef QOETRACKER_H_
#define QOETRACKER_H_
#include "PracticalSocket.h"
#include "writer.h"
#include "elements.h"
#include "reader.h"
class QoETracker{
private:
	std::string userIP;
	std::string userMac;
	int bufferingNumber;
	time_t lastQoETime;
public:

	QoETracker();
	~QoETracker();
	void randomBackOfftime();
	int getAuthenticatedUserCount();
	std::string getQoEMeasurementFromClientsInString(TCPSocket *sock);//Socket connection
	void shiftQoEFromClientsToLeft(std::string userId, std::string ip,std::string userMAC, std::string time, json::Object& jobj);
	void storeInformationOfQoE();
	std::string getUsername(std::string ip);
	bool checkUserAuthenticated(std::string ip); //may be, we can use upmain authenticity
	bool checkUserExistence(std::string userId);
	static void* run(void*);
	static void* qoeShifter(void *);
	//static std::string exec(char*);
};


#endif /* QOETRACKER_H_ */
