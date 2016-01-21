/*
 * UserSession.cpp
 *
 *  Created on: 07.08.2012
 *      Author: cem
 */
#include "UserSession.h"
#include "UserBandwidth.h"
#include "UserSessionProfile.h"
#include <vector>
#include "BlackBoard.h"
#include "Utilities.h"
#include "writer.h"
#include "elements.h"
#include "reader.h"

using namespace json;
UserSession::UserSession() {

}
UserSession::~UserSession() {

}
void UserSession::calculateSessionTime() {

	this->sessionTime=difftime(this->departureTime,this->arrivalTime);
	std::cout<<"User Session:Arrival Time: "<<Utilities::getTimeFromGivenTime(this->arrivalTime)<<std::endl;
	std::cout<<"User Session:Departure Time: "<<Utilities::getTimeFromGivenTime(this->departureTime)<<std::endl;
}
void UserSession::calculateSessionAverage() {
	this->calculateSessionTime();
	this->sessionAverage=(this->sessionAverage*(this->authenticationNumber-1)+this->sessionTime)/this->authenticationNumber;
	//std::cout<<"User Session:Session Time: "<<this->sessionAverage<<std::endl;
}
void UserSession::calculateVisitingFrequency(unsigned int &place,Object &userModel) {
	Array arraySesMin = userModel["sessionsPerMin"];
	Number jsonValue;
	try{
		for (unsigned int j = 0; j < arraySesMin.Size(); j++) {
			Object objSession = arraySesMin[j];
			jsonValue = objSession["visitingFrequency"];
			this->visitingFrequency = jsonValue.Value(); //TODO: may be exception due to the
			if (j == place) {
				this->visitingFrequency = (((this->visitFrequencyNumber-1) * this->visitingFrequency) + 1 )/ this->visitFrequencyNumber;
				objSession["visitingFrequency"] = Number(this->visitingFrequency);
			}else{
				this->visitingFrequency = ((this->visitFrequencyNumber-1) * this->visitingFrequency )/ this->visitFrequencyNumber;
				objSession["visitingFrequency"] = Number(this->visitingFrequency);
			}
			arraySesMin[j] = objSession;
		}
	userModel["sessionsPerMin"]=arraySesMin;
	}catch(json::Reader::ParseException& e){
		cerr<<"User Session: "<<e.what()<<endl;
	}
	catch(exception& e){
		cerr<<"User Session: "<<e.what()<<endl;
	}
}

void UserSession::addListToJSON(json::Object& userModel, UserSessionProfile *uprofile, std::string& ip) {

	time_t currentTime;
	currentTime = time(NULL);
	checkDailyAuthNum(currentTime, this->lastDepartureTime);
	this->visitFrequencyNumber = this->visitFrequencyNumber + 1;
	std::string sessionTime = Utilities::getCurrentTimeAsString();
	int interval = 10;
	std::vector<std::string> startTime;
	try {
		startTime = Utilities::split(Utilities::getTimeFromGivenTime(this->getAdequateTime(sessionTime, interval)), ':');
		//cout<<"STARTTIME "<<startTime[0]<<":"<<startTime[1]<<endl;
		unsigned int placeOfTime = atoi(startTime[0].c_str()) * 6+ (atoi(startTime[1].c_str()) / uprofile->timeInterval);
		Array arraySesMin = userModel["sessionsPerMin"];
		Object objSession = arraySesMin[placeOfTime];
		const Number sessionBW = objSession["bandwidth"];
		uprofile->bandwidth->sessionBandwidthAveragePerMin = sessionBW.Value();
		//uprofile->bandwidth->calculateSessionBandwidthAveragePerMin(*uprofile,ip);
		uprofile->bandwidth->calculateSessionBandwidthAveragePerMin(uprofile,ip);
		objSession["bandwidth"] = Number(uprofile->bandwidth->sessionBandwidthAveragePerMin);
		arraySesMin[placeOfTime] = objSession;
		userModel["sessionsPerMin"] = arraySesMin;
		this->calculateVisitingFrequency(placeOfTime, userModel); //TODO:check result
		userModel["visitFrequencyNumber"] = Number(this->visitFrequencyNumber); //add new daily VFP
	} catch (json::Exception& e) {
		cerr << "User Session: " << e.what() << endl;
	} catch (exception& e) {
		cerr << "User Session: " << e.what() << endl;
	}
}
time_t UserSession::getAdequateTime(std::string& t,int& timeInterval) {
	std::vector<std::string> time=Utilities::split(t,':');

	int hour=atoi(time[0].c_str());
	int min=atoi(time[1].c_str());

	int rest=min%timeInterval;
	int division=min/timeInterval;

	    if((hour!=0)){                                                  //23:xx
	        if(min==0){                                                 //12:00
	            hour=hour-1;                                            //11:00
	            time[0]=Utilities::convertIntToString(hour);
	            time[1]="50";                                           //11:50
	        }else{                                                          //12:xx
	            if(rest==0){                                                //12:50
	                    int result=division*10-timeInterval;                //12:40
	                    time[1]=Utilities::convertIntToString(result);
	            }else{
	                int result=division*10;                             //12:02
	                time[1]=Utilities::convertIntToString(result);      //12:10
	            }
	        }
	    }else{//00:xx
	        if(min==0){                                                 //00:00
	            time[0]="23";//decrease one hour                        //23:50
	            time[1]="50"; // set minute to 00

	        }else{
	            if(rest==0){                                            //00:50
	                int result=division*10-timeInterval;                //00:40
	                time[1]=Utilities::convertIntToString(result);
	            }else{
	                int result=division*10;                             //00:02
	                time[1]=Utilities::convertIntToString(result);      //00:00
	            }
	        }
	    }
		return Utilities::convertTimeStringToMilisecond(time[0]+":"+time[1]+":00");
}

void UserSession::setParametersToUserModel(json::Object& userModel,
		UserSessionProfile *uprofile, std::string &ip) {
	//std::cout<<"User Session: Storage Request for user \" "<<uprofile->userId<<" \""<<std::endl;
	try {
		userModel["authenticationNumber"]=Number(this->authenticationNumber);
		addListToJSON(userModel, uprofile, ip);
		if(this->isTimeAssigned()){
			this->calculateSessionAverage();
			userModel["sessionAverage"]=String(Utilities::longToTimeString(this->sessionAverage));//convert
			userModel["dailyAuthenticationNumber"]=Number(this->dailyAuthenticationNumber);
			userModel["lastDepartureTime"]= String(Utilities::getCurrentDateAndTimeAsString());//day hour converting should be done.
		}
	} catch (json::Exception& e) {
		cerr<<"User Session: "<<e.what()<<endl;
	}catch(exception& e){
		cerr<<"User Session: "<<e.what()<<endl;
	}
}
void UserSession::getParametersFromUserModel(json::Object& userModel) {
	try {

		Number an=userModel["authenticationNumber"];
		this->authenticationNumber=an;
		//Number dp=userModel["dailyVFProbability"];
		Number dp=userModel["visitFrequencyNumber"];
		//this->dailyVFProbability=dp;
		this->visitFrequencyNumber=dp;
		Number da=userModel["dailyAuthenticationNumber"];
		this->dailyAuthenticationNumber=da;
		String& ld=userModel["lastDepartureTime"];
		this->lastDepartureTime=Utilities::convertDateTimeStringToMilisecond(ld.Value());//converting should be performed
		String& sa=userModel["sessionAverage"];
		this->sessionAverage=Utilities::convertStringToMilisecond(sa.Value());

	} catch (json::Exception& e) {
		// TODO Auto-generated catch block
		cerr<<"User Session: "<<e.what()<<endl;
	}catch(exception& e){
		cerr<<"User Session: "<<e.what()<<endl;
	}

}
void UserSession::checkDailyAuthNum(time_t now,time_t lastDepart) {
	//get days according to the given data.

	struct tm * timeinfo1;
	timeinfo1 = localtime(&now);

	struct tm * timeinfo2;
	timeinfo2 = localtime(&lastDepart);
	int day1=timeinfo1->tm_mday;
	int day2=timeinfo2->tm_mday;
    int month1=timeinfo1->tm_mon+1;
    int month2=timeinfo2->tm_mon+1;

   // int year1=timeinfo1->tm_year+1900;//if needed we can add also year but it is not required in normal case.
    //int year2=timeinfo1->tm_year+1900;

	if (this->dailyAuthenticationNumber == 0 && this->lastDepartureTime == 0) {//first meeting
		this->dailyAuthenticationNumber = this->dailyAuthenticationNumber + 1;
	}

	if (day1 != day2) {
		this->dailyAuthenticationNumber = this->dailyAuthenticationNumber + 1;
	} else if (day1==day2 && month1!=month2){
        this->dailyAuthenticationNumber = this->dailyAuthenticationNumber + 1;
	}
	//    else if (day1==day2 && month1==month2 && year1!=year2){
	//        this->dailyAuthenticationNumber = this->dailyAuthenticationNumber + 1;
	//		this->dailyVFProbability = 0;
	//        this->lastDepartureTime=now;//I added here
	//
	//        //	std::cout << "days are  equal days:" << +day1 << " " << day2<< std::endl;
	//	}

}
void UserSession::setArrivalTime(time_t arrivalTime) {
	this->arrivalTime=arrivalTime;
}
void UserSession::setDepartureTime(time_t departureTime) {

	this->departureTime=departureTime;
}
void UserSession::setTimeisAssigned(bool isTimeAssigned) {
	this->timeAssigned=isTimeAssigned;
}
bool UserSession::isTimeAssigned() {
	return this->timeAssigned;
}
long UserSession::getAuthenticationNumber() {
	return this->authenticationNumber;
}
void UserSession::setAuthenticationNumber(long authenticationNum) {
	this->authenticationNumber=authenticationNum;
}

