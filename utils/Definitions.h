/*
 * Definitions.h
 *
 *  Created on: Feb 28, 2013
 *      Author: cem
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

class Definitions {
public:
	Definitions();
	virtual ~Definitions();
	//UBM Connections
	static int const AssoAuthThreadServerPortNumber=5011;
	static int const MPServerPortNumber=14131;
	static int const MPClientPortNumber=14132;
	//Package qoETracker
	static int const QoEServerPortNumber=13132;
    static long const ITGTimerDuration=100000;
    static int const qoEExperienceShiftTime = 20000;
	//File paths
	//static  const char *ARPFILEPATH="/proc/net/arp";
    //Client
        static int const QoEClient=13131;
        //MainDME
        static int const UserServerPort = 49154;
        static int const AccessPointServerPort = 49155;
        static int const AccessPointResponsePort = 49156;

};

#endif /* DEFINITIONS_H_ */
