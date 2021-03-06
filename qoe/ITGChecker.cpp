/*
 * ITGChecker.cpp
 *
 *  Created on: 10.08.2012
 *      Author: cem
 */

#include "ITGChecker.h"
//#include "Utilities.h"
#include <string>
#include <iostream>
#include "BlackBoard.h"
#include <stdlib.h>
//#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "PracticalSocket.h"

ITGChecker::ITGChecker(){}
ITGChecker::~ITGChecker(){}

void* ITGChecker::run(void* object) {
	using namespace std;
	printf("ITG: started \n");
	ITGChecker *itg=(ITGChecker*)object;
	//int oneSecond = 1000000;
	//system("sh $(pwd)/ITG.sh");
	/*
	 * IPERF module is added here for mobile user
	 * IPERF works in the collaboration with Smart Network Manager designed for Android Mobile Users.
	 */
	system("iperf -s &");
	/*
	 * ITG (Internet Traffic Generator), in contrast to IPERF, is implemented for only Linux CNM Users
	 * It functions Linux PC. It is possible to utilize, of course, IPERF in Linux PC, however the provided information
	 * from ITG is more than IPERF.
	 */
	system("killall ITGRecv");

	while(true){
		try{
			//start itg
			system("ITGRecv &");
			sleep(1200);
			system("killall ITGRecv");
			BlackBoard::writeConsole("ITG Checker: Restarting the ITG");
		}catch (SocketException& e) {
			std::cerr <<"ITG Checker:" <<e.what() << std::endl;
			ITGChecker::run(object);
		}
		catch (std::exception& e) {
			std::cerr <<"ITG Checker:" <<e.what() << std::endl;
			ITGChecker::run(object);
		}
	}

}
