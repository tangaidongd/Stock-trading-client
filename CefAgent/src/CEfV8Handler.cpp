#include "stdafx.h"
#include "CEfV8Handler.h"
#include <Windows.h>

CCefV8Handler::CCefV8Handler()
{
	
}

bool CCefV8Handler::Execute( const CefString& name, CefRefPtr<CefV8Value> object, \
							const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception )
{ 
	if (name == "NativeLogin") 
	{
		return true;
	}
	else if (name == "TradeLogin")	// 进入模拟交易
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("TradeLogin");
		// Send the process message to the browser process.
		CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		return true;
	}
	else if (name == "OpenAccount")	// 实盘开户
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("OpenAccount");
		CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		return true;
	}
	else if (name == "Register")	// 行情注册
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("Register");
		CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		return true;
	}
	else if (name == "HQLogin")		// 行情登录
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("HQLogin");
		CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		return true;
	}
    else if(name == "myRegist")
    {
        CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("myRegist");
        CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
        return true;
    }
	else if (name == "ImageUrl")	// 点击图片
	{ 
		if (1 == arguments.size())  // 带一个参数
		{
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ImageUrl");
			CefString sUrl = arguments.at(0)->GetStringValue();
			CefRefPtr<CefListValue> args = msg->GetArgumentList();

			// Populate the argument values.
			args->SetSize(1);
			args->SetString(0, sUrl);
			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}

		return true;
	}
	else if (name == "ReLoginPC") 
	{
		if (1 == arguments.size())  // 带1个参数, 账号
		{
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ReLoginPC");
			CefString sUser = arguments.at(0)->GetStringValue();
			CefRefPtr<CefListValue> args = msg->GetArgumentList();

			args->SetSize(1);
			args->SetString(0, sUser);
			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
		else if (0 == arguments.size())	// 不带参数
		{
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ReLoginPC");
			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
		return true;
	}
	else if (name == "ThirdLoginRsp") 
	{
		if (2 == arguments.size())  // 带2个参数, 类型, code
		{
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ThirdLoginRsp");
			CefString sType = arguments.at(0)->GetStringValue();
			CefString sCode = arguments.at(1)->GetStringValue();
			CefRefPtr<CefListValue> args = msg->GetArgumentList();

			args->SetSize(2);
			args->SetString(0, sType);
			args->SetString(1, sCode);
			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
		return true;
	}
	else if (name == "PersonCenterRsp") 
	{
		if (1 == arguments.size())  // 带1个参数, 昵称
		{
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("PersonCenterRsp");
			CefString sName = arguments.at(0)->GetStringValue();
			//CefString sSucc = arguments.at(1)->GetStringValue();
			CefRefPtr<CefListValue> args = msg->GetArgumentList();

			args->SetSize(1);
			args->SetString(0, sName);
			//args->SetString(1, sSucc);
			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
		return true;
	}
	else if (name == "NativeOpenCfm")
	{

		if (1 == arguments.size())	// 带1个参数,版面名称
		{
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("NativeOpenCfm");
			CefString sName = arguments.at(0)->GetStringValue();
			CefRefPtr<CefListValue> args = msg->GetArgumentList();

			args->SetSize(1);
			args->SetString(0, sName);
			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
		else if (2 == arguments.size()) //带2个参数, 个股资讯 第一个参数版面名称，第二个参数完整Url
		{
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("NativeOpenCfm");
			CefString sName = arguments.at(0)->GetStringValue();
			CefString sFlag = arguments.at(1)->GetStringValue();

			CefRefPtr<CefListValue> args = msg->GetArgumentList();

			args->SetSize(2);
			args->SetString(0, sName);
			args->SetString(1, sFlag);

			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
		else if (3 == arguments.size()) //带3个参数, 对应特殊的文字直播
		{
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("NativeOpenCfm");
			CefString sName = arguments.at(0)->GetStringValue();
			CefInt32 iID= arguments.at(1)->GetIntValue();
			CefString sFlag = arguments.at(2)->GetStringValue();

			CefRefPtr<CefListValue> args = msg->GetArgumentList();

			args->SetSize(3);
			args->SetString(0, sName);
			args->SetInt(1, iID);
			args->SetString(2, sFlag);

			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
		return true;
	}
	else if (name == "linkageWebTrend")
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("linkageWebTrend");
		if (3 == arguments.size())
		{
			CefInt32  iMarketid= arguments.at(0)->GetIntValue();
			CefString sMerchCode = arguments.at(1)->GetStringValue();
			CefString ssMerchName = arguments.at(2)->GetStringValue();

			CefRefPtr<CefListValue> args = msg->GetArgumentList();

			args->SetSize(3);
			args->SetInt(0, iMarketid);
			args->SetString(1, sMerchCode);
			args->SetString(2, ssMerchName);

			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
		return true;
	}
	else if (name == "queryIndustry")
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("queryIndustry");
		if (1 == arguments.size())
		{
			CefString ssMerchName = arguments.at(0)->GetStringValue();
			CefRefPtr<CefListValue> args = msg->GetArgumentList();
			args->SetSize(1);
			args->SetString(0, ssMerchName);
			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
		return true;
	}
	else if (name == "queryUserInfoByWeb")
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("queryUserInfoByWeb");
		CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		return true;
	}
	else if (name == "queryuserblockInfo")
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("queryuserblockInfo");
		CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		return true;
	}
	else if (name == "addUserStock")
	{
		if (3 == arguments.size())  // 带3个参数, code,marketid,bDel
		{
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("addUserStock");
			CefString sCode		= arguments.at(0)->GetStringValue();
			CefString sMarketid = arguments.at(1)->GetStringValue();
			bool	  bIsDel	= arguments.at(2)->GetBoolValue();
			CefRefPtr<CefListValue> args = msg->GetArgumentList();

			args->SetSize(3);
			args->SetString(0, sCode);
			args->SetString(1, sMarketid);
			args->SetBool(2, bIsDel);
			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
		return true;
	}
	else if (name == "ToLogin")
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ToLogin");
		CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		return true;
	}
	else if (name == "OpenOutWeb")
	{
		if (1 == arguments.size())	// 带1个参数,网页地址
		{
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("OpenOutWeb");
			CefString sUrl = arguments.at(0)->GetStringValue();
			CefRefPtr<CefListValue> args = msg->GetArgumentList();

			args->SetSize(1);
			args->SetString(0, sUrl);
			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
		return true;
	}
	else if (name == "ErrorBack")
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ErrorBack");
		CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		return true;
	}
	else if (name == "ShowWebDlg")
	{
		if (3 == arguments.size())  // 带3个参数, url, width, height
		{
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ShowWebDlg");
			CefString sUrl = arguments.at(0)->GetStringValue();
			CefString sWidth = arguments.at(1)->GetStringValue();
			CefString sHeight = arguments.at(2)->GetStringValue();
			CefRefPtr<CefListValue> args = msg->GetArgumentList();

			args->SetSize(3);
			args->SetString(0, sUrl);
			args->SetString(1, sWidth);
			args->SetString(2, sHeight);
			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
		return true;
	}
	else if (name == "CloseWebDlg")
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("CloseWebDlg");
		CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		return true;
	}
	else if (name == "DownloadTrade")
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("DownloadTrade");
		
		CefString sJsonValue = arguments.at(0)->GetStringValue();
		
		CefRefPtr<CefListValue> args = msg->GetArgumentList();

		args->SetSize(1);
		args->SetString(0, sJsonValue);

		CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		return true;
	}
	else if (name == "CloseOnlineServDlg")
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("CloseOnlineServDlg");
		CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		return true;
	}
	else if(name.compare("executeNative") == 0)
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("executeNative");
		if (arguments.size() == 1)
		{
			CefString sJsonValue = arguments.at(0)->GetStringValue();
			CefRefPtr<CefListValue> args = msg->GetArgumentList();
			args->SetSize(1);
			args->SetString(0, sJsonValue);
			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
	}

	else if(name.compare("callNativeCallback") == 0)
	{
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("callNativeCallback");
		if (arguments.size() == 1)
		{
			CefString sJsonValue = arguments.at(0)->GetStringValue();
			CefRefPtr<CefListValue> args = msg->GetArgumentList();
			args->SetSize(1);
			args->SetString(0, sJsonValue);
			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		}
	}

	// Function does not exist.
	return false;
}