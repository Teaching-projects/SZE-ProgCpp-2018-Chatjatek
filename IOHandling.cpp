#include "IOHandling.hpp"



IOHandling::IOHandling()
{
}


IOHandling::~IOHandling()
{
}

void IOHandling::setDigit(int digit)
{
	this->digit = digit;
}

void IOHandling::setMatch(char match)
{
	this->match = match;

}

void IOHandling::screenShifter(int shift) const
{
	cout << string(shift, '\n');
}

void IOHandling::main_menu() const {
	screenShifter(100);
	cout << "1..Chat" << endl;
	cout << "2..Halozat felderitese" << endl;
	cout << "3..Legutobbi felderites eredmenye" << endl;
	cout << "4..Csatlakozas peer-hez" << endl;
	cout << "5..Kapcsolat bontasa peer-el" << endl;
	cout << "6..Peerek hiba uzenetei" << endl;
	cout << "7..Peer kapcsolatok torlese" << endl;
	cout << "8..Jatek kezdemenyezes" << endl;

	cout << "0..Kilepes" << endl;
}

bool IOHandling::correctInputDigit(string &input, int &digit, int min, int max) const {
	try {
		digit = stoi(input);

		if (min > 0 && digit < min)
			throw bad_input_err("Tul kicsi erteket adott meg!");
		if (max > 0 && digit > max)
			throw bad_input_err("Tul nagy erteket adott meg!");
	}
	catch (invalid_argument) {
		cout << "Nem szamot adott meg!" << endl;
		return false;
	}
	catch (out_of_range) {
		cout << "Tul nagy szamot adott meg, tulcsordulas!" << endl;
		return false;
	}
	catch (bad_input_err ex) {
		cout << ex.what() << endl;
		return false;
	}
	return true;
}

bool IOHandling::correctInputChar(string &input, char match) const {
	if (input.length() > 1 || input.length() < 1)
		return false;
	if (input[0] != match)
		return false;

	return true;
}

bool IOHandling::correctInputIP(string &input) const {
	int length = input.length();

	if (length < 7 || length > 15)
		return false;
	if (input[0] == '.')
		return false;
	if (input[length - 1] == '.')
		return false;

	string tmp = "";
	int dotNmb = 0;

	for (int i = 0; i < length; i++) {
		if ((input[i] < '0' || input[i] > '9') && input[i] != '.')
			return false;

		tmp += input[i];

		if (input[i] == '.') {
			tmp = "";
			dotNmb++;
		}

		if (tmp.length() > 3 || dotNmb > 3)
			return false;

	}

	if (dotNmb != 3)
		return false;

	return true;
}

void IOHandling::waitToUserInteraction() const {
	cout << "Type y to continue: " << endl;
	string in;
	do {
		cin >> in;
	} while (!correctInputChar(in, 'y'));//y
}

//void IOHandling::getUserInput(bool (*check) (string&), bool exitCodeEnabled) {
//	string in;
//	do {
//		cin >> in;
//		if (in.length() == 1 && in[0] == '0')
//			return;
//	} while ((*check)(in));
//}
