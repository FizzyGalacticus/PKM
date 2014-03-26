//Dustin Dodson
//Test for reading PKM files

#include <fstream>
using std::ifstream;

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <cstdint>

typedef uint8_t  CHAR;
typedef uint16_t WORD;
typedef uint32_t DWORD;

typedef int8_t  BYTE;
typedef int16_t SHORT;
typedef int32_t LONG;

typedef LONG INT;
typedef INT BOOL;

int main(const int argc, const char * argv[])
{
	std::ifstream in;
	vector<char> binData;
	vector<WORD> checksumWords;
	int checksum = 0;
	
	if(argc > 1)
	{
		in.open(argv[1], ifstream::in|ifstream::binary);
		
		if(in)
		{
			char tempChar;
			
			while(!in.eof())
			{
				in.read(&tempChar, sizeof(tempChar));
				binData.push_back(tempChar);
			}
			in.close();
			
			bool firstHalfIsInitialized = false;
			WORD myWord = 0;
			for(int i = 0x08; i < 0x87; i++)
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
					myWord << 8;
					firstHalfIsInitialized = true;
				}
			}
			
			for(int i = 0; i < checksumWords.size(); i++)
				checksum += checksumWords[i];
		}
		else cout << "Failed to open " << argv[1] << " for reading." << endl;
		
		cout << argv[1] << " is " << binData.size() << " bytes!" << endl;
		cout << "Checksum is " << checksum << "!" << endl;
	}
	
	return 0;
}