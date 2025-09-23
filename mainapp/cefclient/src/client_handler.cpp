// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include "stdafx.h"
#include "client_handler.h"
#include <stdio.h>
#include <algorithm>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <windows.h>
#include <shlobj.h> 

#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "resource.h"

#include "DlgNewLogin.h"
#include "include/base/cef_bind.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_path_util.h"
#include "include/cef_process_util.h"
#include "include/cef_trace.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include "client_switches.h"
#include "coding.h"


#define ID_WARN_CONSOLEMESSAGE          32000
#define ID_WARN_DOWNLOADCOMPLETE        32001
#define ID_WARN_DOWNLOADERROR           32002


namespace {

	// Custom menu command Ids.
	enum client_menu_ids {
		CLIENT_ID_SHOW_DEVTOOLS   = MENU_ID_USER_FIRST,
		CLIENT_ID_CLOSE_DEVTOOLS,
		CLIENT_ID_INSPECT_ELEMENT,
		CLIENT_ID_REFRESH,
		CLIENT_ID_TESTMENU_SUBMENU,
		CLIENT_ID_TESTMENU_CHECKITEM,
		CLIENT_ID_TESTMENU_RADIOITEM1,
		CLIENT_ID_TESTMENU_RADIOITEM2,
		CLIENT_ID_TESTMENU_RADIOITEM3,
	};

}  // namespace

namespace switches {

	// CEF and Chromium support a wide range of command-line switches. This file
	// only contains command-line switches specific to the cefclient application.
	// View CEF/Chromium documentation or search for *_switches.cc files in the
	// Chromium source code to identify other existing command-line switches.
	// Below is a partial listing of relevant *_switches.cc files:
	//   base/base_switches.cc
	//   cef/libcef/common/cef_switches.cc
	//   chrome/common/chrome_switches.cc (not all apply)
	//   content/public/common/content_switches.cc

	const char kMultiThreadedMessageLoop[] = "multi-threaded-message-loop";
	const char kCachePath[] = "cache-path";
	const char kUrl[] = "url";
	const char kOffScreenRenderingEnabled[] = "off-screen-rendering-enabled";
	const char kOffScreenFrameRate[] = "off-screen-frame-rate";
	const char kTransparentPaintingEnabled[] = "transparent-painting-enabled";
	const char kShowUpdateRect[] = "show-update-rect";
	const char kMouseCursorChangeDisabled[] = "mouse-cursor-change-disabled";
	const char kRequestContextPerBrowser[] = "request-context-per-browser";
	const char kBackgroundColor[] = "background-color";
	const char kEnableGPU[] = "enable-gpu";

}  // namespace switches

int ClientHandler::browser_count_ = 0;

// 特殊的请求协议
const char* KStrPecialPro = "tencent://";

ClientHandler::ClientHandler(HWND hWnd)
: browser_id_(0),
is_closing_(false),
main_handle_(NULL),
edit_handle_(NULL),
back_handle_(NULL),
forward_handle_(NULL),
stop_handle_(NULL),
reload_handle_(NULL),
focus_on_editable_field_(false) {
#if defined(OS_LINUX)
	gtk_dialog_ = NULL;
#endif

	// Read command line settings.
	CefRefPtr<CefCommandLine> command_line =
		CefCommandLine::GetGlobalCommandLine();

	m_bShowDevTools = command_line->HasSwitch(cefclient::kShowDevTools);

	mouse_cursor_change_disabled_ =
		command_line->HasSwitch(switches::kMouseCursorChangeDisabled);

	m_hWnd = hWnd;
	m_sBackUrl = _T("");
	m_LoadDataStr = "";
	m_bSetFocus = true;
}

ClientHandler::~ClientHandler() {
}

void ClientHandler::OnBeforeContextMenu(
										CefRefPtr<CefBrowser> browser,
										CefRefPtr<CefFrame> frame,
										CefRefPtr<CefContextMenuParams> params,
										CefRefPtr<CefMenuModel> model)
{
	CEF_REQUIRE_UI_THREAD();
	if ((params->GetTypeFlags() & (CM_TYPEFLAG_PAGE | CM_TYPEFLAG_FRAME)) != 0) 
	{
		if ( params->GetTypeFlags() & CM_TYPEFLAG_PAGE )  
		{

			//普通页面的右键消息  
			//model->SetLabel(MENU_ID_BACK, L"后退");  
			//model->SetLabel(MENU_ID_FORWARD, L"前进");  
			model->SetLabel(MENU_ID_VIEW_SOURCE, L"查看源代码");  
			model->SetLabel(MENU_ID_PRINT, L"打印");  
			model->SetLabel(MENU_ID_RELOAD, L"刷新");  
			model->SetLabel(MENU_ID_RELOAD_NOCACHE, L"强制刷新");  
			model->SetLabel(MENU_ID_STOPLOAD, L"停止加载");  
			model->SetLabel(MENU_ID_REDO, L"重复");  
			model->Remove(MENU_ID_BACK);
			model->Remove(MENU_ID_FORWARD);
			model->Remove(MENU_ID_VIEW_SOURCE);

			if (model->GetCount() > 0)
				model->AddSeparator();
			model->AddItem(CLIENT_ID_REFRESH, L"刷新");
		} 

		if ( params->GetTypeFlags() & CM_TYPEFLAG_EDITABLE)  
		{
			//编辑框的右键消息  
			model->SetLabel(MENU_ID_UNDO, L"撤销");  
			model->SetLabel(MENU_ID_REDO, L"重做");  
			model->SetLabel(MENU_ID_CUT, L"剪切");  
			model->SetLabel(MENU_ID_COPY, L"复制");  
			model->SetLabel(MENU_ID_PASTE, L"粘贴");  
			model->SetLabel(MENU_ID_DELETE, L"删除");  
			model->SetLabel(MENU_ID_SELECT_ALL, L"全选");  
		} 

		if (m_bShowDevTools)
		{
			model->AddItem(CLIENT_ID_SHOW_DEVTOOLS, "&Show DevTools");
		}

		// Test context menu features.
		//	BuildTestMenu(model);
	}
}

void ParseFillIndustry(string strMerchData, CString& strIndustryOut)
{
	vector<T_IndustryData> vIndustryData;
	vIndustryData.clear();

	Json::Value root;
	Json::Value arrayObj;
	Json::Value item;

	Json::Reader reader;
	Json::Value value;
	if (reader.parse(strMerchData.c_str(), value))
	{   
		if (value.size() <= 0)
		{
			strIndustryOut = _T("");
			return;
		}

		if (!value.isMember("MerchData")) 
		{
			strIndustryOut = _T("");
			return;
		}
		const Json::Value arrayData = value["MerchData"];
		for (unsigned int i = 0; i < arrayData.size(); i++)
		{   
			int iMarketId = 0;		
			T_IndustryData stIndustryData;

			if (!arrayData[i].isMember("MarketId")) 
				continue;
			stIndustryData.iMarketId = arrayData[i]["MarketId"].asInt();

			if (!arrayData[i].isMember("MerchCode")) 
				continue;
			stIndustryData.strCode = arrayData[i]["MerchCode"].asCString();

			if (!arrayData[i].isMember("MerchName")) 
				continue;
			stIndustryData.strCnName = arrayData[i]["MerchName"].asCString();

			if (!arrayData[i].isMember("MarketVlue")) 
				continue;
			stIndustryData.iMarketVlue = arrayData[i]["MarketVlue"].asInt();

			vIndustryData.push_back(stIndustryData);
		}   

		arrayObj.clear();
		for (int i = 0; i < vIndustryData.size(); ++i)
		{
			CString StrIndustry = L"";
			CString strMerchCode = vIndustryData.at(i).strCode.c_str();
			// 得到所属板块信息
			CBlockCollection::BlockArray aBlocks;
			CBlockConfig::Instance()->GetBlocksByMerch(vIndustryData.at(i).iMarketId, strMerchCode, aBlocks);
			for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
			{
				if ( aBlocks[i]->m_blockCollection.m_StrName == L"行业板块" )
				{
					StrIndustry = aBlocks[i]->m_blockInfo.m_StrBlockName;
				}
			}

			// 不管找不找的到，都写进去
			item.clear();
			item["MarketId"] = vIndustryData.at(i).iMarketId;
			item["MerchCode"] = vIndustryData.at(i).strCode;
			item["MerchName"] = vIndustryData.at(i).strCnName;
			string strIndustryTemp = CStringA(StrIndustry);
			item["Industry"] = strIndustryTemp;
			item["MarketVlue"] = vIndustryData.at(i).iMarketVlue;
			arrayObj.append(item);
		}
		root["MerchData"] = arrayObj;
	} 

	string strWebDisData = root.toStyledString();

	int index = 0;
	if( !strWebDisData.empty())
	{
		while( (index = strWebDisData.find('\n',index)) != string::npos)
		{
			strWebDisData.erase(index,1);
		}
	}

	strIndustryOut = strWebDisData.c_str();
	if (0 == vIndustryData.size())
	{
		strIndustryOut.Empty();
	}
}

bool ClientHandler::OnContextMenuCommand(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefContextMenuParams> params,
	int command_id,
	EventFlags event_flags) {
		CEF_REQUIRE_UI_THREAD();

		switch (command_id) 
		{
		case CLIENT_ID_SHOW_DEVTOOLS:
			ShowDevTools(browser, CefPoint());
			return true;
		case CLIENT_ID_REFRESH:
			{
				// 文字直播特殊处理
				CefString sUrl = browser->GetMainFrame()->GetURL();
				CString StrUrl = sUrl.c_str();
				CString StrRoomId = _T(""), StrFlag = _T("");

				UrlParser uParser(StrUrl); 
				StrRoomId = uParser.GetQueryValue(L"roomid");
				StrFlag = uParser.GetQueryValue(L"question");

				if(!StrFlag.IsEmpty() && !StrRoomId.IsEmpty())
				{
					uParser.SetQueryValue(L"roomid", L"");
					uParser.SetQueryValue(L"question", L"");
					StrUrl = uParser.GetUrl();

					browser->GetMainFrame()->LoadURL(_W2A(StrUrl));
				}
				else
				{
					browser->Reload();
				}

				return true;
			}
		default:  // Allow default handling, if any.
			return ExecuteTestMenu(command_id);
		}
}

bool ClientHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
									 const CefString& message,
									 const CefString& source,
									 int line) {
	CEF_REQUIRE_UI_THREAD();
/*
	 bool first_message;
	 std::string logFile;

	 {
		 first_message = log_file_.empty();
		 if (first_message) {
			 std::stringstream ss;
			 ss << "D:\\stone";
#if defined(OS_WIN)
			 ss << "\\";
#else
			 ss << "/";
#endif
			 ss << "console.log";
			 log_file_ = ss.str();
		 }
		 logFile = log_file_;
	 }

	 FILE* file = fopen(logFile.c_str(), "a");
	 if (file) {
		 std::stringstream ss;
		 ss << "Message: " << std::string(message) << "\r\nSource: " <<
			 std::string(source) << "\r\nLine: " << line <<
			 "\r\n-----------------------\r\n";
		 fputs(ss.str().c_str(), file);
		 fclose(file);

		 if (first_message)
			 SendNotification(NOTIFY_CONSOLE_MESSAGE);
	 }
	 */

	 return false;
}

void ClientHandler::OnBeforeDownload(
									 CefRefPtr<CefBrowser> browser,
									 CefRefPtr<CefDownloadItem> download_item,
									 const CefString& suggested_name,
									 CefRefPtr<CefBeforeDownloadCallback> callback) {
	 CEF_REQUIRE_UI_THREAD();

	 // Continue the download and show the "Save As" dialog.
	 callback->Continue(GetDownloadPath(suggested_name), true);
}

void ClientHandler::OnDownloadUpdated(
									  CefRefPtr<CefBrowser> browser,
									  CefRefPtr<CefDownloadItem> download_item,
									  CefRefPtr<CefDownloadItemCallback> callback) {
	  CEF_REQUIRE_UI_THREAD();

	  if (download_item->IsComplete()) {
		  SetLastDownloadFile(download_item->GetFullPath());
		  SendNotification(NOTIFY_DOWNLOAD_COMPLETE);
	  }
}

bool ClientHandler::OnDragEnter(CefRefPtr<CefBrowser> browser,
								CefRefPtr<CefDragData> dragData,
								CefDragHandler::DragOperationsMask mask) {
	CEF_REQUIRE_UI_THREAD();

	// Forbid dragging of link URLs.
	if (mask & DRAG_OPERATION_LINK)
		return true;

	return false;
}

bool ClientHandler::OnRequestGeolocationPermission(
	CefRefPtr<CefBrowser> browser,
	const CefString& requesting_url,
	int request_id,
	CefRefPtr<CefGeolocationCallback> callback) {
	CEF_REQUIRE_UI_THREAD();

	// Allow geolocation access from all websites.
	callback->Continue(true);
	return true;
}

#if !defined(OS_LINUX)

bool ClientHandler::OnJSDialog(CefRefPtr<CefBrowser> browser,
							   const CefString& origin_url,
							   const CefString& accept_lang,
							   JSDialogType dialog_type,
							   const CefString& message_text,
							   const CefString& default_prompt_text,
							   CefRefPtr<CefJSDialogCallback> callback,
							   bool& suppress_message) {
   CEF_REQUIRE_UI_THREAD();
   return false;
}

bool ClientHandler::OnBeforeUnloadDialog(
	CefRefPtr<CefBrowser> browser,
	const CefString& message_text,
	bool is_reload,
	CefRefPtr<CefJSDialogCallback> callback) {
	CEF_REQUIRE_UI_THREAD();
	return false;
}

void ClientHandler::OnResetDialogState(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();
}

#endif  // !defined(OS_LINUX)

bool ClientHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
								  const CefKeyEvent& event,
								  CefEventHandle os_event,
 								  bool* is_keyboard_shortcut) {
// 	  CEF_REQUIRE_UI_THREAD();
// 
// 	  if (!event.focus_on_editable_field && event.windows_key_code == 0x20) {
// 		  // Special handling for the space character when an input element does not
// 		  // have focus. Handling the event in OnPreKeyEvent() keeps the event from
// 		  // being processed in the renderer. If we instead handled the event in the
// 		  // OnKeyEvent() method the space key would cause the window to scroll in
// 		  // addition to showing the alert box.
// 		  if (event.type == KEYEVENT_RAWKEYDOWN) {
// 			  browser->GetMainFrame()->ExecuteJavaScript(
// 				  "alert('You pressed the space bar!');", "", 0);
// 		  }
// 		  return true;
// 	  }

	  return false;
}

bool ClientHandler::OnBeforePopup(
								  CefRefPtr<CefBrowser> browser,
								  CefRefPtr<CefFrame> frame,
								  const CefString& target_url,
								  const CefString& target_frame_name,
								  CefLifeSpanHandler::WindowOpenDisposition target_disposition,
								  bool user_gesture,
								  const CefPopupFeatures& popupFeatures,
								  CefWindowInfo& windowInfo,
								  CefRefPtr<CefClient>& client,
								  CefBrowserSettings& settings,
								  bool* no_javascript_access)
{
	CEF_REQUIRE_IO_THREAD();

	// 在这里处理点击网页内链接的消息

	T_NewsContentInfo newsContent;
	newsContent.m_StrContent = target_url.c_str();
	newsContent.m_StrTitle = newsContent.m_StrContent;
//	newsContent.m_pWindowInfo = &windowInfo;

	HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
	::SendMessage(hWnd, WM_SHOW_JPNEWS, (WPARAM)&newsContent, 0);

	return true;
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	if (!message_router_) {
		// Create the browser-side router for query handling.
		CefMessageRouterConfig config;
		message_router_ = CefMessageRouterBrowserSide::Create(config);

		// Register handlers with the router.
		CreateMessageHandlers(message_handler_set_);
		MessageHandlerSet::const_iterator it = message_handler_set_.begin();
		for (; it != message_handler_set_.end(); ++it)
			message_router_->AddHandler(*(it), false);
	}

	// Disable mouse cursor change if requested via the command-line flag.
	if (mouse_cursor_change_disabled_)
		browser->GetHost()->SetMouseCursorChangeDisabled(true);

	if (!GetBrowser())   {
		base::AutoLock lock_scope(lock_);
		// We need to keep the main child window, but not popup windows
		browser_ = browser;
		browser_id_ = browser->GetIdentifier();
	} else if (browser->IsPopup()) {
		// Add to the list of popup browsers.
		popup_browsers_.push_back(browser);

		// Give focus to the popup browser. Perform asynchronously because the
		// parent window may attempt to keep focus after launching the popup.
		CefPostTask(TID_UI,
			base::Bind(&CefBrowserHost::SetFocus, browser->GetHost().get(), true));
	}

	browser_count_++;

	if (NULL != m_hWnd)
	{
		::SendMessage(m_hWnd, WM_COMMAND, UM_CEF_AFTERCREATED, 0);
	}
}

bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed destription of this
	// process.
	if (GetBrowserId() == browser->GetIdentifier()) {
		base::AutoLock lock_scope(lock_);
		// Set a flag to indicate that the window close should be allowed.
		is_closing_ = true;
	}

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	message_router_->OnBeforeClose(browser);

	if (GetBrowserId() == browser->GetIdentifier()) {
		{
			base::AutoLock lock_scope(lock_);
			// Free the browser pointer so that the browser can be destroyed
			browser_ = NULL;
		}

		if (osr_handler_.get()) {
			osr_handler_->OnBeforeClose(browser);
			osr_handler_ = NULL;
		}
	} else if (browser->IsPopup()) {
		// Remove from the browser popup list.
		BrowserList::iterator bit = popup_browsers_.begin();
		for (; bit != popup_browsers_.end(); ++bit) {
			if ((*bit)->IsSame(browser)) {
				popup_browsers_.erase(bit);
				break;
			}
		}
	}

	if (--browser_count_ == 0) {
		// All browser windows have closed.
		// Remove and delete message router handlers.
		MessageHandlerSet::const_iterator it = message_handler_set_.begin();
		for (; it != message_handler_set_.end(); ++it) {
			message_router_->RemoveHandler(*(it));
			delete *(it);
		}
		message_handler_set_.clear();
		message_router_ = NULL;

		// Quit the application message loop.
		CefQuitMessageLoop();
	}
}

void ClientHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
										 bool isLoading,
										 bool canGoBack,
										 bool canGoForward) {
	 CEF_REQUIRE_UI_THREAD();

	 SetLoading(isLoading);
	 SetNavState(canGoBack, canGoForward);
}

void ClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
								CefRefPtr<CefFrame> frame,
								ErrorCode errorCode,
								const CefString& errorText,
								const CefString& failedUrl) {
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
	{
		return;
	}

	// Don't display an error for external protocols that we allow the OS to
	// handle. See OnProtocolExecution().
	if (errorCode == ERR_UNKNOWN_URL_SCHEME) 
	{
		std::string urlStr = frame->GetURL();
		if (0 == urlStr.find(KStrPecialPro))
		{
			return;
		}
	}

	//// Display a load error message.
	//std::stringstream ss;
	//ss << "<html><body bgcolor=\"white\">"
	//	"<h2>Failed to load URL " << std::string(failedUrl) <<
	//	" with error " << std::string(errorText) << " (" << errorCode <<
	//	").</h2></body></html>";
	//frame->LoadString(ss.str(), failedUrl);

	// 失败了加载默认网页
	{
		TCHAR szPath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szPath, MAX_PATH);
		CString StrPath = szPath;

		int pos = StrPath.ReverseFind('\\');
		if (pos > 0)
		{ 
			StrPath = StrPath.Left(pos + 1);
			StrPath = _T("file:///") + StrPath;
			StrPath += _T("html/error.html");

			m_sBackUrl = failedUrl;
			frame->LoadURL(_W2A(StrPath));
		}
	}
}

bool ClientHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
								   CefRefPtr<CefFrame> frame,
								   CefRefPtr<CefRequest> request,
								   bool is_redirect)
{
	CEF_REQUIRE_UI_THREAD();

// 	HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
// 	::SendMessage(hWnd, WM_BROWER_PTR, (WPARAM)browser.get(), 0);

	// 屏蔽获取焦点，避免失去上层焦点
	if (m_bSetFocus)
	{
		browser->GetHost()->SetFocus(true);
	}
	else
	{
		AfxGetApp()->GetMainWnd()->SetFocus();
	}

	// 特殊的请求协议
	std::string url = request->GetURL().ToString();
	return (0 == url.find(KStrPecialPro));
}

bool ClientHandler::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
								   const CefString& origin_url,
								   CefInt64 new_size,
								   CefRefPtr<CefRequestCallback> callback) {
									   CEF_REQUIRE_IO_THREAD();

	static const CefInt64 max_size = 1024 * 1024 * 20;  // 20mb.

	// Grant the quota request if the size is reasonable.
	callback->Continue(new_size <= max_size);
	return true;
}


void ClientHandler::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
										const CefString& url,
										bool& allow_os_execution) {
	CEF_REQUIRE_UI_THREAD();
	std::string urlStr = url;
	if (0 == urlStr.find(KStrPecialPro))
	{
		allow_os_execution = true;
	}
}

void ClientHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
											  TerminationStatus status) {
	  CEF_REQUIRE_UI_THREAD();

	  message_router_->OnRenderProcessTerminated(browser);

	  // Load the startup URL if that's not the website that we terminated on.
	  CefRefPtr<CefFrame> frame = browser->GetMainFrame();
	  std::string url = frame->GetURL();
	  std::transform(url.begin(), url.end(), url.begin(), tolower);

	  std::string startupURL = GetStartupURL();
	  if (startupURL != "chrome://crash" && !url.empty() &&
		  url.find(startupURL) != 0) {
			  frame->LoadURL(startupURL);
	  }
}

CefRefPtr<CefResourceHandler> ClientHandler::GetResourceHandler(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request) {
		CEF_REQUIRE_IO_THREAD();
		/*
		//if (request->GetURL() == "http://m.baidu.com/")
		if (request->GetResourceType() == RT_XHR)  
		{  
		CefRefPtr<ClientResourceHandler> resHandler = new ClientResourceHandler();  
		resHandler->_browser = browser;  
		resHandler->_frame = frame;  
		resHandler->_clientHandler = this;  

		return  resHandler;  
		}  
		*/
		return NULL;  
}


bool ClientHandler::GetRootScreenRect(CefRefPtr<CefBrowser> browser,
									  CefRect& rect) {
	CEF_REQUIRE_UI_THREAD();
	if (!osr_handler_.get())
	  return false;
	return osr_handler_->GetRootScreenRect(browser, rect);
}

bool ClientHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
	CEF_REQUIRE_UI_THREAD();
	if (!osr_handler_.get())
		return false;
	return osr_handler_->GetViewRect(browser, rect);
}

bool ClientHandler::GetScreenPoint(CefRefPtr<CefBrowser> browser,
								   int viewX,
								   int viewY,
								   int& screenX,
								   int& screenY) {
	   CEF_REQUIRE_UI_THREAD();
	   if (!osr_handler_.get())
		   return false;
	   return osr_handler_->GetScreenPoint(browser, viewX, viewY, screenX, screenY);
}

bool ClientHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser,
								  CefScreenInfo& screen_info) {
	  CEF_REQUIRE_UI_THREAD();
	  if (!osr_handler_.get())
		  return false;
	  return osr_handler_->GetScreenInfo(browser, screen_info);
}

void ClientHandler::OnPopupShow(CefRefPtr<CefBrowser> browser,
								bool show) {
	CEF_REQUIRE_UI_THREAD();
	if (!osr_handler_.get())
		return;
	return osr_handler_->OnPopupShow(browser, show);
}

void ClientHandler::OnPopupSize(CefRefPtr<CefBrowser> browser,
								const CefRect& rect) {
	CEF_REQUIRE_UI_THREAD();
	if (!osr_handler_.get())
		return;
	return osr_handler_->OnPopupSize(browser, rect);
}

void ClientHandler::OnPaint(CefRefPtr<CefBrowser> browser,
							PaintElementType type,
							const RectList& dirtyRects,
							const void* buffer,
							int width,
							int height) {
	CEF_REQUIRE_UI_THREAD();
	if (!osr_handler_.get())
		return;
	osr_handler_->OnPaint(browser, type, dirtyRects, buffer, width, height);
}

void ClientHandler::OnCursorChange(CefRefPtr<CefBrowser> browser,
								   CefCursorHandle cursor,
								   CursorType type,
								   const CefCursorInfo& custom_cursor_info) {
	CEF_REQUIRE_UI_THREAD();
	if (!osr_handler_.get())
	   return;
	osr_handler_->OnCursorChange(browser, cursor, type, custom_cursor_info);
}

bool ClientHandler::StartDragging(CefRefPtr<CefBrowser> browser,
								  CefRefPtr<CefDragData> drag_data,
								  CefRenderHandler::DragOperationsMask allowed_ops,
								  int x, int y) {
	CEF_REQUIRE_UI_THREAD();
	if (!osr_handler_.get())
	  return false;
	return osr_handler_->StartDragging(browser, drag_data, allowed_ops, x, y);
}

void ClientHandler::UpdateDragCursor(CefRefPtr<CefBrowser> browser,
									 CefRenderHandler::DragOperation operation) {
	CEF_REQUIRE_UI_THREAD();
	if (!osr_handler_.get())
	 return;
	osr_handler_->UpdateDragCursor(browser, operation);
}

void ClientHandler::SetMainWindowHandle(ClientWindowHandle handle) {
	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
			base::Bind(&ClientHandler::SetMainWindowHandle, this, handle));
		return;
	}

	main_handle_ = handle;
}

ClientWindowHandle ClientHandler::GetMainWindowHandle() const {
	CEF_REQUIRE_UI_THREAD();
	return main_handle_;
}

void ClientHandler::SetEditWindowHandle(ClientWindowHandle handle) {
	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
			base::Bind(&ClientHandler::SetEditWindowHandle, this, handle));
		return;
	}

	edit_handle_ = handle;
}

void ClientHandler::SetButtonWindowHandles(ClientWindowHandle backHandle,
										   ClientWindowHandle forwardHandle,
										   ClientWindowHandle reloadHandle,
										   ClientWindowHandle stopHandle) {
	if (!CefCurrentlyOn(TID_UI)) {
	   // Execute on the UI thread.
	   CefPostTask(TID_UI,
		   base::Bind(&ClientHandler::SetButtonWindowHandles, this,
		   backHandle, forwardHandle, reloadHandle, stopHandle));
	   return;
	}

	back_handle_ = backHandle;
	forward_handle_ = forwardHandle;
	reload_handle_ = reloadHandle;
	stop_handle_ = stopHandle;
}

void ClientHandler::SetOSRHandler(CefRefPtr<RenderHandler> handler) {
	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
			base::Bind(&ClientHandler::SetOSRHandler, this, handler));
		return;
	}

	osr_handler_ = handler;
}

CefRefPtr<ClientHandler::RenderHandler> ClientHandler::GetOSRHandler() const {
	return osr_handler_; 
}

CefRefPtr<CefBrowser> ClientHandler::GetBrowser() const {
	base::AutoLock lock_scope(lock_);
	return browser_;
}

int ClientHandler::GetBrowserId() const {
	base::AutoLock lock_scope(lock_);
	return browser_id_;
}

void ClientHandler::CloseAllBrowsers(bool force_close) {
	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
			base::Bind(&ClientHandler::CloseAllBrowsers, this, force_close));
		return;
	}

	if (!popup_browsers_.empty()) {
		// Request that any popup browsers close.
		BrowserList::const_iterator it = popup_browsers_.begin();
		for (; it != popup_browsers_.end(); ++it)
			(*it)->GetHost()->CloseBrowser(force_close);
	}

	if (browser_.get()) {
		// Request that the main browser close.
		browser_->GetHost()->CloseBrowser(force_close);
	}
}

bool ClientHandler::IsClosing() const {
	base::AutoLock lock_scope(lock_);
	return is_closing_;
}

std::string ClientHandler::GetLogFile() const {
	CEF_REQUIRE_UI_THREAD();
	return log_file_;
}

void ClientHandler::SetLastDownloadFile(const std::string& fileName) {
	CEF_REQUIRE_UI_THREAD();
	last_download_file_ = fileName;
}

std::string ClientHandler::GetLastDownloadFile() const {
	CEF_REQUIRE_UI_THREAD();
	return last_download_file_;
}

void ClientHandler::ShowDevTools(CefRefPtr<CefBrowser> browser,
								 const CefPoint& inspect_element_at) {
	CefWindowInfo windowInfo;
	CefBrowserSettings settings;

#if defined(OS_WIN)
	windowInfo.SetAsPopup(browser->GetHost()->GetWindowHandle(), "DevTools");
#endif

	browser->GetHost()->ShowDevTools(windowInfo, this, settings,
	 inspect_element_at);
}

void ClientHandler::CloseDevTools(CefRefPtr<CefBrowser> browser) {
	browser->GetHost()->CloseDevTools();
}

std::string ClientHandler::GetStartupURL() const {
	return startup_url_;
}

void ClientHandler::BeginTracing() {
	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI, base::Bind(&ClientHandler::BeginTracing, this));
		return;
	}

	CefBeginTracing(CefString(), NULL);
}

void ClientHandler::EndTracing() {
	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI, base::Bind(&ClientHandler::EndTracing, this));
		return;
	}

	class Client : public CefEndTracingCallback,
		public CefRunFileDialogCallback {
	public:
		explicit Client(CefRefPtr<ClientHandler> handler)
			: handler_(handler) {
				RunDialog();
		}

		void RunDialog() {
			static const char kDefaultFileName[] = "trace.txt";
			std::string path = handler_->GetDownloadPath(kDefaultFileName);
			if (path.empty())
				path = kDefaultFileName;

			// Results in a call to OnFileDialogDismissed.
			handler_->GetBrowser()->GetHost()->RunFileDialog(
				FILE_DIALOG_SAVE, CefString(), path, std::vector<CefString>(),0,
				this);
		}

		void OnFileDialogDismissed(
			int selected_accept_filter,
			const std::vector<CefString>& file_paths) OVERRIDE {
				if (!file_paths.empty()) {
					// File selected. Results in a call to OnEndTracingComplete.
					CefEndTracing(file_paths.front(), this);
				} else {
					// No file selected. Discard the trace data.
					CefEndTracing(CefString(), NULL);
				}
		}

		virtual void OnEndTracingComplete(
			const CefString& tracing_file) OVERRIDE {
				CEF_REQUIRE_UI_THREAD();
				handler_->SetLastDownloadFile(tracing_file.ToString());
				handler_->SendNotification(NOTIFY_DOWNLOAD_COMPLETE);
		}

	private:
		CefRefPtr<ClientHandler> handler_;

		IMPLEMENT_REFCOUNTING(Client);
	};

	new Client(this);
}

bool ClientHandler::Save(const std::string& path, const std::string& data) {
	FILE* f = fopen(path.c_str(), "w");
	if (!f)
		return false;
	size_t total = 0;
	do {
		size_t write = fwrite(data.c_str() + total, 1, data.size() - total, f);
		if (write == 0)
			break;
		total += write;
	} while (total < data.size());
	fclose(f);
	return true;
}

// static
void ClientHandler::CreateMessageHandlers(MessageHandlerSet& handlers) {
	//// Create the dialog test handlers.
	//dialog_test::CreateMessageHandlers(handlers);

	//// Create the binding test handlers.
	//binding_test::CreateMessageHandlers(handlers);

	//// Create the window test handlers.
	//window_test::CreateMessageHandlers(handlers);
}

void ClientHandler::BuildTestMenu(CefRefPtr<CefMenuModel> model) {
	if (model->GetCount() > 0)
		model->AddSeparator();

	// Build the sub menu.
	CefRefPtr<CefMenuModel> submenu =
		model->AddSubMenu(CLIENT_ID_TESTMENU_SUBMENU, "Context Menu Test");
	submenu->AddCheckItem(CLIENT_ID_TESTMENU_CHECKITEM, "Check Item");
	submenu->AddRadioItem(CLIENT_ID_TESTMENU_RADIOITEM1, "Radio Item 1", 0);
	submenu->AddRadioItem(CLIENT_ID_TESTMENU_RADIOITEM2, "Radio Item 2", 0);
	submenu->AddRadioItem(CLIENT_ID_TESTMENU_RADIOITEM3, "Radio Item 3", 0);

	// Check the check item.
	if (test_menu_state_.check_item)
		submenu->SetChecked(CLIENT_ID_TESTMENU_CHECKITEM, true);

	// Check the selected radio item.
	submenu->SetChecked(
		CLIENT_ID_TESTMENU_RADIOITEM1 + test_menu_state_.radio_item, true);
}

bool ClientHandler::ExecuteTestMenu(int command_id) {
	if (command_id == CLIENT_ID_TESTMENU_CHECKITEM) {
		// Toggle the check item.
		test_menu_state_.check_item ^= 1;
		return true;
	} else if (command_id >= CLIENT_ID_TESTMENU_RADIOITEM1 &&
		command_id <= CLIENT_ID_TESTMENU_RADIOITEM3) {
			// Store the selected radio item.
			test_menu_state_.radio_item = (command_id - CLIENT_ID_TESTMENU_RADIOITEM1);
			return true;
	}

	// Allow default handling to proceed.
	return false;
}

void ClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
									CefRefPtr<CefFrame> frame,
									const CefString& url) {
	CEF_REQUIRE_UI_THREAD();

	if (GetBrowserId() == browser->GetIdentifier() && frame->IsMain()) {
		// Set the edit window text
		SetWindowText(edit_handle_, std::wstring(url).c_str());
	}
}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
								  const CefString& title)
{
	CEF_REQUIRE_UI_THREAD();
	// Set the frame window title bar
	CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
	if (GetBrowserId() == browser->GetIdentifier())
	{
		// The frame window will be the parent of the browser window
		hwnd = GetParent(hwnd);
	}
	SetWindowText(hwnd, std::wstring(title).c_str());

	CefString str = browser->GetMainFrame()->GetURL();	// 链接地址，用于判断修改哪个tab的标题
	HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
	::SendMessage(hWnd,WM_JPNEWS_TITLE,(WPARAM)title.c_str(),(LPARAM)str.c_str());
}

void ClientHandler::SendNotification(NotificationType type) {
// 	UINT id;
// 	switch (type) {
//   case NOTIFY_CONSOLE_MESSAGE:
// 	  id = ID_WARN_CONSOLEMESSAGE;
// 	  break;
//   case NOTIFY_DOWNLOAD_COMPLETE:
// 	  id = ID_WARN_DOWNLOADCOMPLETE;
// 	  break;
//   case NOTIFY_DOWNLOAD_ERROR:
// 	  id = ID_WARN_DOWNLOADERROR;
// 	  break;
//   default:
// 	  return;
// 	}
// 	PostMessage(main_handle_, WM_COMMAND, id, 0);
}

void ClientHandler::SetLoading(bool isLoading)
{
	//DCHECK(edit_handle_ != NULL && reload_handle_ != NULL && stop_handle_ != NULL);
	//EnableWindow(edit_handle_, TRUE);
	//EnableWindow(reload_handle_, !isLoading);
	//EnableWindow(stop_handle_, isLoading);

	if (NULL != edit_handle_)
	{
		EnableWindow(edit_handle_, TRUE);
	}
	if (NULL != reload_handle_)
	{
		EnableWindow(reload_handle_, !isLoading);
	}
	if (NULL != stop_handle_)
	{
		EnableWindow(stop_handle_, isLoading);
	}
}

void ClientHandler::SetNavState(bool canGoBack, bool canGoForward) {
	//DCHECK(back_handle_ != NULL && forward_handle_ != NULL);
	//EnableWindow(back_handle_, canGoBack);
	//EnableWindow(forward_handle_, canGoForward);

	if (NULL != back_handle_)
	{
		EnableWindow(back_handle_, canGoBack);
	}
	if (NULL != forward_handle_)
	{
		EnableWindow(forward_handle_, canGoForward);
	}
}

std::string ClientHandler::GetDownloadPath(const std::string& file_name) {
	TCHAR szFolderPath[MAX_PATH];
	std::string path;

	// Save the file in the user's "My Documents" folder.
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE,
		NULL, 0, szFolderPath))) {
			path = CefString(szFolderPath);
			path += "\\" + file_name;
	}

	return path;
}

bool ClientHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
											 CefProcessId source_process,
											 CefRefPtr<CefProcessMessage> message)
{
	const std::string& messageName = message->GetName();

	if (messageName == "TradeLogin")
	{
		HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		::SendMessage(hWnd, WM_COMMAND, UM_SIMULATE_TRADE_SHOW, 0);
		return true;
	}
	else if (messageName == "OpenAccount")
	{
		HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		::SendMessage(hWnd, WM_COMMAND, UM_SIMULATE_TRADE_OPENACCOUNT, 0);
		return true;
	}
	else if (messageName == "Register")
	{
		HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		::SendMessage(hWnd, WM_COMMAND, UM_SIMULATE_TRADE_HQREG, 0);
		return true;
	}
	else if (messageName == "HQLogin")
	{
		HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		int32 iChoose = IDNO;
		CString StrPrompt = L"是否退出应用程序，登录其它行情账号 ?";
		iChoose = ::MessageBox(hWnd, StrPrompt, AfxGetApp()->m_pszAppName, MB_YESNO);
		if (iChoose == IDYES)
		{
			::SendMessage(hWnd, WM_COMMAND, UM_SIMULATE_TRADE_HQLOGIN, 0);
		}
		return true;
	}
	else if(messageName == "myRegist")
	{
		HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		::SendMessage(hWnd, WM_COMMAND, UM_HQ_REGISTER_SUCCED, 0);
		return true;
	}
	else if (messageName == "ImageUrl")
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		CefString sUrl = args->GetString(0);
		CString strUrl = sUrl.c_str();

		HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		::SendMessage(hWnd, UM_SIMULATE_TRADE_IMGURL, (WPARAM)&strUrl, 0);

		return true;
	}
	else if ("ReLoginPC" == messageName)		// 重新登录PC客户端
	{
		CString StrUserName = _T("");
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		int iCnt = args->GetSize();				// 参数个数
		if (iCnt > 0)
		{
			CefString sUserName = args->GetString(0);
			StrUserName = sUserName.c_str();
		}

		HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		::SendMessage(hWnd, UM_ReLoginPC, (WPARAM)&StrUserName, 0);

		return true;
	}
	else if ("ThirdLoginRsp" == messageName)	// web三方登录返回
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		CefString sType = args->GetString(0);   // 类型（1 微信 2 QQ  3微博)
		CefString sCode = args->GetString(1);	// code
		CString StrType = sType.c_str();
		CString StrCode = sCode.c_str();

		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		HWND hWnd;
		if (NULL != pDoc->m_pDlgLogIn)
		{
			// 发送到登录界面
			hWnd = pDoc->m_pDlgLogIn->m_hWnd;
		}
		else
		{
			// 发送到主界面
			hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		}

		::SendMessage(hWnd, UM_ThirdLoginRsp, (WPARAM)&StrType, (LPARAM)&StrCode);
		return true;
	}
	else if ("PersonCenterRsp" == messageName)	// 个人中心返回信息
	{
		CString StrNickName = _T("");
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		int iCnt = args->GetSize();				// 参数个数
		if (iCnt > 0)
		{
			CefString sNickName = args->GetString(0);
			StrNickName = sNickName.c_str();
		}

		HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		::SendMessage(hWnd, UM_PersonCenterRsp, (WPARAM)&StrNickName, 0);

		return true;
	}
	else if ("NativeOpenCfm" == messageName)	// 打开一个指定的版面
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();

		T_WebOpenCfm *pstParam = new T_WebOpenCfm();
		int32 iSize = args->GetSize();
		if (1 == iSize)
		{
			CefString StrName = args->GetString(0);
			pstParam->StrCfmName = StrName.c_str();
		}
		else if (2 == iSize)
		{
			CefString StrName = args->GetString(0);
			CefString StrFlag = args->GetString(1);

			pstParam->StrCfmName = StrName.c_str();
			pstParam->StrFlag = StrFlag.c_str();		// 个股资讯Url
		}
		else if (3 == iSize)
		{
			CefString StrName = args->GetString(0);
			int32 iID = args->GetInt(1);
			CefString StrFlag = args->GetString(2);

			pstParam->StrCfmName = StrName.c_str();
			pstParam->iID = iID;
			pstParam->StrFlag = StrFlag.c_str();
		}

		HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		::PostMessage(hWnd, UM_NativeOpenCfm, (WPARAM)pstParam, 0);

		return true;
	}
	else if ("linkageWebTrend" == messageName)	// 打开分时
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();

		T_WebOpenTrend *pstParam = new T_WebOpenTrend();
		int32 iSize = args->GetSize();
		if (3 != iSize)
		{
			return false;		// 只要三个参数
		}

		int32 iMarketId = args->GetInt(0);
		CefString StrCode = args->GetString(1);
		CefString StrName = args->GetString(2);

		pstParam->iMarketId = iMarketId;
		pstParam->strCode   = StrCode.c_str();
		pstParam->strCnName = StrName.c_str();

		HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		::PostMessage(hWnd, UM_LinkageWebTrend, (WPARAM)pstParam, 0);

		return true;
	}
	else if ("queryIndustry" == messageName)	//请求板块信息
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();

		int32 iSize = args->GetSize();
		if (1 != iSize)
		{
			return false;	
		}

		CefString StrQuery = args->GetString(0);

		CStringA strReqData = CStringA(StrQuery.c_str());

		std::string strContent = strReqData;

		CString strOunt;
		ParseFillIndustry(strContent, strOunt);

		string strTransDataUtf8;
		UnicodeToUtf8(strOunt, strTransDataUtf8);

		char *pchData = NULL;
		// 给30个字节给函数名称
		pchData = new char[strTransDataUtf8.length() + 30];
		memset(pchData, 0, strTransDataUtf8.length() + 30);
		sprintf(pchData, "queryIndustryRsp('%s')", strTransDataUtf8.c_str());

		std::string strPassData = pchData;
		if (pchData)
		{
			delete[] pchData;
			pchData = NULL;
		}
		browser->GetMainFrame()->ExecuteJavaScript(strPassData,"about:blank" , 0);	// 链接地址，用于判断修改哪个tab的标题
		return true;
	}
	else if ("queryuserblockInfo" == messageName)	// 请求自选股数据
	{
		CMainFrame  * pMainFrame = (CMainFrame *)AfxGetApp()->GetMainWnd();
		if (pMainFrame)
		{

			string strJason;
			pMainFrame->PackUserBlockInfo(strJason);

			// 转换格式
			CString strUserInfo = strJason.c_str();
			string strTransDataUtf8;
			UnicodeToUtf8(strUserInfo, strTransDataUtf8);

			char *pchData = NULL;
			// 给30个字节给函数名称
			pchData = new char[strTransDataUtf8.length() + 30];
			memset(pchData, 0, strTransDataUtf8.length() + 30);
			sprintf(pchData, "getOptional('%s')", strTransDataUtf8.c_str());

			std::string strPassData = pchData;
			if (pchData)
			{
				delete[] pchData;
				pchData = NULL;
			}
			browser->GetMainFrame()->ExecuteJavaScript(strPassData,"about:blank" , 0);	// 链接地址，用于判断修改哪个tab的标题
		}

		return true;
	}
	else if ("addUserStock" == messageName)
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		CefString sCode		 = args->GetString(0);
		CefString sMarketid	 = args->GetString(1);
		bool	  bIsDel	 = args->GetBool(2);
		CString strCode		= sCode.c_str();
		CString strMarketid = sMarketid.c_str();


		// 参数太多了，就不发消息了。直接调用函数吧
		CMainFrame  * pMainFrame = (CMainFrame *)AfxGetApp()->GetMainWnd();
		if (pMainFrame)
		{
			pMainFrame->AddOrDelUserblockMerch(strCode, strMarketid, bIsDel);
		}

		return true;
	}
	else if ("queryUserInfoByWeb" == messageName)	//请求用户信息
	{
		CMainFrame  * pMainFrame = (CMainFrame *)AfxGetApp()->GetMainWnd();
		if (pMainFrame)
		{
			T_UserInfo  stUserInfo;
			pMainFrame->GetUserInfo(stUserInfo);

			Json::Value userInfo;
			userInfo["userId"] = stUserInfo.iUserId;
			userInfo["userMobile"] = _W2A(stUserInfo.wszMobile);
			string strWebDisData = userInfo.toStyledString();
			int index = 0;
			if( !strWebDisData.empty())
			{
				while( (index = strWebDisData.find('\n',index)) != string::npos)
				{
					strWebDisData.erase(index,1);
				}
			}

			// 转换格式
			CString strUserInfo = strWebDisData.c_str();
			string strTransDataUtf8;
			UnicodeToUtf8(strUserInfo, strTransDataUtf8);

			char *pchData = NULL;
			// 给30个字节给函数名称
			pchData = new char[strTransDataUtf8.length() + 30];
			memset(pchData, 0, strTransDataUtf8.length() + 30);
			sprintf(pchData, "queryUserInfoByWebRsp('%s')", strTransDataUtf8.c_str());

			std::string strPassData = pchData;
			if (pchData)
			{
				delete[] pchData;
				pchData = NULL;
			}
			browser->GetMainFrame()->ExecuteJavaScript(strPassData,"about:blank" , 0);	// 链接地址，用于判断修改哪个tab的标题
		}


		return true;
	}
	else if (messageName == "ToLogin")		// 回到登录界面
	{
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		HWND hWnd;
		if (NULL != pDoc->m_pDlgLogIn)
		{
			// 发送到登录界面
			hWnd = pDoc->m_pDlgLogIn->m_hWnd;
		}
		else
		{
			// 发送到主界面
			hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		}
		::SendMessage(hWnd, UM_CLOSE_DLG, 0, 0);
		return true;
	}
	else if ("OpenOutWeb" == messageName)
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		CefString sUrl = args->GetString(0);
		CString StrUrl = sUrl.c_str();

		HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		::SendMessage(hWnd, UM_OpenOutWeb, (WPARAM)&StrUrl, 0);

		return true;
	}
	else if ("ErrorBack" == messageName)
	{
		browser->GetMainFrame()->LoadURL(m_sBackUrl);
		return true;
	}
	else if ("ShowWebDlg" == messageName)
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		CefString sUrl = args->GetString(0);	// url
		CefString sWidth = args->GetString(1);	// width
		CefString sHeight = args->GetString(2);	// height

		T_ShowWebDlg *pstParam = new T_ShowWebDlg();
		pstParam->StrUrl = sUrl.c_str();
		pstParam->StrWidth = sWidth.c_str();
		pstParam->StrHeight = sHeight.c_str();

		HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		::SendMessage(hWnd, UM_ShowWebDlg, (WPARAM)pstParam, 0);

		return true;
	}
	else if ("CloseWebDlg" == messageName)		// 关闭对话框
	{
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		HWND hWnd;
		if (NULL != pDoc->m_pDlgLogIn)
		{
			// 发送到登录界面
			hWnd = pDoc->m_pDlgLogIn->m_hWnd;
		}
		else
		{
			// 发送到主界面
			hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		}
		::SendMessage(hWnd, UM_CLOSE_DLG, 1, 0);
		return true;
	}
	else if ("DownloadTrade" == messageName)	// 下载交易程序
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		CefString sJsonValue = args->GetString(0);	// json value
		CString StrJsonValue = sJsonValue.c_str();


		::SendMessage(GetParent(m_hWnd),UM_DownloadTrade,(WPARAM)&StrJsonValue ,0 );

		return true;
	}
	else if ("CloseOnlineServDlg" == messageName)
	{
		HWND hWnd;
		hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		::SendMessage(hWnd, UM_CloseOnlineServDlg, 0, 0);
	}
	else if ("executeNative" == messageName)
	{	

		CefRefPtr<CefListValue> args = message->GetArgumentList();
		if (args->GetSize() > 0)
		{
			CefString sJsonValue = args->GetString(0);	// json value
			CString StrJsonValue = sJsonValue.c_str();
			HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
			::SendMessage(hWnd, UM_WEB_COMMAND, (WPARAM)&StrJsonValue, 0);
		}
	}
	else if ("callNativeCallback" == messageName)
	{	
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		if (args->GetSize() > 0)
		{
			CefString sJsonValue = args->GetString(0);	// json value
			CString StrJsonValue = sJsonValue.c_str();
			HWND hWnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
			::SendMessage(hWnd, UM_WEB_CALL_BACK, (WPARAM)&StrJsonValue, 0);
		}
	}


	return false;
}
