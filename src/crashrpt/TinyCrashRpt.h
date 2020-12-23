/************************************************************************************* 
This file is a part of CrashRpt library.
Copyright (c) 2003-2013 The CrashRpt project authors. All Rights Reserved.

Use of this source code is governed by a BSD-style license
that can be found in the License.txt file in the root of the source
tree. All contributing project authors may
be found in the Authors.txt file in the root of the source tree.
***************************************************************************************/

/*! \file   CrashRpt.h
*  \brief  Defines the interface for the CrashRpt.DLL.
*  \date   2003
*  \author Michael Carruth 
*  \author Oleg Krivtsov (zeXspectrum) 
*/

#ifndef _CRASHRPT_H_
#define _CRASHRPT_H_

#ifndef __reserved
#define __reserved
#endif

#include <windows.h>
//#include <dbghelp.h>


typedef enum _MINIDUMP_TYPE { 
  MiniDumpNormal                          = 0x00000000,
  MiniDumpWithDataSegs                    = 0x00000001,
  MiniDumpWithFullMemory                  = 0x00000002,
  MiniDumpWithHandleData                  = 0x00000004,
  MiniDumpFilterMemory                    = 0x00000008,
  MiniDumpScanMemory                      = 0x00000010,
  MiniDumpWithUnloadedModules             = 0x00000020,
  MiniDumpWithIndirectlyReferencedMemory  = 0x00000040,
  MiniDumpFilterModulePaths               = 0x00000080,
  MiniDumpWithProcessThreadData           = 0x00000100,
  MiniDumpWithPrivateReadWriteMemory      = 0x00000200,
  MiniDumpWithoutOptionalData             = 0x00000400,
  MiniDumpWithFullMemoryInfo              = 0x00000800,
  MiniDumpWithThreadInfo                  = 0x00001000,
  MiniDumpWithCodeSegs                    = 0x00002000,
  MiniDumpWithoutAuxiliaryState           = 0x00004000,
  MiniDumpWithFullAuxiliaryState          = 0x00008000,
  MiniDumpWithPrivateWriteCopyMemory      = 0x00010000,
  MiniDumpIgnoreInaccessibleMemory        = 0x00020000,
  MiniDumpWithTokenInformation            = 0x00040000,
  MiniDumpWithModuleHeaders               = 0x00080000,
  MiniDumpFilterTriage                    = 0x00100000,
  MiniDumpValidTypeFlags                  = 0x001fffff
} MINIDUMP_TYPE;


typedef BOOL (CALLBACK *LPGETLOGFILE) (__reserved LPVOID lpvState);

/*! \ingroup CrashRptStructs
*  \struct CR_INSTALL_INFOA
*  \copydoc CR_INSTALL_INFOW
*/

typedef struct tagCR_INSTALL_INFOA
{
    WORD cb;                       //!< Size of this structure in bytes; must be initialized before using!
    LPCSTR pszAppName;             //!< Name of application.
    LPCSTR pszAppVersion;          //!< Application version.
    LPCSTR pszEmailTo;             //!< E-mail address of crash reports recipient.
    LPCSTR pszEmailSubject;        //!< Subject of crash report e-mail. 
    LPCSTR pszUrl;                 //!< URL of server-side script (used in HTTP connection).
    LPCSTR pszCrashSenderPath;     //!< Directory name where CrashSender.exe is located.
    LPGETLOGFILE pfnCrashCallback; //!< Deprecated, do not use.
    UINT uPriorities[5];           //!< Array of error sending transport priorities.
    DWORD dwFlags;                 //!< Flags.
    LPCSTR pszPrivacyPolicyURL;    //!< URL of privacy policy agreement.
    LPCSTR pszDebugHelpDLL;        //!< File name or folder of Debug help DLL.
    MINIDUMP_TYPE uMiniDumpType;   //!< Mini dump type.
    LPCSTR pszErrorReportSaveDir;  //!< Directory where to save error reports.
    LPCSTR pszRestartCmdLine;      //!< Command line for application restart (without executable name).
    LPCSTR pszLangFilePath;        //!< Path to the language file (including file name).
    LPCSTR pszEmailText;           //!< Custom E-mail text (used when deliverying report as E-mail).
    LPCSTR pszSmtpProxy;           //!< Network address and port to be used as SMTP proxy.
    LPCSTR pszCustomSenderIcon;    //!< Custom icon used for Error Report dialog.
	LPCSTR pszSmtpLogin;           //!< Login name used for SMTP authentication when sending error report as E-mail.
	LPCSTR pszSmtpPassword;        //!< Password used for SMTP authentication when sending error report as E-mail.	
	int nRestartTimeout;           //!< Timeout for application restart.
}
CR_INSTALL_INFOA;

typedef CR_INSTALL_INFOA* PCR_INSTALL_INFOA;


int WINAPI crInstallA(PCR_INSTALL_INFOA pInfo);


// Array indices for CR_INSTALL_INFO::uPriorities.
#define CR_HTTP 0  //!< Send error report via HTTP (or HTTPS) connection.
#define CR_SMTP 1  //!< Send error report via SMTP connection.
#define CR_SMAPI 2 //!< Send error report via simple MAPI (using default mail client).

//! Special priority constant that allows to skip certain delivery method.
#define CR_NEGATIVE_PRIORITY ((UINT)-1)

// Flags for CR_INSTALL_INFO::dwFlags
#define CR_INST_STRUCTURED_EXCEPTION_HANDLER      0x1 //!< Install SEH handler (deprecated name, use \ref CR_INST_SEH_EXCEPTION_HANDLER instead).
#define CR_INST_SEH_EXCEPTION_HANDLER             0x1 //!< Install SEH handler.
#define CR_INST_TERMINATE_HANDLER                 0x2 //!< Install terminate handler.
#define CR_INST_UNEXPECTED_HANDLER                0x4 //!< Install unexpected handler.
#define CR_INST_PURE_CALL_HANDLER                 0x8 //!< Install pure call handler (VS .NET and later).
#define CR_INST_NEW_OPERATOR_ERROR_HANDLER       0x10 //!< Install new operator error handler (VS .NET and later).
#define CR_INST_SECURITY_ERROR_HANDLER           0x20 //!< Install security error handler (VS .NET and later).
#define CR_INST_INVALID_PARAMETER_HANDLER        0x40 //!< Install invalid parameter handler (VS 2005 and later).
#define CR_INST_SIGABRT_HANDLER                  0x80 //!< Install SIGABRT signal handler.
#define CR_INST_SIGFPE_HANDLER                  0x100 //!< Install SIGFPE signal handler.   
#define CR_INST_SIGILL_HANDLER                  0x200 //!< Install SIGILL signal handler.  
#define CR_INST_SIGINT_HANDLER                  0x400 //!< Install SIGINT signal handler.  
#define CR_INST_SIGSEGV_HANDLER                 0x800 //!< Install SIGSEGV signal handler.
#define CR_INST_SIGTERM_HANDLER                0x1000 //!< Install SIGTERM signal handler.  

#define CR_INST_ALL_POSSIBLE_HANDLERS          0x1FFF //!< Install all possible exception handlers.
#define CR_INST_CRT_EXCEPTION_HANDLERS         0x1FFE //!< Install exception handlers for the linked CRT module.

#define CR_INST_NO_GUI                         0x2000 //!< Do not show GUI, send report silently (use for non-GUI apps only).
#define CR_INST_HTTP_BINARY_ENCODING           0x4000 //!< Deprecated, do not use.
#define CR_INST_DONT_SEND_REPORT               0x8000 //!< Don't send error report immediately, just save it locally.
#define CR_INST_APP_RESTART                   0x10000 //!< Restart the application on crash.
#define CR_INST_NO_MINIDUMP                   0x20000 //!< Do not include minidump file to crash report.
#define CR_INST_SEND_QUEUED_REPORTS           0x40000 //!< CrashRpt should send error reports that are waiting to be delivered.
#define CR_INST_STORE_ZIP_ARCHIVES            0x80000 //!< CrashRpt should store both uncompressed error report files and ZIP archives.
#define CR_INST_SEND_MANDATORY				 0x100000 //!< This flag removes the "Close" and "Other actions" buttons from Error Report dialog, thus making the sending procedure mandatory for user.
#define CR_INST_SHOW_ADDITIONAL_INFO_FIELDS	 0x200000 //!< Makes "Your E-mail" and "Describe what you were doing when the problem occurred" fields of Error Report dialog always visible.
#define CR_INST_ALLOW_ATTACH_MORE_FILES		 0x400000 //!< Adds an ability for user to attach more files to crash report by clicking "Attach More File(s)" item from context menu of Error Report Details dialog.
#define CR_INST_AUTO_THREAD_HANDLERS         0x800000 //!< If this flag is set, installs exception handlers for newly created threads automatically.


#endif //_CRASHRPT_H_


