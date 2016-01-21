/*
 * IWConfig.cpp
 *
 *  Created on: Mar 1, 2013
 *      Author: cem
 */

#include "IWConfig.h"
#include "Utilities.h"
#include <fstream>
#include <sstream>

string IWConfig::interface=Utilities::getNetworkInterface();
IWConfig::IWConfig() {
	// TODO Auto-generated constructor stub
	//Utilities::exec("")
}
IWConfig::IWConfig(int maxEstimatedThreshold,int minEstimatedThreshold) {
	this->maxSignalThreshold = maxEstimatedThreshold;
		this->minSignalThreshold=minEstimatedThreshold;
		this->findThresholds();
}
IWConfig::~IWConfig() {
	// TODO Auto-generated destructor stub
}
int IWConfig::maxAllowableValue=0;
int IWConfig::minAllowableValue=0;
int IWConfig::maxSignalThreshold=128;
int IWConfig::minSignalThreshold=128;
void IWConfig:: findThresholds(){

	int maxValue=0,minValue=0;
		bool maxFound=false;
		for(int i=maxSignalThreshold;minSignalThreshold<i;i--){
			if( IWConfig::isGivenPowerAcceptable(i) &&maxFound==false){
				maxValue=i;
				IWConfig::maxAllowableValue=maxValue;
				maxFound=true;
			}else if(! IWConfig::isGivenPowerAcceptable(i)&& maxFound==true){
				minValue=i+1;
				IWConfig::minAllowableValue=minValue;
				break;
			}
		}


}
/*
 * wlan0     IEEE 802.11bgn  Mode:Master  Tx-Power=26 dBm
          RTS thr:off   Fragment thr:off
          Power Management:on

 */
void IWConfig::scanForNetgear(){
	std::string cmd = "iwconfig "+IWConfig::interface ;
	istringstream infile(Utilities::exec(cmd.c_str()));
	string iface,dontcare,cardType,mode,txpower,rts,frag,powMan;
	while (infile >> iface>>dontcare>>cardType>>mode>>txpower>>dontcare>>dontcare>>rts>>dontcare>>frag>>dontcare>>powMan){
		cout << iface << "\n " << cardType << "\n " << mode<< "\n "<< txpower << endl;
	}
	this->mode=mode;
	this->txpower=Utilities::split(txpower,'=')[1];

}
/**
 *  iwconfig eth1

eth1     IEEE 802.11g  ESSID:"OSU_PUB"
         Mode:Managed  Frequency:2.427 GHz  Access Point: 00:0D:9D:C6:38:2D
         Bit Rate=48 Mb/s   Tx-Power=20 dBm   Sensitivity=8/0
         Retry limit:7   RTS thr:off   Fragment thr:off
         Power Management:off
         Link Quality=91/100  Signal level=-39 dBm  Noise level=-87 dBm
         Rx invalid nwid:0  Rx invalid crypt:860  Rx invalid frag:0
         Tx excessive retries:0  Invalid misc:39   Missed beacon:8
*/
void IWConfig::scan() {
	std::string cmd = "iwconfig "+IWConfig::interface ;
	istringstream infile(Utilities::exec(cmd.c_str()));

	string iface, cardIns, cardType, essid, mode, freq, freqMetric, ac, point,
			mac, bit, bitrate, txpower, dontcare, sens, retryLim, rts, frag,
			powMan, link, linkQua, sig, sigLev, nois, noiseLev;
	while (infile >> iface >> cardIns >> cardType >> essid >> mode >> freq
			>> freqMetric >> ac >> point >> mac >> bit >> bitrate >> dontcare
			>> txpower >> dontcare >> sens >> dontcare >> retryLim >> dontcare
			>> rts >> dontcare >> frag >> dontcare >> powMan >> link >> linkQua
			>> sig >> sigLev >> dontcare >> nois >> noiseLev) {
		cout << iface << "\n " << cardType << "\n " << essid << "\n " << mode
				<< "\n " << freq << "\n " << mac << "\n " << bitrate << "\n "
				<< txpower << "\n " << retryLim << "\n " << link << linkQua
				<< "\n " << sig << sigLev << "\n " << nois << noiseLev << endl;
	}

	this->mode=mode;
	this->mac=mac;
	this->ssid=Utilities::split(essid,':')[1];
	this->freq=Utilities::split(freq,':')[1];
	this->txpower=Utilities::split(txpower,'=')[1];
	this->bitrate=Utilities::split(bitrate,'=')[1];

	this->linkQuality=Utilities::split(linkQua,'=')[1];
	this->signalLevel=Utilities::split(sigLev,'=')[1];
	this->noiseLevel=Utilities::split(noiseLev,'=')[1];

}
string IWConfig::getSNR(){
	int sigLev=Utilities::covertStringToIntC(this->signalLevel);
	int noiseLev=Utilities::covertStringToIntC(this->noiseLevel);
	int res=sigLev-noiseLev;
	return  Utilities::convertIntToString(res);
}
string IWConfig::getSSID(){
	return this->ssid;
}
string IWConfig::getTXPower(){
	return this->txpower;
}
string IWConfig::getBitRate(){
	return this->bitrate;
}
string IWConfig::getRetryLimit(){
	return this->retryLim;
}
string IWConfig::getLinkQaulity(){
	return this->linkQuality;
}
string IWConfig::getSignalLevel(){
	return this->signalLevel;
}
string IWConfig::getNoiseLevel(){
	return this->noiseLevel;
}
//The following two functions do the same thing
void IWConfig::setTransmissionPower(int value){
	std::string s = "iwconfig "+interface+" txpower ";
	s = s + Utilities::convertIntToString(value);
	cout << s << endl;
	system(s.c_str());
}

bool IWConfig::isGivenPowerAcceptable(int& power){

	std::string s = "iwconfig "+interface+" txpower ";
	s = s + Utilities::convertIntToString(power);
	cout << s << endl;
	string result = Utilities::exec(s);
	string str2("Error for wireless request \"Set Tx Power\" (8B26) :");
	size_t found;
	//wlan0
	found = result.find(str2);
	if (found == string::npos) {
		return true;
	} else {
		return false;
	}
}
string IWConfig::readFile(string& fileName) {
	ifstream infile;
	infile.open(fileName.c_str());
	string sLine;
	while (!infile.eof()) {
		getline(infile, sLine);
		cout << sLine << endl;
		sLine += sLine;
	}
	infile.close();
	return sLine;
}
