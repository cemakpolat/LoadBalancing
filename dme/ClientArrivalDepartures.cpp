/*
 * ClientArrivalDepartures.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#include "ClientArrivalDepartures.h"
#include "PracticalSocket.h"
#include <iostream>


//#include "Utilities.h"
//#include "CommLang.h"

string ClientArrivalDepartures::className="ClientArrivalDepartures";

ClientArrivalDepartures::ClientArrivalDepartures() {
	// TODO Auto-generated constructor stub


}

ClientArrivalDepartures::~ClientArrivalDepartures() {
	// TODO Auto-generated destructor stub
}

void* ClientArrivalDepartures::run(void* object) {

	ClientArrivalDepartures *cad=(ClientArrivalDepartures*)object;
	cad->portNumber=5012;//TODO: Find another way to query the authenticity of users
	//int AssoAuthThreadServerPortNumber=5011;//add this port number to Definitions file
	//TCPConnection *con;
	while(true){
		TCPServerSocket servSock(cad->portNumber);
		//con=new TCPConnection();
		try{
			json::Object message=cad->getMessage(servSock.accept());
			//string mes=con->getMessageFirstSizeInInt(servSock.accept());
			//json::Object message=Utilities::convertStringToJSON(mes);
			std::string processesUserId=json::String(message["userId"]);
			std::string state=json::String(message["State"]);
			std::string mac=json::String(message["MAC"]);
			cad->processesUserId=processesUserId;
			cad->processedState=state;
			cad->processedMAC=mac;
			std::cout<<cad->className<<" MESSAGE HOSTAPD:"<<cad->processedMAC<<std::endl;
		}catch (json::Reader::ScanException& e) {
			cerr<<cad->className<<" "<<e.what()<<endl;
		}
		switch(cad->getRelatedMessageNumber(cad->processedState)){
			case ClientArrivalDepartures::Association:
				std::cout<<ClientArrivalDepartures::className<<cad->processesUserId<< "is associated"<<std::endl;

				MainDME::Instance()->clientPopFE->addClientArrival(true);
				break;
			case ClientArrivalDepartures::Authentication:
				std::cout<<ClientArrivalDepartures::className<< "arrival of a client with MAC "<< cad->processedMAC<<std::endl;

				MainDME::Instance()->clientPopFE->addClientArrival(true);
				cad->insertClientToMacList(cad->processedMAC);
				break;
			case ClientArrivalDepartures::Disassociation:
				std::cout<<ClientArrivalDepartures::className<< "dissassociation of"<<cad->processedMAC<<std::endl;

				MainDME::Instance()->clientPopFE->addClientArrival(true);
				cad->deleteClientFromMACList(cad->processedMAC);
				break;
			case ClientArrivalDepartures::Deauthentication:
				std::cout<<ClientArrivalDepartures::className<<cad->processedIP <<" is deauthenticated"<<std::endl;
				//cad->dme->clientPopFE->addClientArrival(true);
				MainDME::Instance()->clientPopFE->addClientArrival(true);
				cad->deleteClientFromMACList(cad->processedMAC);
				break;
			default:
				std::cout<<cad->className<<" "<<"Hostapd message is discarded"<<std::endl;
				break;
		}
	}

}


int ClientArrivalDepartures::getNumberOfClients(){
 return this->clientMACArray.size();
}
json::Object ClientArrivalDepartures::getMessage(TCPSocket *sock) {

	//read Message block and return message
	int val[1];
	sock->recv(val, sizeof(val));
	char echoBuffer2[val[0]];
	sock->recv(echoBuffer2, val[0]);
	echoBuffer2[val[0]] = '\0'; //TODO: you should add '\0' end of the line so as to prevent the additional data.
	delete sock;
	json::Object jsonObj;
	try {
		//convert here json
		istringstream jsonString(echoBuffer2);
		json::Reader::Read(jsonObj, jsonString);
	} catch (json::Exception& e) {
		e.what();
		jsonObj["userId"] = json::String("");
		jsonObj["MAC"] = json::String("");
		jsonObj["State"] = json::String("");
		jsonObj["Token"] = json::String("");
	}
	//return converted JSON Version
	return jsonObj;

}
void ClientArrivalDepartures::deleteClientFromMACList(std::string& mac) {
	bool found = false;
	unsigned int index = 0;
	for (unsigned int i = 0; i < this->clientMACArray.size(); i++) {
		if (this->clientMACArray[i] == this->processedMAC) {
			found = true;
			index = i;
			i = this->clientMACArray.size();
		}
	}
	if (found) {
		this->clientMACArray.erase(this->clientMACArray.begin()+index);
		cout << "Client MAC is removed " << this->processedMAC << endl;
	}
}
void ClientArrivalDepartures::insertClientToMacList(string& mac) {
	bool found = false;
	//int index = 0;
	for (unsigned int i = 0; i < this->clientMACArray.size(); i++) {
		if (this->clientMACArray[i] == this->processedMAC) {
			found = true;

			i = this->clientMACArray.size();
		}
	}
	if (!found) {
		this->clientMACArray.push_back(this->processedMAC);
		cout << "Client MAC is addded " << this->processedMAC << endl;
	}
}

//TODO: Re-edit this function because you need static defined parameters or arrays
int ClientArrivalDepartures::getRelatedMessageNumber(string& message){
	int stateNum = ClientArrivalDepartures::Disassociation;
	if (message == "Authentication") {
		stateNum = 0;
	} else if (message == "Association") {
		stateNum = 1;
	} else if (message == "Disassocation") {
		stateNum = 2;
	} else if (message == "Deauthentication") {
		stateNum = 3;
	}

	return stateNum;

}
