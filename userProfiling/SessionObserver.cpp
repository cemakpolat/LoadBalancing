/*
 * SessionObserver.cpp
 *
 *  Created on: Mar 18, 2013
 *      Author: cem
 */

#include "SessionObserver.h"
#include "UPMain.h"
#include "IWConfig.h"
#include "Utilities.h"
using namespace std;

//int periodOfDayForStoring=0;
//singleton pattern required
SessionObserver* SessionObserver::m_sesObInstance = NULL;
SessionObserver* SessionObserver::Instance() {
	if (!m_sesObInstance)   // Only allow one instance of class to be generated.
		m_sesObInstance = new SessionObserver();
	return m_sesObInstance;
}
SessionObserver::SessionObserver() {
	// TODO Auto-generated constructor stub
	WhichDay=1;
	WhichPeriodInDay=1;
	meetingCounter=1;
	dailyCounter=0;
	currentAuthenticationState=0;
	previousBlockProbability=0;
	bandwidthCounter=0;
	previousBWIN=0;
	previousBWOUT=0;
	oldStateAuthAverage=0;
	previousIncreaseGrade=0;
	this->unstabilityCounter=0;
	this->unStableStateMin=0.3;
	this->unStableStateMax=0.7;
}

SessionObserver::~SessionObserver() {
	// TODO Auto-generated destructor stub
}


void* SessionObserver::run(void * object){

	SessionObserver *so=(SessionObserver*)object;
	vector<double> list;

	int authenticated;//=UPMain::Instance()->getAuthenticatedUserCount();
	//bool authenticated=UPMain::getUserAuthenticated(userId);
	while(true){//TODO: While true
		authenticated=UPMain::Instance()->getAuthenticatedUserCount();
		//Historical data can be evaluated here after analyzing the whole aggregated measurements.


		if(so->dailyCounter==6*10*60*24){
			so->meetingCounter++;
			so->WhichDay++;
			so->WhichPeriodInDay=0;
			if(so->WhichDay>7){
				so->WhichDay=0;
			}
		}

		if(authenticated>0){//From UBM
			so->currentAuthenticationState=1;
			list.push_back(1);
		}else{
			so->currentAuthenticationState=0;
			list.push_back(0);
		}
		so->currentAuthenticationState=(so->previousBlockProbability+so->currentAuthenticationState)/2;
		so->previousBlockProbability=so->currentAuthenticationState;

		//Event Counter for transmission power increase/decrease
		if(so->eventCounter==6){
			so->changeTXPower(so->currentAuthenticationState);
			so->eventCounter=0;
			so->oldStateAuthAverage=so->currentAuthenticationState;

		}
		//Bandwidth Observer
		if(so->bandwidthCounter==12){

			vector<string> currentEventBlock=Utilities::split(so->observationList[so->storingEventCounter],',');

			//This is valid for the first time
			if(currentEventBlock.size()<list.size()){
				currentEventBlock.clear();
				for(int i=0;i<12;i++){
					currentEventBlock.push_back(0);
				}
			}

			for(unsigned int i=0;i<list.size();i++){

				list[i]=(Utilities::convertStringDouble(currentEventBlock[i])*so->meetingCounter+list[i])/(so->meetingCounter+1);//that should be average
			}

			so->observationList[so->storingEventCounter]=so->convertVectorToString(list);//co


			//callIPTables
			vector<string> bandwidthBlock=Utilities::split(so->bandwidthObservationList[so->storingEventCounter],',');

		//	double currentBandwidthAmount=0;
			bandwidthObj obj=so->getBytes();
			double bandwidthAmountIN=obj.IN-so->previousBWIN;
			double bandwidthAmountOUT=obj.OUT-so->previousBWOUT;

			if(bandwidthAmountIN <0 && bandwidthAmountOUT<0){
				bandwidthAmountIN=0;
				bandwidthAmountOUT=0;
			}

			so->previousBWIN=bandwidthAmountIN;
			so->previousBWOUT=bandwidthAmountOUT;

			bandwidthBlock[0]=(Utilities::convertStringDouble(bandwidthBlock[0])*so->meetingCounter+bandwidthAmountIN)/(so->meetingCounter+1);
			bandwidthBlock[1]=(Utilities::convertStringDouble(bandwidthBlock[1])*so->meetingCounter+bandwidthAmountOUT)/(so->meetingCounter+1);
			so->bandwidthObservationList[so->storingEventCounter]= bandwidthBlock[0]+","+bandwidthBlock[1];

			so->dailyTakenMeasurement(list,bandwidthAmountIN,bandwidthAmountOUT);

			so->bandwidthCounter=0;
			list.clear();
		}

		//Storing Event Counter
		if(so->storingEventCounter==60*6*4){//10 minutes
			so->stroreObservations();
			so->storingEventCounter=0;
			//these variables should normally be 0, however in any except case we have to start a new period in a day
			so->bandwidthCounter=0;
			so->eventCounter=0;

			so->WhichPeriodInDay++;
			if(so->WhichPeriodInDay==4){
				so->WhichPeriodInDay=0;
			}
			so->getPeriodicalData();
		}

		so->eventCounter++;
		so->bandwidthCounter++;
		so->storingEventCounter++;
		so->dailyCounter++;
	}

}
void SessionObserver::dailyTakenMeasurement(vector<double> list,double in,double out) {
	vector<string> currentEventBlock = Utilities::split(
			this->observationListDaily[this->storingEventCounter], ',');
	double newValue = 0;
	//This is valid for the first time
	if (currentEventBlock.size() < list.size()) {
		currentEventBlock.clear();
		for (int i = 0; i < 12; i++) {
			currentEventBlock.push_back(0);
		}
	}

	for (unsigned int i = 0; i < list.size(); i++) {

		newValue = (Utilities::convertStringDouble(currentEventBlock[i])
				* this->meetingCounter + list[i]) / (this->meetingCounter + 1); //that should be average
		list[i] = newValue;
	}

	this->observationListDaily[this->storingEventCounter] =
			this->convertVectorToString(list);


	////---------------
	vector<string> bandwidthBlock=Utilities::split(this->bandwidthObservationListDaily[this->storingEventCounter],',');

	bandwidthBlock[0]=(Utilities::convertStringDouble(bandwidthBlock[0])*this->meetingCounter+in)/(this->meetingCounter+1);
	bandwidthBlock[1]=(Utilities::convertStringDouble(bandwidthBlock[1])*this->meetingCounter+out)/(this->meetingCounter+1);

	this->bandwidthObservationListDaily[this->storingEventCounter]= bandwidthBlock[0]+","+bandwidthBlock[1];


}
string SessionObserver::convertVectorToString(vector<double>& list){
string str="";
	for(unsigned int i=0;i<list.size()-1;i++){
		str=str+Utilities::convertDoubleToString(list[i])+",";
	}
	str=str+ Utilities::convertDoubleToString(list[list.size()-1]);
	return str;
}
void SessionObserver::getPeriodicalData() {
	string fileName = Utilities::convertIntToString(this->WhichDay) + "_" + Utilities::convertIntToString(this->WhichPeriodInDay);
	string in = Utilities::readFile(fileName);
	observationList = Utilities::split(in, ';');

	//Daily Taken Authentication Measurement
	fileName = "Generic_" +  Utilities::convertIntToString(this->WhichPeriodInDay);
	in = Utilities::readFile(fileName);
	observationListDaily = Utilities::split(in, ';');


	fileName = Utilities::convertIntToString(this->WhichDay) + "_" + Utilities::convertIntToString(this->WhichPeriodInDay) + "_BW";
	in = Utilities::readFile(fileName);
	bandwidthObservationList = Utilities::split(in, ';');

	//Daily taken bandwidth Measurement
	fileName = "Generic_" +  Utilities::convertIntToString(this->WhichPeriodInDay)+"_BW";
	in = Utilities::readFile(fileName);
	bandwidthObservationListDaily = Utilities::split(in, ';');

}
void SessionObserver::stroreObservations() {
	string str = "";
	for (unsigned int i = 0; i < this->observationList.size(); i++) {
		str = str + observationList[i] + ";";
	}
	string fileName = this->WhichDay + "_" + this->WhichPeriodInDay;
	Utilities::fileWrite(fileName, str);
	fileName = "Generic_" + this->WhichPeriodInDay;
	Utilities::fileWrite(fileName, str);

	//Bandwidth Storing
	str = "";
	for (unsigned int i = 0; i < this->bandwidthObservationList.size(); i++) {

		str = str + bandwidthObservationList[i] + ";";
	}
	fileName = Utilities::convertIntToString(this->WhichDay) + "_" + Utilities::convertIntToString(this->WhichPeriodInDay) + "_BW";
	Utilities::fileWrite(fileName, str);
	fileName = "Generic_" + Utilities::convertIntToString(this->WhichPeriodInDay)+"_BW";
	Utilities::fileWrite(fileName, str);

	this->observationList.clear();
	this->bandwidthObservationList.clear();
}


//TODO:Look at total consumed data

bandwidthObj SessionObserver::getBytes(){
	system("iptables -L FORWARD -n -v -x | awk '$1 ~ /^[0-9]+$/ { print  $6,$7 , $2 }'>IPT_BW.txt");

	std::ifstream infile("/root/IPT_BW.txt");
	double byteForwardedToClients = 0;
	try {

		std::string line;
		while (std::getline(infile, line)) {
			std::istringstream iss(line);
			std::string in, out, byte;
			if ((iss >> in >> out >> byte)) {
				if (in == "*" && out == "*") {	//ip target to all
					byteForwardedToClients = atoi(byte.c_str());

				}
			}
		}
		double byteForwardedFromUsers = 0;
		bandwidthObj obj;
		obj.IN = byteForwardedToClients;
		obj.OUT = byteForwardedFromUsers;
		return obj;
	} catch (std::exception& e) {
		e.what();
		bandwidthObj obj;
		obj.IN = 0;
		obj.OUT = 0;
		return obj;
	}

}

void SessionObserver::increaseTXPower(int grade) {
	IWConfig *iw = new IWConfig();
	if (iw->isGivenPowerAcceptable(grade)) {
		cout << "Client: Transmission Power is increased" << endl;
	} else {
		cout << "Client: You use the maximum transmission power level. "
				<< endl;
	}
	delete iw;
}
void SessionObserver::decreaseTXPower(int grade) {
	IWConfig *iw = new IWConfig();
	if (iw->isGivenPowerAcceptable(grade)) {
		cout << "Client: Transmission Power is decreased" << endl;
	} else {
		cout << "Client: You use the minimum transmission power level. "
				<< endl;
	}
	delete iw;
}

void SessionObserver::changeTXPower(double currentState) {
	bool state = true;
	if ((currentState - oldStateAuthAverage) < distanceBetweenBlocks) {
		//do Nothing
		state = false;
	}
	if (state) {
		if (currentState < unAuthenticatedThreshold) {
			decreaseTXPower(LEVEL1);
		} else if (currentState > authenticatedThreshold) {
			increaseTXPower(LEVEL5);
		} else if (unStableStateMin < currentState
				&& currentState < unStableStateMax) {
				unstabilityCounter++;
			if (unstabilityCounter > 2) { //subsequently
				int increaseGrade = previousIncreaseGrade + 2;
				increaseTXPower(increaseGrade);
				previousIncreaseGrade = increaseGrade;
				unstabilityCounter=0;
			} else {
				decreaseTXPower(LEVEL1);
			}

		}
	}
}

//void SessionObserver::insertIPTableCommands(){
//	//removeIPTableCommands();
//	std::string in="iptables -I FORWARD -j ACCEPT";
//	std::string out="iptables -I FORWARD  -j ACCEPT";
//	system(in.c_str());
//	system(out.c_str());
//}
//void SessionObserver::removeIPTableCommands(){
//	std::string in="iptables -D FORWARD  -j ACCEPT";
//	std::string out="iptables -D FORWARD  -j ACCEPT";
//	system(in.c_str());
//	system(out.c_str());
//}
