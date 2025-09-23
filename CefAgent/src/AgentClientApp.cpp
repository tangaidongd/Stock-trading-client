// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// This file is shared by cefclient and cef_unittests so don't include using
// a qualified path.

#include "stdafx.h"
#include "AgentClientApp.h"
#include <string>
#include "include/cef_cookie.h"
#include "include/cef_process_message.h"
#include "include/cef_task.h"
#include "include/cef_v8.h"
#include "include/cef_runnable.h"

ClientApp::ClientApp()
{
}

void ClientApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
								 CefRefPtr<CefFrame> frame,
								 CefRefPtr<CefV8Context> context)
{
	//	OutputDebugString(_T("ClientAppRenderer::OnContextCreated, create window binding\r\n"));

	// Retrieve the context's window object.
	CefRefPtr<CefV8Value> object = context->GetGlobal();

	// 第三方登录
	CefRefPtr<CefV8Value> funcThirdLogin = CefV8Value::CreateFunction("ThirdLoginRsp", new CCefV8Handler());
	object->SetValue("ThirdLoginRsp", funcThirdLogin, V8_PROPERTY_ATTRIBUTE_NONE);

	// PC重新登录
	CefRefPtr<CefV8Value> funcReLogin = CefV8Value::CreateFunction("ReLoginPC", new CCefV8Handler());
	object->SetValue("ReLoginPC", funcReLogin, V8_PROPERTY_ATTRIBUTE_NONE);

	// 个人中心页面修改 
	CefRefPtr<CefV8Value> funcPerson = CefV8Value::CreateFunction("PersonCenterRsp", new CCefV8Handler());
	object->SetValue("PersonCenterRsp", funcPerson, V8_PROPERTY_ATTRIBUTE_NONE);

	// 打开指定版面
	CefRefPtr<CefV8Value> funcNativeOpenCfm = CefV8Value::CreateFunction("NativeOpenCfm", new CCefV8Handler());
	object->SetValue("NativeOpenCfm", funcNativeOpenCfm, V8_PROPERTY_ATTRIBUTE_NONE);

	// 打开指定版面
	CefRefPtr<CefV8Value> funcLinkageWebTrend = CefV8Value::CreateFunction("linkageWebTrend", new CCefV8Handler());
	object->SetValue("linkageWebTrend", funcLinkageWebTrend, V8_PROPERTY_ATTRIBUTE_NONE);

	// 查询所属行业
	CefRefPtr<CefV8Value> funcQueryIndustry = CefV8Value::CreateFunction("queryIndustry", new CCefV8Handler());
	object->SetValue("queryIndustry", funcQueryIndustry, V8_PROPERTY_ATTRIBUTE_NONE);

	// 查询用户信息
	CefRefPtr<CefV8Value> funcqueryUserInfoByWeb = CefV8Value::CreateFunction("queryUserInfoByWeb", new CCefV8Handler());
	object->SetValue("queryUserInfoByWeb", funcqueryUserInfoByWeb, V8_PROPERTY_ATTRIBUTE_NONE);

	// 查询用户自选股信息
	CefRefPtr<CefV8Value> funcqueryuserblockInfo = CefV8Value::CreateFunction("queryuserblockInfo", new CCefV8Handler());
	object->SetValue("queryuserblockInfo", funcqueryuserblockInfo, V8_PROPERTY_ATTRIBUTE_NONE);
	
	// 自选股操作
	CefRefPtr<CefV8Value> funcaddUserStock = CefV8Value::CreateFunction("addUserStock", new CCefV8Handler());
	object->SetValue("addUserStock", funcaddUserStock, V8_PROPERTY_ATTRIBUTE_NONE);


	// 回到登录界面
	CefRefPtr<CefV8Value> funcToLogin = CefV8Value::CreateFunction("ToLogin", new CCefV8Handler());
	object->SetValue("ToLogin", funcToLogin, V8_PROPERTY_ATTRIBUTE_NONE);

	// 点击网页里的链接在外面浏览器打开
	CefRefPtr<CefV8Value> funcOutWeb = CefV8Value::CreateFunction("OpenOutWeb", new CCefV8Handler());
	object->SetValue("OpenOutWeb", funcOutWeb, V8_PROPERTY_ATTRIBUTE_NONE);

	// 弹出注册框
	CefRefPtr<CefV8Value> funcRegister = CefV8Value::CreateFunction("Register", new CCefV8Handler());
	object->SetValue("Register", funcRegister, V8_PROPERTY_ATTRIBUTE_NONE);

	// 网页出错时，刷新当前页面
	CefRefPtr<CefV8Value> funcErrorBack = CefV8Value::CreateFunction("ErrorBack", new CCefV8Handler());
	object->SetValue("ErrorBack", funcErrorBack, V8_PROPERTY_ATTRIBUTE_NONE);

	// 点击网页里的链接弹出对话框
	CefRefPtr<CefV8Value> funcWebLink = CefV8Value::CreateFunction("ShowWebDlg", new CCefV8Handler());
	object->SetValue("ShowWebDlg", funcWebLink, V8_PROPERTY_ATTRIBUTE_NONE);

	// 关闭web对话框
	CefRefPtr<CefV8Value> funcColseDlg = CefV8Value::CreateFunction("CloseWebDlg", new CCefV8Handler());
	object->SetValue("CloseWebDlg", funcColseDlg, V8_PROPERTY_ATTRIBUTE_NONE);
	
	// 下载交易程序
	CefRefPtr<CefV8Value> funcDldTrade = CefV8Value::CreateFunction("DownloadTrade", new CCefV8Handler());
	object->SetValue("DownloadTrade", funcDldTrade, V8_PROPERTY_ATTRIBUTE_NONE);
	
	// 关闭在线客服对话框
	CefRefPtr<CefV8Value> funcCloseImDlg = CefV8Value::CreateFunction("CloseOnlineServDlg", new CCefV8Handler());
	object->SetValue("CloseOnlineServDlg", funcCloseImDlg, V8_PROPERTY_ATTRIBUTE_NONE);

	// JS调用PC端接口
	CefRefPtr<CefV8Value> funcexecuteNative = CefV8Value::CreateFunction("executeNative", new CCefV8Handler());
	object->SetValue("executeNative", funcexecuteNative, V8_PROPERTY_ATTRIBUTE_NONE);

	// JS给PC端返回值
	CefRefPtr<CefV8Value> funccallNativeCallback = CefV8Value::CreateFunction("callNativeCallback", new CCefV8Handler());
	object->SetValue("callNativeCallback", funccallNativeCallback, V8_PROPERTY_ATTRIBUTE_NONE);
}

void ClientApp::OnWebKitInitialized()	
{
	//	OutputDebugString(_T("ClientAppRenderer::OnWebKitInitialized, create js extensions\r\n"));

	std::string app_SimulateTrade =
		"var external;"
		"if (!external)"
		"    external = {};"
		"(function() {"
		"    external.TradeLogin = function() {"
		"        native function TradeLogin();"
		"        return TradeLogin();"
		"    };"
		"})();"
		"var external;"
		"if (!external)"
		"    external = {};"
		"(function() {"
		"    external.OpenAccount = function() {"
		"        native function OpenAccount();"
		"        return OpenAccount();"
		"    };"
		"})();"
		"var external;"
		"if (!external)"
		"    external = {};"
		"(function() {"
		"    external.Register = function() {"
		"        native function Register();"
		"        return Register();"
		"    };"
		"})();"
		"var external;"
		"if (!external)"
		"    external = {};"
		"(function() {"
		"    external.HQLogin = function() {"
		"        native function HQLogin();"
		"        return HQLogin();"
		"    };"
		"})();"
		"var external;"
		"if (!external)"
		"    external = {};"
		"(function() {"
		"    external.myRegist = function() {"
		"        native function myRegist();"
		"        return myRegist();"
		"    };"
		"})();"
		"var external;"
		"if (!external)"
		"    external = {};"
		"(function() {"
		"    external.ImageUrl = function(arg0) {"
		"        native function ImageUrl(arg0);"
		"        return ImageUrl(arg0);"
		"    };"
		"})();";

	CefRegisterExtension("v8/root.external", app_SimulateTrade, new CCefV8Handler()/*m_v8Handler*/);
}

void ClientApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line)
{
	__super::OnBeforeCommandLineProcessing(process_type, command_line);
}
