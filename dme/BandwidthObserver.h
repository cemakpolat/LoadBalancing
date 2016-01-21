/*
 * BandwidthObserver.h
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#ifndef BANDWIDTHOBSERVER_H_
#define BANDWIDTHOBSERVER_H_
#include <string>
#include <iostream>

class BandwidthObserver {
public:
	std::string filePath;
	long previousTransmitted;
	long previousReceived;
	long currentTransmitted;
	long currentReceived;
	long bandwidthTX;
	long bandwidthRX;
	int waitForCycle;
	static  std::string className;

	BandwidthObserver();
	virtual ~BandwidthObserver();
	long getBandwidthRate();
	void writeConsole(std::string& msg);
};

#endif /* BANDWIDTHOBSERVER_H_ */
