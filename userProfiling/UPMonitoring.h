/*
 * UPMonitoring.h
 *
 *  Created on: 04.08.2012
 *      Author: cem
 */

#ifndef UPMONITORING_H_
#define UPMONITORING_H_

#include <string>
#include "UserAuthListObject.h"
#include "UserSessionProfile.h"
#include <pthread.h>


struct timer {
	unsigned long duration;
	bool state;
	int count;
};

class UPMonitoring{
private:
	std::string userPacket;
	json::Object userJSONModel;
	UserSessionProfile *uprofile;
	int timeInterval;
	pthread_t timerObj;
	struct timer t;
	//bool authenticated;
	//UPMonitoring(string Mac,string IP,string userId,string token);
public:
	std::string Mac;
	std::string IP;
	std::string token;
	std::string userId;
	bool left;
	UPMonitoring();
	~UPMonitoring(); //TODO:destructor raises a failure in which it is said that it is private!
	UPMonitoring(UserAuthListObject userObj);//TODO reference could be better than copy
	UPMonitoring(UserAuthListObject* userObj);
	static void* run(void*);
	bool checkUserAuthenticity(std::string userId);
	//void getUserModelFromStoragePlace(string& userId,Object& usermodel);
	void getUserModelFromStoragePlace();
	void updateProcessedPacket();
	void storeUserModelInStoragePlace();
	int getPacketIndex();
	void setPacketIndex();//problem can arise owing to vector list
	int getUnprocessedPacketCount(std::string ip);
	void timer(bool state,UPMonitoring *upm);
	static void* timerFunction(void*);
	void callTimerTask(std::string& userId);
	void getUserModelFromStoragePlace1(std::string& userId);
	void insertIPTableCommands(std::string& ip,std::string& mac);
	void removeIPTableCommands(std::string& ip,std::string& mac);




};


#endif /* UPMONITORING_H_ */
