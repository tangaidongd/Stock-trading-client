#ifndef _PROXYAUTHCLIENTHELPER_H_
#define _PROXYAUTHCLIENTHELPER_H_

#include "proxy_auth_client_base.h"
#include <windows.h>
#include "Module.h"

class CProxyAuthClientHelper: public CTempldateHelperCheck<auth::iProxyAuthClient>
{
public:
#ifdef _DEBUG
	CProxyAuthClientHelper(void) : CTempldateHelperCheck<auth::iProxyAuthClient>(TEXT("ProxyAuthClientd.dll"), "GetProxyAuthClientObj", auth::g_iAuthVer) { }
#else
	CProxyAuthClientHelper(void) : CTempldateHelperCheck<auth::iProxyAuthClient>(TEXT("ProxyAuthClient.dll"), "GetProxyAuthClientObj", auth::g_iAuthVer) { }
#endif
};

#endif