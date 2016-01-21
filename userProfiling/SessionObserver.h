/*
 * SessionObserver.h
 *
 *  Created on: Mar 18, 2013
 *      Author: cem
 */

#ifndef SESSIONOBSERVER_H_
#define SESSIONOBSERVER_H_
#include <string>
#include <vector>
struct bandwidthObj{
	double IN;
	double OUT;
};
enum POWERLEVEL{//INCREASE GRADE POWER=LEVEL1/10;
	LEVEL1=0,
	LEVEL2=2,
	LEVEL3=4,
	LEVEL4=6,
	LEVEL5=8,
	LEVEL6=10
};
class SessionObserver {
private:
	static SessionObserver* m_sesObInstance;

public:
	int oldStateAuthAverage;
	int previousIncreaseGrade;
	//Counter
	std::string userId;
	int eventCounter;
	int bandwidthCounter;
	int storingEventCounter;
	int meetingCounter;
	int dailyCounter;
	int WhichDay;
	int WhichPeriodInDay;
	double previousBWIN;
	double previousBWOUT;
	int unstabilityCounter;
	std::vector<std::string> bandwidthObservationList;
	std::vector<std::string> bandwidthObservationListDaily;
	std::vector<std::string> observationList;
	std::vector<std::string> observationListDaily;
	//int eventsPerPeriod;//12 in 2 minutes every 10 seconds
	//int fetchedEventCountFromFile;//60 Events
	double previousBlockProbability;
	double currentAuthenticationState;
	double distanceBetweenBlocks;
	double unAuthenticatedThreshold;//0.1
	double  authenticatedThreshold;//0.9
	double unStableStateMin;//0.3
	double unStableStateMax;//0.7
	SessionObserver();
	~SessionObserver();
	static SessionObserver* Instance();
	void dailyTakenMeasurement(std::vector<double> list,double in,double out);
	static void* run(void*);
	void decreaseTXPower(int grade);
	void increaseTXPower(int grade);
	void storeDataInFile(std::string& userId);
	void changeTXPower(double currentState);
	bandwidthObj getBytes();
	void getPeriodicalData();
	void stroreObservations();
	std::string convertVectorToString(std::vector<double>& list);

};

#endif /* SESSIONOBSERVER_H_ */
