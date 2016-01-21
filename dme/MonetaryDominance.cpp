/*
 * MonetaryDominance.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#include "MonetaryDominance.h"
#include "MainDME.h"
#include <math.h>
#include "Utilities.h"
//using namespace std;
string MonetaryDominance::className="MonetaryDominance";
MonetaryDominance::MonetaryDominance() {
	// TODO Auto-generated constructor stub
	this->K_monetary=1;
	this->A_monetary=0.1;
	this->B_monetary=100;
	this->k_monetary=-1;
	this->normalizationFactor=1;

	this->creditTrack=MainDME::Instance()->creditTrack;
	this->debugMonetaryDominance=this->className+"_monetaryDominance.txt";
	this->debugTimer=this->className+"_timer.txt";
}

MonetaryDominance::~MonetaryDominance() {
	// TODO Auto-generated destructor stub
}
double MonetaryDominance::importancePi(double ownCredit){
	ownCredit=ownCredit/this->normalizationFactor;
	double expIn=k_monetary*A_monetary*(B_monetary-ownCredit);
	double expPart=exp(expIn);
	double temp=K_monetary*(1.0/(1.0+expPart));
	std::string toWrite=Utilities::convertDoubleToString(temp);
	Utilities::fileWrite(this->debugMonetaryDominance,toWrite);
	toWrite=Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
	Utilities::fileWrite(this->debugTimer,toWrite);//TODO:
	return  temp;
}
double MonetaryDominance::getMonetaryComponent(double proposedMoney,double ownCredit){
	double temp=proposedMoney*importancePi(ownCredit);
	std::cout<<"Data in monetary component importance: \n OwnCredit is: "<<ownCredit <<"\n Monetary Component is: "<<temp<<std::endl;
	return temp;
}

