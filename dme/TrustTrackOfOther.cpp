/*
 * TrustTrackOfOther.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#include "TrustTrackOfOther.h"
#include <string>
#include <memory>

 std::string TrustTrackOfOther::className="TrustTrackOfOthers";
TrustTrackOfOther::TrustTrackOfOther() {
	// TODO Auto-generated constructor stub
	this->waitTime=300*1000;
}

TrustTrackOfOther::~TrustTrackOfOther() {
	// TODO Auto-generated destructor stub
}
TrustTrackOfOther::TrustTrackOfOther(std::string ssidOfOtherAP,std::string apOfOtherAP){
	this->addAPObjectToTrustList(90,ssidOfOtherAP,apOfOtherAP);

}
void* TrustTrackOfOther::run(void* object) {
	TrustTrackOfOther *tto=(TrustTrackOfOther*)object;
	while(true){
		try{
			sleep(tto->waitTime/1000);
			for(unsigned int i=0;i<tto->turstListOfAPs.size();i++){
				tto->turstListOfAPs[i]->reduceTrustLevel();
			}
		}catch(std::exception& e){
			std::cerr<<className<<" "<<e.what()<<std::endl;
		}
	}

	pthread_exit(0);
}
//TODO:Memory leak
TrustObject TrustTrackOfOther::addAPObjectToTrustList(int turstLevel,std::string ipAdd,std::string apSSID){
	 TrustObject *tob=new TrustObject(turstLevel,ipAdd,apSSID);
	 this->turstListOfAPs.push_back(tob);
	 return *tob;
}
bool TrustTrackOfOther::removeAPObjectToTrustList(std::string apSSID){
	for(unsigned i=0;i<this->turstListOfAPs.size();i++){
		if(this->turstListOfAPs[i]->getApSSID()==apSSID){
			this->turstListOfAPs.erase(this->turstListOfAPs.begin()+i);
			i=this->turstListOfAPs.size();
			return true;
		}
	}
	return false;
}
bool TrustTrackOfOther::increaseAPObjectTrust(std::string& apssid){
	for(unsigned i=0;i<this->turstListOfAPs.size();i++){
			if(this->turstListOfAPs[i]->getApSSID()==apssid){
				this->turstListOfAPs[i]->reduceTrustLevelExtreme();
				i=this->turstListOfAPs.size();
				return true;
			}
		}
	return false;
}
bool TrustTrackOfOther::decreaseAPObjectTrust(std::string& apssid){
	for(unsigned i=0;i<this->turstListOfAPs.size();i++){
				if(this->turstListOfAPs[i]->getApSSID()==apssid){
					this->turstListOfAPs[i]->increaseTrustLevelExtreme();
					i=this->turstListOfAPs.size();
					return true;
				}
			}
		return false;

}
