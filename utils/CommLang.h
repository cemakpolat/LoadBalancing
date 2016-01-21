/*
 * CommLang.h
 *
 *  Created on: Dec 18, 2012
 *      Author: cem
 */

#ifndef COMMLANG_H_
#define COMMLANG_H_


#include <string>
//#include "CommObject.h"
//#include <vector>
//using namespace std;
/*
 * Communication rule structure
 * @ruleId represents the id of the defined rule
 * @definition shows the definition of the rule
 * */
struct commRule{
	unsigned int ruleId;
	std::string definition;
};
class CommLang{
public:
	//static std::vector<commRule> rules;
	//define required communication rules in order to provide the communication
	//general requests
	static const unsigned int getCurrentRemainedBandwidthUsage=1;
	static const unsigned int getOnlineUsers=2;
	//user specific results
	static const unsigned int getCurrentBWUsageOfKnownID=10;
	static const unsigned int getLastQoSResultOfKnownID=20;
	static const unsigned int getfirstCongestedUser=29;
	static const unsigned int getWorstCongestedUser=30;
	static const unsigned int getCongestedUsers=31;
	static const unsigned int getAverageDelayTime=40;
	static const unsigned int getAverageJitter=50;
	static const unsigned int getPacketLoss=60;
	static const unsigned int getBitRate=70;
	//some functionalities
	static void showCommRules();
	static void preapareCommRules();
	std::string createCommObject(int ssn,int commRuleId,std::string userId);
	CommLang();
	std::string createRequestMesObj(int port,std::string& ip,int request);
	std::string createResponseMesObj(std::string response);
	std::string createOfferMesToAP(std::string& ip, std::string& offer);
	std::string createMessageObjToClient(std::string& ip,std::string& message);
	std::string createGenericMessage(std::string& message);

};

#endif /* COMMLANG_H_ */
