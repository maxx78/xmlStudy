#include <stdlib.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <fstream>
#include <string.h>
#include "parserxml_xml.h"
#ifdef _WIN32
#pragma warning(disable: 4996)
#endif

#define XMLPARSER_USE_RAPIDXML 1

namespace xmlparser
{
#define SAFEFREE(ptr) if (ptr) free(ptr); ptr = NULL;
#define SAFEDELETE(ptr) if (ptr) delete(ptr); ptr = NULL;
	
	const char* GetXMLValueForKey(const char* pKey, rapidxml::xml_node<>* parentNode)
	{
		if (!pKey || !parentNode)
			return NULL;
		rapidxml::xml_node<>* node = parentNode->first_node(pKey);
        if (node && node->first_node())
        {
            return node->first_node()->value();
        }
		return node ? node->value() : NULL;
	}
	const char* GetXMLAttribute(const char* pKey, rapidxml::xml_node<>* parentNode)
	{
		if (!pKey || !parentNode)
			return NULL;
		rapidxml::xml_attribute<>* attr = parentNode->first_attribute(pKey);

		return attr ? attr->value() : NULL;
	}

    void SetXMLAtrribute(const char* pKey, const char* value, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* parentNode)
    {
        parentNode->append_attribute(doc.allocate_attribute(doc.allocate_string(pKey), doc.allocate_string(value)));
    }
    void SetXMLValueForKey(const char* pKey, const char* value, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* parentNode)
    {
        // check the params
        if (!pKey || !value || !parentNode)
        {
            return;
        }

        // find the node
        rapidxml::xml_node<>* node = parentNode->first_node(pKey);

        if (node)
        {
            if (node->first_node())
            {
                node->first_node()->value(doc.allocate_string(value));
            }
            else
            {
                node->value(doc.allocate_string(value));
            }
        }
        else
        {
            parentNode->append_node(doc.allocate_node(rapidxml::node_element, doc.allocate_string(pKey), doc.allocate_string(value)));
        }
    }

	bool XmlParser::GotoChild(const char* szKey/* = NULL*/)
	{
		if (m_curNode)
		{
			rapidxml::xml_node<>* node = m_curNode->first_node(szKey);
			if (node)
			{
				m_curNode = node;
				return true;
			}
		}
		else
		{
			rapidxml::xml_node<>* node = m_doc.first_node(szKey);
			if (node)
			{
				m_curNode = node;
				return true;
			}
		}
		return false;
	}

	bool XmlParser::FirstNode(const char* szKey/* = NULL*/)
	{
		rapidxml::xml_node<>* node = m_doc.first_node(szKey);
		if (node)
		{
			m_curNode = node;
			return true;
		}
		return false;
	}
	bool XmlParser::NextSibling(const char* szKey /*= NULL*/)
	{
		if (m_curNode)
		{
			rapidxml::xml_node<>* node = m_curNode->next_sibling(szKey);
			if (node)
			{
				m_curNode = node;
				return true;
			}
		}
		return false;
	}

    void XmlParser::Parent()
	{
		if (m_curNode)
		{
			rapidxml::xml_node<>* node = m_curNode->parent();
			if (node)
				m_curNode = node;
		}
	}

    void XmlParser::Parent(const char* szName)
    {
        while (m_curNode && strcmp(CurNodeName(), szName) != 0)
        {
            rapidxml::xml_node<>* node = m_curNode->parent();
            if (node)
            {
                m_curNode = node;
            }
            else
            {
                break;
            }
            
        }
    }


	XmlParser::XmlParser(const char* filename /*= NULL*/) : 
		m_file(NULL)
		, m_curNode(NULL)
        , m_szFileName(NULL)
	{
		if (filename)
			Read(filename);
	}

	XmlParser::~XmlParser()
	{
		SAFEFREE(m_szFileName)
		SAFEDELETE(m_file)

	}

    bool XmlParser::IsFileExist(const char* filename)
    {
        bool bRet = false;
        if (filename)
        {
            struct stat buf;
            bRet = access(filename, 0) == 0;
            if (bRet)
            {
                int cc;
                cc = stat(filename, &buf);
                if ((cc == 0 && (buf.st_mode & S_IFDIR)) || cc == -1)
                {
                    bRet = false;
                }
                else
                {
                    bRet = true;
                }
            }
        }
        return bRet;
    }

    bool XmlParser::Read(const char* filename)
    {
        SAFEFREE(m_szFileName)
        SAFEDELETE(m_file)

		if (filename /*&& IsFileExist(filename)*/)
		{
            if (!IsFileExist(filename))
            {
                return false;
            }
            m_szFileName = strdup(filename);
            m_file = new rapidxml::file<>(filename);
            if (m_file->size() != 0)
            {
                m_doc.parse<0>(m_file->data());
                return true;
            }
		}

		return false;
	}

    bool    XmlParser::Create(const char* filename)
    {
        bool bRet = false;

        SAFEFREE(m_szFileName);
        if (filename)
        {
            m_szFileName = strdup(filename);

            m_doc.clear();

            // new root node
            rapidxml::xml_node<>* node = m_doc.allocate_node(rapidxml::node_pi,
                m_doc.allocate_string("xml version=\"1.0\" encoding=\"UTF-8\""));
            m_doc.append_node(node);

            m_curNode = NULL;

            bRet = true;

        }
        

        return bRet;
    }

    void XmlParser::Save()
    {
        if (m_szFileName)
        {
            // save file and free doc
			std::ofstream out(m_szFileName);
            out << m_doc;
            out.close();
            out.clear();

        }
    }

    const char* XmlParser::CurNodeName()
    {
#ifdef XMLPARSER_USE_RAPIDXML
        if (m_curNode)
            return m_curNode->name();
        return NULL;
#elif XMLPARSER_USE_LIBXML
        if (m_pCurNode)
            return (const char*)m_pCurNode->name;
        return NULL;
#endif
    }

    const char* XmlParser::CurNodeValue()
    {
#ifdef XMLPARSER_USE_RAPIDXML
        if (m_curNode)
            return m_curNode->value();
        return NULL;
#elif XMLPARSER_USE_LIBXML
        if (m_pCurNode)
            return (const char*)m_pCurNode->value;
        return NULL;
#endif
    }

    bool XmlParser::GetBooleanForKey(const char* szKey, bool& bRet)
    {
        const char* value = GetStringForKey(szKey);

        if (value == NULL)
            return false;

        if (strcmp(value, "true") == 0
            || strcmp(value, "1") == 0
            || strcmp(value, "yes") == 0
            || strcmp(value, "ok") == 0)
        {
            bRet = true;
        }
        else
        {
            bRet = false;
        }

        return true;
    }
	bool XmlParser::GetInt32ForKey(const char* szKey, int32_t& iRet)
	{
		const char* szValue = GetStringForKey(szKey);
		return szValue ? (iRet = atoi(szValue)), true : false;
	}
	bool XmlParser::GetInt64ForKey(const char* szKey, int64_t& lRet)
	{
		const char* szValue = GetStringForKey(szKey);
		return szValue ? (lRet = atoll(szValue)), true : false;
	}
	bool XmlParser::GetFloatForKey(const char* szKey, float& fRet)
	{
		const char* szValue = GetStringForKey(szKey);
		return szValue ? (fRet = (float)atof(szValue)), true : false;
	}
	bool XmlParser::GetDoubleForKey(const char* szKey, double& dRet)
	{
		const char* szValue = GetStringForKey(szKey);
		return szValue ? (dRet = atof(szValue)), true : false;
	}
	const char* XmlParser::GetStringForKey(const char* szKey)
	{
#ifdef XMLPARSER_USE_RAPIDXML
        const char* szValue = GetXMLValueForKey(szKey, m_curNode);
        if (szValue == NULL)
        {
            //TRACE_WARN("Parse Xml FileName = %s, Key = %s is NULL", m_szFileName, szKey);
        }
        return szValue;
#elif XMLPARSER_USE_LIBXML
		return GetXMLValueForKey(szKey, m_pCurNode);
#endif
	}

#if defined(_WIN32)
#define stricmp _stricmp
#else
#define stricmp strcasecmp
#endif

    bool XmlParser::GetBooleanForAttribute(const char* szKey, bool& bRet, bool bDef/* = false*/)
    {
        bRet = bDef;

        const char* value = GetStringForAttribute(szKey);

        if (value == NULL)
            return false;

        if (stricmp(value, "true") == 0
            || strcmp(value, "1") == 0
            || stricmp(value, "yes") == 0
            || stricmp(value, "ok") == 0)
        {
            bRet = true;
        }
        else
        {
            bRet = false;
        }

        return true;
    }
    bool XmlParser::GetInt16ForAttribute(const char* szKey, int16_t& iRet, int16_t iDef /*= 0*/)
	{
        iRet = iDef;

        const char* szValue = GetStringForAttribute(szKey);
        if (szValue == NULL)
        {
            return false;
        }
        if (strlen(szValue) > 0)
        {
            iRet = atoi(szValue);
        }
        return true;
	}
	bool XmlParser::GetInt32ForAttribute(const char* szKey, int32_t& iRet, int32_t iDef/* = 0*/)
	{
        iRet = iDef;

		const char* szValue = GetStringForAttribute(szKey);
        if (szValue == NULL)
        {
            return false;
        }
        if (strlen(szValue) > 0)
        {
            iRet = atoi(szValue);
        }
        return true;
	}
    bool XmlParser::GetInt64ForAttribute(const char* szKey, int64_t& lRet, int64_t lDef /*= 0*/)
	{
        lRet = lDef;

        const char* szValue = GetStringForAttribute(szKey);
        if (szValue == NULL)
        {
            return false;
        }
        if (strlen(szValue) > 0)
        {
            lRet = atoll(szValue);
        }
        return true;
	}
    bool XmlParser::GetFloatForAttribute(const char* szKey, float& fRet, float fDef /*= .0f*/)
	{
        fRet = fDef;

		const char* szValue = GetStringForAttribute(szKey);
        if (szValue == NULL)
        {
            return false;
        }
        if (strlen(szValue) > 0)
        {
            fRet = (float)atof(szValue);
        }
        return true;
	}

    bool XmlParser::GetDoubleForAttribute(const char* szKey, double& dRet, double dDef/* = .0*/)
	{
        dRet = dDef;

        const char* szValue = GetStringForAttribute(szKey);
        if (szValue == NULL)
        {
            return false;
        }
        if (strlen(szValue) > 0)
        {
            dRet = atof(szValue);
        }
        return true;
	}

	const char* XmlParser::GetStringForAttribute(const char* szKey)
	{
#ifdef XMLPARSER_USE_RAPIDXML
        const char* szValue = GetXMLAttribute(szKey, m_curNode);
        if (szValue == NULL)
        {
            //TRACE_WARN("Parse Xml FileName = %s, Attribute = %s is NULL\n", m_szFileName, szKey);
        }
        return szValue;
#elif XMLPARSER_USE_LIBXML
		return GetXMLAttribute(szKey, m_pCurNode);
#endif
	}

    bool XmlParser::NewGroup(const char* groupName)
    {
#ifdef XMLPARSER_USE_RAPIDXML
        rapidxml::xml_node<>* pNewNode = m_doc.allocate_node(rapidxml::node_element, m_doc.allocate_string(groupName), NULL);
        if (pNewNode == NULL)
        {
            return false;
        }
        if (m_curNode != NULL)
        {
            m_curNode->append_node(pNewNode);
        }
        else
        {
            m_doc.append_node(pNewNode);
        }
        m_curNode = pNewNode;
#endif

        return true;
    }

    void XmlParser::SetBoolForKey(const char* szKey, bool bValue)
    {
        if (bValue)
            SetStringForKey(szKey, "true");
        else
            SetStringForKey(szKey, "false");
    }
    void XmlParser::SetInt32ForKey(const char* szKey, int32_t iValue)
    {
        if (!szKey)
        {
            return;
        }
        // format the value
        char tmp[33];
        memset(tmp, 0, 33);
        sprintf(tmp, "%d", iValue);

        SetStringForKey(szKey, tmp);
    }
    void XmlParser::SetInt64ForKey(const char* szKey, int64_t iValue)
    {
        if (!szKey)
        {
            return;
        }
        // format the value
        char tmp[65];
        memset(tmp, 0, 65);
        sprintf(tmp, "%ld", iValue);

        SetStringForKey(szKey, tmp);
    }
    void XmlParser::SetFloatForKey(const char* szKey, float fValue)
    {
        SetDoubleForKey(szKey, fValue);
    }
    void XmlParser::SetDoubleForKey(const char* szKey, double dValue)
    {
        if (!szKey)
        {
            return;
        }
        // format the value
        char tmp[65];
        memset(tmp, 0, 65);
        sprintf(tmp, "%lf", dValue);

        SetStringForKey(szKey, tmp);
    }
    void XmlParser::SetStringForKey(const char* szKey, const char* szValue)
    {
        // check key
        if (!szKey)
        {
            return;
        }
#ifdef XMLPARSER_USE_RAPIDXML
        SetXMLValueForKey(szKey, szValue, m_doc, m_curNode);
#endif
    }

    void XmlParser::SetBoolForAttribute(const char* szKey, bool bValue)
    {
        // save bool value as sring
        if (bValue)
            SetStringForAttribute(szKey, "true");
        else
            SetStringForAttribute(szKey, "false");
    }

    void XmlParser::SetInt8ForAttribute(const char* szKey, int8_t iValue)
    {
        SetInt32ForAttribute(szKey, iValue);
    }
    void		XmlParser::SetInt16ForAttribute(const char* szKey, int16_t iValue)
    {
        SetInt32ForAttribute(szKey, iValue);
    }
    void XmlParser::SetInt32ForAttribute(const char* szKey, int32_t iValue)
    {
        if (!szKey)
        {
            return;
        }
        // format the value
        char tmp[33];
        memset(tmp, 0, 33);
        sprintf(tmp, "%d", iValue);

        SetStringForAttribute(szKey, tmp);
    }
    void XmlParser::SetInt64ForAttribute(const char* szKey, int64_t iValue)
    {
        if (!szKey)
        {
            return;
        }
        // format the value
        char tmp[65];
        memset(tmp, 0, 65);
        sprintf(tmp, "%ld", iValue);

        SetStringForAttribute(szKey, tmp);
    }
    void XmlParser::SetFloatForAttribute(const char* szKey, float fValue)
    {
        SetDoubleForAttribute(szKey, fValue);
    }
    void XmlParser::SetDoubleForAttribute(const char* szKey, double dValue)
    {
        if (!szKey)
        {
            return;
        }

        // format the value
        char tmp[65];
        memset(tmp, 0, 65);
        sprintf(tmp, "%lf", dValue);

        SetStringForAttribute(szKey, tmp);
    }

    void XmlParser::SetStringForAttribute(const char* szKey, const char* szValue)
    {
        // check key
        if (!szKey)
        {
            return;
        }
#ifdef XMLPARSER_USE_RAPIDXML
        SetXMLAtrribute(szKey, szValue, m_doc, m_curNode);
#endif
    }

    void XmlParser::GetAllAttributeName(std::vector<std::string> &vec_Attr)
    {
        rapidxml::xml_attribute<> *attr = m_curNode->first_attribute();
        while(attr)
        {
            vec_Attr.push_back(attr->name());
            attr = attr->next_attribute();
        }
    }

}
