/*
 * UserBandwidth.cpp
 *
 *  Created on: 07.08.2012
 *      Author: cem
 */
#include "UserBandwidth.h"
#include "UserSessionProfile.h"
#include <sstream>
#include <stdio.h>
#include "Utilities.h"
#include "UPMain.h"

#include <fstream>

UserBandwidth::UserBandwidth() {
	totalUploadBW=0;
	totalUploadBW=0;
}
UserBandwidth::~UserBandwidth() {
}

void UserBandwidth::calculateTotalBandwidth(string& ip) {
	std::cout<<"UBM: Traffic information for client: 	" << this->userId <<std::endl;
	getBytes();
	vector<iptObject> templist = this->iptList;
	long int totalByteRecieved=0;
	long int totalByteTransmitted = 0;
	string IN="IN";
	string OUT="OUT";
	for(unsigned int i=0;i<this->iptList.size();i++){
		if(templist[i].direction  == IN){
			totalByteRecieved=this->iptList[i].byte-this->totalDownloadBW ;
			this->totalDownloadBW=this->iptList[i].byte;
		}
		else if(templist[i].direction == OUT){
			totalByteTransmitted = this->iptList[i].byte -this->totalUploadBW;
			this->totalUploadBW = this->iptList[i].byte;
		}
	}
	cout<<"User Bandwidth:Periodic accounting session upload: 	"<<totalByteTransmitted << " bytes"<<endl;
	cout<<"User Bandwidth:Periodic accounting session download: "<<totalByteRecieved<< " bytes"<<endl;
	cout<<"User Bandwidth:Total Upload: 	"<<this->totalUploadBW<< " bytes"<<endl;
	cout<<"User Bandwidth:Total Download: 	"<<this->totalDownloadBW<< " bytes"<<endl;
	this->iptList.clear();
	this->currentDownloadByte=totalByteRecieved;
	this->currentUploadByte=totalByteTransmitted;
}

void UserBandwidth::calculateSessionBandwidthAveragePerMin(UserSessionProfile& uprofile, std::string& ip) {
	this->calculateTotalBandwidth(ip);
	//TODO:add here also UPload code
	sessionBandwidthAveragePerMin=(this->sessionBandwidthAveragePerMin*(uprofile.session->dailyAuthenticationNumber-1)+this->currentDownloadByte/(uprofile.timeInterval*60))/(uprofile.session->dailyAuthenticationNumber);
}
void UserBandwidth::calculateSessionBandwidthAveragePerMin(UserSessionProfile  *uprofile, std::string& ip) {
	this->calculateTotalBandwidth(ip);
	//TODO:add here also UPload code
	sessionBandwidthAveragePerMin=(this->sessionBandwidthAveragePerMin*(uprofile->session->dailyAuthenticationNumber-1)+this->currentDownloadByte/(uprofile->timeInterval*60))/(uprofile->session->dailyAuthenticationNumber);
}

void UserBandwidth::calculateBandwidthAverage(UserSession& session) {
	std::cout<<"User Bandwidth:BandwidthAverage: 		"<<this->bandwidthAverage<<" bytes"<<std::endl;
	std::cout<<"User Bandwidth:TotalDownloadBW: 		"<<this->totalDownloadBW<< " bytes"<<std::endl;
	std::cout<<"User Bandwidth:AuthenticationNumber: 	"<<session.authenticationNumber<<std::endl;
	std::cout<<"User Bandwidth:Session Frequency: 		"<<session.sessionTime<<std::endl;
	std::cout<<"User Bandwidth:Session Average Time : 	"<<Utilities::longToTimeString(session.sessionAverage)<<std::endl;

	bandwidthAverage=(this->bandwidthAverage*(session.authenticationNumber-1)+this->totalDownloadBW/(session.sessionTime))/session.authenticationNumber;
}

void UserBandwidth::setParametersToUserModel(json::Object& userModel,
		UserSessionProfile *uprofile) {
	try {
		if (uprofile->session->isTimeAssigned()) {
			this->calculateBandwidthAverage(*(uprofile->session));
			Number bwa=this->bandwidthAverage;
			userModel["bandwidthAverage"] = bwa;
		}
	}catch (json::Exception& e) {
		// TODO Auto-generated catch block
		cerr<<"User Bandwidth: "<<e.what()<<endl;
	}catch(exception& e){
		cerr<<"User Bandwidth: "<<e.what()<<endl;
	}
}
void UserBandwidth::getParametersFromUserModel(json::Object& userModel) {
	try {
		this->sessionBandwidthAveragePerMin=0;
		this->totalDownloadBW=0;
		this->totalTCPBW=0;
		this->totalUDPBW=0;
		this->totalUploadBW=0;
		Number bwa=userModel["bandwidthAverage"];
		Number tcpa=userModel["TCPAverage"];
		Number udpa=userModel["UDPAverage"];
		this->bandwidthAverage =bwa;
		this->TCPAverage =tcpa;
		this->UDPAverage = udpa;
	}
	catch (json::Exception& e) {
		// TODO Auto-generated catch block
		cerr<<"User Bandwidth: "<<e.what()<<endl;
	}catch(exception& e){
		cerr<<"User Bandwidth: "<<e.what()<<endl;
	}
}

void UserBandwidth::getBytes(){

	system("iptables -L FORWARD -n -v -x | awk '$1 ~ /^[0-9]+$/ { print  $9 , $2 }'>IPT_UBM_IN.txt");
	system("iptables -L FORWARD -n -v -x | awk '$1 ~ /^[0-9]+$/ { print  $8 , $2 }'>IPT_UBM_OUT.txt");
	string ip = this->ip;
	std::ifstream infile("/root/IPT_UBM_IN.txt");
	std::ifstream outfile("/root/IPT_UBM_OUT.txt");
	try {
		pthread_mutex_lock(&UPMain::Instance()->mutex1);
		std::string line;
		while (std::getline(infile, line)) {
			std::istringstream iss(line);
			std::string new_ip, byte;
			if ((iss >> new_ip >> byte)) {
				iptObject obj;

				if (ip == new_ip) {
					obj.byte = atoi(byte.c_str());
					obj.direction = "IN";
					this->iptList.push_back(obj);
				}
			}
		}
		std::string line1;
		while (std::getline(outfile, line1)) {
			std::istringstream iss(line1);
			std::string new_ip, byte;
			if ((iss >> new_ip >> byte)) {
				iptObject obj;
				if (ip == new_ip) {
					obj.byte = atoi(byte.c_str());
					obj.direction = "OUT";
					this->iptList.push_back(obj);
				}
			}
		}
		pthread_mutex_unlock(&UPMain::Instance()->mutex1);

	} catch (std::exception& e) {
		e.what();
		std::cout << "User Bandwidth:Empty object is created!" << std::endl;
		iptObject obj;
		obj.byte = 0;
		obj.direction = "";
		this->iptList.push_back(obj);
	}

}

