/**
* Copyright (c) 2014 by Wenpeng.
* All Rights Reserved.
*
* FileName: parserxml_xml.h
*
* Author: wenpeng
*
* Description:
*
*/

#ifndef __PARSERXML_XML_H_
#define __PARSERXML_XML_H_

#include <stddef.h>  // for size_t

#ifndef _MSC_VER
#include <inttypes.h>
#else
#include <stdint.h>
#endif  /* _MSC_VER */

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"


namespace xmlparser
{
	class XmlParser
	{
	public:
		/**
		*
		*/
		XmlParser(const char* filename = NULL);
		virtual ~XmlParser();


		/**
		*	读取xml文件
		*/
		bool	Read(const char* filename);

        /**
        * 创建一个新的xml
        */
        bool    Create(const char* filename);

        /**
        * 保存
        */
        void    Save();

        /**
        * 判断文件是否存在
        */
        bool    IsFileExist(const char* filename);

		/**
		*	当前节点名字
		*/
		const char* CurNodeName();
        const char* CurNodeValue();
		bool		GotoChild(const char* szKey = NULL);
		bool		NextSibling(const char* szKey = NULL);
		/* 返回上层节点 */
		void		Parent();
        /* 返回到指定名称的上层节点 */
        void        Parent(const char* szName);
		/*  */
		bool		FirstNode(const char* szKey = NULL);

		/**
		 *   读取value
		 */
        bool		GetBooleanForKey(const char* szKey, bool& bRet);
		bool		GetInt32ForKey(const char* szKey, int32_t& iRet);
		bool		GetInt64ForKey(const char* szKey, int64_t& lRet);
		bool		GetFloatForKey(const char* szKey, float& fRet);
		bool		GetDoubleForKey(const char* szKey, double& dRet);
		const char*	GetStringForKey(const char* szKey);

		/**
		*   读取Attribute
		*/
        bool		GetBooleanForAttribute(const char* szKey, bool& bRet, bool bDef = false);
        bool		GetInt16ForAttribute(const char* szKey, int16_t& iRet, int16_t iDef = 0);
		bool		GetInt32ForAttribute(const char* szKey, int32_t& iRet, int32_t iDef = 0);
        bool		GetInt64ForAttribute(const char* szKey, int64_t& lRet, int64_t lDef = 0);
        bool		GetFloatForAttribute(const char* szKey, float& fRet, float fDef = .0f);
        bool		GetDoubleForAttribute(const char* szKey, double& dRet, double dDef = .0);
		const char* GetStringForAttribute(const char* szKey);


        /**
        */
        bool	    NewGroup(const char* groupName);

		///**
		//*   设置value
		//*/
        void		SetBoolForKey(const char* szKey, bool bValue);
        void		SetInt32ForKey(const char* szKey, int32_t iValue);
        void		SetInt64ForKey(const char* szKey, int64_t iValue);
        void		SetFloatForKey(const char* szKey, float fValue);
        void		SetDoubleForKey(const char* szKey, double dValue);
        void		SetStringForKey(const char* szKey, const char* szValue);

		///**
		//*   设置Attribute
		//*/
        void		SetBoolForAttribute(const char* szKey, bool bValue);
        void		SetInt8ForAttribute(const char* szKey, int8_t iValue);
        void		SetInt16ForAttribute(const char* szKey, int16_t iValue);
        void		SetInt32ForAttribute(const char* szKey, int32_t iValue);
        void		SetInt64ForAttribute(const char* szKey, int64_t iValue);
        void		SetFloatForAttribute(const char* szKey, float fValue);
        void		SetDoubleForAttribute(const char* szKey, double dValue);
        void		SetStringForAttribute(const char* szKey, const char* szValue);

	public:

        void GetAllAttributeName(std::vector<std::string> &vec_Attr);

	private:
		rapidxml::file<>*			m_file;
		rapidxml::xml_document<>	m_doc;
		rapidxml::xml_node<>*		m_curNode;

	char*			m_szFileName;
	};
    //自动上跳只能跳一次
    class XmlParentGuard
    {
    public:
        XmlParentGuard(XmlParser& parser) : parser_(parser)
        {
        }

        ~XmlParentGuard()
        {
            parser_.Parent();
        }

    private:
        XmlParser& parser_;
    };
}


#endif //__PARSERXML_XML_H_
