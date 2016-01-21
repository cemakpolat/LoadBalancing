/*
 * BandwidthObserver.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#include "BandwidthObserver.h"
#include <fstream>
#include "Utilities.h"
#include <vector>
#include <stdlib.h>
std::string BandwidthObserver::className="BandwidthObserver";
BandwidthObserver::BandwidthObserver() {
	// TODO Auto-generated constructor stub
	//this->className="BandwidthObserver";
	this->bandwidthRX=0;
	this->bandwidthTX=0;
	this->waitForCycle=200;
}

BandwidthObserver::~BandwidthObserver() {
	// TODO Auto-generated destructor stub
}

long BandwidthObserver::getBandwidthRate(){
	this->bandwidthRX=0;
	this->bandwidthTX=0;
	this->currentTransmitted=0;
	this->previousReceived=0;
	this->previousTransmitted=0;
	this->currentReceived=0;

	for(int times=0;times<5;times++){
		try{
			std::ifstream infile;

			infile.open(filePath.c_str());
			std::string sLine;
			char del='/';
			while (!infile.eof())
			{
					getline(infile, sLine);

					 std::vector<std::string> temp=Utilities::split(sLine,del);
					 for(unsigned int i=0;i<temp.size();i++){
						 if (std::string::npos != temp[i].find("ath1"))
						 {
						  if(times==0){
							  this->previousTransmitted=atol(temp[i+9].c_str());
							  this->previousReceived=atol(temp[i+1].c_str());
						  }
						  else{
							  this->currentTransmitted=atol(temp[i+9].c_str());
							  this->currentReceived=atol(temp[i+1].c_str());
							  long bytesPerSecondTx=this->currentTransmitted-this->previousTransmitted;
							  long bytesPerSecondRx=this->currentReceived-this->previousReceived;
							  this->previousReceived=this->currentReceived;
							  this->previousTransmitted=this->currentTransmitted;
							  if(bytesPerSecondTx>this->bandwidthTX){
								  this->bandwidthTX=bytesPerSecondTx;
							  }
							  if(bytesPerSecondRx>this->bandwidthRX){
								  this->bandwidthRX=bytesPerSecondRx;
							  }
						  }
						  std::cout<<"Received bytes on ath1: "<<temp[i+1]<<std::endl;
						  std::cout<<"Transmitted bytes from ath1: "<< temp[i+9]<<std::endl;
					 }

				 }
					 std::cout << sLine << std::endl;
		}
		infile.close();
		sleep(this->waitForCycle/1000);
		}
		catch(std::exception& e){
			std::cerr<<this->className<<e.what()<<std::endl;
		}
	}
	if(this->bandwidthRX>=this->bandwidthTX){
		std::cout<<"Returning total bytes received in "<<this->waitForCycle<<" miliseconds as "<<this->bandwidthRX<<std::endl;
		return this->bandwidthRX;
	}
	else{
		std::cout<<"Returning total bytes transmitted in "<<this->waitForCycle<<" miliseconds as "<<this->bandwidthTX<<std::endl;
		return this->bandwidthTX;
	}



}


void readFromFile(std::string& fileName){
	std::ifstream infile;

	infile.open(fileName.c_str());
	std::string sLine;

	while (!infile.eof())
	{
		getline(infile, sLine);
		std::cout << sLine << std::endl;

	}
	infile.close();

}
//Basic writeConsole, sometimes it does not work interestingly
//void BandwidthObserver::writeConsole(std::string& msg){
//	std::cout<<msg<<std::endl;
//}
//File reading
//void read() {
//
//	std::ifstream infile("thefile.txt");
//	int a, b; //if file structure is already known, let say : 3 5 and the other line 3 4. the following function
//	while (infile >> a >> b) {
//		// process pair (a,b)
//	}
//#include <sstream>
//#include <string>
//
//	std::string line;
//	while (std::getline(infile, line)) {
//		std::istringstream iss(line);
//		int a, b;
//
//		if (!(iss >> a >> b)) {
//			break;
//		} // error
//
//		// process pair (a,b)
//	}
//
//}
