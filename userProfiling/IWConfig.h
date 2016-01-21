/*
 * IWConfig.h
 *
 *  Created on: Mar 1, 2013
 *      Author: cem
 */

#ifndef IWCONFIG_H_
#define IWCONFIG_H_
#include <string>
using namespace std;

class IWConfig {
public:
	static string interface;
	string ssid;
	string mode;
	string freq;
	string bitrate;
	string txpower;
	string mac;
	string linkQuality;
	string signalLevel;
	string noiseLevel;
	string retryLim;
	static int  maxSignalThreshold,minSignalThreshold;
	static int maxAllowableValue,minAllowableValue;
	IWConfig();
	IWConfig(int maxEstimatedThreshold,int minEstimatedThreshold);
	virtual ~IWConfig();
	string getSSID();
	string getTXPower();
	string getBitRate();
	string getRetryLimit();
	string getLinkQaulity();
	string getSignalLevel();
	string getNoiseLevel();
	string getSNR();
	void scan();
	void scanForNetgear();
	static void findThresholds();
	static void setTransmissionPower(int value);
	string readFile(string& fileName);
	static bool isGivenPowerAcceptable(int& power);
};

#endif /* IWCONFIG_H_ */
