//Dustin Dodson
//Test for reading PKM files

#include <fstream>
using std::ifstream;

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <cstdint>

typedef uint8_t  CHAR;
typedef uint16_t WORD;
typedef uint32_t DWORD;

typedef int8_t  BYTE;
typedef int16_t SHORT;
typedef int32_t LONG;

typedef LONG INT;
typedef INT BOOL;

const vector<char> getBinaryData(const string & filename)
{
	std::ifstream in;
	vector<char> binData;
	
	in.open(filename.c_str(), ifstream::in|ifstream::binary);
		
	if(in)
	{
		char tempChar;
		
		while(!in.eof())
		{
			in.read(&tempChar, sizeof(tempChar));
			binData.push_back(tempChar);
		}
		in.close();
	}
	else cout << "Failed to open " << filename << " for reading." << endl;
	
	return binData;
}

const WORD getPersonalityValue(const vector<char> & binData)
{
	WORD personalityValue = 0;
	
	if(binData.size())
	{
		personalityValue += binData[0];
		personalityValue = (personalityValue << 8);
		personalityValue += binData[1];
	}
	
	return personalityValue;
}

const int getCheckSum(const vector<char> & binData)
{
	vector<WORD> checksumWords;
	int checksum = 0;
	
	bool firstHalfIsInitialized = false;
	WORD myWord = 0;
	for(int i = 0x08; i < 0x87 && i < binData.size(); i++)
	{
		if(firstHalfIsInitialized)
		{
			myWord += binData[i];
			checksumWords.push_back(myWord);
			myWord = 0;
			firstHalfIsInitialized = false;
		}
		else
		{
			myWord = binData[i];
			myWord = (myWord << 8);
			firstHalfIsInitialized = true;
		}
	}
	
	for(int i = 0; i < checksumWords.size(); i++)
		checksum += checksumWords[i];
	
	return checksum;
}

int main(const int argc, const char * argv[])
{
	vector<char> binData;
	int checksum = 0;
	
	if(argc > 1)
	{
		binData = getBinaryData(argv[1]);
		WORD personalityValue = getPersonalityValue(binData);
		checksum = getCheckSum(binData);
		
		cout << argv[1] << " is " << binData.size() << " bytes!" << endl;
		cout << "Personality Value: " << personalityValue << endl;
		cout << "Checksum: " << checksum << endl;
	}
	
	return 0;
}