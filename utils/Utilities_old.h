/*
 * Utilities.h
 *
 *  Created on: 07.08.2012
 *      Author: cem
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "writer.h"
#include "elements.h"
#include "reader.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
//using namespace std;
//using namespace json;

class Utilities{
public:
	Utilities();
	~Utilities();

	static void errorCatcher(std::string funcName,int position){
		std::cout <<"FunctionName: "<<funcName<< " position: "<<position << std::endl;
	}
	static int second(int second) {
		//int oneSecond=1000000;
		return 1000000*second;
	}
	static unsigned int minute(int min) {
		//int oneSecond=1000000*60;
		return 1000000*60*min;
	}
	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	    std::stringstream ss(s);
	    std::string item;
	    while(std::getline(ss, item, delim)) {
	        elems.push_back(item);
	    }
	    return elems;
	}
	/**
	 * Return true or false if the given parameter contains the parameters being searched.
	 */
	static long convertStringToLong(std::string& s){
		try {
			return atol(s.c_str());
		} catch (std::exception& e) {
			std::cout
					<< "Utilities:Converting could not be successfully performed! "
					<< e.what() << std::endl;
			return 0;
		}
	}
	static int convertStringToInt(std::string& s){
		try {
			return atoi(s.c_str());
		} catch (std::exception& e) {
			std::cout
					<< "Utilities:Converting could not be successfully performed! "
					<< e.what() << std::endl;
			return 0;
		}
	}
	static double convertStringDouble(std::string& s){
		try {

			return atof(s.c_str());
		} catch (std::exception& e) {
			std::cout
					<< "Utilities:Converting could not be successfully performed! "
					<< e.what() << std::endl;
			return 0;
		}
	}
	static bool contains(std::string& text, std::string searchParam) {
		if (std::string::npos != text.find(searchParam)) {
			return true;
		} else {
			return false;
		}
	}
	static std::vector<std::string> split(const std::string &s, char delim) {
	    std::vector<std::string> elems;
	    return split(s, delim, elems);
	}

	static std::string convertIntToString(int& number)
	{
	   std::stringstream ss;//create a stringstream
	   ss << number;//add number to the stream
	   return ss.str();//return a string with the contents of the stream
	}
	static std::string convertDoubleToString(double& number)
		{
		   std::stringstream ss;//create a stringstream
		   ss << number;//add number to the stream
		   return ss.str();//return a string with the contents of the stream
		}
	//
	static bool timeCompareWithCurrentTime(int value,std::string& time){
		//std::cout<<"time in dhcp: "<<atoi(time.c_str())<<std::endl;
		//std::cout<<"current time: "<<getCurrentTimeAsSecond()<<std::endl;
		if((getCurrentTimeAsSecond()-atoi(time.c_str()))<value){
			return true;
		}else{
			return false;
		}

	}
	//the following string to int converter is likely seen as c++ version, however the next one is mostly used in C.
	static int convertStringToIntCPP(std::string& s){
		  std::stringstream ss(s); // Could of course also have done ss("1234") directly.
		  int i;
		  if( (ss >> i).fail() )
		  {
		     std::cout<<"Utilities:Error occured"<<std::endl;
		  }
		  std::cout << i << std::endl;
		  return i;
	}
	static int covertStringToIntC(std::string& def){
		int val=0;
		try{
			val=atoi(def.c_str());
			return val;
		}catch(std::exception& e){
			std::cout<<"Utilities:Converting could not be successfully performed! "<<e.what()<<std::endl;
			return val;
		}
		return val;
	}
	static int getCurrentMinute(){

		std::string time=getCurrentTimeAsString();
		std::vector<std::string> list=split(time,':');
		int hour=convertStringToInt(list[0]);
		int min=convertStringToInt(list[1]);
		return hour*60+min;
	}
	static std::string getCurrentDateAndTimeAsString(){
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, 80, "%Y/%m/%d-%H:%M:%S", timeinfo);
		std::stringstream ss;
		ss<<buffer;
		return ss.str();

	}

	static std::string getCurrentTimeAsString(){
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, 80, "%H:%M:%S", timeinfo);
		std::stringstream ss;
		ss<<buffer;
		return ss.str();

	}
	static std::string getTimeFromGivenTime(const time_t& time){//time should be already assigned to time value.
		struct tm * timeinfo;
		char buffer[80];
		timeinfo = localtime(&time);
		strftime(buffer, 80, "%H:%M:%S", timeinfo);
		std::stringstream ss;
		ss << buffer;
		return ss.str();
	}
	static std::string getCurrentDateAsString(){
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, 80, "%Y/%m/%d", timeinfo);
		std::stringstream ss;
		ss<<buffer;
		return ss.str();

	}

	static std::string longToTimeString(long second){
		long elapsedTime = second;
		int hours = (int) elapsedTime / (60 * 60);
		int minutes = (int) (elapsedTime / (60)) % 60;
		int seconds = (int) (elapsedTime) % 60;
		//int mili = (int) (milisecond) % (1000);
		// System.out.println("convertMiliSecondTimeToString: "+milisecond);
		// System.out.println("hour: "+hours + ":" + minutes + ":" + seconds +
		// ":" + mili);
		std::string result= convertIntToString(hours) + ":" + convertIntToString(minutes) + ":" + convertIntToString(seconds) ;
		//std::cout<<"longToString: "<<result<<std::endl;
		return result;
	}
	static long convertStringToMilisecond(std::string time) {

		std::vector<std::string> strTime=Utilities::split(time,':');
		long second = 0;
		second = atoi(strTime[0].c_str()) * 1 * 60 * 60 + second;
		second = atoi(strTime[1].c_str()) * 1 * 60 + second;
		second = atoi(strTime[2].c_str()) * 1 + second;
		//milisecond = atoi(strTime[3].c_str()) + milisecond;
		// System.out.println(" convertStringToMilisecond: "+milisecond);
		return second;
	}
	static time_t getCurrentTimeAsSecond(){
		time_t seconds;
		seconds = time (NULL);
		return seconds;
	}
	static std::string convertMilisecondToString(const time_t& time){//time should be already assigned to time value.
		struct tm * timeinfo;
		char buffer[80];
		timeinfo = localtime(&time);
		strftime(buffer, 80, "%Y/%m/%d-%H:%M:%S", timeinfo);
		std::stringstream ss;
		ss << buffer;
		return ss.str();
	}

	static time_t convertDateTimeStringToMilisecond(const std::string& time1 ){
		if(time1!="0"){
		std::vector<std::string> date;
		std::vector<std::string> d;
		std::vector<std::string> t;
		date=split(time1,'-');
		d=split(date[0],'/');
		t=split(date[1],':');
		time_t rawtime;
		struct tm * timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		timeinfo->tm_year = atoi(d[0].c_str())-1900;
		timeinfo->tm_mon = atoi(d[1].c_str())-1;
		timeinfo->tm_mday =atoi(d[2].c_str());
		timeinfo->tm_hour= atoi(t[0].c_str());
		timeinfo->tm_min = atoi(t[1].c_str());
		timeinfo->tm_sec = atoi(t[2].c_str());
		time_t x=mktime (timeinfo);
		return x;
		}else{

			return 0;
		}
	}
	static time_t convertTimeStringToMilisecond(const std::string& time1 ){
		if(time1!="0"){
		std::vector<std::string> t;
		t=split(time1,':');
		time_t rawtime;
		struct tm * timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
	//	timeinfo->tm_year = atoi(d[0].c_str())-1900;
	//	timeinfo->tm_mon = atoi(d[1].c_str())-1;
	//	timeinfo->tm_mday =atoi(d[2].c_str());
		timeinfo->tm_hour= atoi(t[0].c_str());
		timeinfo->tm_min = atoi(t[1].c_str());
		timeinfo->tm_sec = atoi(t[2].c_str());
		time_t x=mktime (timeinfo);
		return x;
		}else{
			return 0;
		}
	}
	static json::Object convertStringToJSON(std::string& str){
		using namespace json;
		//std::istringstream sBadDocument("{\"cem\":\"yes\",\"array\":\"xx\"}"); // missing comma!
		try{
			std::istringstream incomingString(str); // missing comma!
			Object jsonObj;
			Reader::Read(jsonObj, incomingString);
			return jsonObj;
		}catch (Reader::ParseException& e){
			 std::cout << "Utilities:2:Caught json::ParseException: " << e.what() << ", Line/offset: " << e.m_locTokenBegin.m_nLine + 1
			                << '/' << e.m_locTokenBegin.m_nLineOffset + 1 << std::endl << std::endl;
		}
		catch(json::Exception& e){
			e.what();
		}
		Object *obj=NULL;
		return *obj;//TODO: Exception ?
	}

	static int randomGenerator(int min,int max) {

		time_t t;
		time(&t);
		srand((unsigned int) t);
		int num = rand() % max + min;
		//printf("generated number %d\n ", num);
		return num;
	}
	static void randomBackOfftime() {

		int randResult = 1 + (rand() % 5);
		//string randStr=" in randombackoff time state: "+Utilities::convertIntToString(randResult);
		try {
			usleep(randResult * Utilities::second(1)); //usleep is in microsecond
		} catch (std::exception& e) {
			e.what();
		}

	}
	/**random variable is choosen from 1 to a given value. Given value is referred to as maximumvalue,
	 * it means the max random value could be the given value.
	 * @maxValue
	 */
	static void randomBackOfftime(int maxValue) {

		int randResult = 1 + (rand() % maxValue);
		//string randStr=" in randombackoff time state: "+Utilities::convertIntToString(randResult);
		try {
			usleep(randResult * Utilities::second(1)); //usleep is in microsecond
		} catch (std::exception& e) {
			e.what();
		}

	}

		/**
		 * Execute the given command on the back console
		 * then return the output of the executed command
		 */
		static std::string exec(std::string cmd) {
			FILE* pipe = popen(cmd.c_str(), "r");
			if (!pipe)
				return "ERROR";
			char buffer[128];
			std::string result = "";
			while (!feof(pipe)) {
				if (fgets(buffer, 128, pipe) != NULL)
					result += buffer;
			}
			pclose(pipe);
			return result;
		}
		/**
		 * Get Network Interface*/
	static std::string getNetworkInterface(){

		std::string result = "";
		std::string str2("ESSID");
			size_t found;
			//wlan0
			result = exec("iwconfig wlan0");
			found = result.find(str2);
			if (found != std::string::npos) {
				//cout<<"device is wlan0"<<endl;
				return "wlan0";
			}
			//eth1
			result = exec("iwconfig eth1");
			found = result.find(str2);
			if (found != std::string::npos) {
				return "eth1";
			}
			//en1
			result = exec("iwconfig en1");
			found = result.find(str2);
			if (found != std::string::npos) {
				return "en1";
			}
			//eth0
			result = exec("iwconfig eth0");
			found = result.find(str2);
			if (found != std::string::npos) {
				return "eth0";
			}

	}
		/*
		 * Scan all Access Point around the current AP return as a json string
		 * including MAC,ESSID and signal level
		 */
		static std::string scanAllAP() {
			std::string result = "";
			std::string s="sudo iwlist "+getNetworkInterface()+" scanning|awk '/Address:/{print $4$5}/ESSID:/{print $1}/Quality/{print $3}'>scanningResult";
			result = exec(s);
			std::ifstream infile("/root/scanningResult");
			try {
				std::string line;
				size_t found;
				//define json array Object
				json::Array sessionsPerMin;
				//define json object
				json::Object sessionSample;
				sessionSample["Address"]=json::String("");
				sessionSample["ESSID"]=json::String("");
				sessionSample["SNR"]=json::String("");
				while (std::getline(infile, line)) {
					std::string str("Address");
					found = line.find(str);
					if (found != std::string::npos) {
						sessionSample["Address"] = json::String(line);
					}
					str = "ESSID";
					found = line.find(str);
					if (found != std::string::npos) {
						sessionSample["ESSID"] = json::String(line);
					}
					str = "level";
					found = line.find(str);
					if (found != std::string::npos) {
						sessionSample["SNR"] = json::String(line);
						sessionsPerMin.Insert(sessionSample);//last value, then add in the list
					}
				}
				//convert in string
				std::stringstream stream1;
				json::Writer::Write(sessionsPerMin, stream1);
				return stream1.str();
			} catch (std::exception& e) {
				e.what();

			}
		}
		//Ad here also JSON TUTORIAL which might ease its usage
		static std::string convertJSONToString(json::Object jo){
			std::stringstream stream1;
			try{
			json::Writer::Write(jo, stream1);
			}catch(json::Exception &e){
				std::cerr<<"JSON Writing Exception "<<e.what()<<std::endl;
			}
			return stream1.str();
		}
		static void fileWrite(std::string& fileName,
				std::string& outputString) {
			try {
				std::ofstream file;
				file.open(fileName.c_str());
				if (file.is_open()) {
					file << outputString;
					file << "\n";
					file.close();
				}
			} catch (std::exception& e) {
				std::cerr << " " << e.what() << std::endl;
			}
		}
	static json::Object readJSONFile() {

		std::string path = "";
		//pathFinder(DataAccess::userModelFolderName,userId);
		const char *fileName = (path + ".txt").c_str(); //TODO: error can arise here owing to the const value

		std::stringstream buffer;
		json::Object elemRootFile1;

		std::ifstream file(fileName);
		try {
			if (file) {
				std::cout << "JsonDataAccess: User's Profile exists" << std::endl;
				buffer << file.rdbuf();
				file.close();

				try {
					json::Reader::Read(elemRootFile1, buffer);
					return elemRootFile1;
				} catch (json::Reader::ParseException& e) {

				}
			} else {
				std::cout << "File does not exist" << std::endl;
			}
		} catch (std::exception& e) {
			std::cout << "File does not exist" << std::endl;
		}
		return elemRootFile1;
	}

	static void fileJSONWrite(json::Object& userModel, std::string& fileName) {

		//bool succes = false;
		std::ofstream myfile;
		std::stringstream stream1;
		//string path;
		try {

			json::Writer::Write(userModel, stream1);
		//	path = pathFinder(DataAccess::userModelFolderName,fileName);
			//const char *fileName = (path + ".txt").c_str();
			myfile.open(fileName.c_str());
			if (myfile.is_open()) {
				myfile << stream1.str();
				myfile.close();
			}
			//succes = true;

		} catch (json::Exception& e) {
			e.what();
		}
	}

	static std::string pathFinder(const std::string folderName, const std::string fileName) {
		char resolved_path[200];
		//locate the requested paths
		std::string x = "./" + folderName;
		realpath(x.c_str(), resolved_path);

		struct stat sb;
		if (!(stat(resolved_path, &sb) == 0 && S_ISDIR(sb.st_mode))) {
			std::string cmd = "mkdir " + folderName;
			system(cmd.c_str());
		}
		std::string path = resolved_path;
		path = path + "/" + fileName;

		return path;
	}
	// trim from start
		static inline std::string &ltrim(std::string &s) {
				s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
				return s;
		}
		//s.erase(remove_if(s.begin(), s.end(), isspace), s.end());

		// trim from end
		static inline std::string &rtrim(std::string &s) {
				s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
				return s;
		}

		// trim from both ends
		static inline std::string &trim(std::string &s) {
				return ltrim(rtrim(s));
		}
};



#endif /* UTILITIES_H_ */
