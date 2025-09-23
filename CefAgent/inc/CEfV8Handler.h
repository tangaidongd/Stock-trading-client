#pragma once
#include "include/cef_client.h"
#include "include/cef_v8.h"

class CCefV8Handler : 
	public CefV8Handler
{
public:
	CCefV8Handler();
	virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, \
		const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCefV8Handler);
};
