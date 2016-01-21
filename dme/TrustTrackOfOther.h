/*
 * TrustTrackOfOther.h
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#ifndef TRUSTTRACKOFOTHER_H_
#define TRUSTTRACKOFOTHER_H_
#include <string>
#include "TrustObject.h"
#include <iostream>
#include <vector>
class TrustTrackOfOther {
public:
	static std::string  className;
	std::vector<TrustObject*> turstListOfAPs;
	int waitTime;
	TrustTrackOfOther();
	TrustTrackOfOther(std::string ssidOfOtherAP,std::string apOfOtherAP);
	virtual ~TrustTrackOfOther();
	static void* run(void*);
	TrustObject addAPObjectToTrustList(int turstLevel,std::string ipAdd,std::string apSSID);
	bool removeAPObjectToTrustList(std::string apSSID);
	bool increaseAPObjectTrust(std::string& apssid);
	bool decreaseAPObjectTrust(std::string& apssid);


};

#endif /* TRUSTTRACKOFOTHER_H_ */
