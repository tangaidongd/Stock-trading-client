// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// This file is shared by cefclient and cef_unittests so don't include using
// a qualified path.
#include "stdafx.h"
#include "client_app.h"
#include <string>
#include "include/cef_cookie.h"
#include "include/cef_process_message.h"
#include "include/cef_task.h"
#include "include/cef_v8.h"
#include "include/cef_runnable.h"

ClientApp::ClientApp()
{
}

int ClientApp::Init(HINSTANCE instance)
{
	CefEnableHighDPISupport();
	CefMainArgs main_args(instance);
	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, this, NULL);
	if (exit_code >= 0) 
	{
		// The sub-process has completed so return here.
		return exit_code;
	}

    CefSettings settings;
    CefSettingsTraits::init(&settings);
    settings.single_process = false;
    settings.multi_threaded_message_loop = true;

    //	CefString(&settings.cache_path) = "mycookies";
    settings.persist_session_cookies = true;
   //	settings.remote_debugging_port = 8088;
    settings.log_severity = LOGSEVERITY_DISABLE; 
	settings.no_sandbox = true;
	settings.background_color = CefColorSetARGB(255, 35, 34, 40);
	CefString(&settings.browser_subprocess_path).FromASCII("CefAgent.exe");

    // Initialize CEF.
    CefInitialize(main_args, settings, this, NULL);
    return exit_code;
}

void ClientApp::Exit()
{
	CefShutdown();
}

void ClientApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line)
{
	__super::OnBeforeCommandLineProcessing(process_type, command_line);

	command_line->AppendSwitch("no-proxy-server");
//	command_line->AppendSwitch("--enable-system-flash");
	//同一个域下的使用同一个渲染进程
	command_line->AppendSwitch("process-per-site");
	command_line->AppendSwitch("disable-gpu");
	command_line->AppendSwitch("disable-gpu-compositing");
	// 支持flash播放
	command_line->AppendSwitchWithValue("ppapi-flash-version", "27.0.0.159");
	command_line->AppendSwitchWithValue("ppapi-flash-path", "plugins\\pepflashplayer.dll");

	//tchar  exepath[MAX_PATH];
	//CString  strdir,tmpdir; 
	//memset(exepath,0,MAX_PATH); 
	//GetModuleFileName(NULL,exepath,MAX_PATH); 
	//tmpdir=exepath; 
	//strdir=tmpdir.Left(tmpdir.ReverseFind('\\'));
	//strdir = strdir+ _T("\\plugins\\pepflashplayer.dll");

	//strdir += _T(";application/x-shockwave-flash");
	//command_line->AppendSwitchWithValue("--register-pepper-plugins",strdir.GetBuffer());
}