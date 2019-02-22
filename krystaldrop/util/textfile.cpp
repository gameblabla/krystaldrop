#include "textfile.h"

#include "logfile.h"
#include <stdio.h>
#include <memory.h>
#include <assert.h>

KD_TextFile::KD_TextFile() : pos(0)
{

}

KD_TextFile::KD_TextFile(char *fileName) : pos(0)
{
	Load(fileName);
}

bool KD_TextFile::Load(char *fileName)
{
	char tmp[512];
	FILE *fpt;

	snprintf(tmp, sizeof(tmp), "art/txt/%s", fileName);
	fpt = fopen(tmp,"rb");
	
	if (!fpt) 
	{
		printf("File %s not found!",tmp);
		KD_LogFile::printf("File %s not found!",tmp);
		KD_LogFile::Close();
		assert(fpt);
		return false;
	}

	fseek(fpt, 0, SEEK_END);
	size = ftell(fpt);
	fseek(fpt, 0, SEEK_SET);

	text = new char[size+1];

	fread(text, size*sizeof(char), 1, fpt);
	text[size]=0;

	fclose(fpt);
	return true;
}

KD_TextFile::~KD_TextFile()
{
	delete[] text;
}

char *KD_TextFile::getPosition()
{
	return text+pos;
}

char *KD_TextFile::readNewLine()
{
	while(1)
	{   if (text[pos]== 13 || text[pos]== 10) break;
		if (isEOF()) break;
		pos++;
	}
    
    while( (!isEOF()) && (text[pos]== 13 || text[pos]== 10) ) pos++;

	return text+pos;
}

/**
	Goes to the next valid line (a line containing something else than ' '.
  */
char *KD_TextFile::jumpLine()
{
	readNewLine();
  
/*	pos++;*/
  /*  if (isEOF()) return text+ pos;
	if (text[pos]==10) pos++;
    if (isEOF()) return text+ pos;
*/
	while (1)
	{
		int newpos = pos;

		// If space
		if (text[newpos]==' ')
			newpos++;
		// If new line: go to new line and return;
		else if (text[newpos]== 13 || text[newpos]== 10)
		{
			newpos++;
			if (text[newpos]== 13 || text[newpos]== 10) newpos++;
			pos = newpos;
		}
		// If end of file.
		else if (isEOF()) break;
		// if anything else
		else break;
	}

	return text+pos;
}

bool KD_TextFile::isEOF()
{
	if (pos>=size) return true;
	else return false;
}

string KD_TextFile::getString()
{
	string str="";

	while(1)
	{
		// If ok
		if (text[pos]=='"') break;
		// If new line: go to new line and return;
		if (text[pos]==13)
		{
			pos++;
			if (text[pos]==10) pos++;
			return "";
		}
		// If end of file.
		if (isEOF()) return "";
		pos++;
	}

	if (isEOF()) return "";

	pos++;

	while(1)
	{
		// If ok
		if (text[pos]=='"') break;
		// If new line: go to new line and return;
		if (text[pos]==13)
		{
			pos++;
			if (text[pos]==10) pos++;
			return "";
		}
		// If end of file.
		if (isEOF()) return "";

		str += text[pos];

		pos++;
	}

	pos++;

	return str;
}
