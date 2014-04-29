#ifndef BASIC_CAESAR_CIPHER
#define BASIC_CAESAR_CIPHER

#include <iostream>
#include <string>

using namespace std;

void main(void)
{
	printf("Please enter the name of the file you want to encrypt: ");
	char name[256], single_char[1], *file_content;
	int size = 0, otherSize;
	FILE *temp = NULL;
	cin >> name;
	printf(name);
	fopen_s(&temp, name, "rb");
	if(!temp) return;
	else printf("HOORAY!");
	fseek(temp, 0, SEEK_END);
	size = ftell(temp);
	rewind(temp);
	file_content = (char*)malloc(sizeof(char)*size);
	for(int i = 0; i < size; i++)
	{
		fread_s(single_char, sizeof(char), sizeof(char), 1, temp);
		if(single_char[0] == 254) single_char[0] = 0;
		else if(single_char[0] == 255) single_char[0] = 1;
		else single_char[0] += 2;
		file_content[i] = (single_char[0]);
	}
	fclose(temp);
	fopen_s(&temp, name, "wb");
	for(int i = 0; i < size; i ++)
	{
		fprintf(temp, "%c", file_content[i]);
	}
	fclose(temp);
	return ;
}

#endif