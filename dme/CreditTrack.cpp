/*
 * CreditTrack.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#include "CreditTrack.h"
#include "Utilities.h"
#include "math.h"
#include "PracticalSocket.h"
#include "TCPConnection.h"

//TODO:CreditTrack is conceptualized as singleton, but in this implementation we avoided to use the singleton pattern,
//instead we access creditrack through MainDME. it might be possible in the further days to re-edit for singleton.
//using namespace std;
std::string CreditTrack::className="CreditTrack";
using namespace std;
CreditTrack::CreditTrack() {
	// TODO Auto-generated constructor stub
	this->unitResourceCost = 0;
	this->totalConsumedBandwidth = 0;
	this->consumedBandwidthDuringWaitTime = 0;
	this->totalCredits = 0;
	this->waitTime = 5 * 100;
	this->filePath = "/proc/net/dev";

	//QOE Tracker

	this->K_averageDelay = 1;
	this->A_averageDelay = 0.5;
	this->B_averageDelay = 10;
	this->k_averageDelay = -1;

	this->K_packetLossRate = 1;
	this->A_packetLossRate = 0.5;
	this->B_packetLossRate = 10;
	this->k_packetLossRate = -1;

	this->K_jitter = 1;
	this->A_jitter = 0.5;
	this->B_jitter = 10;
	this->k_jitter = -1;

	this->K_bitRate = 1;
	this->A_bitRate = 0.5;
	this->B_bitRate = 10;
	this->k_bitRate = -1;

	this->averageDelayNormalizationFactor = 0.2;
	this->packetLossRateNormalizationFactor = 10;
	this->jitterNormalizationFactor = 0.2;
	this->bitRateNormalizationFactor = 700;

	//QoE is started here in the original code.
	this->debugUnitResource=this->className+"_unitResourceCost.txt";
	this->debugTotalCredits=this->className+"_totatlCredits.txt";
	this->debugTimer=this->className+"_bandwidthUtilization.txt";


}

CreditTrack::~CreditTrack() {
	// TODO Auto-generated destructor stub
}
void* CreditTrack::run(void* object) {

	CreditTrack *ct=(CreditTrack*)object;
	ct->setTotalPreviousConsumedBWtoZero();
	while(true){
		try{

		}catch(std::exception& e){
			cout<<className<<" "<<e.what()<<endl;
		}
		ct->unitResourceCost=ct->calculateCurrentUnitResourceCost();
		cout<<"Unit resource cost "<<ct->unitResourceCost<<endl;
		ct->consumedBandwidthDuringWaitTime=ct->getConsumedBWDutingWaitTime();
		cout<<"Consumed Bandwidth "<<ct->consumedBandwidthDuringWaitTime<<endl;
		ct->totalCredits=ct->totalCredits+ct->consumedBandwidthDuringWaitTime*ct->unitResourceCost;
		cout<<"Total Credit "<<ct->totalCredits<<endl;

		string toWrite=Utilities::convertDoubleToString(ct->totalCredits);
		ct->fileWrite(ct->debugTotalCredits,toWrite);

		toWrite=Utilities::convertDoubleToString(ct->unitResourceCost);
		ct->fileWrite(ct->debugUnitResource,toWrite);

		toWrite=Utilities::convertDoubleToString(ct->consumedBandwidthDuringWaitTime);
		ct->fileWrite(ct->debugUnitResource,toWrite);

		toWrite = Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
		ct->fileWrite(ct->debugTimer,toWrite);


	}
	pthread_exit(0);


}
void CreditTrack::setTotalPreviousConsumedBWtoZero() {
	try {
		ifstream infile;
		infile.open(filePath.c_str());
		string sLine;
		char del = '/';//TODO: this delimeter is not true, you should change it: ask mursel what means "\\s+", probably whole space
		while (!infile.eof()) {
			getline(infile, sLine);

			std::vector<std::string> temp = Utilities::split(sLine, del);
			for (unsigned int i = 0; i < temp.size(); i++) {
				if (std::string::npos != temp[i].find("ath1")) {
					long previousTransmitted = atol(temp[i + 9].c_str());
					long previousReceived = atol(temp[i + 1].c_str());
					long totalBWConsumption = previousTransmitted+ previousReceived;
					this->totalConsumedBandwidth = totalBWConsumption;
				}
			}
			cout << sLine << endl;
		}
		infile.close();

	} catch (std::exception& e) {
		cerr << this->className << e.what() << endl;
		this->consumedBandwidthDuringWaitTime=0;
	}
}
double CreditTrack::getConsumedBWDutingWaitTime() {

	try {
			ifstream infile;
			infile.open(filePath.c_str());
			string sLine;
			char del = '/';//TODO: this delimeter is not true, you should change it: ask mursel what means "\\s+", probably whole space
			while (!infile.eof()) {
				getline(infile, sLine);

				std::vector<std::string> temp = Utilities::split(sLine, del);
				for (unsigned int i = 0; i < temp.size(); i++) {
					if (std::string::npos != temp[i].find("ath1")) {
						long previousTransmitted = atol(temp[i + 9].c_str());
						long previousReceived = atol(temp[i + 1].c_str());
						long totalBWConsumption = previousTransmitted+ previousReceived;
						this->consumedBandwidthDuringWaitTime=totalBWConsumption-this->totalConsumedBandwidth;
						this->totalConsumedBandwidth = totalBWConsumption;
					}
				}
				cout << sLine << endl;
			}
			infile.close();

		} catch (std::exception& e) {
			cerr << this->className << e.what() << endl;
			this->consumedBandwidthDuringWaitTime=0;
		}
		if(this->consumedBandwidthDuringWaitTime<0){
			return 0;
		}
		else{
			return this->consumedBandwidthDuringWaitTime/1000;
		}
}
double CreditTrack::calculateCurrentUnitResourceCost() {
	double averageDelayComp = getAverageDelayComponent();
	double averageJitterComp = getAverageJitterComponent();
	double averagePacketLossComp = getAveragePacketLossComponent();
	double bitRateComp = getAverageBitRateComponent();
	double temp = averageDelayComp + averageJitterComp+ averagePacketLossComp + bitRateComp;
	cout<<"unit resource components: "<<bitRateComp<<" "<<averagePacketLossComp<<" "<<averageJitterComp<<" "<<averageDelayComp<<endl;
	cout<<"Calculate current unit resource unit: "<<temp<<endl;

	string unitResourceFileName = this->className + "_unitResourceBasedOnAverageDelay.txt";
	string toWrite=Utilities::convertDoubleToString(temp);
	this->fileWrite(unitResourceFileName, toWrite);
	string averageDelayFileName = this->className + "averageDelayCompUnitResourceCost.txt";
	toWrite=Utilities::convertDoubleToString(averageDelayComp);
	this->fileWrite(averageDelayFileName,toWrite);
	return  temp;
}
double CreditTrack::getAverageBitRateComponent() {
	// TODO Auto-generated method stub
	double averageBitRate = this->averageBitRate();
	cout << "Average bit rate is " << averageBitRate << endl;
	if (averageBitRate == 0) {
		return 0;
	} else {
		double expIn = k_bitRate * A_bitRate * (B_bitRate - averageBitRate);
		double expPart = exp(expIn);
		double temp = K_bitRate * (1.0 / (1.0 + expPart));
		return temp;
	}
}
double CreditTrack::getAveragePacketLossComponent() {
	double averagePacketLoss = this->averagePacketLoss();

		cout << "Average  packet loss  is " << averagePacketLoss << endl;
		if (averagePacketLoss == 0){
			return 0;
		}
		else{
			double expIn = k_packetLossRate * A_packetLossRate * (B_packetLossRate - averagePacketLoss);
			double expPart = exp( expIn );
			double temp = K_packetLossRate * ( 1.0 / (1.0 + expPart) ) ;
			return temp;
		}
}
double CreditTrack::getAverageJitterComponent() {
	double averageJitter = this->averageJitter();
		cout << "Average jitter  is " << averageJitter << endl;

		if( averageJitter == 0){
			return 0;
		}
		else{
			double expIn = k_jitter * A_jitter * (B_jitter - averageJitter);
			double expPart = exp( expIn );
			double temp = K_jitter * (  1.0 / (1.0 + expPart) );
			return temp;
		}
}
double CreditTrack::getAverageDelayComponent() {
	double averageDelay = this->averageDelay();

			cout << "Average delay  is " << averageDelay << endl;

			if (averageDelay == 0){
				return 0;
			}
			else{
				double expIn =  k_averageDelay * A_averageDelay * (B_averageDelay - averageDelay);
				double expPart = exp( expIn );
				double temp = K_averageDelay * (  1.0 / (1.0 + expPart) );
				return temp;
			}
}
void CreditTrack::setTotalconsumedBW(long totalBW) {
	this->totalConsumedBandwidth = totalBW;
}
long CreditTrack::getTotalConsumedBW() {
	return this->totalConsumedBandwidth;
}
void CreditTrack::setCredit(long credit) {
	this->totalCredits=credit;
}
void CreditTrack::addCredit(long credit) {
	this->totalCredits=credit+this->totalCredits;
}
void CreditTrack::removeCredit(long credit) {
	this->totalCredits=this->totalCredits-credit;
}
double CreditTrack::getOwnCredit() {
	return this->totalCredits;
}
int CreditTrack::getUnitResourceCost() {
	return this->unitResourceCost;
}
double CreditTrack::averageBitRate() {
	return sendGetMessage(CommLang::getBitRate);

}
double CreditTrack::averagePacketLoss() {
	return sendGetMessage(CommLang::getPacketLoss);
}

double CreditTrack::averageDelay() {

	string averageDelayFileName = this->className + "_averageDelayForUnitResourceCost.txt";
	double res=sendGetMessage(CommLang::getAverageDelayTime);
	double maximumAverageDelay=(res*this->averageDelayNormalizationFactor)/10;
	string a=Utilities::convertDoubleToString(maximumAverageDelay);
	this->fileWrite(averageDelayFileName,a);

}

double CreditTrack::sendGetMessage(int requestedValue){
	double val;
	TCPConnection *con=new TCPConnection();
	val=con->sendReqToUBMRecvMessageFromUBM(requestedValue);
	delete con;
	return val;
}


double CreditTrack::averageJitter(){
	return sendGetMessage(CommLang::getAverageJitter);
}


void CreditTrack::fileWrite(std::string& debugOutFile,
		std::string& outputString) {
	try {
		std::ofstream file(debugOutFile.c_str());
		if (file.is_open()) {
			file << outputString;
			file << "\n";
			file.close();
		}
	} catch (std::exception& e) {
		std::cerr << className << " " << e.what() << std::endl;
	}
}

