#ifndef FilePath_H
#define FilePath_H

#include "../Tools/defines.h"

#include <string>
#include <map>
using namespace std;

/**
	File containing a filepath. Very useful to "normalize" a file path.
	FilePath can handle standard paths, and compressed paths (like C:/temp/file.zip/toto/tata.jpeg)

	TODO: add some support for /./ repertories
	Find a way to have the FULL path stored!
	It's full of bugs!
	For example: ../../titi.jpg will be transformed in titi.jpg!
  */
class DllExport KD_FilePath
{
	string fileName;
	string filePath;
	string archiveName;

	map<string, string> parameters;

	/**
		Removes all dir/../
	*/
	static string removeDoubleDot(string path);

	/**
		Removes all ./ (must be called after removeDoubleDot)
	*/
	//static string removeSimpleDot(string path);

public:
	/**
		Default Constructor
	*/
	KD_FilePath();

	/**
		Constructor from a char *
	*/
	KD_FilePath(char *path);

	/**
		Constructor from a string
	*/
	KD_FilePath(const string &path);

	/**
		Copy constructor
	*/
	KD_FilePath(const KD_FilePath &file);

	KD_FilePath& operator = (const KD_FilePath& file);

	bool operator == (const KD_FilePath &file) const;

	bool operator < (const KD_FilePath &file) const;

	/**
	*/
	virtual ~KD_FilePath();

	/**
		Adds a directory before the filePath....
	*/
	void KD_FilePath::PrefixDirectory(const string &path);


	/**
		Adds a parameter to the file
	*/
	void AddParameter(string name, string value);


	/**
		Compute the object from a string.
	*/
	void ComputePath(const string &path);

	/**
		Returns the path of the file (without the archive if there is one).
	*/
	string GetPath() const;

	/**
		Returns the path of the file (with the archive if there is one).
	*/
	string GetFullPath() const;

	/**
		Returns the path of the archive file.
	*/
	string GetArchiveName() const;

	/**
		Returns true if the file is in an archive.
	*/
	bool IsArchived() const;

	/**
		Returns the directory of the file without the archive.
	*/
	string GetDirectory() const;

	/**
		Returns the directory of the file with the archive.
	*/
	string GetFullDirectory() const;

	/**
		Returns the file extension in lower case
	*/
	string GetFileExtension() const;

	/**
		Normalizes the path by removing any dir/../ and ./
	*/
	static string NormalizePath(string path);

	void DebugPrint();
};

#endif