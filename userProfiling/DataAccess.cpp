/*
 * DataAccess.cpp
 *
 *  Created on: 06.08.2012
 *      Author: cem
 */
#include "DataAccess.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "BlackBoard.h"

//#include "UPMain.h"
using namespace json;
pthread_mutex_t DataAccess::mutexFile=PTHREAD_MUTEX_INITIALIZER;

const string DataAccess::userModelFolderName="Users";
const string DataAccess::qoEFolderName="QoExperinces";
string DataAccess::pathFinder(const string folderName, const string fileName){
	char resolved_path[200];

	//locate the requested paths
	string x="./"+folderName;
	realpath(x.c_str(), resolved_path);
	//printf("\n%s\n", resolved_path);

	//check whether path refers to an object or not
	struct stat sb;
	if (!(stat(resolved_path, &sb) == 0 && S_ISDIR(sb.st_mode))) {
		string cmd="mkdir "+folderName;
		system(cmd.c_str());
	}
	string path = resolved_path;
	path = path +"/"+ fileName;
	//cout << path << endl;

	return path;
}
int storeExcept=0;
void DataAccess::storeUserModel(Object& userModel,string& userId){

	bool succes=false;
	ofstream myfile;
	std::stringstream stream1;
	string path;
	try{

		Writer::Write(userModel, stream1);
		path=pathFinder(DataAccess::userModelFolderName,userId);
		const char *fileName=(path+".txt").c_str();
		pthread_mutex_lock(&mutexFile);
		myfile.open(fileName);
		if (myfile.is_open()) {
			myfile << stream1.str();
			myfile.close();
		}
		succes=true;
		pthread_mutex_unlock(&mutexFile);
	}catch(Exception& e){e.what();
		cerr<<"JsonDataAccess:File could not be stored, re-storing is being attempted."<<endl;
		if(succes==false&&storeExcept<2){
			storeExcept++;
			storeUserModel(userModel,userId);
		}else if(storeExcept==2 && succes==false){
			cerr<<"JsonDataAccess:File could not be stored,a void user profile is being stored."<<endl;
			json::Object elemRootFile1;
			std::istringstream sBadDocument("{\"TCPAverage\":0,\"UDPAverage\":0,\"lastDepartureTime\":\"0\",\"dailyAuthenticationNumber\":0,\"visitFrequencyNumber\":0,\"authenticationNumber\":0,\"bandwidthAverage\":0,\"userId\":\"\",\"sessionAverage\":\"00:00:00:0\",\"sessionsPerMin\":[]}");
			Reader::Read(elemRootFile1,sBadDocument );
			const char *fileName=(path+".txt").c_str();
			pthread_mutex_lock(&mutexFile);
			myfile.open(fileName);
			if (myfile.is_open()) {
				myfile << stream1.str();
				myfile.close();
			}
			pthread_mutex_unlock(&mutexFile);
		}
	}
	storeExcept=0;
}
/*void DataAccess::getUserModel(string& userId,json::Object& up){
	string path=pathFinder(DataAccess::userModelFolderName,userId);
	const char *fileName=(path+".txt").c_str();

	std::stringstream buffer;
	json::Object elemRootFile1;

	std::ifstream file(fileName);
	try{

		if (file) {
			pthread_mutex_lock(&mutexFile);
			cout << "DA: User's Profile exists" << endl;
			buffer << file.rdbuf();
			file.close();
			pthread_mutex_unlock(&mutexFile);
			try {
				Reader::Read(up, buffer);
				//up=elemRootFile1;
			} catch (json::Reader::ParseException& e) {
				cerr<< "DA: User's Profile encountered to the parsing error, new profile is being created."<< endl;
				std::istringstream sBadDocument(
						"{\"TCPAverage\":0,\"UDPAverage\":0,\"lastDepartureTime\":\"0\",\"dailyAuthenticationNumber\":0,\"dailyVFProbability\":0,\"authenticationNumber\":0,\"bandwidthAverage\":0,\"userId\":\"\",\"sessionAverage\":\"00:00:00:0\",\"sessionsPerMin\":[]}");
				Reader::Read(up, sBadDocument);
				//up=elemRootFile1;
			} catch (Reader::ScanException& e) {
				std::cout << "Caught json::ScanException: " << e.what()<< ", Line/offset: " << e.m_locError.m_nLine + 1 << '/'
						<< e.m_locError.m_nLineOffset + 1 << std::endl<< std::endl;
				cerr<< "DA: User's Profile encountered to the parsing error, new profile is being created."<< endl;
				std::istringstream sBadDocument(
						"{\"TCPAverage\":0,\"UDPAverage\":0,\"lastDepartureTime\":\"0\",\"dailyAuthenticationNumber\":0,\"dailyVFProbability\":0,\"authenticationNumber\":0,\"bandwidthAverage\":0,\"userId\":\"\",\"sessionAverage\":\"00:00:00:0\",\"sessionsPerMin\":[]}");
				Reader::Read(up, sBadDocument);
				//up=elemRootFile1;
			}


		} else {
			cout
					<< "DA: User's Profile does not exist, new Profile is being created. "
					<< endl;
			std::istringstream sBadDocument(
					"{\"TCPAverage\":0,\"UDPAverage\":0,\"lastDepartureTime\":\"0\",\"dailyAuthenticationNumber\":0,\"dailyVFProbability\":0,\"authenticationNumber\":0,\"bandwidthAverage\":0,\"userId\":\"\",\"sessionAverage\":\"00:00:00:0\",\"sessionsPerMin\":[]}");
			Reader::Read(up, sBadDocument);
			//up=elemRootFile1;
		}

	} catch (Reader::ScanException& e) {
		std::cout << "Caught json::ScanException: " << e.what()
				<< ", Line/offset: " << e.m_locError.m_nLine + 1 << '/'
				<< e.m_locError.m_nLineOffset + 1 << std::endl << std::endl;
		cout
				<< "DA: User's Profile encountered to the parsing error, new profile is being created"
				<< endl;
		std::istringstream sBadDocument(
				"{\"TCPAverage\":0,\"UDPAverage\":0,\"lastDepartureTime\":\"0\",\"dailyAuthenticationNumber\":0,\"dailyVFProbability\":0,\"authenticationNumber\":0,\"bandwidthAverage\":0,\"userId\":\"\",\"sessionAverage\":\"00:00:00:0\",\"sessionsPerMin\":[]}");
		Reader::Read(up, sBadDocument);
		//up=elemRootFile1;
	} catch (Exception& e) {
		e.what();
		cerr
				<< "DA: User's Profile encountered to the parsing or file error, new profile is being created"
				<< endl;
		std::istringstream sBadDocument(
				"{\"TCPAverage\":0,\"UDPAverage\":0,\"lastDepartureTime\":\"0\",\"dailyAuthenticationNumber\":0,\"dailyVFProbability\":0,\"authenticationNumber\":0,\"bandwidthAverage\":0,\"userId\":\"\",\"sessionAverage\":\"00:00:00:0\",\"sessionsPerMin\":[]}");
		Reader::Read(up, sBadDocument);
		//up=elemRootFile1;
	}

}*/
json::Object DataAccess::getUserModel(string& userId){
	string path=pathFinder(DataAccess::userModelFolderName,userId);
	const char *fileName=(path+".txt").c_str();//TODO: error can arise here owing to the const value

	std::stringstream buffer;
	json::Object elemRootFile1;

	std::ifstream file(fileName);
	try{

		if (file) {
			//pthread_mutex_lock(&UPMain::Instance()->mutexFile);
			pthread_mutex_lock(&mutexFile);
			cout << "JsonDataAccess: User's Profile exists" << endl;
			buffer << file.rdbuf();
			file.close();
			//pthread_mutex_unlock(&UPMain::Instance()->mutexFile);
			pthread_mutex_unlock(&mutexFile);
			try {
				Reader::Read(elemRootFile1, buffer);

			} catch (json::Reader::ParseException& e) {
				cerr
						<< "JsonDataAccess: User's Profile encountered to the parsing error, new profile is being created."
						<< endl;
				std::istringstream sBadDocument(
						"{\"TCPAverage\":0,\"UDPAverage\":0,\"lastDepartureTime\":\"0\",\"dailyAuthenticationNumber\":0,\"dailyVFProbability\":0,\"authenticationNumber\":0,\"bandwidthAverage\":0,\"userId\":\"\",\"sessionAverage\":\"00:00:00:0\",\"sessionsPerMin\":[]}");
				Reader::Read(elemRootFile1, sBadDocument);
				return elemRootFile1;
			} catch (Reader::ScanException& e) {
				std::cout << "JsonDataAccess:Caught json::ScanException: " << e.what()
						<< ", Line/offset: " << e.m_locError.m_nLine + 1 << '/'
						<< e.m_locError.m_nLineOffset + 1 << std::endl
						<< std::endl;
				cerr
						<< "JsonDataAccess: User's Profile encountered to the parsing error, new profile is being created."
						<< endl;
				std::istringstream sBadDocument(
						"{\"TCPAverage\":0,\"UDPAverage\":0,\"lastDepartureTime\":\"0\",\"dailyAuthenticationNumber\":0,\"dailyVFProbability\":0,\"authenticationNumber\":0,\"bandwidthAverage\":0,\"userId\":\"\",\"sessionAverage\":\"00:00:00:0\",\"sessionsPerMin\":[]}");
				Reader::Read(elemRootFile1, sBadDocument);
				return elemRootFile1;
			}
			return elemRootFile1;

		} else {
			cout
					<< "JsonDataAccess: User's Profile does not exist, new Profile is being created. "
					<< endl;
			std::istringstream sBadDocument(
					"{\"TCPAverage\":0,\"UDPAverage\":0,\"lastDepartureTime\":\"0\",\"dailyAuthenticationNumber\":0,\"dailyVFProbability\":0,\"authenticationNumber\":0,\"bandwidthAverage\":0,\"userId\":\"\",\"sessionAverage\":\"00:00:00:0\",\"sessionsPerMin\":[]}");
			Reader::Read(elemRootFile1, sBadDocument);
			return elemRootFile1;
		}

	} catch (Reader::ScanException& e) {
		std::cout << "Caught json::ScanException: " << e.what()
				<< ", Line/offset: " << e.m_locError.m_nLine + 1 << '/'
				<< e.m_locError.m_nLineOffset + 1 << std::endl << std::endl;
		cout
				<< "JsonDataAccess: User's Profile encountered to the parsing error, new profile is being created"
				<< endl;
		std::istringstream sBadDocument(
				"{\"TCPAverage\":0,\"UDPAverage\":0,\"lastDepartureTime\":\"0\",\"dailyAuthenticationNumber\":0,\"dailyVFProbability\":0,\"authenticationNumber\":0,\"bandwidthAverage\":0,\"userId\":\"\",\"sessionAverage\":\"00:00:00:0\",\"sessionsPerMin\":[]}");
		Reader::Read(elemRootFile1, sBadDocument);
		return elemRootFile1;
	} catch (Exception& e) {
		e.what();
		cerr
				<< "JsonDataAccess: User's Profile encountered to the parsing or file error, new profile is being created"
				<< endl;
		std::istringstream sBadDocument(
				"{\"TCPAverage\":0,\"UDPAverage\":0,\"lastDepartureTime\":\"0\",\"dailyAuthenticationNumber\":0,\"dailyVFProbability\":0,\"authenticationNumber\":0,\"bandwidthAverage\":0,\"userId\":\"\",\"sessionAverage\":\"00:00:00:0\",\"sessionsPerMin\":[]}");
		Reader::Read(elemRootFile1, sBadDocument);
		return elemRootFile1;
	}
}
void DataAccess::storeQoEInfo(string& time,Object& qoe,string& userId){
//write here qoe info
	//if(userId!=""){
		bool succes=false;
		ofstream myfile;
		std::stringstream stream1;
		string path;
		json::Object obj;
		obj["time"]=String(time);
		obj["qoeMes"]=qoe;

		try{
			//BlackBoard::writeConsole("QoE store Qoe");

			Writer::Write(obj, stream1);
			path=pathFinder(DataAccess::qoEFolderName,userId);
			const char *fileName=(path+".txt").c_str();
			pthread_mutex_lock(&mutexFile);
			myfile.open(fileName,ios::app);//open (filename, mode);
			if (myfile.is_open()) {
				myfile << stream1.str();
				myfile.close();
			}
			succes=true;
			pthread_mutex_unlock(&mutexFile);
		}catch(Exception& e){e.what();
			cerr<<"JsonDataAccess:File could not be stored, re-storing is being attempted."<<endl;
			if(succes==false&&storeExcept<2){
				storeExcept++;
				storeQoEInfo(time,qoe,userId);
			}else if(storeExcept==2 && succes==false){
				cerr<<"JsonDataAccess:File could not be stored,a void user profile is being stored."<<endl;
				json::Object elemRootFile1;
				std::istringstream sBadDocument("{\"Qoe:\":\"Error\"}");
				Reader::Read(elemRootFile1,sBadDocument );
				const char *fileName=(path+".txt").c_str();
				pthread_mutex_lock(&mutexFile);
				myfile.open(fileName);
				if (myfile.is_open()) {
					myfile << stream1.str();
					myfile.close();
				}
				pthread_mutex_unlock(&mutexFile);
			}
		}
		storeExcept=0;
	//}

}
//TODO:Need to re-edit getUserQoE
Object DataAccess::getUserQoE(string& userId){
}
void DataAccess::synchronizedStoringData(Object& userModel,string& userId){
/*
	pthread_mutex_lock(&mutexFile);
	std::stringstream stream1;
	Writer::Write(userModel, stream1);

	cout << "STREAM CONTENT :" + stream1.str() << endl;
	ofstream myfile;
	try {
		const char *fileName = (userId + ".txt").c_str();
		myfile.open(fileName);
		if (myfile.is_open()) {
			myfile << stream1.str();
			//myfile << "This is another line.\n";
			myfile.close();
		}
	} catch (Exception& e) {
		e.what();
		//reasons could be file could not be opened owing to the fact that it does not exist

	} //TODO:add here I/O exception not general exception
	pthread_mutex_unlock(mutexFile);
*/

}
void DataAccess::synchronizedGettingData(string& userId){}

