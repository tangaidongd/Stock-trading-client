#ifndef CEF_TESTS_CEFCLIENT_CLIENT_APP_H_
#define CEF_TESTS_CEFCLIENT_CLIENT_APP_H_
#pragma once

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "include/cef_app.h"
#include "CEfV8Handler.h"

class ClientApp : public CefApp,
                  public CefBrowserProcessHandler,
                  public CefRenderProcessHandler {
public:
	ClientApp();

	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
		{ return this; }
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
		{ return this; }

	void OnWebKitInitialized();
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context);

	virtual void OnBeforeCommandLineProcessing(
		const CefString& process_type,
		CefRefPtr<CefCommandLine> command_line);

	IMPLEMENT_REFCOUNTING(ClientApp);
};

#endif  // CEF_TESTS_CEFCLIENT_CLIENT_APP_H_
