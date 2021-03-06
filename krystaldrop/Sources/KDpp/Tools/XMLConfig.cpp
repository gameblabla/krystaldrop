#include <assert.h>

#include "XMLConfig.h"
#include "Logfile.h"

#ifdef WIN32
#define snprintf _snprintf
#endif

KD_XMLConfig::KD_XMLConfig()
{
    configDocument = 0;
    rootNode = 0;
}

KD_XMLConfig::~KD_XMLConfig()
{
    Unload();
}

bool KD_XMLConfig::Load()
{
    char user_file[128];
#ifndef WIN32
    /* find the user's home, the UNIX way */
    char* home= getenv("HOME");
#else
    /* find the user's home, the Windoze way */
    /* Document Settings\username\ ? */
    // Pfff too hard under windows: what the f... name of My Documents in Polish for example? hu?
    // k: if you do not know, i wont :p
    //string hometemp = getenv("USERPROFILE");
    char* home= ".";
#endif

    if (home != NULL)
    {
        // try $(HOME)/kdrop.xml
        snprintf (user_file, 128, "%s/kdrop.xml", home);
        user_file[127]= 0;
        if (Load(user_file)== true)
            return true;

        // try $(HOME)/.kdroprc
        snprintf (user_file, 128, "%s/.kdroprc", home);
        user_file[127]= 0;
        if (Load(user_file)== true)
            return true;
    }

    // try ./kdrop.xml
    if (Load("kdrop.xml")==true)
        return true;

#ifndef WIN32
    /* for UNIXes system, try CFGDIR/kdrop.xml */
    snprintf (user_file, 128, "%s/kdrop.xml", CFGDIR);
    user_file[127]= 0;
    if (Load(user_file)== true)
        return true;
#endif

    // everything failed :(
    return false;
}

bool KD_XMLConfig::Load(const KD_FilePath &xmlFileName)
{
    // First, try to open the file
    FILE *fpt = fopen (xmlFileName.GetFullPath().c_str(), "r");
    if (fpt == NULL)
      return false;
    else
      fclose(fpt);

    // ok we are here so the file does exist.
    // let's open it with a real URI
    configDocument = xmlParseFile(xmlFileName.GetFullPath().c_str());

    if (configDocument == NULL)
        return false;

    rootNode = xmlDocGetRootElement(configDocument);

    if (rootNode == NULL)
    {
        KD_LogFile::printf2("Warning! Could not parse config file %s: no root node in XML document.\n",xmlFileName.GetFullPath().c_str());
        xmlFreeDoc(configDocument);
        return false;
    }

    if (xmlStrcmp(rootNode->name, (const xmlChar *) "config"))
    {
        KD_LogFile::printf2("Warning! Root node of file %s is not of type \"config\".\nTrying to continue parsing.\n",xmlFileName.GetFullPath().c_str());
    }

    return true;
}

bool KD_XMLConfig::Unload()
{
    if (configDocument)
    {
        xmlFreeDoc(configDocument);
        configDocument=0;
    }
    return true;
}

xmlDocPtr KD_XMLConfig::GetConfigDocument() const
{
    return configDocument;
}

xmlNodePtr KD_XMLConfig::GetRootNode() const
{
    return rootNode;
}

xmlNodePtr KD_XMLConfig::FindFirstByName(xmlNodePtr node, string name)
{
    assert(node);

    xmlNodePtr child;
    child = node->children;
    while (child != NULL)
    {
        if ((!xmlStrcmp(child->name, (const xmlChar *)name.c_str())))
        {
            return child;
        }

        child = child->next;
    }
    return NULL;
}

string KD_XMLConfig::GetNodeText(xmlNodePtr node) const
{
    xmlChar *key = xmlNodeListGetString(configDocument, node->xmlChildrenNode, 1);
    string txt = (char *)key;
    xmlFree(key);
    return txt;
}

string KD_XMLConfig::GetAttributeFromNode(xmlNodePtr xmlnode, const string &name) const
{
    xmlAttrPtr prop = xmlnode->properties;

    for (; prop!=0; prop = prop->next)
    {
        if (xmlStrEqual(prop->name, reinterpret_cast<const xmlChar*>(name.c_str())))
        {
            xmlChar *key = xmlNodeListGetString(configDocument, prop->xmlChildrenNode, 1);
            string txt = (char *)key;
            xmlFree(key);
            return txt;
        }
    }
    return "";
}

string KD_XMLConfig::GetConfigurationString (string config_item) const
{
    xmlNodePtr pCurrentNode = rootNode;
    while (pCurrentNode)
    {
        string::size_type i = config_item.find('/');
        if (i == string::npos)
        {
            xmlNodePtr p = FindFirstByName (pCurrentNode, config_item);
            GetNodeText (p);
            return GetNodeText(FindFirstByName(pCurrentNode, config_item));
        }

        // the slash is ignored if it is the first character
        if (i)
        {
            // update the XML node
            string prefix = config_item.substr (0, i);
            pCurrentNode = FindFirstByName(pCurrentNode, prefix);
            config_item = config_item.substr (i+ 1, config_item.length());
        }
    }

    return "";
}

bool KD_XMLConfig::SetAttributeFromNode(xmlNodePtr xmlnode, const string &name, const string &value)
{
    xmlSetProp(xmlnode, reinterpret_cast<const xmlChar*>(name.c_str()) , reinterpret_cast<const xmlChar*>(value.c_str()));

    return true;
}

bool KD_XMLConfig::Save(const KD_FilePath &xmlFileName)
{
    if (!xmlFileName.IsArchived())
    {
        xmlSaveFormatFile (xmlFileName.GetFullPath().c_str(), configDocument, 1);
        return true;
    }
    else
        return false;
}
