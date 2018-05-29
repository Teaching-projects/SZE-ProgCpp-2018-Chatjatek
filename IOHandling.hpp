#pragma once
#include "Exceptions.h"
#include <string>
#include <iostream>

using namespace std;

//typedef bool(*InputCheck)(string &input);

class IOHandling
{
private:
	int digit;
	char match;
public:
	IOHandling();
	~IOHandling();

	void setDigit(int digit);
	void setMatch(char match);

	void screenShifter(int shift) const;

	void main_menu() const;

	//void getUserInput(bool (*check) (string&), bool exitCodeEnabled = false);

	bool correctInputDigit(string &input, int &digit, int min = -1, int max = -1) const;

	bool correctInputChar(string &input, char match) const;

	bool correctInputIP(string &input) const;

	void waitToUserInteraction() const;
};

