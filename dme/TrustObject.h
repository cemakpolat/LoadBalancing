/*
 * TrustObject.h
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#ifndef TRUSTOBJECT_H_
#define TRUSTOBJECT_H_
#include <string>
class TrustObject {
public:

	std::string apSSID;
	int trustLevel;
	std::string ipAddr;
	double counter;
	double K_trust;
	double k_trust;
	double A_trust;
	double B_trust;

	int reduceExtreme;
	double reduce;
	int increaseExtreme;
	double increase;

	int counterUpperLevel;
	int counterLowerLevel;

	std::string debugTrustLevel;
	std::string debugTimer;

	TrustObject();
	TrustObject(int& trustLevel,std::string ipAddr,std::string apSSID);
	virtual ~TrustObject();
	void setIpAddress(std::string& ipAddr);
	void setTrustLevel(int trustLevel);
	void setApSSID(std::string& apssid);
	std::string getIPAddr();
	int getTrustLevel();
	std::string getApSSID();
	int reduceTrustLevel();
	int reduceTrustLevelExtreme();
	int increaseTrustLevelExtreme();
	int increaseTrustLevel();
private:
	static std::string className;
};

#endif /* TRUSTOBJECT_H_ */
