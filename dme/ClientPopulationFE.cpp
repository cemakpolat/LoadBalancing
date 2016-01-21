/*
 * ClientPopulationFE.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#include "ClientPopulationFE.h"
#include "Utilities.h"
#include <math.h>
std::string ClientPopulationFE::className="ClientPopulationFE";
ClientPopulationFE::ClientPopulationFE() {

	this->criticalPopulationProbInPercent=0;
	this->desolatePopulationProbInPercent=0;
	this->normalPopulationProbInPercent=0;
	em=new EM(className);

	em->debugOutFileCoopLambdas=this->className+"_clientDepartureLambdas.txt";
	em->debugOutFileCoopLambdasTime=this->className+"clientDepartureLambdasTime.txt";
	em->debugOutFileCoopTaos=this->className+"_clientDepartureLambdasTaos.txt";

	em->debugOutFileNoCoopLambdas=this->className+"_clientArrivalLambdas.txt";
	em->debugOutFileNoCoopLambdasTime=this->className+"_clientArrivalLambdasTime.txt";
	em->debugOutFileNoCoopTaos=this->className+"_clientArrivalLambdasTaos.txt";

	em->debugOutFileDecisions=this->className+"_clientArrivalDeparture.txt";
	em->debugOutFileDecesionsTime=this->className+"_clientArrivalDepartureTime.txt";

	em->debugOutFileProbabilities=this->className+"_clientADStateProbabilities.txt";
	em->debugOutFileProbabilitiesTime=this->className+"_clientADStateProbabilitiesTime.txt";

	em->prepareAllPath();
}

ClientPopulationFE::~ClientPopulationFE() {
	// TODO Auto-generated destructor stub
	delete em;
}
void* ClientPopulationFE::run(void* object) {
	ClientPopulationFE *cpfe = (ClientPopulationFE*) object;
	while (true) {
		try {
			sleep(cpfe->em->period/1000);
			cpfe->addClientDeparture(false);
			cpfe->addClientArrival(false);
			std::string a = "0";
			cpfe->em->fileWrite(cpfe->em->debugOutFileDecisions, a);
			a = Utilities::longToTimeString(
					Utilities::getCurrentTimeAsSecond());
			cpfe->em->fileWrite(cpfe->em->debugOutFileDecesionsTime, a);
		} catch (std::exception &e) {
			std::cerr << cpfe->className << " " << e.what() << std::endl;

		}
	}

}
void ClientPopulationFE::calculateProbabilities(){

	double tempSum = (em->adminLambda + em->previousLambda_N);
	double probabilityNonCooperativeNom = pow(  tempSum ,2);
	double probabilityNonCooperativeDe = em->previousLambda_P * (tempSum) + probabilityNonCooperativeNom + pow(  em->previousLambda_P ,2);
	double probabilityCritical = probabilityNonCooperativeNom / probabilityNonCooperativeDe;
	this->criticalPopulationProbInPercent = (int) ( 100 * probabilityCritical);

	double tempSquare = pow(  em->previousLambda_P ,2);
	probabilityCritical = tempSquare / probabilityNonCooperativeDe;
	this->desolatePopulationProbInPercent = (int) ( 100 * probabilityCritical);

	double normalDenominator = tempSum * em->previousLambda_P;
	probabilityCritical = normalDenominator / probabilityNonCooperativeDe;
	this->normalPopulationProbInPercent = (int) ( 100 * probabilityCritical);

	std::string toWrite = Utilities::convertIntToString(
			this->criticalPopulationProbInPercent) + " "
			+ Utilities::convertIntToString(this->normalPopulationProbInPercent)
			+ " "
			+ Utilities::convertIntToString(
					this->desolatePopulationProbInPercent);

	em->fileWrite(em->debugOutFileProbabilities, toWrite);
	toWrite = Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
	em->fileWrite(em->debugOutFileProbabilitiesTime, toWrite);

	std::cout<<className << " Probabilities are: "
			<< this->criticalPopulationProbInPercent << " "
			<< this->normalPopulationProbInPercent << " "
			<< this->desolatePopulationProbInPercent << "\n\n" << std::endl;
}
int ClientPopulationFE::getCriticalPopulationProbabilityInPercent(){
	return this->criticalPopulationProbInPercent;
}
int ClientPopulationFE::getDesolatePopulationProbabilityInPercent(){
	return this->desolatePopulationProbInPercent;
}
int ClientPopulationFE::getNormalPopulationProbabilityInPercent(){
	return this->normalPopulationProbInPercent;
}
void ClientPopulationFE::addClientDeparture(bool occurance) {
	em->trackingWindow_P.push_back(occurance);
	em->trackingWindow_P.erase(em->trackingWindow_P.begin());
	if (occurance) {
		std::string a = "1";
		em->fileWrite(em->debugOutFileDecisions, a);
		a = Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
		em->fileWrite(em->debugOutFileDecesionsTime, a);
	}
	em->runEM_P();
	em->runEM_N();
	this->calculateProbabilities();
}

void ClientPopulationFE::addClientArrival(bool occurance) {
	em->trackingWindow_N.push_back(occurance);
	em->trackingWindow_N.erase(em->trackingWindow_P.begin());
	if (occurance) {
		std::string a = "-1";
		em->fileWrite(em->debugOutFileDecisions, a);
		a = Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
		em->fileWrite(em->debugOutFileDecesionsTime, a);
	}
	em->runEM_P();
	em->runEM_N();
	this->calculateProbabilities();
}
