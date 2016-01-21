/*
 * MigrateWorstQoEClient.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#include "MigrateWorstQoEClient.h"
#include "PracticalSocket.h"
#include "TCPConnection.h"
#include "CommLang.h"
#include "Utilities.h"
#include "Definitions.h"

MigrateWorstQoEClient::MigrateWorstQoEClient() {
	// TODO Auto-generated constructor stub
	this->apSSID="wpaclient_ufit-lect.conf";
	this->otherSSID="wpaclient_ufit-right.conf";
	this->clientPortOfUser=Definitions::QoEClient;
	this->measurementPlaneServer=Definitions::MPServerPortNumber;
	this->loadBalancingServerPort=14555;
	this->localAddress="127.0.0.1";
}

MigrateWorstQoEClient::~MigrateWorstQoEClient() {
	// TODO Auto-generated destructor stub
}

int MigrateWorstQoEClient::migrateWorstCongestedClient(std::string& apSSID){
	using namespace std;
	this->otherSSID="Wpaclient_"+apSSID+".conf";
	json::Object incomingMessage;
	json::Object requestMessage;
	string userIP="";
	string ipAddress="";
	CommLang *cm = new CommLang();
		try{

			TCPServerSocket *serv=new TCPServerSocket(this->loadBalancingServerPort);
			TCPConnection *senReqToMP =new TCPConnection();

			// SEND REQUEST MESSAGE TO MEASUREMENT PLANE
			senReqToMP->sendMessageFirstSizeInInt(this->localAddress,
				cm->createRequestMesObj(this->loadBalancingServerPort,
						this->localAddress, CommLang::getWorstCongestedUser),
				this->measurementPlaneServer);

			//GET RESPONSE FROM UBM
			string temp=senReqToMP->getMessageFirstSizeInInt(serv->accept());
			incomingMessage=Utilities::convertStringToJSON(temp);
			string ipAddress=json::String(incomingMessage["ip"]);
			json::Object response;
			response=incomingMessage["response"];
			userIP=json::String(response["congestedUser"]);
			delete senReqToMP;


		}catch(json::Exception& e){
			cerr<<"Migrate QoE Client: "<<e.what()<<endl;
		}
		delete cm;
		cout<<"Congested User: "<<userIP<<endl;
		CommLang *messageToUser = new CommLang();
		//send message to user so as to force cluent to change AP
		TCPConnection *senMesToUser=new TCPConnection();
		string command=this->otherSSID;
		if(senMesToUser->sendMessageFirstSizeInInt(ipAddress,messageToUser->createMessageObjToClient(ipAddress,command),this->clientPortOfUser)){
			delete senMesToUser;delete messageToUser;
			return 0;
		}else{
			delete senMesToUser;delete messageToUser;
			cout<<"Error in connection to the client"<<endl;
			return -1;
		}
		//delete senMesToUser;
		return 0;
}

std::string MigrateWorstQoEClient::getIPAddr(){

}
