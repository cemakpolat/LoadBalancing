/*
 * BandwidthUtilizationFE.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#include "BandwidthUtilizationFE.h"
#include "Utilities.h"
#include "math.h"

string BandwidthUtilizationFE::className="BandwidthUtilizationFE";
BandwidthUtilizationFE::BandwidthUtilizationFE() {
	em=new EM(className);


	this->economicalBWUtilProbabInPercent = 0;
	this->normalBWUtilProbabInPercent = 0;
	this->highBWUtilProbabInPercent = 0;

	em->debugOutFileNegLambdas=this->className+"_NegLamdas.txt";
	em->debugOutFileNegLambdasTime=this->className+"_NegLamdasTime.txt";
	em->debugOutFileNegTaos=this->className+"_NegLamdesTaos.txt";
	em->debugOutFileNegotiations=this->className+"_Negotiations.txt";
	em->debugOutFileNegotiationsTime=this->className+"_NegotiationsTime.txt";
	em->debugOutFileProbabilities=this->className+"_BWStateProbabilities.txt";
	em->debugOutFileProbabilitiesTime=this->className+"_BWStateProbabilitiesTime.txt";
	em->prepareAllPath();
}

BandwidthUtilizationFE::~BandwidthUtilizationFE() {
	// TODO Auto-generated destructor stub
	delete em;
}
void* BandwidthUtilizationFE::run(void* object) {
	BandwidthUtilizationFE *bufe = (BandwidthUtilizationFE*) object;
	while (true) {
		try {
			sleep(bufe->em->period / 1000);
			bufe->addNegotiationDecision(false);
			string a = "0";
			bufe->em->fileWrite(bufe->em->debugOutFileNegotiations, a);
			a = Utilities::longToTimeString(
					Utilities::getCurrentTimeAsSecond());
			bufe->em->fileWrite(bufe->em->debugOutFileNegotiationsTime, a);
		} catch (std::exception &e) {
			cerr << bufe->className << " " << e.what() << endl;

		}
	}

}

void BandwidthUtilizationFE::calculateProbabilities(){
	double probabEconNom=pow(em->adminLambda,2);
	double previousLambdaSquare=pow(em->previousLambda_P,2);
	double multipOfLambda=em->previousLambda_P*em->adminLambda;
	double probabEconoDe=probabEconNom+multipOfLambda+previousLambdaSquare;

	double probabCritical=probabEconNom/probabEconoDe;
	this->economicalBWUtilProbabInPercent=100*probabCritical;

	probabCritical=previousLambdaSquare/probabEconoDe;
	this->highBWUtilProbabInPercent=100*probabCritical;

	probabCritical=multipOfLambda/probabEconoDe;
	this->normalBWUtilProbabInPercent=100*probabCritical;

	string toWrite=Utilities::convertIntToString(this->economicalBWUtilProbabInPercent)+" "+
			Utilities::convertIntToString(this->normalBWUtilProbabInPercent)+" "+
			Utilities::convertIntToString(this->highBWUtilProbabInPercent);

	em->fileWrite(em->debugOutFileProbabilities,toWrite);
	toWrite= Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
	em->fileWrite(em->debugOutFileProbabilitiesTime,toWrite);

	cout<<className<<" Probabilities are: "<<this->economicalBWUtilProbabInPercent<<" "<<this->normalBWUtilProbabInPercent<<" "
			<<this->highBWUtilProbabInPercent<<"\n\n"<<endl;
}
int BandwidthUtilizationFE::getHighBWProbabilityInPercent() {
	return this->highBWUtilProbabInPercent;
}
int BandwidthUtilizationFE::getNormalBWProbabInPercent(){
	return this->normalBWUtilProbabInPercent;
}
int BandwidthUtilizationFE::getEconomicalBWProbabInPercent(){
	return this->economicalBWUtilProbabInPercent;
}
void BandwidthUtilizationFE::addNegotiationDecision(bool occurance) {

	em->trackingWindow_P.push_back(occurance);
	em->trackingWindow_P.erase(em->trackingWindow_P.begin());
	if(occurance){
		string a="1";
		em->fileWrite(em->debugOutFileNegotiations,a);
		a = Utilities::longToTimeString(
							Utilities::getCurrentTimeAsSecond());
		em->fileWrite(em->debugOutFileNegotiationsTime,a);
	}
	em->runEM_P();
	this->calculateProbabilities();

}
//void BandwidthUtilizationFE::addOccuranceTrack_P(int& occurance){
//	this->numberOfOccurance_P.push_back(occurance);
//	this->numberOfOccurance_P.erase(this->numberOfOccurance_P.begin());
//}
//double BandwidthUtilizationFE::EMStep_P(){
//
//	double maximumBasedOnTao = 0;
//			double newTaoNC = 0;
//			double newTaoN = 0;
//			double newTaoC = 0;
//
//			double previousTaoNC = this->taoNC_P;
//			double previousTaoN = this->taoN_P;
//			double previousTaoC = this->taoC_P;
//
//			double startTaoNC = 0.01;
//			double finishTaoNC = 0.99;
//			if ( (this->taoNC_P - 0.20) > 0  ){
//				startTaoNC = (this->taoNC_P - 0.20);
//			}
//			if ( (this->taoNC_P + 0.20) < 1 ) {
//				finishTaoNC = (this->taoNC_P + 0.20);
//			}
//
//			double startTaoN = 0.01;
//			if ( (this->taoN_P - 0.20) > 0  ){
//				startTaoN = (this->taoN_P - 0.20);
//			}
//
//			for (double taoNC = startTaoNC; taoNC < finishTaoNC ; taoNC = taoNC + 0.02){
//
//				double finishTaoN = 0.99;
//				if ( (this->taoN_P + 0.20) < (1 - taoNC) ) {
//					finishTaoN = (this->taoN_P + 0.20);
//				}else {
//					finishTaoN = (1 - taoNC);
//				}
//
//				for (double taoN = startTaoN; taoN < finishTaoN ; taoN = taoN + 0.02){
//
//					double taoC = 1 - taoN - taoNC;
//
//					double Expectation = 0;
//					for (int i=0; i < windowSize_P ; i++) {
//						int k = numberOfOccurance_P[i];
//						double nominatorNC = taoNC * exp(-this->lambdaNC_P) * pow(this->lambdaNC_P, k);
//						double nominatorC = taoC * exp(-this->lambdaC_P) * pow(this->lambdaC_P, k);
//						double nominatorN = taoN * exp(-this->lambdaN_P) * pow(this->lambdaN_P, k);
//						double denominator = nominatorN + nominatorNC + nominatorC;
//						double portionNC = nominatorNC / denominator;
//						double portionC = nominatorC / denominator;
//						double portionN = nominatorN / denominator;
//						double tempDouble = this->calculateLogFact(k);
//						double logPartNC = log(taoNC) - this->lambdaNC_P + k * log(this->lambdaNC_P) - tempDouble;
//						double logPartC = log(taoC) - this->lambdaC_P + k * log(this->lambdaC_P) - tempDouble;
//						double logPartN = log(taoN) - this->lambdaN_P + k * log(this->lambdaN_P) - tempDouble;
//						Expectation = portionNC * logPartNC + portionC * logPartC + portionN * logPartN + Expectation;
//					}
//
//					if( (taoNC == startTaoNC) && (taoN == startTaoN) ) {
//						//CommonFunctions.writeConsoleSecond("firstly equationg maximum NC to Expectation");
//						//writeConsole("firstly equationg maximum NC to Expectation " + Expectation);
//						maximumBasedOnTao = Expectation;
//						newTaoNC = taoNC;
//						newTaoC = taoC;
//						newTaoN = taoN;
//					}
//					if ( Expectation > maximumBasedOnTao){
//						maximumBasedOnTao = Expectation;
//						maximumBasedOnTao = Expectation;
//						newTaoNC = taoNC;
//						newTaoC = taoC;
//						newTaoN = taoN;
//					}
//
//				}
//			}
//
//			this->taoNC_P = newTaoNC;
//			this->taoN_P = newTaoN;
//			this->taoC_P = newTaoC;
//			//writeConsole("\n\nNegotiation Initiation Decision data: New values for tao s: \nTaoNC is "
//			//		+ this.taoNC_P + "\nTaoN is " + this.taoN_P + "\nTaoC is " + this.taoC_P );
//
//			string toWrite=Utilities::convertDoubleToString(this->taoNC_P)+" "+
//						Utilities::convertDoubleToString(this->taoN_P)+" "+
//						Utilities::convertDoubleToString(this->taoC_P);
//			this->fileWrite( this->debugOutFieNegTaos, toWrite);
//			int startLambdaNC = 0;
//			int startLambdaN = 0;
//			int startLambdaC = 0;
//			int finishLambdaC = 0;
//			double maximumBasedOnLambda = 0;
//			int newLambdaNC = this->lambdaNC_P;
//			int newLambdaN = this->lambdaN_P;
//			int newLambdaC = this->lambdaC_P;
//
//			if ( (this->lambdaC_P + sliderSize_P) > windowSize_P ) {
//				finishLambdaC = windowSize_P + 1;
//			}else {
//				finishLambdaC = (this->lambdaC_P + 5);
//			}
//
//			if ( (this->lambdaC_P - sliderSize_P) > 1 ) {
//				startLambdaC = (this->lambdaC_P - sliderSize_P);
//			}else {
//				startLambdaC = 1;
//			}
//
//			if ( (this->lambdaNC_P - sliderSize_P) > 1 ) {
//				startLambdaNC = (this->lambdaNC_P - sliderSize_P);
//			}else {
//				startLambdaNC = 1;
//			}
//
//			if ( (this->lambdaN_P - sliderSize_P) > 1 ) {
//				startLambdaN = (this->lambdaN_P - sliderSize_P);
//			}else {
//				startLambdaN = 1;
//			}
//
//			for (int lambdaC = startLambdaC; lambdaC < finishLambdaC ; lambdaC++){
//				for (int lambdaN = startLambdaN; lambdaN < lambdaC ; lambdaN++){
//					for (int lambdaNC = startLambdaNC; lambdaNC < lambdaN ; lambdaNC++){
//						double Expectation = 0;
//						for (int i=0; i < windowSize_P ; i++) {
//							int k = numberOfOccurance_P[i];
//							double nominatorNC = this->taoNC_P * exp(-lambdaNC) * pow(lambdaNC, k);
//							double nominatorC = this->taoC_P * exp(-lambdaC) * pow(lambdaC, k);
//							double nominatorN = this->taoN_P * exp(-lambdaN) * pow(lambdaN, k);
//							double denominator = nominatorN + nominatorNC + nominatorC;
//							double portionNC = nominatorNC / denominator;
//							double portionC = nominatorC / denominator;
//							double portionN = nominatorN / denominator;
//							double tempDouble = calculateLogFact(k);
//							double logPartNC = log(this->taoNC_P) - lambdaNC + k * log(lambdaNC) - tempDouble;
//							double logPartC = log(this->taoC_P) - lambdaC + k * log(lambdaC) - tempDouble;
//							double logPartN = log(this->taoN_P) - lambdaN + k * log(lambdaN) - tempDouble;
//							Expectation = portionNC * logPartNC + portionC * logPartC + portionN * logPartN + Expectation;
//						}
//
//						if( (lambdaNC == startLambdaNC) && (lambdaN == startLambdaN) && (lambdaC == startLambdaC) ) {
//							//CommonFunctions.writeConsoleSecond("firstly equationg maximum NC to Expectation");
//							//writeConsole("firstly equationg maximum Lambda based to Expectation " + Expectation);
//							maximumBasedOnLambda = Expectation;
//							newLambdaNC = lambdaNC;
//							newLambdaN = lambdaN;
//							newLambdaC = lambdaC;
//						}
//						if ( Expectation > maximumBasedOnLambda){
//							maximumBasedOnLambda = Expectation;
//							newLambdaNC = lambdaNC;
//							newLambdaN = lambdaN;
//							newLambdaC = lambdaC;
//						}
//					}
//				}
//			}
//
//			this->lambdaC_P = newLambdaC;
//			this->lambdaNC_P = newLambdaNC;
//			this->lambdaN_P = newLambdaN;
//			//writeConsole("Negotiation Initiation Decision data: EM MESSAGE, new values for lambdas \nlambdaNC = " + this.lambdaNC_P +
//			//		" \nlambdaN = " + this.lambdaN_P + "\nlambdaC = " + this.lambdaC_P);
//
//			double rateOfChangeNC = this->taoNC_P - previousTaoNC;
//			double rateOfChangeN = this->taoN_P - previousTaoN;
//			double rateOfChangeC = this->taoC_P - previousTaoC;
//
//			double maxRateOfChange = getMax(rateOfChangeNC, rateOfChangeN, rateOfChangeC);
//			/*
//			 * Comparison of rate of changes
//			 */
//			int lambdaForMMAP = this->previousLambda_P;
//			if ( (rateOfChangeNC == 0) && (rateOfChangeN == 0) && (rateOfChangeC == 0)){
//				if ( this->taoNC_P > this->taoN_P ){
//					if ( this->taoNC_P > this->taoC_P ){
//						lambdaForMMAP = this->lambdaNC_P;
//						//writeConsole("MMAP MESSAGE, rate of changes 0 for " + this.className);
//					}else {
//						lambdaForMMAP = this->lambdaC_P;
//						//writeConsole("MMAP MESSAGE, rate of changes 0 for " + this.className);
//					}
//				}else{
//					if ( this->taoN_P > this->taoC_P ){
//						lambdaForMMAP = this->lambdaN_P;
//						//writeConsole("MMAP MESSAGE, rate of changes 0 for " + this.className);
//					}else {
//						lambdaForMMAP = this->lambdaC_P;
//						//writeConsole("MMAP MESSAGE, rate of changes 0 for " + this.className);
//					}
//				}
//			}
//			else if ( rateOfChangeNC == maxRateOfChange ){
//				lambdaForMMAP = this->lambdaNC_P;
//				//writeConsole("MMAP MESSAGE, max rateofChange in NC " + this.className + " " + rateOfChangeNC);
//				//writeConsole("MMAP MESSAGE, previous taoNC " + previousTaoNC + " now " + this.taoNC_P);
//			} else if ( rateOfChangeN == maxRateOfChange ){
//				//writeConsole("MMAP MESSAGE, max rateofChange in N " + this.className + " " + rateOfChangeN);
//				//writeConsole("MMAP MESSAGE, previous taoN " + previousTaoN + " now " + this.taoN_P);
//				lambdaForMMAP = this->lambdaN_P;
//			} else if ( rateOfChangeC == maxRateOfChange ){
//				//writeConsole("MMAP MESSAGE, max rateofChange in C " + this.className + " " + rateOfChangeC);
//				//writeConsole("MMAP MESSAGE, previous taoC " + previousTaoC + " now " + this.taoC_P);
//				lambdaForMMAP = this->lambdaC_P;
//			}
//			this->previousLambda_P = lambdaForMMAP;
//
//			/*
//			double probabilityCriticalNom = 3 * Math.pow(this.adminLambda,2) + 3 * this.adminLambda * lambdaForMMAP + Math.pow(lambdaForMMAP, 2);
//			double probabilityCriticalDe = Math.pow(  (3* this.adminLambda + lambdaForMMAP ), 2);
//			double probabilityCritical = probabilityCriticalNom / probabilityCriticalDe;
//			this.priority_P = (int) ( 100 * probabilityCritical); */
//			cout<<"Negotiation Initiation Decision data: MMAP MESSAGE, new lambda is " << this->previousLambda_P <<" \n\n"<<endl;
//			toWrite=this->previousLambda_P;
//			this->fileWrite( this->debugOutFileNegLambdas,toWrite );
//			toWrite=Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
//			this->fileWrite( this->debugOutFileNegLamdasTime, toWrite );
//			return 0;
//
//
//
//
//}
//void BandwidthUtilizationFE::runEM_P(){
//	if(this->sliderCounter_P>=this->sliderSize_P){
//		this->sliderCounter_P=0;
//		int occuranceN=0;
//		for(int i=0;i<this->windowSize_P;i++){
//			if(this->trackingWindow_P[i])
//			occuranceN=occuranceN+1;
//		}
//		this->addOccuranceTrack_P(occuranceN);
//		this->EMStep_P();
//	}else{
//		this->sliderCounter_P=this->sliderCounter_P+1;
//	}
//}
//
//void BandwidthUtilizationFE::setAdminLambda(int i){
//	this->adminLambda=i;
//}
//void BandwidthUtilizationFE::setPeriod(int per){
//	this->period=per*1000;
//}
//double BandwidthUtilizationFE::getMax(double& rateOfChangeNC, double& rateOfChangeN,double& rateOfChangeC){
//	double temp=rateOfChangeNC;
//	if(temp<rateOfChangeN){
//		temp=rateOfChangeN;
//	}
//	if(temp<rateOfChangeC){
//		temp=rateOfChangeC;
//	}
//	return temp;
//}
//double BandwidthUtilizationFE::calculateLogFact(int& k){
//	double temp=0;
//	for(int i=1;i<=k;i++){
//		temp=temp+log(i);
//	}
//	return temp;
//}
//void BandwidthUtilizationFE::fileWrite(string& debugOutFile,
//		string& outputString) {
//	try {
//		ofstream file(debugOutFile.c_str());
//		if (file.is_open()) {
//			file << outputString;
//			file << "\n";
//			file.close();
//		}
//	} catch (Exception& e) {
//		cerr << this->className << " " << e.what() << endl;
//	}
//}
//void BandwidthUtilizationFE::updateTestInteger(int i){
//	this->testInteger=i;
//}
//int BandwidthUtilizationFE::getTestInteger(){
//	return this->testInteger;
//}
//void BandwidthUtilizationFE::runTest(){
//	while(true){
//		try{
//		sleep(1);
//		this->testInteger++;
//		}catch(Exception &e){
//			cerr<<this->className<<" "<<e.what()<<endl;
//		}
//
//	}
//}

