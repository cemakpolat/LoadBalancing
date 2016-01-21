/*
 * UPMonitoring.cpp
 *
 *  Created on: 27.07.2012
 *      Author: cem
 */

#include "UPMonitoring.h"
#include "UPMain.h"
//#include "UserAuthListObject.h"
#include "DataAccess.h"
//#include <string>
//#include "UserSessionProfile.h"
#include <vector>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <csignal>
#include <sys/time.h>
#include "Utilities.h"
#include <memory>
using namespace std;
UPMonitoring::UPMonitoring(UserAuthListObject* userObj) {
	this->Mac = userObj->Mac;
	this->IP = userObj->ip;
	this->userId = userObj->userId;

	this->token = userObj->token;
	this->left=false;
	this->insertIPTableCommands(this->IP, this->Mac);
	cout<<"User Profile Monitoring is created "<<"IP "<<this->IP<<" MAC: "<<this->Mac<<"active state: "<<userObj->active <<endl;

}

UPMonitoring::UPMonitoring(UserAuthListObject userObj) {

	this->Mac = userObj.Mac;
	this->IP = userObj.ip;
	this->userId = userObj.userId;
	this->token = userObj.token;
	this->left=false;
	this->insertIPTableCommands(this->IP, this->Mac);
	cout<<"User Profile Monitoring is created "<<"IP: "<<this->IP<<" MAC: "<<this->Mac <<endl;
}
UPMonitoring::UPMonitoring() {
}
UPMonitoring::~UPMonitoring() {
	delete uprofile;
}

void* UPMonitoring::run(void* object) {
	try{
		cout<<"User Profile Monitoring:UPMonitoring started"<<endl;
		UPMonitoring *upm=(UPMonitoring*)object;
		upm->timeInterval=10;//10 minutes//should be transported in ConfigParameters File in the further steps
		bool authenticatedState=UPMain::Instance()->checkUserAuthenticityByUID(upm->userId);//upm->checkUserAuthenticity(upm->userId);
		struct timeval arrTime,depTime;
		//upm->authenticated=true;
		//DataAccess *da=new DataAccess();

		if(authenticatedState ){
			cout<<"User Profile Monitoring:\" " <<upm->userId<<" \" is authenticated"<<endl;
			//upm->userJSONModel=da->getUserModel(upm->userId);
			upm->getUserModelFromStoragePlace();
			//upm->getUserModelFromStoragePlace(upm->userId,upm->userJSONModel);
			upm->uprofile=new UserSessionProfile(upm->userJSONModel,upm->userId,upm->IP, upm->timeInterval);
			gettimeofday(&arrTime,NULL);
			time_t arrivalTime=arrTime.tv_sec ;
			cout<<"User Profile Monitoring:Arrival Time: "<<Utilities::convertMilisecondToString(arrivalTime)<<endl;
			upm->uprofile->session->setArrivalTime(arrivalTime);//TODO:please check the taken time from cpu
			upm->uprofile->session->timeAssigned=false;
			upm->timer(false,upm);
			upm->uprofile->session->setAuthenticationNumber(upm->uprofile->session->getAuthenticationNumber()+1);
		}
		while(authenticatedState ){
		try{
			if(upm!=NULL){
					usleep(Utilities::second(2));
					authenticatedState=UPMain::Instance()->checkUserAuthenticityByUID(upm->userId);
				}else{
					cout<<"User Profile Monitoring:Client Object was gone!"<<endl;
					authenticatedState=false;
					throw "UPM is null!";
				}
			}catch(const char* e){
				cout<<e<<endl;
				pthread_exit(NULL);

			}
		}
		gettimeofday(&depTime,NULL);
		time_t departureTime=depTime.tv_sec;
		cout<<"User Profile Monitoring:Departure Time: "<<Utilities::convertMilisecondToString(departureTime)<<endl;
		upm->uprofile->session->setDepartureTime(departureTime);
		upm->uprofile->session->setTimeisAssigned(true);
		upm->timer(true,upm);
		upm->t.state=false;
		//usleep(Utilities::second(3));//after timer
		upm->storeUserModelInStoragePlace();
		upm->removeIPTableCommands(upm->IP, upm->Mac);
		cout<<"User Profile Monitoring:\" " <<upm->userId<<" \"'s Thread is going to leave!"<<endl;
		//upm->authenticated=false;
		//upm->left=true;
		//delete upm->uprofile;
		//delete da;
		pthread_exit(NULL);
		delete (UPMonitoring*)object;
		return NULL;
	}
	catch(exception& e){
		cerr<<"User Profile Monitoring: Exception occurred: "<< e.what()<<endl;
		run(object);
	}

	 //exit from thread, but this object will take place in memory.
}

void UPMonitoring::getUserModelFromStoragePlace() {//TODO: return object can be reference.
	std::auto_ptr<DataAccess> da(new DataAccess());
	this->userJSONModel=da->getUserModel(this->userId);

}

void UPMonitoring::storeUserModelInStoragePlace() {
	std::auto_ptr<DataAccess> da(new DataAccess());

	this->uprofile->transferUserProfileInfoIntoUserModel(this->userJSONModel);
	da->storeUserModel(this->userJSONModel,userId);
	//TODO:Clear bandwidth here
}

void UPMonitoring::timer(bool state,UPMonitoring *upm) {

	upm->t.state = true;
	upm->t.duration = Utilities::minute(1);//60 sn
	upm->t.count = 0;
	int rc;
	if(state==false){
		//CHANGE: thread object was altered.
		if ((rc = pthread_create(&upm->timerObj, NULL, &UPMonitoring::timerFunction, (void*) upm))) {
			cout<<"User Profile Monitoring:Thread creation failed: "<<rc<<endl;
		}
	}else {
		upm->t.state=false;
	}
}
//TODO:check Timer in terms of time intervalgetBytes
void *UPMonitoring::timerFunction(void* data){
		UPMonitoring *upm=(UPMonitoring*)data;
		//First meeting
		int restTime=upm->timeInterval-((upm->uprofile->session->arrivalTime/60)%60)%10;
		if(restTime<10){
			//cout<<"!!!restTime:    "<<restTime<<endl;
			usleep(upm->t.duration*restTime);
			upm->t.count=upm->timeInterval;
		}
		while (upm->t.state) {
			if (upm->t.count < upm->timeInterval) {
				upm->t.count++;
				usleep(upm->t.duration);
			}/* else {
				upm->t.state = false;
			}*/
			else if(upm->t.count==upm->timeInterval){
				upm->t.count=0;
				upm->callTimerTask(upm->userId);
			}
		}
	pthread_exit(NULL);
}
void UPMonitoring::callTimerTask(string& userId){
	printf("User Profile Monitoring:Periodic bandwidth function is invoked.\n");
	if(UPMain::Instance()->checkUserAuthenticityByUID(this->userId)){
		try {
			this->storeUserModelInStoragePlace();
			//TODO:Ask Mursel, it might be a bad policy!!!
			/*if(this->uprofile->bandwidth->currentDownloadByte==0 && this->uprofile->bandwidth->currentUploadByte==0){
				this->authenticated=false;
			}*/

		} catch (json::Exception& e) {
			cerr<<" User Profile Monitoring: "<<e.what()<<endl;
		}
		 catch (exception& e) {
				cerr<<" User Profile Monitoring: "<<e.what()<<endl;
		}
	}

}

void UPMonitoring::insertIPTableCommands(string& ip,string& mac){
	removeIPTableCommands(ip,mac);
	cout<<"User Profile Monitoring: Iptables rules are being inserted for IP: "<<ip<<endl;
	std::string in="iptables -I FORWARD -d "+ip+" -m mac --mac-source "+mac +" -j ACCEPT";
	std::string out="iptables -I FORWARD -s "+ip+" -m mac --mac-source "+mac +" -j ACCEPT";
	system(in.c_str());
	system(out.c_str());
}
void UPMonitoring::removeIPTableCommands(string& ip,string& mac){
	cout<<"User Profile Monitoring: Iptables rules are being removed for IP: "<<ip<<endl;

	std::string in="iptables -D FORWARD -d "+ip+" -m mac --mac-source "+mac +" -j ACCEPT";
	std::string out="iptables -D FORWARD -s "+ip+" -m mac --mac-source "+mac +" -j ACCEPT";
	system(in.c_str());
	system(out.c_str());
}


