// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// This file is shared by cefclient and cef_unittests so don't include using
// a qualified path.
#include "stdafx.h"
#include "client_AnalysisWeb.h"


#define  STR_WEB_PAGE_ONE		_T("http://")
#define  STR_WEB_PAGE_TWO		_T("https://")
#define  STR_LOCAL_FILE			_T("file://")
#define  STR_VIEW				_T("view://")
#define  STR_LOCAL_APP			_T("app://")
#define  STR_PASS_MSG			_T("msg://")


CAnalysisWeb::CAnalysisWeb()
{
	m_multimapParamsInfo.clear();	
	m_strCommand	= _T("");	
	m_strRawdata    = "";
	m_strCommadID	= _T("");
	m_enumComType	= COM_OPEN_INVALID;	
};

CAnalysisWeb::~CAnalysisWeb()
{
	m_multimapParamsInfo.clear();	
	m_strCommand	= _T("");			
	m_enumComType	= COM_OPEN_INVALID;	
};


CString GetStringFromJsonValue(Json::Value value)
{
	CString strResult;
	if (value.isDouble())
	{
		strResult.Format(_T("%.2f"), value.asDouble());
	}
	else if (value.isInt())
	{
		strResult.Format(_T("%d"), value.asInt());
	}
	else if (value.isBool())
	{
		strResult.Format(_T("%d"), value.asBool());
	}
	else if (value.isString())
	{
		strResult = value.asString().c_str();
	}
	else if (value.isUInt())
	{
		strResult.Format(_T("%u"), value.asUInt());
	}
	return strResult;
}



CStringA CAnalysisWeb::excuteJS111()
{
	Json::Value jsValue;
	CString strToken;
	string strFieldTemp;

	jsValue.clear();
	string strCommandID = CStringA(m_strCommadID);
	jsValue["id"]		= strCommandID.c_str();

	
	Json::Value jsPayloadValue;
	jsPayloadValue["name"]		= "test";
	jsPayloadValue["params"]	= "22222222222222222222";
	jsValue["payload"] = jsPayloadValue;

	string strWebDisData = jsValue.toStyledString();
	int index = 0;
	if( !strWebDisData.empty())
	{
		while( (index = strWebDisData.find('\n',index)) != string::npos)
		{
			strWebDisData.erase(index,1);
		}
	}

	CStringA strFormat;
	strFormat.Format("excuteJS('%s')", strWebDisData.c_str());

	return strFormat;
}



CStringA CAnalysisWeb::excuteJSCallbackPackJsonCmd()
{
	CStringA strFormat;
	strFormat.Format("excuteJSCallback('%s')", m_strRawdata.c_str());
	
	return strFormat;
}

bool CAnalysisWeb::AnalysisJsonCmd(CString strJsonCmd)
{
	vector<int>::iterator result ;
	map<int32,T_SmartStockPickItem> mapPickModelInfo;
	mapPickModelInfo.clear();
	

	string strJson = CStringA(strJsonCmd);
	// 保存原始数据
	m_strRawdata = strJson;

	Json::Reader reader;
	Json::Value value;
	if (reader.parse(strJson.c_str(), value))
	{   
		if (value.size() <= 0)
		{
			return false;
		}


		if (!value.isMember("id")) 
		{
			return false;
		}
		m_strCommadID = GetStringFromJsonValue(value["id"]); 


		if (!value.isMember("payload")) 
		{
			return false;
		}
		
		Json::Value CommadValue = value["payload"];
		// 解析命令
		if (!CommadValue.isMember("command")) 
		{
			return false;
		}
		CString strCommand111 = CommadValue["command"].asCString();
		string strCommand = CommadValue["command"].asString();
		CString strParse = strCommand.c_str();
		if (_tcsnccmp(strParse, STR_WEB_PAGE_ONE, _tcslen(STR_WEB_PAGE_ONE)) == 0)
		{
			m_strCommand	= strParse.Mid(_tcslen(STR_WEB_PAGE_ONE));
			m_enumComType	= COM_OPEN_WEB_PAGE_HTTP;
		}
		else if (_tcsnccmp(strParse, STR_WEB_PAGE_TWO, _tcslen(STR_WEB_PAGE_TWO)) == 0)
		{
			m_strCommand	= strParse.Mid(_tcslen(STR_WEB_PAGE_TWO));
			m_enumComType	= COM_OPEN_WEB_PAGE_HTTPS;
		}
		else if (_tcsnccmp(strParse, STR_LOCAL_FILE, _tcslen(STR_LOCAL_FILE)) == 0)
		{
			m_strCommand	= strParse.Mid(_tcslen(STR_LOCAL_FILE));
			m_enumComType	= COM_OPEN_LOCAL_FILE;
		}
		else if (_tcsnccmp(strParse, STR_VIEW, _tcslen(STR_VIEW)) == 0)
		{
			m_strCommand	= strParse.Mid(_tcslen(STR_VIEW));
			m_enumComType	= COM_OPEN_VIEW;
		}
		else if (_tcsnccmp(strParse, STR_LOCAL_APP, _tcslen(STR_LOCAL_APP)) == 0)
		{
			m_strCommand	= strParse.Mid(_tcslen(STR_LOCAL_APP));
			m_enumComType	= COM_OPEN_LOCAL_APP;
		}
		else if (_tcsnccmp(strParse, STR_PASS_MSG, _tcslen(STR_PASS_MSG)) == 0)
		{
			m_strCommand	= strParse.Mid(_tcslen(STR_PASS_MSG));
			m_enumComType	= COM_OPEN_PASS_MSG;
		}


		// 解析参数 , 暂时不做解析
		if (!CommadValue.isMember("params")) 
		{
			return false;
		}
		m_multimapParamsInfo.clear();
		const Json::Value arrayData = CommadValue["params"];
		Json::Value::Members arrayMember = arrayData.getMemberNames(); //ayyayMember是成员名称的集合，即name,age;
		for(Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter) //遍历json成员
		{
			string member_name= *iter;
			CString strValue;
			if (arrayData[member_name].isArray())
			{
				int a = arrayData[member_name].size();
				for (int i = 0; i < arrayData[member_name].size(); i++)
				{
					strValue = GetStringFromJsonValue(arrayData[member_name][i]);
					if (!strValue.IsEmpty())
					{
						string strMemberKey = member_name;
						strMemberKey = strMemberKey.append("[]");
						m_multimapParamsInfo.insert(pair<CString, CString>(strMemberKey.c_str(), strValue));
					}
				}	
			}
			else if (arrayData[member_name].isString())
			{
				strValue = arrayData[member_name].asString().c_str();
				m_multimapParamsInfo.insert(pair<CString, CString>(member_name.c_str(), strValue));
			}
			else if (arrayData[member_name].isInt())
			{
				strValue = GetStringFromJsonValue(arrayData[member_name].asInt());
				m_multimapParamsInfo.insert(pair<CString, CString>(member_name.c_str(), strValue));
			}
			else if (arrayData[member_name].isDouble())
			{
				strValue = GetStringFromJsonValue(arrayData[member_name].asDouble());
				m_multimapParamsInfo.insert(pair<CString, CString>(member_name.c_str(), strValue));
			} 
		}
	}

	return true;

}


