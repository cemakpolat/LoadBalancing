/*
 * TrustObject.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#include "TrustObject.h"
#include "Utilities.h"
#include <math.h>

TrustObject::TrustObject() {
	// TODO Auto-generated constructor stub
	this->counter=0;
	this->K_trust=100;
	this->k_trust=-1;
	this->A_trust=5;
	this->B_trust=10;

	this->reduceExtreme=5;
	this->reduce=0.5;
	this->increaseExtreme=5;
	this->increase=0.5;

	this->counterUpperLevel=20;
	this->counterLowerLevel=20;
}

TrustObject::~TrustObject() {
	// TODO Auto-generated destructor stub
}

TrustObject::TrustObject(int& trustLevel,std::string ipAddr,std::string apSSID){
	this->apSSID=apSSID;
	this->setTrustLevel(trustLevel);
	this->ipAddr=ipAddr;
	this->debugTrustLevel=this->apSSID+" +_trustLevel.txt";
	this->debugTimer=this->apSSID+"_timer.txt";
}
void TrustObject::setIpAddress(std::string& ipAddr){
	this->ipAddr=ipAddr;
}
void TrustObject::setTrustLevel(int trustLevel){
	double tempTrustLevel=trustLevel;
	this->counter=log(tempTrustLevel/100)*-1;
	std::cout<<"Counter is "<<this->counter<<" trust level is "<<trustLevel<<std::endl;
	this->trustLevel=trustLevel;
}
void TrustObject::setApSSID(std::string& apssid){
	this->apSSID=apssid;
}
std::string TrustObject::getIPAddr(){
return this->ipAddr;
}
int TrustObject::getTrustLevel(){
return this->trustLevel;
}
std::string TrustObject::getApSSID(){
	return this->apSSID;
}
int TrustObject::reduceTrustLevel(){
	if(this->counter>this->counterUpperLevel){
		//?
	}else{
		this->counter=this->counter+this->reduce;
	}
	double expIn=k_trust*A_trust*(B_trust-this->counter);
	std::cout<<"expIn "<<this->apSSID<<" is"<<expIn<<std::endl;
	double expPart=exp(expIn);
	std::cout<<"expPart "<<this->apSSID<<" is"<<expPart<<std::endl;
	double temp=K_trust*(1.0+1.0/(1.0+expPart))-100;
	std::cout<<"Trust Level new temp of "<<this->apSSID<<" is reduced to"<<temp<<std::endl;

	std::string toWrite=Utilities::convertDoubleToString(temp);
	Utilities::fileWrite(this->debugTrustLevel,toWrite);
	toWrite=Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
	Utilities::fileWrite(this->debugTimer,toWrite);//TODO:
	return temp;
}
int TrustObject::reduceTrustLevelExtreme(){
	if( this->counter > this->counterUpperLevel) {

		}else{
			this->counter = this->counter + this->reduceExtreme;
		}
		double temp = 100* exp(-1*this->counter);
		std::cout<<"Trust level of "<< this->apSSID << " is reduced to "<< (int)temp <<std::endl;
		std::string toWrite=Utilities::convertDoubleToString(temp);
		Utilities::fileWrite(this->debugTrustLevel,toWrite);
		toWrite=Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
		Utilities::fileWrite(this->debugTimer,toWrite);//TODO:
		return temp;
}

int TrustObject::increaseTrustLevelExtreme(){
	if( this->counter < this->counterLowerLevel) {

		}else{
			this->counter = this->counter + this->increaseExtreme;
		}
		double temp = 100* exp(-1*this->counter);
		std::cout<<"Trust level of "<< this->apSSID << " is increased to "<< (int)temp <<std::endl;
		std::string toWrite=Utilities::convertDoubleToString(temp);
		Utilities::fileWrite(this->debugTrustLevel,toWrite);
		toWrite=Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
		Utilities::fileWrite(this->debugTimer,toWrite);//TODO:
		return temp;

}

int TrustObject::increaseTrustLevel() {

	if (this->counter < this->counterLowerLevel) {

	} else {
		this->counter = this->counter + this->increase;
	}
	double temp = 100 * exp(-1 * this->counter);
	std::cout << "Trust level of " << this->apSSID << " is increased to "
			<< (int) temp << std::endl;

	std::string toWrite=Utilities::convertDoubleToString(temp);
	Utilities::fileWrite(this->debugTrustLevel,toWrite);
	toWrite=Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
	Utilities::fileWrite(this->debugTimer,toWrite);//TODO:
	return temp;
}


