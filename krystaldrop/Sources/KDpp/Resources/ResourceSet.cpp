#include "ResourceSet.h"

#include <assert.h>

#include "ResourceManager.h"
#include "../Tools/Textfile.h"
#include "../Tools/Logfile.h"
#include "XMLResourceParser.h"


KD_ResourceSet::KD_ResourceSet()
{

}

KD_ResourceSet::~KD_ResourceSet()
{

}

KD_Resource *KD_ResourceSet::GetResource(const string &name)
{
	return KD_ResourceManager::GetResourceManager()->GetResource(files[name]);
}

void KD_ResourceSet::ReleaseResource(const string &name)
{
	assert(IsDefined(name));
	KD_ResourceManager::GetResourceManager()->ReleaseResource(files[name]);
}

void KD_ResourceSet::ReleaseResource(KD_Resource *resource)
{
	KD_ResourceManager::GetResourceManager()->ReleaseResource(resource);
}

bool KD_ResourceSet::IsDefined(const string &name) const
{
	if (files.find(name) == files.end())
		return false;
	else
		return true;
}

bool KD_ResourceSet::RegisterResource(string resourceName, KD_FilePath fileName)
{
	if (IsDefined(resourceName))
		return false;

	files[resourceName] = fileName;
	return true;
}

bool KD_ResourceSet::RegisterResource(string resourceName, KD_Resource *resource)
{
	if (IsDefined(resourceName))
		return false;

	KD_ResourceManager *resManager = KD_ResourceManager::GetResourceManager();
	
	char buf[1000];
	sprintf(buf, "\"undefined_%s_%d\"", resourceName.c_str(), resManager->GetUndefinedCount());

	/*string pseudoFileName = "\"undefined_"+resourceName;

	pseudoFileName.append("_");
	pseudoFileName += resManager->GetUndefinedCount();
	pseudoFileName.append("\"");*/
	string pseudoFileName = buf;

	files[resourceName] = pseudoFileName;
	
	resManager->resources[pseudoFileName] = resource;

	resManager->IncreaseUndefinedCount();
    
	return true;
}

bool KD_ResourceSet::UnRegisterResource(string resourceName)
{
	if (files.find(resourceName) == files.end())
		return false;
	
	files.erase(resourceName);
	return true;
}

KD_FilePath KD_ResourceSet::GetFileName(string resourceName)
{
	assert(IsDefined(resourceName)); 
	return files[resourceName];
}

/*bool KD_ResourceSet::LoadResourceFile(const KD_FilePath &fileName)
{
	KD_TextFile file;

	bool res = file.Load(fileName);
	
	if (res == false)
	{
		KD_LogFile::printf2("ResourceSet.cpp: Warning, could not load text file \"%s\"\n",fileName.GetFullPath().c_str());
		return false;
	}

	int i=0;

	while (!file.IsEOF())
	{
		string resourceName = file.GetString();
		string resourceFileName = file.GetString();
		
		if (resourceName == "" || resourceFileName == "")
		{
			KD_LogFile::printf2("ResourceSet.cpp: Warning, line %d invalid in text file \"%s\"\n",i,fileName.GetFullPath().c_str());
		}

		files[resourceName] = fileName.GetDirectory() + resourceFileName;
		i++;
	}
	return true;
}*/

bool KD_ResourceSet::LoadResourceFile(const KD_FilePath &fileName)
{
	KD_XMLResourceParser Parser = KD_XMLResourceParser(fileName, this);
	return Parser.Parse();
}