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

#include <map>
using std::map;

#include <stdlib.h>

#include <cstdint>

typedef uint8_t  CHAR;
typedef uint16_t WORD;
typedef uint32_t DWORD;

typedef int8_t  BYTE;
typedef int16_t SHORT;
typedef int32_t LONG;

typedef LONG INT;
typedef INT BOOL;

map<int, string> blockOrder = 	{{0, "ABCD"}, {1, "ABDC"}, {2, "ACBD"}, {3, "ADBC"}, {4, "ACDB"}, 
										{5, "ADCB"}, {6, "BACD"}, {7, "BADC"}, {8, "CABD"}, {9, "DABC"},
										{10, "CADB"}, {11, "DACB"}, {12, "BCAD"}, {13, "BDAC"}, {14, "CBAD"}, 
										{15, "DBAC"}, {16, "CDAB"}, {17, "DCAB"}, {18, "BCDA"}, {19, "BDCA"},
										{20, "CBDA"}, {21, "DBCA"}, {22, "CDBA"}, {23, "DCBA"}};

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

const int getCheckSum(const vector<WORD> & checksumWords)
{
	int checksum = 0;
	
	for(auto word : checksumWords)
		checksum += word;
	
	return checksum;
}

const vector<WORD> getEncryptedDataWords(const vector<char> & binData)
{
	vector<WORD> encryptedWords;
	bool firstHalfIsInitialized = false;
	WORD myWord = 0;
	for(int i = 0x08; i < 0x87 && i < binData.size(); i++)
	{
		if(firstHalfIsInitialized)
		{
			myWord += binData[i];
			encryptedWords.push_back(myWord);
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
	
	return encryptedWords;
}

const vector<WORD> decryptDataWords(const vector<WORD> encryptedDataWords)
{
	vector<WORD> decryptedDataWords;
	
	for(auto word : encryptedDataWords)
	{
		WORD temp = word xor rand();
		//Switch to little-endian format?
		WORD newWord = ((temp << 8) & (temp >> 8));
		decryptedDataWords.push_back(newWord);
	}
	
	return decryptedDataWords;
}

const vector<char> shuffleBlocks(const vector<WORD> & decryptedDataWords, int shiftValue)
{
	vector<WORD> A, B, C, D;
	vector<char> shuffledBlocks;
	const string blockOrderString = blockOrder[shiftValue];
	
	//Go through first 32 bytes
	for(int i = 0; i < 16; i++)
		A.push_back(decryptedDataWords[i]);
	
	//next 32, etc
	for(int i = 16; i < 32; i++)
		B.push_back(decryptedDataWords[i]);
	
	for(int i = 32; i < 48; i++)
		C.push_back(decryptedDataWords[i]);
	
	for(int i = 48; i < 64; i++)
		D.push_back(decryptedDataWords[i]);
	
	//Put in correct order
	for(auto letter : blockOrderString)
	{
		if(letter == 'A')
			for(auto word : A)
			{
				char temp = ((word << 8) >> 8);
				shuffledBlocks.push_back(temp);
				shuffledBlocks.push_back((word >> 8));
			}
		
		if(letter == 'B')
			for(auto word : B)
			{
				char temp = ((word << 8) >> 8);
				shuffledBlocks.push_back(temp);
				shuffledBlocks.push_back((word >> 8));
			}
		
		if(letter == 'C')
			for(auto word : C)
			{
				char temp = ((word << 8) >> 8);
				shuffledBlocks.push_back(temp);
				shuffledBlocks.push_back((word >> 8));
			}
		
		if(letter == 'D')
			for(auto word : D)
			{
				char temp = ((word << 8) >> 8);
				shuffledBlocks.push_back(temp);
				shuffledBlocks.push_back((word >> 8));
			}
	}
	
	return shuffledBlocks;
}

int main(const int argc, const char * argv[])
{
	vector<char> binData, trueData;
	
	if(argc > 1)
	{
		binData = getBinaryData(argv[1]);
		const WORD personalityValue = getPersonalityValue(binData);
		int shiftValue = ((personalityValue & 0x3E000) >> 0xD) % 24;
		const vector<WORD> encryptedDataWords = getEncryptedDataWords(binData),
			decryptedDataWords = decryptDataWords(encryptedDataWords);
		const int checksum = getCheckSum(encryptedDataWords);
		trueData = shuffleBlocks(decryptedDataWords, shiftValue);
		
		srand(checksum);
		
		cout << argv[1] << " is " << binData.size() << " bytes!" << endl;
		cout << "Personality Value: " << personalityValue << endl;
		cout << "Shift Value: " << shiftValue << endl;
		cout << "Shift Order: '" << blockOrder[shiftValue] << "'" << endl;
		cout << "Checksum: " << checksum << endl;
		cout << "Size of pure data (bytes): " << trueData.size() << endl;
		cout << "Pokemon name: ";
		for(int i = 0x48; i < 0x5D; i++) cout << trueData[i];
		cout << endl;
	}
	
	return 0;
}
