/*
 * MainDME.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#include "MainDME.h"
#include "Utilities.h"
using namespace std;
MainDME::MainDME() {
	// TODO Auto-generated constructor stub


}
void MainDME::initiateAll(){

	this->ssidOfOtherAP = "ufit-right";

	//string ssidOfOtherAP;
	this->apOfOtherAP = "192.168.124.18";
	this->migrationFunctionality = new MigrateWorstQoEClient();
	/*Future behaviour Tracking related parameters*/

	this->thresholdNonCooperative = 60;
	this->bandwidthUtilizationPeriod = 100;
	this->reputationPeriod = 100;
	this->populationPeriod = 100;
	/*Incentive Tracking*/
	// pthread_t *trustOfOtherRunnable;
	// pthread_t *creditRunnable;
	/*Bandwidth observation related parameters*/
	this->bandwidthObserver = new BandwidthObserver();
	this->thresholdBandwidthUtilization = 500;
	this->A_load = 500;
	this->load_th = 800;
	/*Client Arrival and Departures*/

	/*POMDP related Parameters*/
	// pthread_t requesterPOMDP;
	// pthread_t requesteePOMDP;
	this->waitRequesterPOMDP = 10 * 1000;
	this->waitRequesteePOMDP = 10 * 1000;
	/*Communication Parameters*/
	this->userServerPort = 49154;
	this->accessPointServerPort = 49155;
	this->accessPointResponsePort = 49156;
	this->accessPointMessageServer=new TCPServerSocket(this->accessPointServerPort);
	this->accessPointResponseServer=new TCPServerSocket(this->accessPointResponsePort);


	this->monetaryDominance = new MonetaryDominance();
	this->resconOfClientPerDayTime =
			new ResConOfClientPerDayTime();
	//this->timeOfDay = new DateUtils();
	/*Normalization Components*/
	this->C_s_Load_Normalizer = 0.01;
	this->C_s_Reputation_Normalizer = 1;
	this->C_s_Both_Normalizer = 0.0001;
	/*History holder for requester*/
	this->unitResourceCostHolder[20];
	this->loadHolder[20];
	this->loadOrientedUtilityHolder[20];
	this->revenueOrientedutilityHolder[20];
	this->basicRevenueHolder[20];
	//Add in MainDME

	this->reputationRF = new ReputationFE();
	this->clientArrDeparture = new ClientArrivalDepartures();
	this->creditTrack = new CreditTrack();
	this->trustOtherTrack = new TrustTrackOfOther();
	this->previousBasicRevenue = 0;
	this->previousClientNumber = 0;
	this->bandwidthUtilFE = new BandwidthUtilizationFE();
	this->clientPopFE = new ClientPopulationFE();
}

MainDME::~MainDME() {
	// TODO Auto-generated destructor stub
	delete  MainDME::Instance()->bandwidthObserver;
	delete  MainDME::Instance()->monetaryDominance;
	delete  MainDME::Instance()->resconOfClientPerDayTime;
	//delete  MainDME::Instance()->timeOfDay;
	 //Add in MainDME
	delete	MainDME::Instance()->reputationRF;
	delete	MainDME::Instance()->clientArrDeparture;
	delete	MainDME::Instance()->creditTrack;
	delete	MainDME::Instance()->trustOtherTrack;
	delete 	MainDME::Instance()->migrationFunctionality;
	delete MainDME::Instance()->bandwidthUtilFE;
	delete MainDME::Instance()->clientPopFE;
	delete MainDME::Instance()->accessPointMessageServer;
	delete MainDME::Instance()->accessPointResponseServer;
}

//singleton pattern required
MainDME* MainDME::mdme_Instance = NULL;
MainDME* MainDME::Instance() {


	if (!mdme_Instance) {  // Only allow one instance of class to be generated.
		 ////Utilities::errorCatcher("MainMDE null",99999);
		mdme_Instance = new MainDME();
	}
	 ////Utilities::errorCatcher("MainMDE not null",10000);
	return mdme_Instance;
}
 void* MainDME::run(void* object){


	std::cout << "Initializing 0" << endl;
	//MainDME *mp = MainDME::Instance();
	MainDME *mp=(MainDME*)object;
	mp->initiateAll();
	mp->startReputationTrack();
	mp->startBWTrack();
	mp->startPopulationTrack();
	mp->startCreditTrack();
	mp->startTrustOfOthersTrack();
	mp->startClientArrivalDepartures();
	//moved some instance initialization to instructor:monetaryDominance,resourceCons, timeofDay
	mp->startRequesterPOMDP();
	mp->startRequesteePOMDP();
	pthread_join(mp->ReputationRunnable,NULL);
	pthread_join(mp->BWRunnable,NULL);
	pthread_join(mp->PopulationRunnable,NULL);
	pthread_join(mp->clientArrivalDepartureRunnable,NULL);
	pthread_join(mp->trustOfOtherRunnable,NULL);
	pthread_join(mp->creditRunnable,NULL);
	pthread_join(mp->requesterPOMDP,NULL);
	pthread_join(mp->requesteePOMDP,NULL);




}
/**
 * Template function will be used in order to not repeat the pthread process, they do the same process
 */
template <class T,class U,class B>
T MainDME::runCenter (T& a, U& b,B* data ) {
  	int rc1;
  		//start QoETracker

  		if ((rc1 = pthread_create(&a, NULL,&b, data))) {
  			//printf("QoS Block: QoS Tracker thread creation failed: %d\n", rc1);
  			int attempt = 0;
  			while (attempt < 3) {
  				bool success = true;
  				Utilities::randomBackOfftime(7);
  				if ((rc1 = pthread_create(&a, NULL,&b, data))) {
  					success = false;
  					printf(
  							"clientArrivalDepartureRunnable Block: clientArrivalDepartureRunnable thread creation failed: %d\n",
  							rc1);
  					if (attempt == 2) {
  						printf(
  								"clientArrivalDepartureRunnable Block:clientArrivalDepartureRunnable thread creation failed,please re-start the program. \n");
  					}
  				}
  				if (success == true) {
  					attempt = 3;
  				}
  			}
  		}
}


///The whole instantiation in the start functions will be removed and associated to the MainDME as an object.
// The object were already defined but they must also written in .h file.

void MainDME::startClientArrivalDepartures() {
	this->runCenter(MainDME::Instance()->clientArrivalDepartureRunnable,ClientArrivalDepartures::run,MainDME::Instance()->clientArrDeparture);
	//Utilities::errorCatcher("startClientArrivalDepartures",1);

}

void MainDME::startTrustOfOthersTrack() {
	this->runCenter(MainDME::Instance()->trustOfOtherRunnable,TrustTrackOfOther::run,MainDME::Instance()->trustOtherTrack);
	//Utilities::errorCatcher("startTrustOfOthersTrack",1);
}

void MainDME::startCreditTrack() {
	this->runCenter(MainDME::Instance()->creditRunnable,CreditTrack::run,MainDME::Instance()->creditTrack);
	//Utilities::errorCatcher("startCreditTrack",1);
}
string MainDME::runRequesterPOMDP() {
	cout<<className<<"Requester:POMDP running for requester"<<endl;
	long temp=MainDME::Instance()->bandwidthObserver->getBandwidthRate();
	cout<<"Requester: bandwidth observation is "<<temp<<endl;
	if(temp>MainDME::Instance()->thresholdBandwidthUtilization){
		cout<<"Requester: returning load oriented"<<endl;
		return "loadOriented";
	}
	else{
		cout<<"Requester:returning revenue oriented"<<endl;
		return "revenueOriented";
	}
}

string MainDME::runRequesteePOMDP() {
	long tempBW=MainDME::Instance()->bandwidthObserver->getBandwidthRate();
	int tempNonCoop=MainDME::Instance()->reputationRF->getNonCooperativeProbabilityInPercent();
	if(tempBW>MainDME::Instance()->thresholdBandwidthUtilization){

		return "loadOriented";
	}else if(tempNonCoop>MainDME::Instance()->thresholdNonCooperative){
		return "reputationOriented";
	}else {
		return "bothOriented";
	}
}
void MainDME::sendOurResponse(string& answer, string& apAddress) {


	//TCPConnection *con = new TCPConnection();
	this->accessPointMessageClient=new TCPConnection();
	cout<<className<<"IPAddress of AP is "<<apAddress<<" our answer is "<<answer<<endl;
	json::Object ans; ans["answer"]=json::String(answer);
	//Utilities::errorCatcher("sendOurResponse socket",1);
	if(accessPointMessageClient->sendMessageFirstSizeInInt(apAddress, Utilities::convertJSONToString(ans), this->accessPointResponsePort)){
//		if(Utilities::contains(answer,"accepted")){
//			this->reputationRF->addPositiveDecision(true);
//			this->reputationRF->addNegativeDecision(false);
//		}else{
//			this->reputationRF->addPositiveDecision(false);
//			this->reputationRF->addNegativeDecision(true);
//		}

	}else{//TODO:Ask Mursel here Socket Timeout or connection error
		cout<<"MainDME: Socket Connection Error"<<endl;
		this->reputationRF->addPositiveDecision(false);
		this->reputationRF->addNegativeDecision(true);

	}
	delete accessPointMessageClient;
}
void MainDME::writeConsole(char& str) {
	std::cout << str << std::endl;
}

void MainDME::startReputationTrack() {
	MainDME::Instance()->reputationRF->em->setPeriod(
			MainDME::Instance()->reputationPeriod);
	this->runCenter(MainDME::Instance()->ReputationRunnable,ReputationFE::run,MainDME::Instance()->reputationRF);
	//Utilities::errorCatcher("startReputationTrack",1);
}

void MainDME::startBWTrack() {
	this->runCenter(MainDME::Instance()->BWRunnable,BandwidthUtilizationFE::run,MainDME::Instance()->bandwidthUtilFE);
	//Utilities::errorCatcher("startBWTrack",1);
}

void MainDME::startPopulationTrack() {
	this->runCenter(MainDME::Instance()->PopulationRunnable,ClientPopulationFE::run,MainDME::Instance()->clientPopFE);
	//Utilities::errorCatcher("startPopulationTrack",1);
}

void MainDME::initiateNegotiation(bool loadOriented) {
	int numberOfClientsToBeMigrated=0;
	double incentive=0;
	int currentNumberOfClients=MainDME::Instance()->clientArrDeparture->getNumberOfClients();
	double currentUnitResourceCost=MainDME::Instance()->creditTrack->getUnitResourceCost();
	if(loadOriented){
		MainDME::Instance()->bandwidthUtilFE->addNegotiationDecision(true);
		numberOfClientsToBeMigrated=1;
	}
	else{
		double maxResourceCost=0;
		int bestClientNumber=0;
		for(int i=0;i<20;i++){
			if(maxResourceCost<MainDME::Instance()->unitResourceCostHolder[i]){
				maxResourceCost=MainDME::Instance()->unitResourceCostHolder[i];
				bestClientNumber=i;
			}
		}
		if(bestClientNumber==0 || (bestClientNumber>currentNumberOfClients)){
			numberOfClientsToBeMigrated=1;
		}else{
			numberOfClientsToBeMigrated=currentNumberOfClients-bestClientNumber;
		}

	}
	int during=0;
	incentive=(MainDME::Instance()->resconOfClientPerDayTime->getEstimatedGain(during,currentUnitResourceCost))*currentNumberOfClients;
	bool accepted=MainDME::Instance()->sendOfferToTheAP(numberOfClientsToBeMigrated,incentive);

	if(accepted){
		MainDME::Instance()->trustOtherTrack->increaseAPObjectTrust(MainDME::Instance()->ssidOfOtherAP);
		for(int i=0;i<numberOfClientsToBeMigrated;i++){
			MainDME::Instance()->migrationFunctionality->migrateWorstCongestedClient(MainDME::Instance()->ssidOfOtherAP);
		}
	}
	else{
		MainDME::Instance()->trustOtherTrack->decreaseAPObjectTrust(MainDME::Instance()->ssidOfOtherAP);
	}
}
bool MainDME::sendOfferToTheAP(int& numberOfClientsToBeMigrated,
		double& incentive) {

		this->accessPointMessageClient=new TCPConnection();
		cout<<"IPAddress of AP is "<<this->apOfOtherAP<<endl;
		json::Object ourOffer;
		ourOffer["messagetype"]=json::String("negotiation");
		ourOffer["numberOfClients"]=json::Number(numberOfClientsToBeMigrated);
		ourOffer["incentive"]=json::Number(incentive);

		if(accessPointMessageClient->sendMessageFirstSizeInInt(this->apOfOtherAP, Utilities::convertJSONToString(ourOffer), this->accessPointServerPort)){
			cout<<"MainDME: Our offer successfully sended (\""<<numberOfClientsToBeMigrated<<","<<incentive<<")/Waiting for answer"<<endl;
			//TCPServerSocket *sock=new TCPServerSocket(this->accessPointServerPort);
			//TODO: it seems a little bit complicate and complex, the terms can be ambiguity
			string message=accessPointMessageClient->getMessageFirstSizeInInt(this->accessPointResponseServer->accept());
			//cout<<"Message Received, from other AP: "<<message<<endl;
//			if(Utilities::contains(message,"accepted")){
//				return true;
//			}
		}
		delete accessPointMessageClient;
	return false;

}


int MainDME::requesterLoadOrientedUtility() {
	int numberOfClients=MainDME::Instance()->clientArrDeparture->getNumberOfClients();
	double unitResourceCost=MainDME::Instance()->creditTrack->getUnitResourceCost();
	long temp=MainDME::Instance()->bandwidthObserver->getBandwidthRate();
	if(temp>MainDME::Instance()->thresholdBandwidthUtilization){
		cout<<"Requester is loaded, utility is positive, BW utilization is "<<temp<<endl;
		if(numberOfClients<20){
			MainDME::Instance()->unitResourceCostHolder[numberOfClients]=unitResourceCost;
			MainDME::Instance()->loadOrientedUtilityHolder[numberOfClients]=1;
			MainDME::Instance()->loadHolder[numberOfClients]=temp;
		}
		return 1;
	}else{
		cout<<"Requester is not loaded,utility ia 0 for handoff, BW utilization is "<<temp<<endl;
		if(numberOfClients<20){
				MainDME::Instance()->unitResourceCostHolder[numberOfClients]=unitResourceCost;
				MainDME::Instance()->loadOrientedUtilityHolder[numberOfClients]=0;
				MainDME::Instance()->loadHolder[numberOfClients]=temp;
		}
		return 0;
	}
}


int MainDME::requesterRevenueOrientedUtility() {
	bool positiveUtility=false;
	int numberOfClients=MainDME::Instance()->clientArrDeparture->getNumberOfClients();
	double unitResourceCost=MainDME::Instance()->creditTrack->getUnitResourceCost();
	double basicRevenue=unitResourceCost*numberOfClients;
	if(basicRevenue<MainDME::Instance()->previousBasicRevenue){
		if(numberOfClients>MainDME::Instance()->previousClientNumber){
			cout<<"Better Revenue with "<<numberOfClients<<" than"<<MainDME::Instance()->previousClientNumber<<" returning positive utility"<<endl;
			cout<<numberOfClients-MainDME::Instance()->previousClientNumber<< " clients will be negotiated "<<endl;
			MainDME::Instance()->previousBasicRevenue=basicRevenue;
			MainDME::Instance()->previousClientNumber=numberOfClients;
			positiveUtility=true;
		}
	}
	if(numberOfClients<20){
		MainDME::Instance()->unitResourceCostHolder[numberOfClients]=unitResourceCost;
		MainDME::Instance()->loadOrientedUtilityHolder[numberOfClients]=1;
		MainDME::Instance()->loadHolder[numberOfClients]=basicRevenue;
	}
	if(positiveUtility){
		return 1;
	}else{
		MainDME::Instance()->previousBasicRevenue=basicRevenue;
		MainDME::Instance()->previousClientNumber=numberOfClients;
		cout << "Number of Clients: "
				<< MainDME::Instance()->previousClientNumber
				<< " basic Reveunue "
				<< MainDME::Instance()->previousBasicRevenue
				<< " Currently better revenue" << endl;
		return 0;
	}

}


double MainDME::requestEEloadOrientedUtility(int& numberOfClientsSuggested,
		double& suggestedIncentive) {
	long currentLoad=MainDME::Instance()->bandwidthObserver->getBandwidthRate();
	double Load;
	double utility=0;
	if(currentLoad<MainDME::Instance()->load_th){
		Load=MainDME::Instance()->A_load/(MainDME::Instance()->load_th-currentLoad);
		double unitResourceCost=MainDME::Instance()->creditTrack->getUnitResourceCost();
		double ownCredit=MainDME::Instance()->creditTrack->getOwnCredit();
		double P_Cr=MainDME::Instance()->clientPopFE->getCriticalPopulationProbabilityInPercent();
		double P_H=MainDME::Instance()->bandwidthUtilFE->getHighBWProbabilityInPercent();
		double z_t=MainDME::Instance()->resconOfClientPerDayTime->getCostOfAClient();
		double C_s=Load*P_Cr*P_H*z_t*MainDME::Instance()->C_s_Load_Normalizer;
		double stateComponent=C_s*numberOfClientsSuggested;
		double monetaryComponent=MainDME::Instance()->monetaryDominance->getMonetaryComponent(suggestedIncentive,ownCredit);
		int during=60;
		double estimatedGain=MainDME::Instance()->resconOfClientPerDayTime->getEstimatedGain(during,unitResourceCost)*numberOfClientsSuggested;
		utility=estimatedGain+monetaryComponent-stateComponent;
		cout<<"\n\n LOAD ORIENTED UTILITY REPORT:\nLoad:"<<Load<<"\nP_Cr:"<<P_Cr<<"\nP_H:"<<P_H<<"\nz_t:"<<z_t<<"\nC_s:"<<C_s<<"\nstateComponent:"
				<<stateComponent<<"\nmonetaryComponent:"<<monetaryComponent<<"\nestimatedGain:"<<estimatedGain<<"\nutility:"<<utility<<"\n\n"
				<<endl;
		return utility;
	}else{
		return -1;
	}

}

double MainDME::requestEELoadReputationOrientedUtility(
		int& numberOfClientsSuggested, int& suggestedIncentive) {
	double utility;
	double unitResourceCost=MainDME::Instance()->creditTrack->getUnitResourceCost();
	double ownCredit=MainDME::Instance()->creditTrack->getOwnCredit();
	double P_Co=MainDME::Instance()->reputationRF->getCooperativeProbabilityInPercent();
	double C_s=P_Co*MainDME::Instance()->C_s_Reputation_Normalizer;
	double stateComponent=C_s*numberOfClientsSuggested;
	double monetaryComponent=MainDME::Instance()->monetaryDominance->getMonetaryComponent(suggestedIncentive,ownCredit);
	int during=60;
	double estimatedGain=MainDME::Instance()->resconOfClientPerDayTime->getEstimatedGain(during,unitResourceCost)*numberOfClientsSuggested;
	utility=estimatedGain+monetaryComponent-stateComponent;
	cout<<"\n\n LOAD ORIENTED UTILITY REPORT:"<<"\nP_Cro:"<<P_Co<<"\nC_s:"<<C_s<<"\nstateComponent:"
			<<stateComponent<<"\nmonetaryComponent:"<<monetaryComponent<<"\nestimatedGain:"<<estimatedGain<<"\nutility:"<<utility<<"\n\n"
			<<endl;
	return utility;

}
//TODO::Load is not initialized here WARNUNG!
double MainDME::requestEEBothOrientedUtility(int& numberOfClientsSuggested,
		int& suggestedIncentive) {
	long currentLoad=MainDME::Instance()->bandwidthObserver->getBandwidthRate();
		double Load;
		double utility=0;
		if(currentLoad<MainDME::Instance()->load_th){
			Load=MainDME::Instance()->A_load/(MainDME::Instance()->load_th-currentLoad);
			double unitResourceCost=MainDME::Instance()->creditTrack->getUnitResourceCost();
			double ownCredit=MainDME::Instance()->creditTrack->getOwnCredit();
			double P_Cr=MainDME::Instance()->clientPopFE->getCriticalPopulationProbabilityInPercent();
			double P_H=MainDME::Instance()->bandwidthUtilFE->getHighBWProbabilityInPercent();
			double z_t=MainDME::Instance()->resconOfClientPerDayTime->getCostOfAClient();
			double P_Co=MainDME::Instance()->reputationRF->getCooperativeProbabilityInPercent();
			double C_s=Load*P_Cr*P_H*z_t*P_Co*MainDME::Instance()->C_s_Both_Normalizer;
			double stateComponent=C_s*numberOfClientsSuggested;
			double monetaryComponent=MainDME::Instance()->monetaryDominance->getMonetaryComponent(suggestedIncentive,ownCredit);
			int during=60;
			double estimatedGain=MainDME::Instance()->resconOfClientPerDayTime->getEstimatedGain(during,unitResourceCost)*numberOfClientsSuggested;
			utility=estimatedGain+monetaryComponent-stateComponent;
			cout<<"\n\n LOAD ORIENTED UTILITY REPORT:\nLoad:"<<Load<<"\nP_Cr:"<<P_Cr<<"\nP_H:"<<P_H<<"\nz_t:"<<z_t<<"\nP_Co"<<P_Co<<"\nC_s:"<<C_s<<"\nstateComponent:"
							<<stateComponent<<"\nmonetaryComponent:"<<monetaryComponent<<"\nestimatedGain:"<<estimatedGain<<"\nutility:"<<utility<<"\n\n"
							<<endl;
			return utility;
		}
		else{
			return -1;
		}
}

void MainDME::startRequesterPOMDP(){
	int rc1;
	if ((rc1 = pthread_create(&MainDME::Instance()->requesterPOMDP, NULL,
			&MainDME::runRequesterPOMDP, NULL))) {
		printf("clientArrivalDepartureRunnable Block: clientArrivalDepartureRunnable thread creation failed: %d\n",rc1);
	}
	//Utilities::errorCatcher("startRequesterPOMDP",1);
};

void MainDME::startRequesteePOMDP(){

	int rc1;
	if ((rc1 = pthread_create(&MainDME::Instance()->requesteePOMDP, NULL,
			&MainDME::runRequesteePOMDP, NULL))) {
		 printf("clientArrivalDepartureRunnable Block: clientArrivalDepartureRunnable thread creation failed: %d\n",rc1);
	}
	//Utilities::errorCatcher("startRequesteePOMDP",1);

};
 void* MainDME::runRequesterPOMDP(void*){
	 while(true){
		 sleep(MainDME::Instance()->waitRequesterPOMDP);
		 cout<<"\n\n Test for the requestee load utility"<<endl;
		 int utility=0;
		 bool loadOriented=false;
		 string ourState=MainDME::Instance()->runRequesterPOMDP();
		 if(ourState=="loadOriented"){
			 utility=MainDME::Instance()->requesterLoadOrientedUtility();
			 loadOriented=true;
		 }else if(ourState=="revenueOriented"){
			 utility=MainDME::Instance()->requesterRevenueOrientedUtility();
		 }
		 if(utility>0){
			 MainDME::Instance()->initiateNegotiation(loadOriented);
		 }
	 }

}
void* MainDME::runRequesteePOMDP(void*) {
	while (true) {
		string message = ""; //MainDME::Instance()->accessPointMessageServer.getMessage();

		//cout << "Message received, from other AP: " << message << endl;
//		if (Utilities::contains(message, "negotiation")) {
//			try {
//				json::Object messageIndex=Utilities::convertStringToJSON(message);
//				json::Object receivedMessage=messageIndex["receivedMessage"];
//				int numberOfClientsSuggested=json::Number(receivedMessage["numberOfClients"]);
//				int suggestedIncentive=json::Number(receivedMessage["incentive"]);
//				string apAddress=json::String(messageIndex["ipClient"]);
//				double utility=0;
//				string ourState=MainDME::Instance()->runRequesteePOMDP();
//				if(ourState=="loadOriented"){
//					utility=MainDME::Instance()->requestEELoadReputationOrientedUtility(numberOfClientsSuggested,suggestedIncentive);
//				}else if(ourState=="reputationOriented"){
//					utility=MainDME::Instance()->requestEELoadReputationOrientedUtility(numberOfClientsSuggested,suggestedIncentive);
//				}else if(ourState=="bothOriented"){
//					utility=MainDME::Instance()->requestEEBothOrientedUtility(numberOfClientsSuggested,suggestedIncentive);
//				}
//				string answer="";
//				if(utility<0){
//					answer="declined";
//				}else{
//					answer="accepted";
//				}
//			} catch (exception& e) {
//				cerr << "Main DME: run Requestee POMDP " << e.what() << endl;
//			}
//		}
	}
}
