/*
 * CreditTrack.h
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#ifndef CREDITTRACK_H_
#define CREDITTRACK_H_
#include <string>
#include <vector>
#include "CommLang.h"

class CreditTrack {
public:
	static std::string className;
	double unitResourceCost;
	long totalConsumedBandwidth;
	double consumedBandwidthDuringWaitTime;
	double totalCredits;
	int waitTime;
	std::string filePath;

	//QoETracker qoETracker;
	/*
	 * Unit cost related parameters
	 */
	int K_averageDelay;
	double A_averageDelay;
	int B_averageDelay;
	int k_averageDelay;

	int K_packetLossRate;
	double A_packetLossRate;
	int B_packetLossRate;
	int k_packetLossRate;

	int K_jitter;
	double A_jitter;
	int B_jitter;
	int k_jitter;

	int K_bitRate;
	double A_bitRate;
	int B_bitRate;
	int k_bitRate;

	double averageDelayNormalizationFactor;
	double packetLossRateNormalizationFactor;
	double jitterNormalizationFactor;
	double bitRateNormalizationFactor;

	std::string debugUnitResource;
	std::string debugTotalCredits;
	std::string debugTimer;
	std::string bandwidthUtilization;

	CreditTrack();
	virtual ~CreditTrack();
	static void* run(void*);
	void setTotalPreviousConsumedBWtoZero();
	double getConsumedBWDutingWaitTime();
	double calculateCurrentUnitResourceCost();
	double getAverageBitRateComponent();
	double getAveragePacketLossComponent();
	double getAverageJitterComponent();
	double getAverageDelayComponent();
	void setTotalconsumedBW(long totalBW);
	long getTotalConsumedBW();
	void setCredit(long credit);
	void addCredit(long credit);
	void removeCredit(long credit);
	double getOwnCredit();
	int getUnitResourceCost();
	double sendGetMessage(int requestedValue);

	double averageBitRate();

	double averagePacketLoss();

	double averageDelay();


	double averageJitter();

	void fileWrite(std::string& debugOutFile,std::string& outputString);

};

#endif /* CREDITTRACK_H_ */
