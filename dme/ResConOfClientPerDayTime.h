/*
 * ResConOfClientPerDayTime.h
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#ifndef RESCONOFCLIENTPERDAYTIME_H_
#define RESCONOFCLIENTPERDAYTIME_H_
#include <string>
//#include "DateUtils.h"
#include "CreditTrack.h"
class ResConOfClientPerDayTime {
public:
	static std::string className;
	//DateUtils timeOfDay;
	double mu;
	double sigma;
	CreditTrack* creditTrack;
	ResConOfClientPerDayTime();
	virtual ~ResConOfClientPerDayTime();
	double getCostOfAClient();
	double getEstimatedGain(int during,double unitResourceCost);
	double phi(double x);
	double phi(double z,double mu,double sigma);
	double Phi(double z);
	double Phi(double z, double mu,double sigma);
	double PhiInverse(double y);
	double PhiInverse(double y,double delta,double lo,double hi);

};

#endif /* RESCONOFCLIENTPERDAYTIME_H_ */
