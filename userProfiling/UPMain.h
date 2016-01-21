/*
 * UPMain.h
 *
 *  Created on: 04.08.2012
 *      Author: cem
 */

#ifndef UPMAIN_H_
#define UPMAIN_H_

#include "UserAuthListObject.h"
#include <vector>

class UPMain{
private:
	   static UPMain* m_pInstance;
public:
	pthread_mutex_t mutex1;
	pthread_mutex_t mutexFile;

	UPMain();
	~UPMain();
	static void* run(void* object);

	static std::vector<UserAuthListObject> userAuthList;
	static UPMain* Instance();
	void upmain();

	int getAuthenticatedUserCount();
	std::string getUserIPByUserId(std::string& userId);
	std::string getUserMAC(std::string& IP);
	std::string getUserNameByMAC(std::string& MAC);
	std::string getUserNameByIP(std::string& IP);
	std::string getUserToken(std::string& IP);
	bool checkUserAuthenticityByUID(std::string& userId);
	bool checkUserAuthenticityByIP(std::string& ip);
	bool checkUserAuthenticityByMAC(std::string& mac);

	time_t getAdequateTime(std::string& t,int& timeInterval);
	int restBandwidthCurrentSessionBW();
	int restBandwidthBasedOnBWAverage();
};

#endif /* UPMAIN_H_ */
