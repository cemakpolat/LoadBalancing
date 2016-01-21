/*
 * MainDME.h
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#ifndef MAINDME_H_
#define MAINDME_H_

#include <string>
#include <iostream>
#include <pthread.h>
#include "MigrateWorstQoEClient.h"
#include "BandwidthObserver.h"
#include "MonetaryDominance.h"
#include "PracticalSocket.h"
#include "ResConOfClientPerDayTime.h"
//#include "DateUtils.h"
#include "ClientArrivalDepartures.h"
#include "TrustTrackOfOther.h"
#include "CreditTrack.h"
#include "ReputationFE.h"
#include "BandwidthUtilizationFE.h"
#include "ClientPopulationFE.h"
#include "TCPConnection.h"
class ClientArrivalDepartures;


class MainDME {

public:

	MainDME();
	virtual ~MainDME();
	std::string className;
	MigrateWorstQoEClient *migrationFunctionality;
	std::string ssidOfOtherAP;
	std::string apOfOtherAP;

	/*Future behaviour Tracking related parameters*/
	 pthread_t ReputationRunnable;//TODO:Pointer possible
	 pthread_t BWRunnable;
	 pthread_t PopulationRunnable;
	 int thresholdNonCooperative;
	 int bandwidthUtilizationPeriod;
	 int reputationPeriod;
	 int populationPeriod;

	 /*Incentive Tracking*/
	 pthread_t trustOfOtherRunnable;
	 pthread_t creditRunnable;

	 /*Bandwidth observation related parameters*/
	 BandwidthObserver *bandwidthObserver;
	 long thresholdBandwidthUtilization;
	 double A_load;
	 double load_th;

	 /*Client Arrival and Departures*/
	 pthread_t clientArrivalDepartureRunnable;

	 /*POMDP related Parameters*/
	 pthread_t requesterPOMDP;
	 pthread_t requesteePOMDP;
	 int waitRequesterPOMDP;
	 int waitRequesteePOMDP;

	 /*Communication Parameters*/
	 TCPServerSocket *accessPointMessageServer;
	 TCPServerSocket *accessPointResponseServer;
	 TCPConnection *userMessageClient;//TODO:That can raise issue
	 TCPConnection *accessPointMessageClient;//TODO:That can raise issue too while deleting it
	  int userServerPort;
	  int accessPointServerPort;
	  int accessPointResponsePort;

	 /*Monetart component*/
	 MonetaryDominance *monetaryDominance;
	 /*Day time and client cost*/
	 ResConOfClientPerDayTime *resconOfClientPerDayTime;
	 /*Time of Day Related Parameters*/
	 //DateUtils *timeOfDay;

	 /*Normalization Components*/
	 double C_s_Load_Normalizer;
	 double C_s_Reputation_Normalizer;
	 double C_s_Both_Normalizer;

	 /*History holder for requester*/
	 double unitResourceCostHolder[];
	 double loadHolder[];
	 double loadOrientedUtilityHolder[];
	 double revenueOrientedutilityHolder[];
	 double basicRevenueHolder[];

	 ReputationFE *reputationRF;
	 ClientArrivalDepartures *clientArrDeparture;
	 CreditTrack *creditTrack;
	 TrustTrackOfOther *trustOtherTrack;
	 BandwidthUtilizationFE *bandwidthUtilFE;
	 ClientPopulationFE *clientPopFE;
	 void initiateAll();
	static MainDME* Instance();
	void writeConsole(char& str);
	void startReputationTrack();
	void startBWTrack();
	void startPopulationTrack();
	void initiateNegotiation(bool loadOriented);
	int requesterLoadOrientedUtility();
	double previousBasicRevenue;
	int previousClientNumber;
	int requesterRevenueOrientedUtility();
	void startRequesterPOMDP();
	void startRequesteePOMDP();
	void startClientArrivalDepartures();
	void startTrustOfOthersTrack();
	void startCreditTrack();
	bool sendOfferToTheAP(int& numberOfClientsToBeMigrated,double& incentive);

	static void* runRequesterPOMDP(void*);
	static void* runRequesteePOMDP(void*);
	template <class T,class U,class B>
	T runCenter (T& a, U& b,B* data);
	//void main();
	static void* run(void* object);
private:
	static MainDME* mdme_Instance;
protected:
	std::string runRequesterPOMDP();
	std::string runRequesteePOMDP();
	void sendOurResponse(std::string& answer,std::string& apAddress);
	double requestEEloadOrientedUtility(int& numberOfClientSuggested,double& suggestedIncentive);
	double requestEELoadReputationOrientedUtility(int& numberOfClientSuggested,int& suggestedIncentive);
	double requestEEBothOrientedUtility(int& numberOfClientSuggested, int& suggestedIncentive);

};

#endif /* MAINDME_H_ */
