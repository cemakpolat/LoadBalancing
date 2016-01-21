/*
 * UPMain.cpp
 *
 *  Created on: 27.07.2012
 *      Author: cem
 */
#include <stdio.h>
#include <string>
//#include <vector>
#include <pthread.h>
#include "UserAuthListObject.h"
#include "AuthenticationPlatformHAPD.h"
#include "UPMain.h"
#include <iostream>
#include "QoE.h"
#include "ITGChecker.h"
#include "QoETracker.h"
#include "Utilities.h"
#include "DataAccess.h"
using namespace std;

//TODO: implement exceptions

//vectors should be defined in order to be active, because they are dependent on upmain object
vector<UserAuthListObject> UPMain::userAuthList;

UPMain::UPMain(){

}
UPMain::~UPMain(){

}
void* UPMain::run(void* object) {
	UPMain *upm=(UPMain*)object;
	//upm->upmain();
	printf("User Profile Block started\n");
		upm->mutex1 = PTHREAD_MUTEX_INITIALIZER;
		upm->mutexFile=PTHREAD_MUTEX_INITIALIZER;
		pthread_t authentication;
	//	AuthenticationPlatform auhtPl;
		AuthenticationPlatformHAPD auhtPl;


		int rc2;
		if ((rc2 = pthread_create(&authentication, NULL,
				&AuthenticationPlatformHAPD::run, &auhtPl))) {
			printf("User Profile Block:Thread creation failed: %d\n", rc2);
			int attempt = 0;
			while (attempt < 3) {
				bool success = true;
				Utilities::randomBackOfftime(7);
				if ((rc2 = pthread_create(&authentication, NULL,
						&AuthenticationPlatformHAPD::run, &auhtPl))) {
					success = false;
					printf("User Profile Block:Thread creation failed: %d\n", rc2);
					if (attempt == 2) {
						printf(
								"User Profile Block:Thread creation failed,please re-start the program. \n");
					}
				}
				if (success == true) {
					attempt = 3;
				}
			}
		}

		pthread_join(authentication,NULL);
		printf("User Profile Block ended \n");
}


//singleton pattern required
UPMain* UPMain::m_pInstance=NULL;
UPMain* UPMain::Instance()
{
   if (!m_pInstance)   // Only allow one instance of class to be generated.
      m_pInstance = new UPMain();
   return m_pInstance;
}

/*------------------|UPMain Functionalities for MP|-------------------*/
/**
 * get the rest of the general bandwidth average for all users.
 * @totalBandwidth indicates the maximum bandwidth amount for an AP.
 */
int UPMain::restBandwidthBasedOnBWAverage(){
	int totalBandwidth=6;
	int restBW=0;
	DataAccess *da=new DataAccess();
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
		for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
			json::Object userModel=da->getUserModel(UPMain::Instance()->userAuthList[i].userId);
			Number bwa=userModel["bandwidthAverage"];
			restBW=restBW+bwa;
		}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	delete da;
	int result=totalBandwidth-restBW;
	return result;
}
/**
 * Get the rest of the bandwidth usage for the current time.
 * @totalBandwidth indicates the maximum bandwidth amount for an AP.
 */
int UPMain::restBandwidthCurrentSessionBW(){
	int totalBandwidth=6*125000;//should be transported in configParameters
	int restBW=0;
	std::string sessionTime=Utilities::getCurrentTimeAsString();
	int timeInterval=10;
	time_t adeqTime=UPMain::Instance()->getAdequateTime(sessionTime,timeInterval);
	std::vector<std::string> startTime;
	startTime = Utilities::split(Utilities::getTimeFromGivenTime(adeqTime),':');
	int placeOfTic = atoi(startTime[0].c_str()) * 6+ (atoi(startTime[1].c_str())/10);
	DataAccess *da=new DataAccess();
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		json::Object userModel=da->getUserModel(UPMain::Instance()->userAuthList[i].userId);
		Array arraySesMin = userModel["sessionsPerMin"];
		Object session=arraySesMin[placeOfTic];
		Number sessionBW = session["bandwidth"];
		restBW=restBW+sessionBW;
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	delete da;
	int result=totalBandwidth-restBW;
	return result;
}
time_t UPMain::getAdequateTime(std::string& t,int& timeInterval) {
	std::vector<std::string> time = Utilities::split(t, ':');

	int hour = atoi(time[0].c_str());
	int min = atoi(time[1].c_str());

	int rest = min % timeInterval;
	int division = min / timeInterval;

	if ((hour != 0)) { //23:xx
		if (min == 0) { //12:00
			hour = hour - 1; //11:00
			time[0] = Utilities::convertIntToString(hour);
			time[1] = "50"; //11:50
		} else { //12:xx
			if (rest == 0) { //12:50
				int result = division * 10 - timeInterval; //12:40
				time[1] = Utilities::convertIntToString(result);
			} else {
				int result = division * 10; //12:02
				time[1] = Utilities::convertIntToString(result); //12:10
			}
		}
	} else { //00:xx
		if (min == 0) { //00:00
			time[0] = "23"; //decrease one hour                        //23:50
			time[1] = "50"; // set minute to 00

		} else {
			if (rest == 0) { //00:50
				int result = division * 10 - timeInterval; //00:40
				time[1] = Utilities::convertIntToString(result);
			} else {
				int result = division * 10; //00:02
				time[1] = Utilities::convertIntToString(result); //00:00
			}
		}
	}
	return Utilities::convertTimeStringToMilisecond(
			time[0] + ":" + time[1] + ":00");
}

/*-----------|Authentication and User related Functionalities|----------*/

/*User Authentication List Functionalities*/
int UPMain::getAuthenticatedUserCount(){
	int i=0;
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	i=UPMain::Instance()->userAuthList.size(); //TODO: fixed: exception can arise due to the mutex, because mutex may not be unlocked on account of "return"
	//assign then return, this causes problems....
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	return i;
}

string UPMain::getUserIPByUserId(string& userId){
	std::string ip="";
		pthread_mutex_lock(&UPMain::Instance()->mutex1);
		for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
			if (UPMain::Instance()->userAuthList[i].userId==userId && UPMain::Instance()->userAuthList[i].active) {
				ip=UPMain::Instance()->userAuthList[i].ip;
				break;
			}
		}
		pthread_mutex_unlock(&UPMain::Instance()->mutex1);
		return ip;
}
string UPMain::getUserNameByIP(string& IP) {
	std::string userId="";
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].ip==IP && UPMain::Instance()->userAuthList[i].active) {
			userId=UPMain::Instance()->userAuthList[i].userId;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	return userId;
}

string UPMain::getUserNameByMAC(string& MAC) {
	std::string userId="";
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].Mac==MAC && UPMain::Instance()->userAuthList[i].active) {
			userId=UPMain::Instance()->userAuthList[i].userId;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	return userId;
}

string UPMain::getUserMAC(string& IP) {
	std::string userMac="";
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].ip==IP && UPMain::Instance()->userAuthList[i].active) {
			userMac=UPMain::Instance()->userAuthList[i].Mac;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	return userMac;
}

string UPMain::getUserToken(string& IP) {
	std::string userToken="";
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].ip==IP && UPMain::Instance()->userAuthList[i].active) {
			userToken= UPMain::Instance()->userAuthList[i].token;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	return userToken;
}

bool UPMain::checkUserAuthenticityByUID(string& userId) {
	bool state=false;
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		std::cout<<"UP: User state "<<userId<<UPMain::Instance()->userAuthList[i].active<<std::endl;

		if (UPMain::Instance()->userAuthList[i].userId==userId && UPMain::Instance()->userAuthList[i].active) {
			state=true;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	std::cout<<"UP: User state "<<state<<std::endl;
	return state;
}
bool UPMain::checkUserAuthenticityByIP(string& IP ) {
	bool state=false;
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].ip==IP && UPMain::Instance()->userAuthList[i].active) {
			state=true;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	//std::cout<<"UP: User state "<<state<<std::endl;
	return state;
}

bool UPMain::checkUserAuthenticityByMAC(string& mac) {
	bool state=false;
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].Mac==mac  && UPMain::Instance()->userAuthList[i].active) {
			state=true;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	//std::cout<<"UP: User state "<<state<<std::endl;
	return state;
}

