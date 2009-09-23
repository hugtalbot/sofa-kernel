/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <string>
#include <typeinfo>
#include <stdlib.h>
#include <sofa/simulation/common/xml/XML.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/SetDirectory.h>
#include <sofa/core/ObjectFactory.h>
#include <string.h>

/* For loading the scene */


namespace sofa
{

namespace simulation
{

namespace xml
{

using std::cout;
using std::endl;

#define is(n1, n2) (! xmlStrcmp((const xmlChar*)n1,(const xmlChar*)n2))
#define getProp(n) ( xmlGetProp(cur, (const xmlChar*)n) )

void recReplaceAttribute(BaseElement* node, const char* attr, const char* value, const char* nodename=NULL)
{
    if (nodename)
    {
        if (node->getName() == nodename)
        {
            node->addReplaceAttribute(attr,value);
        }
    }
    else
    {
        node->addReplaceAttribute(attr,value);
    }
    BaseElement::child_iterator<> it = node->begin();
    BaseElement::child_iterator<> end = node->end();
    while (it != end)
    {
        recReplaceAttribute( it, attr, value, nodename );
        ++it;
    }
}

#ifdef SOFA_XML_PARSER_TINYXML

BaseElement* includeNode  (TiXmlNode* root,const char *basefilename);
BaseElement* attributeNode(TiXmlNode* root,const char *basefilename);

int numDefault=0;

BaseElement* createNode(TiXmlNode* root, const char *basefilename, bool isRoot = false)
{
    //if (!xmlStrcmp(root->name,(const xmlChar*)"text")) return NULL;
    if (root->Type() != TiXmlNode::ELEMENT) return NULL;
    TiXmlElement* element = root->ToElement();
    if (!element || !element->Value() || !element->Value()[0]) return NULL;

    // handle special 'preprocessor' tags

    if (std::string(element->Value())=="include")
    {
        return includeNode(root, basefilename);
    }

    std::string classType,name, type;

    classType = element->Value();

    const char* pname = element->Attribute("name");

    if (pname != NULL)
    {
        name = pname;
    }
    else
    {
        name = "default";
// 		static int num = 0;
        char buf[16];
        sprintf(buf, "%d", numDefault);
        ++numDefault;
        name += buf;
    }

    const char* ptype = element->Attribute("type");

    if (ptype != NULL)
    {
        type = ptype;
    }
    else
    {
        type = "default";
    }

    if (!BaseElement::NodeFactory::HasKey(classType) && type == "default")
    {
        type=classType;
        classType="Object";
    }
    if (classType == "Object" && !sofa::core::ObjectFactory::HasCreator(type))
    {
        // look if we have a replacement XML for this type
        std::string filename = "Objects/";
        filename += type;
        filename += ".xml";

        if (sofa::helper::system::DataRepository.findFileFromFile(filename, basefilename, NULL))
        {
            // we found a replacement xml
            element->SetAttribute("href",filename.c_str());
            element->RemoveAttribute("type");
            return includeNode(root, basefilename);
        }
    }
    BaseElement* node = BaseElement::Create(classType,name,type);
    if (node==NULL)
    {
        std::cerr << "Node "<<element->Value()<<" name "<<name<<" type "<<type<<" creation failed.\n";
        return NULL;
    }

    if (isRoot)
        node->setBaseFile( basefilename );

    //std::cout << "Node "<<root->name<<" name "<<name<<" type "<<type<<" created.\n";

    // List attributes
    for (TiXmlAttribute* attr=element->FirstAttribute(); attr ; attr = attr->Next())
    {
        //std::cout << "attr->Value " <<  attr->Value() << std::endl;
        if (attr->Value()==NULL) continue;
        if (!(strcmp(attr->Name(), "name"))) continue;
        if (!(strcmp(attr->Name(), "type"))) continue;
        node->setAttribute(attr->Name(), attr->Value());
    }

    for (TiXmlNode* child = root->FirstChild() ; child != NULL; child = child->NextSibling())
    {
        BaseElement* childnode = createNode(child, basefilename);
        if (childnode != NULL)
        {
            //  if the current node is an included node, with the special name Group, we only add the objects.
            if (childnode->isGroupType())
            {
                BaseElement::child_iterator<> it(childnode->begin());
                for(; it!=childnode->end(); ++it) {node->addChild(it); childnode->removeChild(it);}
            }
            else
            {
                if (!node->addChild(childnode))
                {
                    std::cerr << "Node "<<childnode->getClass()<<" name "<<childnode->getName()<<" type "<<childnode->getType()
                            <<" cannot be a child of node "<<node->getClass()<<" name "<<node->getName()<<" type "<<node->getType()<<std::endl;
                    delete childnode;
                }
            }
        }
    }
    return node;
}

static void dumpNode(BaseElement* node, std::string prefix0="==", std::string prefix="  ")
{
    std::cout << prefix0;
    std::cout << node->getClass()<<" name "<<node->getName()<<" type "<<node->getType()<<std::endl;
    BaseElement::child_iterator<> it = node->begin();
    BaseElement::child_iterator<> end = node->end();
    while (it != end)
    {
        BaseElement::child_iterator<> next = it;
        ++next;
        if (next==end) dumpNode(it, prefix+"\\-", prefix+"  ");
        else           dumpNode(it, prefix+"+-", prefix+"| ");
        it = next;
    }
}

BaseElement* processXMLLoading(const char *filename, const TiXmlDocument &doc)
{
    const TiXmlElement* hRoot = doc.RootElement();

    if (hRoot == NULL)
    {
        std::cerr << "empty document" << std::endl;
        return NULL;
    }

    //std::cout << "Creating XML graph"<<std::endl;
    std::string basefilename =
        sofa::helper::system::SetDirectory::GetRelativeFromDir(filename,sofa::helper::system::SetDirectory::GetCurrentDir().c_str());
    BaseElement* graph = createNode((TiXmlElement*)hRoot, basefilename.c_str(), true);
    //std::cout << "XML Graph created"<<std::endl;
    //xmlFreeDoc(doc);
    //xmlCleanupParser();
    //xmlMemoryDump();

    if (graph == NULL)
    {
        std::cerr << "XML Graph creation failed."<<std::endl;
        return NULL;
    }

    //print the graph scene
    //dumpNode(graph);

    return graph;
}

BaseElement* loadFromMemory(const char *filename, const char *data, unsigned int /*size*/ )
{


    TiXmlDocument doc; // the resulting document tree

    //xmlSubstituteEntitiesDefault(1);

    if (!(doc.Parse(data)))
    {
        std::cerr << "Failed to open " << filename << "\n" << doc.ErrorDesc() << " at line " << doc.ErrorRow() << " row " << doc.ErrorCol() << std::endl;
        return NULL;
    }
    return processXMLLoading(filename, doc);
}

BaseElement* loadFromFile(const char *filename)
{
    //
    // this initialize the library and check potential ABI mismatches
    // between the version it was compiled for and the actual shared
    // library used.
    //

    TiXmlDocument doc; // the resulting document tree

    // xmlSubstituteEntitiesDefault(1);

    if (!(doc.LoadFile(filename)))
    {
        std::cerr << "Failed to open " << filename << "\n" << doc.ErrorDesc() << " at line " << doc.ErrorRow() << " row " << doc.ErrorCol() << std::endl;
        return NULL;
    }
    return processXMLLoading(filename, doc);

}


BaseElement* includeNode(TiXmlNode* root,const char *basefilename)
{
    //if (root->Type != TiXmlNode::NodeType::ELEMENT) return NULL;
    TiXmlElement* element = root->ToElement();
    if (!element) return NULL;

    std::string filename;
    const char *pfilename = element->Attribute("href");
    if (pfilename)
    {
        filename = (const char*)pfilename;
    }
    if (filename.empty())
    {
        std::cerr << "ERROR: xml include tag requires non empty filename or href attribute." << std::endl;
        return NULL;
    }
    /*  std::cout << "XML: Including external file " << filename << " from " << basefilename << std::endl;*/
    sofa::helper::system::DataRepository.findFileFromFile(filename, basefilename);
    TiXmlDocument doc; // the resulting document tree
    if (!doc.LoadFile(filename.c_str()))
    {
        std::cerr << "ERROR: Failed to parse " << filename << std::endl;
        return NULL;
    }
    TiXmlElement* newroot = doc.RootElement();

    if (newroot == NULL)
    {
        std::cerr << "ERROR: empty document in " << filename << std::endl;
        //xmlFreeDoc(doc);
        return NULL;
    }
    BaseElement* result = createNode(newroot, filename.c_str(), true);
    if (result)
    {
        if (result->getName() == "Group") result->setGroupType(true);
        // Copy attributes
        for (TiXmlAttribute* attr=element->FirstAttribute(); attr != NULL ; attr = attr->Next())
        {
            if (attr->Value()==NULL) continue;
            if (!(strcmp(attr->Name(), "href"))) continue;
            if (!(strcmp(attr->Name(), "name")))
            {
                if(!(strcmp(attr->Value(),"Group")))
                    result->setGroupType(true);
                else
                    result->setGroupType(false);

                if (!result->isGroupType()) result->setName(attr->Value());
            }
            else
            {
                const char* attrname = attr->Name();

                const char* value = attr->Value();
                if (const char* sep = strstr(attrname,"__"))
                {
                    // replace attribute in nodes with a given name
                    std::string nodename(attrname, sep);
                    recReplaceAttribute(result, sep+2, value, nodename.c_str());
                }
                else
                {
                    // replace attribute in all nodes already containing it
                    recReplaceAttribute(result, attrname, value);
                }
            }
        }
    }
    //xmlFreeDoc(doc);
    return result;
}
#endif // SOFA_XML_PARSER_TINYXML

#ifdef SOFA_XML_PARSER_LIBXML
BaseElement* includeNode  (xmlNodePtr root,const char *basefilename);
BaseElement* attributeNode(xmlNodePtr root,const char *basefilename);


int numDefault=0;

BaseElement* createNode(xmlNodePtr root, const char *basefilename, bool isRoot = false)
{
    //if (!xmlStrcmp(root->name,(const xmlChar*)"text")) return NULL;
    if (root->type != XML_ELEMENT_NODE) return NULL;

    // handle special 'preprocessor' tags

    if (!xmlStrcmp(root->name,(const xmlChar*)"include"))
    {
        return includeNode(root, basefilename);
    }

    std::string classType,name, type;

    xmlChar *pname = xmlGetProp(root, (const xmlChar*) "name");
    xmlChar *ptype = xmlGetProp(root, (const xmlChar*) "type");
    classType=(const char*)root->name;
    if (pname != NULL)
    {
        name = (const char*)pname;
        xmlFree(pname);
    }
    else
    {
        name = "default";
// 		static int num = 0;
        char buf[16];
        sprintf(buf, "%d", numDefault);
        ++numDefault;
        name += buf;
    }
    if (ptype != NULL)
    {
        type = (const char*)ptype;
        xmlFree(ptype);
    }
    else
    {
        type = "default";
    }
    BaseElement* node = BaseElement::Create(classType,name,type);
    if (node == NULL)
    {
        type=classType;
        classType="Object";
        node = BaseElement::Create(classType,name,type);
        if (node==NULL)
        {
            std::cerr << "Node "<<root->name<<" name "<<name<<" type "<<type<<" creation failed.\n";
            return NULL;
        }
    }

    if (isRoot)
        node->setBaseFile( basefilename );

    //std::cout << "Node "<<root->name<<" name "<<name<<" type "<<type<<" created.\n";

    // List attributes
    for (xmlAttrPtr attr = root->properties; attr!=NULL; attr = attr->next)
    {
        if (attr->children==NULL) continue;
        if (!xmlStrcmp(attr->name,(const xmlChar*)"name")) continue;
        if (!xmlStrcmp(attr->name,(const xmlChar*)"type")) continue;
        node->setAttribute((const char*)attr->name, (const char*)attr->children->content);
    }

    for (xmlNodePtr child = root->xmlChildrenNode; child != NULL; child = child->next)
    {
        BaseElement* childnode = createNode(child, basefilename);
        if (childnode != NULL)
        {
            //  if the current node is an included node, with the special name Group, we only add the objects.
            if (childnode->isGroupType())
            {
                BaseElement::child_iterator<> it(childnode->begin());
                for(; it!=childnode->end(); ++it) {node->addChild(it); childnode->removeChild(it);}
            }
            else
            {
                if (!node->addChild(childnode))
                {
                    std::cerr << "Node "<<childnode->getClass()<<" name "<<childnode->getName()<<" type "<<childnode->getType()
                            <<" cannot be a child of node "<<node->getClass()<<" name "<<node->getName()<<" type "<<node->getType()<<std::endl;
                    delete childnode;
                }
            }
        }
    }
    return node;
}

static void dumpNode(BaseElement* node, std::string prefix0="==", std::string prefix="  ")
{
    std::cout << prefix0;
    std::cout << node->getClass()<<" name "<<node->getName()<<" type "<<node->getType()<<std::endl;
    BaseElement::child_iterator<> it = node->begin();
    BaseElement::child_iterator<> end = node->end();
    while (it != end)
    {
        BaseElement::child_iterator<> next = it;
        ++next;
        if (next==end) dumpNode(it, prefix+"\\-", prefix+"  ");
        else           dumpNode(it, prefix+"+-", prefix+"| ");
        it = next;
    }
}

BaseElement* processXMLLoading(const char *filename, const xmlDocPtr &doc)
{
    xmlNodePtr root;

    if (doc == NULL)
    {
        std::cerr << "Failed to open " << filename << std::endl;
        return NULL;
    }

    root = xmlDocGetRootElement(doc);

    if (root == NULL)
    {
        std::cerr << "empty document" << std::endl;
        xmlFreeDoc(doc);
        return NULL;
    }

    //std::cout << "Creating XML graph"<<std::endl;
    std::string basefilename =
        sofa::helper::system::SetDirectory::GetRelativeFromDir(filename,sofa::helper::system::SetDirectory::GetCurrentDir().c_str());
    BaseElement* graph = createNode(root, basefilename.c_str(), true);
    //std::cout << "XML Graph created"<<std::endl;
    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();

    if (graph == NULL)
    {
        std::cerr << "XML Graph creation failed."<<std::endl;
        return NULL;
    }

    //print the graph scene
    //dumpNode(graph);

    return graph;
}

BaseElement* loadFromMemory(const char *filename, const char *data, unsigned int size )
{
    //
    // this initialize the library and check potential ABI mismatches
    // between the version it was compiled for and the actual shared
    // library used.
    //
    LIBXML_TEST_VERSION

    xmlDocPtr doc; // the resulting document tree

    xmlSubstituteEntitiesDefault(1);

    doc = xmlParseMemory(data,size);

    return processXMLLoading(filename, doc);
}

BaseElement* loadFromFile(const char *filename)
{
    //
    // this initialize the library and check potential ABI mismatches
    // between the version it was compiled for and the actual shared
    // library used.
    //
    LIBXML_TEST_VERSION

    xmlDocPtr doc; // the resulting document tree

    xmlSubstituteEntitiesDefault(1);

    doc = xmlParseFile(filename);

    return processXMLLoading(filename, doc);

}


BaseElement* includeNode(xmlNodePtr root,const char *basefilename)
{
    std::string filename;
    xmlChar *pfilename = xmlGetProp(root, (const xmlChar*) "href");
    if (pfilename)
    {
        filename = (const char*)pfilename;
        xmlFree(pfilename);
    }
    if (filename.empty())
    {
        std::cerr << "ERROR: xml include tag requires non empty filename or href attribute." << std::endl;
        return NULL;
    }
    /*  std::cout << "XML: Including external file " << filename << " from " << basefilename << std::endl;*/
    sofa::helper::system::DataRepository.findFileFromFile(filename, basefilename);
    xmlDocPtr doc; // the resulting document tree
    doc = xmlParseFile(filename.c_str());
    if (doc == NULL)
    {
        std::cerr << "ERROR: Failed to parse " << filename << std::endl;
        return NULL;
    }

    xmlNodePtr newroot = xmlDocGetRootElement(doc);
    if (newroot == NULL)
    {
        std::cerr << "ERROR: empty document in " << filename << std::endl;
        xmlFreeDoc(doc);
        return NULL;
    }
    BaseElement* result = createNode(newroot, filename.c_str(), true);
    if (result)
    {

        if (result->getName() == "Group") result->setGroupType(true);
        // Copy attributes
        for (xmlAttrPtr attr = root->properties; attr!=NULL; attr = attr->next)
        {
            if (attr->children==NULL) continue;
            if (!xmlStrcmp(attr->name,(const xmlChar*)"href")) continue;
            if (!xmlStrcmp(attr->name,(const xmlChar*)"name"))
            {
                if(!xmlStrcmp(attr->children->content,(const xmlChar*)"Group")) result->setGroupType(true);
                else  result->setGroupType(false);

                if (!result->isGroupType()) result->setName((const char*)attr->children->content);
            }
            else
            {
                const char* attrname = (const char*)attr->name;

                const char* value = (const char*)attr->children->content;
                if (const char* sep = strstr(attrname,"__"))
                {
                    // replace attribute in nodes with a given name
                    std::string nodename(attrname, sep);
                    recReplaceAttribute(result, sep+2, value, nodename.c_str());
                }
                else
                {
                    // replace attribute in all nodes already containing it
                    recReplaceAttribute(result, attrname, value);
                }
            }
        }
    }
    xmlFreeDoc(doc);
    return result;
}

#endif //SOFA_XML_PARSER_LIBXML


} // namespace xml

} // namespace simulation

} // namespace sofa

