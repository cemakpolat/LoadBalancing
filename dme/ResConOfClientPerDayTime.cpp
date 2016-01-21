/*
 * ResConOfClientPerDayTime.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#include "ResConOfClientPerDayTime.h"
//#include "MainDME.h";
#include "Utilities.h"
#include <math.h>

std::string ResConOfClientPerDayTime::className="ResConOfClientPerDayTime";
ResConOfClientPerDayTime::ResConOfClientPerDayTime() {
	// TODO Auto-generated constructor stub
	this->mu=720;
	this->sigma=720;
	//this->creditTrack=MainDME::Instance()->creditTrack;

}

ResConOfClientPerDayTime::~ResConOfClientPerDayTime() {
	// TODO Auto-generated destructor stub
}


double ResConOfClientPerDayTime::getCostOfAClient(){
	int currentMinute=Utilities::getCurrentMinute();//TODO: add current minute in Utilities
	double costOfClient=Phi(currentMinute,this->mu,this->sigma);
	std::cout<<"Cost of the Client is "<<costOfClient<<std::endl;
	return costOfClient;

}
double ResConOfClientPerDayTime::getEstimatedGain(int during,double unitResourceCost){
	int currentMinute=Utilities::getCurrentMinute();
	int estimatedTime;
	double estimatedGain=0;
	std::cout<<"unitResourceCost "<<unitResourceCost<<std::endl;
	for( int i=0;i<during;i++){
		estimatedTime=i+currentMinute;
		double temp=Phi(estimatedTime,this->mu,this->sigma)*unitResourceCost;
		estimatedGain=temp+estimatedGain;
	}
	return estimatedGain;
}
double ResConOfClientPerDayTime::phi(double x){
	return exp(-x*x/2)/sqrt(2*M_PI);
}
double ResConOfClientPerDayTime::phi(double x, double mu, double sigma){
	return phi((x-mu)/sigma)/sigma;
}
double ResConOfClientPerDayTime::Phi(double z){
	if(z<-8.0)return 0.0;
	if(z>8.0) return 1.0;
	double sum=0.0,term=z;
	for(unsigned int i=3;sum+term!=sum;i+=2){
		sum=sum+term;
		term=term*z*z/i;
	}
	return 0.5+sum*phi(z);
}
double ResConOfClientPerDayTime::Phi(double z, double mu, double sigma){
	return Phi((z-mu)/sigma);
}
double ResConOfClientPerDayTime::PhiInverse(double y){
	return PhiInverse(y,.0000001,-8,8);
}
double ResConOfClientPerDayTime::PhiInverse(double y, double delta, double lo, double hi){
	double mid=lo+(hi-lo)/2;
		if(hi-lo<delta)return mid;
		if(Phi(mid)>y) return PhiInverse(y,delta,lo,mid);
		else	return PhiInverse(y,delta,lo,hi);
}
