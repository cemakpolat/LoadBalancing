/*
 * UserBandwidth.h
 *
 *  Created on: 07.08.2012
 *      Author: cem
 */

#ifndef USERBANDWIDTH_H_
#define USERBANDWIDTH_H_
#include "UserSession.h"
#include "writer.h"
#include "elements.h"
#include "reader.h"
#include <vector>
//#include "UserSessionProfile.h"
using namespace std;
class UserSessionProfile;

struct bandwidthObject {
	std::string packetDirection;
	std::string packetTrafficType;
	int packetSize;
};


struct iptObject{
	long int byte;
	string direction;
};
class UserBandwidth {

public:
	std::vector<iptObject> iptList;
	long int currentUploadByte;
	long int currentDownloadByte;
	double bandwidthAverage;
	long int totalDownloadBW;
	long int totalUploadBW;
	double sessionBandwidthAveragePerMin;
	string ip;
	string userId;
	long int totalTCPBW;
	double TCPAverage;
	long int totalUDPBW;
	double UDPAverage;
	UserBandwidth();
	~UserBandwidth();
	void calculateTotalBandwidth(string& ip);
	//void calculateTotalUploadTX();
	//void calculateTotalDownloadRX();
	void calculateSessionBandwidthAveragePerMin(UserSessionProfile& uprofile, std::string& ip);
	void calculateSessionBandwidthAveragePerMin(UserSessionProfile* uprofile, std::string& ip);
	void calculateBandwidthAverage(UserSession& session);
	void calculateProtocolsBW();
	void setParametersToUserModel(json::Object& userModel,UserSessionProfile * uprofile);
	void getParametersFromUserModel(json::Object& userModel);
	void getBytes();

};

#endif /* USERBANDWIDTH_H_ */
