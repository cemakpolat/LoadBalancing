/*
 * ReputationFE.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#include "ReputationFE.h"
#include "Utilities.h"
#include <math.h>

std::string ReputationFE::className="ReputationFE";

ReputationFE::ReputationFE() {

	this->nonCooperativeProbabInPercent=0;
	this->cooperativeProbabInPercent=0;
	this->normalCooperativeProbabInPercent=0;

	em=new EM(className);

	em->debugOutFileCoopLambdas=this->className+"_CoopLambdas.txt";
	em->debugOutFileCoopLambdasTime=this->className+"_CoopLambdasTime.txt";
	em->debugOutFileCoopTaos=this->className+"_clientDepartureLambdasTaos.txt";

	em->debugOutFileNoCoopLambdas=this->className+"_NoCoopLambdas.txt";
	em->debugOutFileNoCoopLambdasTime=this->className+"_NoCoopLambdasTime.txt";
	em->debugOutFileNoCoopTaos=this->className+"_NoCoopLambdasTime.txt";

	em->debugOutFileDecisions=this->className+"_Decisions.txt";
	em->debugOutFileDecesionsTime=this->className+"_DecisionsTime.txt";

	em->debugOutFileProbabilities=this->className+"_ReputationStateProbabilities.txt";
	em->debugOutFileProbabilitiesTime=this->className+"_ReputationStateProbabilitiesTime.txt";
	em->prepareAllPath();
}

ReputationFE::~ReputationFE() {
	// TODO Auto-generated destructor stub
}

void* ReputationFE::run(void* object) {
	ReputationFE *ref = (ReputationFE*) object;

	while (true) {
		try {
			//Utilities::errorCatcher("ReputationFE",1);

			sleep(ref->em->period/1000);
			ref->addPositiveDecision(false);
			ref->addNegativeDecision(false);
			std::string a = "0";
			ref->em->fileWrite(ref->em->debugOutFileDecisions, a);
			a = Utilities::longToTimeString(
					Utilities::getCurrentTimeAsSecond());
			ref->em->fileWrite(ref->em->debugOutFileDecesionsTime, a);
			//Utilities::errorCatcher("ReputationFE",2);
		} catch (std::exception &e) {
			std::cerr << ref->className << " " << e.what() << std::endl;

		}
	}

}
void ReputationFE::calculateProbabilities(){
	//Utilities::errorCatcher("ReputationFE calculateProbabilities",3);
	double tempSum = (em->adminLambda + em->previousLambda_N);
		double probabilityNonCooperativeNom = pow(  tempSum ,2);
		double probabilityNonCooperativeDe = em->previousLambda_P * (tempSum) + probabilityNonCooperativeNom + pow(  em->previousLambda_P ,2);
		double probabilityCritical = probabilityNonCooperativeNom / probabilityNonCooperativeDe;
		this->nonCooperativeProbabInPercent = (int) ( 100 * probabilityCritical);

		double tempSquare = pow(  em->previousLambda_P ,2);
		probabilityCritical = tempSquare / probabilityNonCooperativeDe;
		this->cooperativeProbabInPercent = (int) ( 100 * probabilityCritical);

		double normalDenominator = tempSum * em->previousLambda_P;
		probabilityCritical = normalDenominator / probabilityNonCooperativeDe;
		this->normalCooperativeProbabInPercent = (int) ( 100 * probabilityCritical);

		std::string toWrite = Utilities::convertIntToString(
				this->nonCooperativeProbabInPercent) + " "
				+ Utilities::convertIntToString(this->normalCooperativeProbabInPercent)
				+ " "
				+ Utilities::convertIntToString(
						this->cooperativeProbabInPercent);

		em->fileWrite(em->debugOutFileProbabilities, toWrite);
		toWrite = Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
		em->fileWrite(em->debugOutFileProbabilitiesTime, toWrite);

		std::cout<<className << " Probabilities are: "
				<< this->nonCooperativeProbabInPercent << " "
				<< this->normalCooperativeProbabInPercent << " "
				<< this->cooperativeProbabInPercent << "\n\n" << std::endl;
		//Utilities::errorCatcher("ReputationFE calculateProbabilities",4);
}
int ReputationFE::getNonCooperativeProbabilityInPercent() {
	return this->nonCooperativeProbabInPercent;
}
int ReputationFE::getCooperativeProbabilityInPercent() {
return this->cooperativeProbabInPercent;
}

int ReputationFE::getNormalCooperativeProbabilityInPercent() {
	return this->normalCooperativeProbabInPercent;
}
void ReputationFE::addPositiveDecision(bool occurance) {
	em->trackingWindow_P.push_back(occurance);
	em->trackingWindow_P.erase(em->trackingWindow_P.begin());
	if (occurance) {
		std::string a = "1";
		em->fileWrite(em->debugOutFileDecisions, a);
		a = Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
		em->fileWrite(em->debugOutFileDecesionsTime, a);
	}
	em->runEM_P();
	//em->runEM_N();
	//Utilities::errorCatcher("ReputationFE calculateProbabilities",2);
	this->calculateProbabilities();
}
void ReputationFE::addNegativeDecision(bool occurance) {
	em->trackingWindow_N.push_back(occurance);
	em->trackingWindow_N.erase(em->trackingWindow_N.begin());
	if (occurance) {
		std::string a = "-1";
		em->fileWrite(em->debugOutFileDecisions, a);
		a = Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
		em->fileWrite(em->debugOutFileDecesionsTime, a);
	}
	em->runEM_P();
	em->runEM_N();
	//Utilities::errorCatcher("ReputationFE calculateProbabilities",2);
	this->calculateProbabilities();
}
