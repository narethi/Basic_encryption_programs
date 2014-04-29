#ifndef Shared_Key_Sin_Cos_Multiplier
#define Shared_Key_Sin_Cos_Multiplier

#include <iostream>
#include <GL\freeglut.h>
#include <string>
#include <string.h>
#define VERSION_NUMBER "Version 0.0.0"
using namespace std;

int encrypt(string passcode, string fileName)
{
	//open the file in the read byte mode
	FILE *working_file = NULL;
	fopen_s(&working_file, &fileName[0], "rb");
	//if the file doesn't exist exit the function
	if(!working_file)
	{
		printf_s("Error the file does not exist");
		return -1;
	}
	//move the file pointer to the end, and take the size of the file
	fseek(working_file, 0, SEEK_END);
	int fileSize = ftell(working_file), offset = 0;
	rewind(working_file);
	char* file_contents = (char*)malloc(sizeof(char)*(fileSize + passcode.size() + 2));
	char* currentChar = (char*)malloc(sizeof(char));
	char* header = "|___|CYP|___|Encryption|___|\n";
	char* footer = "\n|||||CYP END;";
	//Read the file contents into file_contents then close the file
	//and insert the verification passcode into the file
	for(int i = 0; i < fileSize + passcode.size() + 2; i++)
	{
		if(i == (passcode[0] + passcode[1]) || 
		  (i == fileSize - 1 && 
		  (passcode[0] + passcode[1]) >= fileSize))
		{
			file_contents[i] = passcode.size();
			offset++;
			for(int j = 0;  j < passcode.size(); j++)
				file_contents[i + j + 1] = passcode[j];
			offset+=passcode.size();
			file_contents[i + offset] = passcode.size();
			offset++;
		}
		fread_s(currentChar, sizeof(char), sizeof(char), 1, working_file);
		file_contents[i + offset] = currentChar[0];
	}
	fclose(working_file);

    /*
	then to shift all of the bits of each char
	then to add in the header and the footer to file and close it
	then remove the original from the computer*/
	//This is where all of the chars are flipped
	double sum = 1;
	for(int j = 0; j < fileSize + passcode.size() + 2; j++)
	{
		for(int i = 0; i < passcode.size(); i++)
			sum *= int(sin(passcode[i]*pow(j,i))*256);
		sum = (int((cos(sum) - sin(sum))*256)%256);
		file_contents[j] += sum;
		sum = 1;
	}
	//here is where we load up the new file and right to it
	fileName+=".cyp";
	fopen_s(&working_file, &fileName[0], "wb");
	fprintf(working_file, header);
	for(int i = 0; i < fileSize + passcode.size() + 2; i++)
		fprintf(working_file, "%c", file_contents[i]);
	fprintf(working_file, footer);
	fclose(working_file);
	for(int i = fileName.size(); i > fileName.size() - 4; i--)
		fileName[i] = '\0';
	remove(&fileName[0]);
	return 0;
}

int decrypt(string passcode, string fileName)
{
	//open the file in the read byte mode
	FILE *working_file = NULL;
	
	string extenstion_check = "";
	//Check the extenstion
	for(int i = fileName.size() - 1; i > fileName.size() - 5; i--)
		extenstion_check+=fileName[i];
	if(extenstion_check != "pyc.")
	{
		printf("This file does not have a .cyp extension, exiting the program");
		return -1;
	}
	
	fopen_s(&working_file, &fileName[0], "rb");
	//if the file doesn't exist exit the function
	if(!working_file)
	{
		printf_s("Error the file does not exist");
		return -1;
	}
	//move the file pointer to the end, and take the size of the file
	fseek(working_file, 0, SEEK_END);
	int fileSize = ftell(working_file), offset = 0;
	rewind(working_file);

	char* header = "|___|CYP|___|Encryption|___|\n";
	char* footer = "\n|||||CYP END;";

	//collect all of the file contents
	char* read_file = (char*)malloc(sizeof(char)*fileSize);
	fread_s(read_file, sizeof(char)*fileSize, sizeof(char), fileSize, working_file);
	fclose(working_file);
	//Next check if the header is at the top of the file
	for(int i = 0; i < string(header).size(); i++) 
	{
		if(read_file[i] != header[i])
		{
			printf("This is not a cyp file format file");
			return -1;
		}
	}

	//Next check to see if the footer exists
	for(int i = string(footer).size(); i--;) 
	{
		if(read_file[fileSize - string(footer).size() + i] != footer[i])
		{
			printf("This is not a cyp file format file");
			return -1;
		}
	}
	//Next remove the header and the footer and flip all of the chars
	//back to where the original positions are supposed to be
	int search_space = fileSize;
	search_space -= string(footer).size();
	search_space -= string(header).size();
	char* original_file = (char*)malloc(sizeof(char)*search_space);
	double sum = 1;
	for(int i = 0; i < search_space; i++)
	{
		for(int j = 0; j < passcode.size(); j++)
			sum *= int(sin(passcode[j]*pow(i,j))*256);
		sum = (int((cos(sum) - sin(sum))*256)%256);
		original_file[i] = read_file[i + string(header).size()] - sum;
		sum = 1;
	}
	//Now find the position that the passcode is at
	int pos = 0;
	char* new_passcode = (char*)malloc(sizeof(char)*(passcode.size() + 2));
	//This sets up the new passcode to discover it
	new_passcode[0] = passcode.size();
	new_passcode[passcode.size() + 1] = passcode.size();
	for(int i = 1; i < passcode.size() + 1; i++)
		new_passcode[i] = passcode[i - 1];

	//This will get the position
	for(int i = 0; i < search_space; i++)
	{
		if(original_file[i] == new_passcode[0])
		{
			for(int j = 0; j < passcode.size() + 2; j++)
			{
				if(original_file[i + j] != new_passcode[j]) break;
				else if(j == passcode.size() + 1) pos = i;
			}
		}
		if(i == search_space - 1 && pos == 0)
		{
			printf("This is an invalid passcode");
			return -1;
		}
	}
	//now remove the passcode and rerotate the file contents
	char* removed_passcode = (char*)malloc(sizeof(char)*(search_space - passcode.size() - 2));
	offset = 0;
	for(int i = 0; i < search_space - passcode.size() - 2; i++)
	{
		if(i == pos) offset = passcode.size() + 2;
		removed_passcode[i] = original_file[i + offset];
	}
	for(int i = fileName.size(); i > fileName.size() - 4; i--)
		fileName[i] = '\0';
	fopen_s(&working_file, &fileName[0], "wb");
	for(int i = 0; i < search_space - passcode.size() - 2; i++)
		fprintf(working_file, "%c", removed_passcode[i]);
	fclose(working_file);
	fileName[fileName.size() - 1] = 'p';
	fileName[fileName.size() - 2] = 'y';
	fileName[fileName.size() - 3] = 'c';
	fileName[fileName.size() - 4] = '.';
	remove(&fileName[0]);
	return 0;
}

/*
This is the way the encryption will work
during compression phase
passcode[0] + passcode[1] 
will determine where the verification code is placed

passcode[1] + passcode[n-1] will determine the string
shift initially

passcode[0] + passcode[n] will determine the shift back
after the char flips

after compression phase
header of the file will give
|___|CYP|___|Encryption|___|
return line
date of encryption in the form
char[0] = day (represented in the integer value)
char[1] = month (represented in the integer value)
char[2] + char[3] = year (represented in integer value base 256)

end of file marked by CYP;
*/
void main(int argc, char* argv[])
{
	string fileName;
	string passcode;
	cout << "Would you like to (E)encrypt or (D)decrypt a file? (enter E or D): ";
	cin >> fileName;
	switch(fileName[0])
	{
	case 'e':
	case 'E':
		cout << endl << "please enter the name of the file you want to encrypt: ";
		cin >> fileName;
		cout << endl << "please enter the encryption passcode: ";
		cin >> passcode;
		cout << "encryption in progress";
		encrypt(passcode, fileName);
		break;
	case 'd':
	case 'D':
		cout << endl << "please enter the name of the file you want to decrypt: ";
		cin >> fileName;
		cout << endl << "please enter the encryption passcode: ";
		cin >> passcode;
		cout << "decryption in progress";
		decrypt(passcode, fileName);
		break;
	default:
		cout << "That is and invalid option";
	}
	return;
}

#endif