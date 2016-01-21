/*
 * UserSessionProfile.cpp
 *
 *  Created on: 06.08.2012
 *      Author: cem
 *
 * JSONModel Sample For UserProfiling\n\n

	{\n
				"userId":"", \n
				"bandwidthAverage":,\n
				"authenticationNumber":,\n
				"sessionAverageTime":"",\n
				"dailyAuthenticationNumber":,\n
				"visitFrequencyNumber":"",\n
				"visitingFrequency":"",\n
				"lastDepartureTime":,\n
				"sessionsPerMin":[ {"visitingFrequency":"","bandwidth":},\n
							 	   {"visitingFrequency":"","bandwidth":},\n
														....\n
								 ]\n
	} \n
 */

#include "UserSessionProfile.h"

using namespace json;

UserSessionProfile::UserSessionProfile() {
	this->bandwidth = new UserBandwidth();
	this->session = new UserSession();
}
UserSessionProfile::UserSessionProfile(Object& userModel, String userId, String ip,
		int timeInterval) {
//	std::cout<<"UserSessionProfile start"<<std::endl;
	this->bandwidth = new UserBandwidth();
	this->bandwidth->ip = ip;
	this->bandwidth->userId = userId;
	this->session = new UserSession();
	this->userId = userId;
	this->timeInterval = timeInterval;

	try {
		this->getUserModelInfo(userModel);
	} catch (Exception& e) {
		e.what();
	}
	//std::cout<<"UserSessionProfile ended"<<std::endl;
}
UserSessionProfile::~UserSessionProfile() {
	delete bandwidth;
	delete session;
}
void UserSessionProfile::getUserModelInfo(Object& userModel) {
	this->firstUserSessionsDeployment(userModel);
	this->bandwidth->getParametersFromUserModel(userModel);
	this->session->getParametersFromUserModel(userModel);
}

void UserSessionProfile::firstUserSessionsDeployment(Object& userModel) {
	try{
	String& userId = userModel["userId"];
	if (userId.Value()=="") {
		int size = 24 * 60 / this->timeInterval;
		Array sessionsPerMin;
		for (int i = 0; i < size; i++) {
			Object sessionSample;
			sessionSample["visitingFrequency"] = Number(0);
			sessionSample["bandwidth"] = Number(0);
			sessionsPerMin.Insert(sessionSample);
		}
		userModel["sessionsPerMin"] = sessionsPerMin;
	}
	}catch(json::Exception& e){
		cerr<<"User Session Profile: JSON Exception occurewd,while creating new profile"<<endl;
	}
}

void UserSessionProfile::transferUserProfileInfoIntoUserModel(
		Object& userModel) {
		this->session->setParametersToUserModel(userModel,this, this->bandwidth->ip);
		this->bandwidth->setParametersToUserModel(userModel,this);
			try {
				userModel["userId"]=String(this->userId);
			} catch (json::Exception& e) {
				// TODO Auto-generated catch block
				e.what();
			}
}
