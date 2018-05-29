#include "Control.hpp"


std::string Client::ownerName;

int main() {
	cout << "Program ismertetese" << endl;

	string input;
	cout << "Adjon meg a nevet: " << endl;
	cin >> input;

	Control control(input);

	int nextState = 0; //melyik állapot legyen kiiratva

	while (true) {	//fut amig nem ad exit kódot

		//alap állapot, kiiratni a menüt
		control.getIO().main_menu();

		//bekéri h melyik menu pontot választja ki
		do {
			cout << "Valasztott menu:  " << endl;
			cin >> input;
		} while (!control.getIO().correctInputDigit(input, nextState));

		if (nextState == 0) break;

		switch (nextState) {
		case 1: {
			control.showChat();
			break;
		}
		case 2: {
			control.udpBroadCast();
			break;
		}
		case 3: {
			control.showIPs();
			break;
		}
		case 4: {
			control.connectToPeer();
			break;
		}
		case 5: {
			control.disconnectionFrom();
			break;
		}
		case 6: {
			control.peerErrs();
			break;
		}
		case 7: {
			control.deletePeer();
			break;
		}
		case 8: {

			break;
		}
		default :
			break;
		}

	}

	cout << "Program kilep!" << endl;

	return 0;
}