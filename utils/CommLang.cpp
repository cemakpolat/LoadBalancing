/*
 * CommLang.cpp
 *
 *  Created on: Dec 18, 2012
 *      Author: cem
 */

#include "CommLang.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "writer.h"
#include "elements.h"
#include "reader.h"
//using namespace json;
//some functionalities

CommLang::CommLang() {
}
std::string CommLang::createRequestMesObj(int port,std::string& ip,int request) {
	json::Object packet;
	packet["ip"]=json::String(ip);
	packet["serverPort"] = json::Number(port);
	packet["request"] = json::Number(request);
	std::stringstream stream1;
	json::Writer::Write(packet, stream1);
	return stream1.str();
}
std::string CommLang::createResponseMesObj(
		std::string response) {
	json::Object packet;
	//packet["ip"] = json::String(ip);
	packet["message"] = json::String(response);
	std::stringstream stream1;
	json::Writer::Write(packet, stream1);
	return stream1.str();
}
std::string CommLang::createOfferMesToAP(std::string& ip, std::string& offer) {
	json::Object packet;
	packet["ip"] = json::String(ip);
	packet["offer"] = json::String(offer);
	std::stringstream stream1;
	json::Writer::Write(packet, stream1);
	return stream1.str();
}
std::string CommLang::createMessageObjToClient(std::string& ip,
		std::string& message) {
	json::Object packet;
	packet["ip"] = json::String(ip);
	packet["message"] = json::String(message);
	std::stringstream stream1;
	json::Writer::Write(packet, stream1);
	return stream1.str();
}
std::string CommLang::createGenericMessage(std::string& message) {
	json::Object packet;
	packet["message"] = json::String(message);
	std::stringstream stream1;
	json::Writer::Write(packet, stream1);
	return stream1.str();
}

//std::vector<commRule> CommLang::rules;

//std::string CommLang::createCommObject(int servSockNum, int commRuleId,
//		std::string userId) {
//
//	json::Object packet;
//	packet["IPAddress"]=json::String("192.168.1.2");//TODO: Replace this static value with ip parameter
//	packet["ServerSocNum"] = json::Number(servSockNum);
//	packet["ReqRuleNum"] = json::Number(commRuleId);
//	packet["userId"] = json::String(userId);
//	packet["content"] = json::String("");
//	std::stringstream stream1;
//	json::Writer::Write(packet, stream1);
//	return stream1.str();
//}
//void CommLang::showCommRules() {
//	for (unsigned int i = 0; i < rules.size(); i++) {
//		std::cout << rules[i].ruleId << " " << rules[i].definition << std::endl;
//	}
//}
//void CommLang::preapareCommRules() {
//	commRule co1, co2, co3, co4;
//
//	//create rules
//	co1.ruleId = getCurrentBWUsageOfKnownID;
//	co1.definition = "Current Remained Bandwidth is being obtaining...";
//	co2.ruleId = getOnlineUsers;
//	co2.definition = "Currently connected Users are being shown...";
//	co3.ruleId = getCurrentBWUsageOfKnownID;
//	co3.definition =
//			"The figures of Current Bandwidth Consumption for a specific user is discerned... ";
//	co4.ruleId = getLastQoSResultOfKnownID;
//	co4.definition = "Last QoS Result for a specific User is shown...";
//
//	//add rules
//	rules.push_back(co1);
//	rules.push_back(co2);
//	rules.push_back(co3);
//	rules.push_back(co4);
//
//}
