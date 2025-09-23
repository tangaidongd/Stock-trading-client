#ifndef CEF_TESTS_CEFCLIENT_CLIENT_APP_H_
#define CEF_TESTS_CEFCLIENT_CLIENT_APP_H_

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "include/cef_app.h"

class ClientApp : public CefApp,
	public CefBrowserProcessHandler,
	public CefRenderProcessHandler {
public:
	ClientApp();

	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
		OVERRIDE { return this; }
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
		OVERRIDE { return this; }

	virtual void OnBeforeCommandLineProcessing(
		const CefString& process_type,
		CefRefPtr<CefCommandLine> command_line) OVERRIDE;

	IMPLEMENT_REFCOUNTING(ClientApp);
public:
	int Init(HINSTANCE instance);
	void Exit();
};

#endif  // CEF_TESTS_CEFCLIENT_CLIENT_APP_H_
