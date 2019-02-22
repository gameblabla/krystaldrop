#ifndef TextFile_H
#define TextFile_H

#include <stdio.h>
#include <stdarg.h>

#ifdef WIN32
#pragma warning(disable:4786)
#endif
#include <string>

using namespace std;

/**
	class used to read text files, whether they are or not in a .ACC file.
  */
class KD_TextFile
{
	/// The text contained in the file
	char *text;
	
	/// The position of the pointer in the file
	int pos;

	/// Size of the file
	int size;

public:
	/**
		Default constructor.
	*/
	KD_TextFile();

	/**
		Opens the textfile named fileName and copies it into memory.
	*/
	KD_TextFile(char *fileName);

	/**
		Opens the textfile named fileName and copies it into memory.
	*/
	bool Load(char *fileName);

	/**
		Destructor, it frees the space allocated for the file.
	*/
	~KD_TextFile();

	/**
		Returns a pointer to the current position in the file.
	*/
	char *getPosition();

	/**
		Goes through the file until we find the character c
		Returns a pointer to that position in the file.
	*/
    char *readNewLine();

	/**
		Goes through the file until we find a return character
		Returns a pointer to the position just after in the file.
	*/
	char *jumpLine();

	/**
		Returns true if the end of File has been reached.
	*/
	bool isEOF();

	/**
		Returns the next string in the file between "..."
	*/
	string getString();
};

#endif
