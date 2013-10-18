// Copyright (c) Hercules Dev Team, licensed under GNU GPL.
// See the LICENSE file
// Base Author: Haru @ http://hercules.ws

/// See sysinfo.h for a description of this file

#include "sysinfo.h"

#include "../common/cbasetypes.h"
#include "../common/core.h"
#include "../common/strlib.h"
#include "../common/malloc.h"

#ifdef WIN32
#include <windows.h>
#include <string.h> // strlen
#endif
#include <stdio.h> // fopen
#include <stdlib.h> // atoi

#define BUFSIZE 256

#define CVSTYPE_UNKNOWN 0
#define CVSTYPE_GIT 1
#define CVSTYPE_SVN 2
#define CVSTYPE_NONE -1

#ifdef WIN32
#define SYSINFO_PLATFORM "Windows"
#define SYSINFO_OSVERSION sysinfo_osversion_str
#define SYSINFO_CPU sysinfo_cpu_str
#define SYSINFO_ARCH sysinfo_arch_str
#define SYSINFO_CFLAGS "N/A"
#define SYSINFO_CVSTYPE sysinfo_cvstype_val
#define SYSINFO_CVSREV sysinfo_cvsrevision_src_str

static char sysinfo_osversion_str[BUFSIZE] = "";
static char sysinfo_cpu_str[BUFSIZE] = "";
static char sysinfo_arch_str[BUFSIZE] = "";
static int sysinfo_cvstype_val = CVSTYPE_UNKNOWN;
static char sysinfo_cvsrevision_src_str[BUFSIZE] = "";

// These aren't defined in MSVC2008/WindowsXP, so we gotta define them here
// Values from: http://msdn.microsoft.com/en-us/library/windows/desktop/ms724358%28v=vs.85%29.aspx
// To be used with GetProductInfo
enum windows_product_type {
	msPRODUCT_UNDEFINED                           = 0x00000000, // An unknown product
	msPRODUCT_ULTIMATE                            = 0x00000001, // Ultimate
	msPRODUCT_HOME_BASIC                          = 0x00000002, // Home Basic
	msPRODUCT_HOME_PREMIUM                        = 0x00000003, // Home Premium
	msPRODUCT_ENTERPRISE                          = 0x00000004, // Enterprise
	msPRODUCT_HOME_BASIC_N                        = 0x00000005, // Home Basic N
	msPRODUCT_BUSINESS                            = 0x00000006, // Business
	msPRODUCT_STANDARD_SERVER                     = 0x00000007, // Server Standard
	msPRODUCT_DATACENTER_SERVER                   = 0x00000008, // Server Datacenter (full installation)
	msPRODUCT_SMALLBUSINESS_SERVER                = 0x00000009, // Windows Small Business Server
	msPRODUCT_ENTERPRISE_SERVER                   = 0x0000000A, // Server Enterprise (full installation)
	msPRODUCT_STARTER                             = 0x0000000B, // Starter
	msPRODUCT_DATACENTER_SERVER_CORE              = 0x0000000C, // Server Datacenter (core installation)
	msPRODUCT_ENTERPRISE_SERVER_CORE              = 0x0000000E, // Server Enterprise (core installation)
	msPRODUCT_STANDARD_SERVER_CORE                = 0x0000000D, // Server Standard (core installation)
	msPRODUCT_ENTERPRISE_SERVER_IA64              = 0x0000000F, // Server Enterprise for Itanium-based Systems
	msPRODUCT_BUSINESS_N                          = 0x00000010, // Business N
	msPRODUCT_WEB_SERVER                          = 0x00000011, // Web Server (full installation)
	msPRODUCT_CLUSTER_SERVER                      = 0x00000012, // HPC Edition
	msPRODUCT_HOME_SERVER                         = 0x00000013, // Windows Storage Server 2008 R2 Essentials
	msPRODUCT_STORAGE_EXPRESS_SERVER              = 0x00000014, // Storage Server Express
	msPRODUCT_STORAGE_STANDARD_SERVER             = 0x00000015, // Storage Server Standard
	msPRODUCT_STORAGE_WORKGROUP_SERVER            = 0x00000016, // Storage Server Workgroup
	msPRODUCT_STORAGE_ENTERPRISE_SERVER           = 0x00000017, // Storage Server Enterprise
	msPRODUCT_SERVER_FOR_SMALLBUSINESS            = 0x00000018, // Windows Server 2008 for Windows Essential Server Solutions
	msPRODUCT_SMALLBUSINESS_SERVER_PREMIUM        = 0x00000019, // Small Business Server Premium
	msPRODUCT_HOME_PREMIUM_N                      = 0x0000001A, // Home Premium N
	msPRODUCT_ENTERPRISE_N                        = 0x0000001B, // Enterprise N
	msPRODUCT_ULTIMATE_N                          = 0x0000001C, // Ultimate N
	msPRODUCT_WEB_SERVER_CORE                     = 0x0000001D, // Web Server (core installation)
	msPRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT    = 0x0000001E, // Windows Essential Business Server Management Server
	msPRODUCT_MEDIUMBUSINESS_SERVER_SECURITY      = 0x0000001F, // Windows Essential Business Server Security Server
	msPRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING     = 0x00000020, // Windows Essential Business Server Messaging Server
	msPRODUCT_SERVER_FOUNDATION                   = 0x00000021, // Server Foundation
	msPRODUCT_HOME_PREMIUM_SERVER                 = 0x00000022, // Windows Home Server 2011
	msPRODUCT_SERVER_FOR_SMALLBUSINESS_V          = 0x00000023, // Windows Server 2008 without Hyper-V for Windows Essential Server Solutions
	msPRODUCT_STANDARD_SERVER_V                   = 0x00000024, // Server Standard without Hyper-V
	msPRODUCT_DATACENTER_SERVER_V                 = 0x00000025, // Server Datacenter without Hyper-V (full installation)
	msPRODUCT_ENTERPRISE_SERVER_V                 = 0x00000026, // Server Enterprise without Hyper-V (full installation)
	msPRODUCT_DATACENTER_SERVER_CORE_V            = 0x00000027, // Server Datacenter without Hyper-V (core installation)
	msPRODUCT_STANDARD_SERVER_CORE_V              = 0x00000028, // Server Standard without Hyper-V (core installation)
	msPRODUCT_ENTERPRISE_SERVER_CORE_V            = 0x00000029, // Server Enterprise without Hyper-V (core installation)
	msPRODUCT_HYPERV                              = 0x0000002A, // Microsoft Hyper-V Server
	msPRODUCT_STORAGE_EXPRESS_SERVER_CORE         = 0x0000002B, // Storage Server Express (core installation)
	msPRODUCT_STORAGE_STANDARD_SERVER_CORE        = 0x0000002C, // Storage Server Standard (core installation)
	msPRODUCT_STORAGE_WORKGROUP_SERVER_CORE       = 0x0000002D, // Storage Server Workgroup (core installation)
	msPRODUCT_STORAGE_ENTERPRISE_SERVER_CORE      = 0x0000002E, // Storage Server Enterprise (core installation)
	msPRODUCT_STARTER_N                           = 0x0000002F, // Starter N
	msPRODUCT_PROFESSIONAL                        = 0x00000030, // Professional
	msPRODUCT_PROFESSIONAL_N                      = 0x00000031, // Professional N
	msPRODUCT_SB_SOLUTION_SERVER                  = 0x00000032, // Windows Small Business Server 2011 Essentials
	msPRODUCT_SERVER_FOR_SB_SOLUTIONS             = 0x00000033, // Server For SB Solutions
	msPRODUCT_STANDARD_SERVER_SOLUTIONS           = 0x00000034, // Server Solutions Premium
	msPRODUCT_STANDARD_SERVER_SOLUTIONS_CORE      = 0x00000035, // Server Solutions Premium (core installation)
	msPRODUCT_SB_SOLUTION_SERVER_EM               = 0x00000036, // Server For SB Solutions EM
	msPRODUCT_SERVER_FOR_SB_SOLUTIONS_EM          = 0x00000037, // Server For SB Solutions EM
	msPRODUCT_SOLUTION_EMBEDDEDSERVER             = 0x00000038, // Windows MultiPoint Server
	msPRODUCT_ESSENTIALBUSINESS_SERVER_MGMT       = 0x0000003B, // Windows Essential Server Solution Management
	msPRODUCT_ESSENTIALBUSINESS_SERVER_ADDL       = 0x0000003C, // Windows Essential Server Solution Additional
	msPRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC    = 0x0000003D, // Windows Essential Server Solution Management SVC
	msPRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC    = 0x0000003E, // Windows Essential Server Solution Additional SVC
	msPRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE   = 0x0000003F, // Small Business Server Premium (core installation)
	msPRODUCT_CLUSTER_SERVER_V                    = 0x00000040, // Server Hyper Core V
	msPRODUCT_STARTER_E                           = 0x00000042, // Not supported
	msPRODUCT_HOME_BASIC_E                        = 0x00000043, // Not supported
	msPRODUCT_HOME_PREMIUM_E                      = 0x00000044, // Not supported
	msPRODUCT_PROFESSIONAL_E                      = 0x00000045, // Not supported
	msPRODUCT_ENTERPRISE_E                        = 0x00000046, // Not supported
	msPRODUCT_ULTIMATE_E                          = 0x00000047, // Not supported
	msPRODUCT_ENTERPRISE_EVALUATION               = 0x00000048, // Server Enterprise (evaluation installation)
	msPRODUCT_MULTIPOINT_STANDARD_SERVER          = 0x0000004C, // Windows MultiPoint Server Standard (full installation)
	msPRODUCT_MULTIPOINT_PREMIUM_SERVER           = 0x0000004D, // Windows MultiPoint Server Premium (full installation)
	msPRODUCT_STANDARD_EVALUATION_SERVER          = 0x0000004F, // Server Standard (evaluation installation)
	msPRODUCT_DATACENTER_EVALUATION_SERVER        = 0x00000050, // Server Datacenter (evaluation installation)
	msPRODUCT_ENTERPRISE_N_EVALUATION             = 0x00000054, // Enterprise N (evaluation installation)
	msPRODUCT_STORAGE_WORKGROUP_EVALUATION_SERVER = 0x0000005F, // Storage Server Workgroup (evaluation installation)
	msPRODUCT_STORAGE_STANDARD_EVALUATION_SERVER  = 0x00000060, // Storage Server Standard (evaluation installation)
	msPRODUCT_CORE_N                              = 0x00000062, // Windows 8 N
	msPRODUCT_CORE_COUNTRYSPECIFIC                = 0x00000063, // Windows 8 China
	msPRODUCT_CORE_SINGLELANGUAGE                 = 0x00000064, // Windows 8 Single Language
	msPRODUCT_CORE                                = 0x00000065, // Windows 8
	msPRODUCT_PROFESSIONAL_WMC                    = 0x00000067, // Professional with Media Center
};

// Values from http://msdn.microsoft.com/en-us/library/windows/desktop/ms724385%28v=vs.85%29.aspx
// To be used with GetSystemMetrics
enum windows_metrics {
	msSM_SERVERR2 = 89,
};

// Values from http://msdn.microsoft.com/en-us/library/windows/desktop/ms724833%28v=vs.85%29.aspx
// To be used with OSVERSIONINFOEX.wSuiteMask
enum windows_ver_suite {
	msVER_SUITE_BLADE          = 0x00000400, // Windows Server 2003, Web Edition is installed.
	msVER_SUITE_STORAGE_SERVER = 0x00002000, // Windows Storage Server 2003 R2 or Windows Storage Server 2003 is installed.
	msVER_SUITE_COMPUTE_SERVER = 0x00004000, // Windows Server 2003, Compute Cluster Edition is installed.
	msVER_SUITE_WH_SERVER      = 0x00008000, // Windows Home Server is installed.
};

#else // not WIN32
// UNIX. Use build-time cached values
#include "sysinfo.inc"
#endif // WIN32

static char sysinfo_cvsrevision_script_str[BUFSIZE] = "";
#define SYSINFO_CVSREV_RUNTIME sysinfo_cvsrevision_script_str

// Compiler detection <http://sourceforge.net/p/predef/wiki/Compilers/>
#if defined(__BORLANDC__)
#define SYSINFO_COMPILER "Borland C++"
#elif defined(__clang__)
#define SYSINFO_COMPILER "Clang v" EXPAND_AND_QUOTE(__clang_major__) "." EXPAND_AND_QUOTE(__clang_minor__) "." EXPAND_AND_QUOTE(__clang_patchlevel__)
#elif defined(__INTEL_COMPILER)
#define SYSINFO_COMPILER "Intel CC v" EXPAND_AND_QUOTE(__INTEL_COMPILER)
#elif defined(__MINGW32__)
#if defined(__MINGW64__)
#define SYSINFO_COMPILER "MinGW-w64 64 Bit v" EXPAND_AND_QUOTE(__MINGW64_VERSION_MAJOR) "." EXPAND_AND_QUOTE(__MINGW64_VERSION_MINOR) \
	" (MinGW " EXPAND_AND_QUOTE(__MINGW32_MAJOR_VERSION) "." EXPAND_AND_QUOTE(__MINGW32_MINOR_VERSION) ")"
#elif defined(__MINGW64_VERSION_MAJOR)
#define SYSINFO_COMPILER "MinGW-w64 32 Bit v" EXPAND_AND_QUOTE(__MINGW64_VERSION_MAJOR) "." EXPAND_AND_QUOTE(__MINGW64_VERSION_MINOR) \
	" (MinGW " EXPAND_AND_QUOTE(__MINGW32_MAJOR_VERSION) "." EXPAND_AND_QUOTE(__MINGW32_MINOR_VERSION) ")"
#else
#define SYSINFO_COMPILER "MinGW32 v" EXPAND_AND_QUOTE(__MINGW32_MAJOR_VERSION) "." EXPAND_AND_QUOTE(__MINGW32_MINOR_VERSION)
#endif
#elif defined(__GNUC__)
#define SYSINFO_COMPILER "GCC v" EXPAND_AND_QUOTE(__GNUC__) "." EXPAND_AND_QUOTE(__GNUC_MINOR__) "." EXPAND_AND_QUOTE(__GNUC_PATCHLEVEL__)
#elif defined(_MSC_VER)
#if _MSC_VER >= 1300 && _MSC_VER < 1310
#define SYSINFO_COMPILER "Microsoft Visual C++ 7.0 (v" EXPAND_AND_QUOTE(_MSC_VER) ")"
#elif _MSC_VER >= 1310 && _MSC_VER < 1400
#define SYSINFO_COMPILER "Microsoft Visual C++ 2003 (v" EXPAND_AND_QUOTE(_MSC_VER) ")"
#elif _MSC_VER >= 1400 && _MSC_VER < 1500
#define SYSINFO_COMPILER "Microsoft Visual C++ 2005 (v" EXPAND_AND_QUOTE(_MSC_VER) ")"
#elif _MSC_VER >= 1500 && _MSC_VER < 1600
#define SYSINFO_COMPILER "Microsoft Visual C++ 2008 (v" EXPAND_AND_QUOTE(_MSC_VER) ")"
#elif _MSC_VER >= 1600 && _MSC_VER < 1700
#define SYSINFO_COMPILER "Microsoft Visual C++ 2010 (v" EXPAND_AND_QUOTE(_MSC_VER) ")"
#elif _MSC_VER >= 1700 && _MSC_VER < 1800
#define SYSINFO_COMPILER "Microsoft Visual C++ 2012 (v" EXPAND_AND_QUOTE(_MSC_VER) ")"
#elif _MSC_VER >= 1800 && _MSC_VER < 1900
#define SYSINFO_COMPILER "Microsoft Visual C++ 2013 (v" EXPAND_AND_QUOTE(_MSC_VER) ")"
#else // < 1300 || >= 1900
#define SYSINFO_COMPILER "Microsoft Visual C++ v" EXPAND_AND_QUOTE(_MSC_VER)
#endif
#else
#define SYSINFO_COMPILER "Unknown"
#endif
// end compiler detection

/**
 * Retrieves the current SVN revision
 *
 * @param out a char buffer to write the output value to
 * @param len maximum buffer length
 * @return true if a revision was correctly detected
 */
bool sysinfo_svn_get_revision(char *out, int len) {
	// Only include SVN support if detected it, or we're on MSVC
#if SYSINFO_CVSTYPE == CVSTYPE_SVN || SYSINFO_CVSTYPE == CVSTYPE_UNKNOWN || defined(WIN32)
	FILE *fp;

	if( !out ) return false;
	out[0] = '\0';

	// subversion 1.7 uses a sqlite3 database
	// FIXME this is hackish at best...
	// - ignores database file structure
	// - assumes the data in NODES.dav_cache column ends with "!svn/ver/<revision>/<path>)"
	// - since it's a cache column, the data might not even exist
	if( (fp = fopen(".svn"PATHSEP_STR"wc.db", "rb")) != NULL || (fp = fopen(".."PATHSEP_STR".svn"PATHSEP_STR"wc.db", "rb")) != NULL ) {

#ifndef SVNNODEPATH //not sure how to handle branches, so i'll leave this overridable define until a better solution comes up
#define SVNNODEPATH trunk
#endif // SVNNODEPATH

		const char* prefix = "!svn/ver/";
		const char* postfix = "/"EXPAND_AND_QUOTE(SVNNODEPATH)")"; // there should exist only 1 entry like this
		size_t prefix_len = strlen(prefix);
		size_t postfix_len = strlen(postfix);
		size_t i,j,flen;
		char* buffer;

		// read file to buffer
		fseek(fp, 0, SEEK_END);
		flen = ftell(fp);
		buffer = (char*)aMalloc(flen + 1);
		fseek(fp, 0, SEEK_SET);
		flen = fread(buffer, 1, flen, fp);
		buffer[flen] = '\0';
		fclose(fp);

		// parse buffer
		for( i = prefix_len + 1; i + postfix_len <= flen; ++i ) {
			if( buffer[i] != postfix[0] || memcmp(buffer + i, postfix, postfix_len) != 0 )
				continue; // postfix mismatch
			for( j = i; j > 0; --j ) { // skip digits
				if( !ISDIGIT(buffer[j - 1]) )
					break;
			}
			if( memcmp(buffer + j - prefix_len, prefix, prefix_len) != 0 )
				continue; // prefix mismatch
			// done
			snprintf(out, len, "%d", atoi(buffer + j));
			break;
		}
		aFree(buffer);

		if( out[0] != '\0' )
			return true;
	}

	// subversion 1.6 and older?
	if ((fp = fopen(".svn/entries", "r")) != NULL) {
		char line[1024];
		int rev;
		// Check the version
		if (fgets(line, sizeof(line), fp)) {
			if(!ISDIGIT(line[0])) {
				// XML File format
				while (fgets(line,sizeof(line),fp))
					if (strstr(line,"revision=")) break;
				if (sscanf(line," %*[^\"]\"%d%*[^\n]", &rev) == 1) {
					snprintf(out, len, "%d", rev);
				}
			} else {
				// Bin File format
				if ( fgets(line, sizeof(line), fp) == NULL ) { printf("Can't get bin name\n"); } // Get the name
				if ( fgets(line, sizeof(line), fp) == NULL ) { printf("Can't get entries kind\n"); } // Get the entries kind
				if(fgets(line, sizeof(line), fp)) { // Get the rev numver
					snprintf(out, len, "%d", atoi(line));
				}
			}
		}
		fclose(fp);

		if( out[0] != '\0' )
			return true;
	}
#endif
	out[0] = '\0';
	return false;
}

/**
 * Retrieves the current Git revision
 *
 * @param out a char buffer to write the output value to
 * @param len maximum buffer length
 * @return true if a revision was correctly detected
 */
bool sysinfo_git_get_revision(char *out, int len) {
	// Only include Git support if we detected it, or we're on MSVC
#if SYSINFO_CVSTYPE == CVSTYPE_GIT || SYSINFO_CVSTYPE == CVSTYPE_UNKNOWN || defined(WIN32)
	FILE *fp;

	if( !out ) return false;
	out[0] = '\0';

#ifndef GITREFPATH //not sure how to handle branches, so i'll leave this overridable define until a better solution comes up
#define GITREFPATH refs/remotes/origin/master
#endif // GITREFPATH
	
	if( (fp = fopen (".git/"EXPAND_AND_QUOTE(GITREFPATH), "r")) != NULL ) {
		char line[64];
		char rev[50];
		
		if( fgets(line, sizeof(line), fp) && sscanf(line, "%49s", rev) )
			snprintf(out, len, "%s", rev);
		
		fclose(fp);

		if( out[0] != '\0' )
			return true;
	}
#endif
	out[0] = '\0';
	return false;
}

#ifdef WIN32
// Windows-specific runtime detection functions

typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
/**
 * Retrieves the Operating System version (Windows only)
 * @param StringBuf an initialized StringBuf pointer, to return the value into
 * @return false if an error happened during version detection
 */
bool sysinfo_osversion_retrieve_sub(StringBuf *buf) {
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !GetVersionEx((OSVERSIONINFO*) &osvi) )
		return false;

	if( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId // Windows NT Family
	 && ((osvi.dwMajorVersion > 4 && osvi.dwMajorVersion < 6) || (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion <= 3)) // Between XP and 8.1
	) {
		if( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion <= 3 ) { // Between Vista and 8.1
			PGPI pGPI;
			DWORD dwType;
			if( osvi.dwMinorVersion == 0 ) {
				StrBuf->AppendStr(buf, osvi.wProductType == VER_NT_WORKSTATION ? "Windows Vista" : "Windows Server 2008");
			} else if( osvi.dwMinorVersion == 1 ) {
				StrBuf->AppendStr(buf, osvi.wProductType == VER_NT_WORKSTATION ? "Windows 7" : "Windows Server 2008 R2");
			} else if( osvi.dwMinorVersion == 2 ) {
				StrBuf->AppendStr(buf, osvi.wProductType == VER_NT_WORKSTATION ? "Windows 8" : "Windows Server 2012");
			} else if( osvi.dwMinorVersion == 3 ) { // Only if the application has been manifested for Windows 8.1; Else it's 6.2
				StrBuf->AppendStr(buf, osvi.wProductType == VER_NT_WORKSTATION ? "Windows 8.1" : "Windows Server 2012 R2");
			}

			pGPI = (PGPI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");

			pGPI(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

			switch( dwType ) {
				case msPRODUCT_ULTIMATE:
				case msPRODUCT_ULTIMATE_N:
					StrBuf->AppendStr(buf, " Ultimate");
					break;
				case msPRODUCT_PROFESSIONAL:
				case msPRODUCT_PROFESSIONAL_N:
				case msPRODUCT_PROFESSIONAL_WMC:
					StrBuf->AppendStr(buf, " Professional");
					break;
				case msPRODUCT_HOME_PREMIUM:
				case msPRODUCT_HOME_PREMIUM_N:
					StrBuf->AppendStr(buf, " Home Premium");
					break;
				case msPRODUCT_HOME_BASIC:
				case msPRODUCT_HOME_BASIC_N:
					StrBuf->AppendStr(buf, " Home Basic");
					break;
				case msPRODUCT_ENTERPRISE:
				case msPRODUCT_ENTERPRISE_N:
				case msPRODUCT_ENTERPRISE_SERVER:
				case msPRODUCT_ENTERPRISE_SERVER_CORE:
				case msPRODUCT_ENTERPRISE_SERVER_IA64:
				case msPRODUCT_ENTERPRISE_SERVER_V:
				case msPRODUCT_ENTERPRISE_SERVER_CORE_V:
				case msPRODUCT_ENTERPRISE_EVALUATION:
				case msPRODUCT_ENTERPRISE_N_EVALUATION:
					StrBuf->AppendStr(buf, " Enterprise");
					break;
				case msPRODUCT_BUSINESS:
				case msPRODUCT_BUSINESS_N:
					StrBuf->AppendStr(buf, " Business");
					break;
				case msPRODUCT_STARTER:
				case msPRODUCT_STARTER_N:
					StrBuf->AppendStr(buf, " Starter");
					break;
				case msPRODUCT_CLUSTER_SERVER:
				case msPRODUCT_CLUSTER_SERVER_V:
					StrBuf->AppendStr(buf, " Cluster Server");
					break;
				case msPRODUCT_DATACENTER_SERVER:
				case msPRODUCT_DATACENTER_SERVER_CORE:
				case msPRODUCT_DATACENTER_SERVER_V:
				case msPRODUCT_DATACENTER_SERVER_CORE_V:
				case msPRODUCT_DATACENTER_EVALUATION_SERVER:
					StrBuf->AppendStr(buf, " Datacenter");
					break;
				case msPRODUCT_SMALLBUSINESS_SERVER:
				case msPRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
				case msPRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE:
					StrBuf->AppendStr(buf, " Small Business Server");
					break;
				case PRODUCT_STANDARD_SERVER:
				case PRODUCT_STANDARD_SERVER_CORE:
				case msPRODUCT_STANDARD_SERVER_V:
				case msPRODUCT_STANDARD_SERVER_CORE_V:
				case msPRODUCT_STANDARD_EVALUATION_SERVER:
					StrBuf->AppendStr(buf, " Standard");
					break;
				case msPRODUCT_WEB_SERVER:
				case msPRODUCT_WEB_SERVER_CORE:
					StrBuf->AppendStr(buf, " Web Server");
					break;
				case msPRODUCT_STORAGE_EXPRESS_SERVER:
				case msPRODUCT_STORAGE_STANDARD_SERVER:
				case msPRODUCT_STORAGE_WORKGROUP_SERVER:
				case msPRODUCT_STORAGE_ENTERPRISE_SERVER:
				case msPRODUCT_STORAGE_EXPRESS_SERVER_CORE:
				case msPRODUCT_STORAGE_STANDARD_SERVER_CORE:
				case msPRODUCT_STORAGE_WORKGROUP_SERVER_CORE:
				case msPRODUCT_STORAGE_ENTERPRISE_SERVER_CORE:
				case msPRODUCT_STORAGE_WORKGROUP_EVALUATION_SERVER:
				case msPRODUCT_STORAGE_STANDARD_EVALUATION_SERVER:
					StrBuf->AppendStr(buf, " Storage Server");
					break;
				case msPRODUCT_HOME_SERVER:
				case msPRODUCT_SERVER_FOR_SMALLBUSINESS:
				case msPRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT:
				case msPRODUCT_MEDIUMBUSINESS_SERVER_SECURITY:
				case msPRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING:
				case msPRODUCT_SERVER_FOR_SMALLBUSINESS_V:
				case msPRODUCT_SERVER_FOUNDATION:
				case msPRODUCT_HOME_PREMIUM_SERVER:
				case msPRODUCT_HYPERV:
				case msPRODUCT_SB_SOLUTION_SERVER:
				case msPRODUCT_SERVER_FOR_SB_SOLUTIONS:
				case msPRODUCT_STANDARD_SERVER_SOLUTIONS:
				case msPRODUCT_STANDARD_SERVER_SOLUTIONS_CORE:
				case msPRODUCT_SB_SOLUTION_SERVER_EM:
				case msPRODUCT_SERVER_FOR_SB_SOLUTIONS_EM:
				case msPRODUCT_SOLUTION_EMBEDDEDSERVER:
				case msPRODUCT_ESSENTIALBUSINESS_SERVER_MGMT:
				case msPRODUCT_ESSENTIALBUSINESS_SERVER_ADDL:
				case msPRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC:
				case msPRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC:
				case msPRODUCT_MULTIPOINT_STANDARD_SERVER:
				case msPRODUCT_MULTIPOINT_PREMIUM_SERVER:
					StrBuf->AppendStr(buf, " Server (other)");
					break;
				case msPRODUCT_CORE_N:
				case msPRODUCT_CORE_COUNTRYSPECIFIC:
				case msPRODUCT_CORE_SINGLELANGUAGE:
				case msPRODUCT_CORE:
					StrBuf->AppendStr(buf, " Workstation (other)");
					break;
			}

		} else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 ) { // XP x64 and Server 2003
			if( osvi.wProductType == VER_NT_WORKSTATION ) {
				StrBuf->AppendStr(buf, "Windows XP Professional");
			} else {
				if( GetSystemMetrics(msSM_SERVERR2) )
					StrBuf->AppendStr(buf, "Windows Server 2003 R2");
				else if( osvi.wSuiteMask & msVER_SUITE_STORAGE_SERVER )
					StrBuf->AppendStr(buf, "Windows Storage Server 2003");
				else if( osvi.wSuiteMask & msVER_SUITE_WH_SERVER )
					StrBuf->AppendStr(buf, "Windows Home Server");
				else
					StrBuf->AppendStr(buf, "Windows Server 2003");

				// Test for the server type.
				if( osvi.wSuiteMask & msVER_SUITE_COMPUTE_SERVER )
					StrBuf->AppendStr(buf, " Compute Cluster");
				else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					StrBuf->AppendStr(buf, " Datacenter");
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					StrBuf->AppendStr(buf, " Enterprise");
				else if( osvi.wSuiteMask & msVER_SUITE_BLADE )
					StrBuf->AppendStr(buf, " Web");
				else
					StrBuf->AppendStr(buf, " Standard");
			}
		} else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 ) { // XP
			StrBuf->AppendStr(buf, "Windows XP");
			if( osvi.wSuiteMask & VER_SUITE_EMBEDDEDNT ) {
				StrBuf->AppendStr(buf, " Embedded");
			} else if( osvi.wSuiteMask & VER_SUITE_PERSONAL ) {
				StrBuf->AppendStr(buf, " Home");
			} else {
				StrBuf->AppendStr(buf, " Professional");
			}
		} else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 ) { // 2000
			StrBuf->AppendStr(buf, "Windows 2000");

			if( osvi.wProductType == VER_NT_WORKSTATION )
				StrBuf->AppendStr(buf, " Professional");
			else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
				StrBuf->AppendStr(buf, " Datacenter Server");
			else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
				StrBuf->AppendStr(buf, " Advanced Server");
			else StrBuf->AppendStr(buf, " Server");
		} else {
			StrBuf->Printf(buf, "Unknown Windows version %d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
		}
	}

	// Include service pack (if any) and build number.

	if( strlen(osvi.szCSDVersion) > 0 ) {
		StrBuf->Printf(buf, " %s", osvi.szCSDVersion);
	}

	StrBuf->Printf(buf, " (build %d)", osvi.dwBuildNumber);

	return true;
}

/**
 * Retrieves the Operating System version (Windows only)
 * Once retrieved, the version string is stored into sysinfo_osversion_str
 */
void sysinfo_osversion_retrieve(void) {
	StringBuf buf;

	StrBuf->Init(&buf);

	if( sysinfo_osversion_retrieve_sub(&buf) ) {
		safestrncpy(sysinfo_osversion_str, StrBuf->Value(&buf), BUFSIZE);
	} else {
		safestrncpy(sysinfo_osversion_str, "Unknown Version", BUFSIZE);
	}

	StrBuf->Destroy(&buf);
}

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
/**
 * Retrieves the CPU type (Windows only)
 * Once retrieved, the name is stored into sysinfo_cpu_str
 */
void sysinfo_cpu_retrieve(void) {
	StringBuf buf;
	PGNSI pGNSI;
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	StrBuf->Init(&buf);

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
	pGNSI = (PGNSI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if( NULL != pGNSI )
		pGNSI(&si);
	else
		GetSystemInfo(&si);

	if( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL
	 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64
	) {
		StrBuf->Printf(&buf, "%s CPU, Family %d, Model %d, Stepping %d",
		               si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL ? "x86" : "x86_64",
		               si.wProcessorLevel,
		               (si.wProcessorRevision&0xff00)>>8,
		               (si.wProcessorRevision&0xff));
	} else {
		StrBuf->AppendStr(&buf, "Unknown");
	}

	safestrncpy(sysinfo_cpu_str, StrBuf->Value(&buf), BUFSIZE);

	StrBuf->Destroy(&buf);
}

/**
 * Retrieves the OS architecture (Windows only)
 * Once retrieved, the name is stored into sysinfo_arch_str
 */
void sysinfo_arch_retrieve(void) {
	PGNSI pGNSI;
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(SYSTEM_INFO));

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
	pGNSI = (PGNSI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if( NULL != pGNSI )
		pGNSI(&si);
	else
		GetSystemInfo(&si);

	if( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ) // x64
		safestrncpy(sysinfo_arch_str, "x86_64", BUFSIZE);
	else if( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL ) // x32
		safestrncpy(sysinfo_arch_str, "x86", BUFSIZE);
	else if( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM ) // ARM
		safestrncpy(sysinfo_arch_str, "ARM", BUFSIZE);
	else if( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ) // Itanium
		safestrncpy(sysinfo_arch_str, "IA-64", BUFSIZE);
	else
		safestrncpy(sysinfo_arch_str, "Unknown", BUFSIZE);
}

/**
 * Retrieves the startup-time CVS revision information.
 */
void sysinfo_cvsrevision_src_retrieve(void) {
	// Try Git, then SVN
	if( sysinfo_git_get_revision(SYSINFO_CVSREV, BUFSIZE) ) {
		SYSINFO_CVSTYPE = CVSTYPE_GIT;
		return;
	}
	if( sysinfo_svn_get_revision(SYSINFO_CVSREV, BUFSIZE) ) {
		SYSINFO_CVSTYPE = CVSTYPE_SVN;
		return;
	}
	SYSINFO_CVSTYPE = CVSTYPE_NONE;
}
#endif // WIN32

/**
 * Returns the platform (OS type) this application is running on.
 * This information is cached at compile time, since it's unlikely to change.
 *
 * @param out a char buffer to write the output value to
 * @param len maximum buffer length
 *
 * Output example: "Linux", "Darwin", "Windows", etc
 */
void sysinfo_platform(char *out, size_t len) {
	if( out )
		safestrncpy(out, SYSINFO_PLATFORM, len);
}

/**
 * Returns the Operating System version the application is running on.
 *
 * On platforms other than Windows (MSVC), this information is cached at
 *   compile time, since it is uncommon that an application is compiled and run
 *   on different machines.
 *
 * @param out a char buffer to write the output value to
 * @param len maximum buffer length
 *
 * Output example: "Windows 2008 Small Business Server", "OS X 10.8 Mountain Lion",
 *   "Gentoo Base System Release 2.2", "Debian GNU/Linux 6.0.6 (squeeze)", etc
 */
void sysinfo_osversion(char *out, size_t len) {
#ifdef WIN32
	if( !*SYSINFO_OSVERSION )
		sysinfo_osversion_retrieve();
#endif
	if( out )
		safestrncpy(out, SYSINFO_OSVERSION, len);
}

/**
 * Returns the CPU model the application is running on.
 * Where available, the number of cores/CPUs is appended to the name, in square
 *   brackets.
 *
 * On platforms other than Windows (MSVC), this information is cached at
 *   compile time, since it is uncommon that an application is compiled and run
 *   on different machines.
 *
 * @param out a char buffer to write the output value to
 * @param len maximum buffer length
 *
 * Output example: "Intel(R) Atom(TM) CPU D2500   @ 1.86GHz [2]",
 *   "Intel(R) Xeon(R) CPU E5-1650 0 @ 3.20GHz [12]", "Intel Core i7 [8]",
 *   "x86 CPU, Family 6, Model 54, Stepping 1"
 */
void sysinfo_cpu(char *out, size_t len) {
#ifdef WIN32
	if( !*SYSINFO_CPU )
		sysinfo_cpu_retrieve();
#endif
	if( out )
		safestrncpy(out, SYSINFO_CPU, len);
}

/**
 * Returns the CPU architecture the application was compiled for.
 *
 * On platforms other than Windows (MSVC), this information is cached at
 *   compile time, since it is uncommon that an application is compiled and run
 *   on different machines.
 *
 * @param out a char buffer to write the output value to
 * @param len maximum buffer length
 *
 * Output example: "x86", "x86_64", "IA-64", "ARM", etc
 */
void sysinfo_arch(char *out, size_t len) {
#ifdef WIN32
	if( !*SYSINFO_ARCH )
		sysinfo_arch_retrieve();
#endif
	if( out )
		safestrncpy(out, SYSINFO_ARCH, len);
}

/**
 * Returns info about the 32 or 64 bit build of Hercules.
 *
 * @return true if this is a 64 bit build
 */
bool sysinfo_is64bit(void) {
#ifdef _LP64
	return true;
#else
	return false;
#endif
}

/**
 * Returns the compiler the application was compiled with.
 *
 * @param out a char buffer to write the output value to
 * @param len maximum buffer length
 *
 * Output example: "Microsoft Visual C++ 2012 (v170050727)",
 *   "Clang v5.0.0", "MinGW32 v3.20", "GCC v4.7.3", etc
 */
void sysinfo_compiler(char *out, size_t len) {
	if( out )
		safestrncpy(out, SYSINFO_COMPILER, len);
}

/**
 * Returns the compiler flags the application was compiled with.
 *
 * On Windows (MSVC), an empty string is returned instead.
 *
 * @param out a char buffer to write the output value to
 * @param len maximum buffer length
 *
 * Output example: "-ggdb -O2 -flto -pipe -ffast-math ..."
 */
void sysinfo_cflags(char *out, size_t len) {
	if( out )
		safestrncpy(out, SYSINFO_CFLAGS, len);
}

/**
 * Returns the Concurrent Versioning System the application was downloaded with.
 *
 * On platforms other than Windows (MSVC), this information is cached at
 *   compile time. On Windows (MSVC), it is cached when the function is first
 *   called (most likely on server startup).
 *
 * @param out a char buffer to write the output value to
 * @param len maximum buffer length
 *
 * Output example: "Git", "SVN", "Exported"
 */
void sysinfo_cvstype(char *out, size_t len) {
#ifdef WIN32
	if( SYSINFO_CVSTYPE == CVSTYPE_UNKNOWN )
		sysinfo_cvsrevision_src_retrieve();
#endif // WIN32
	if( !out )
		return;

	switch( SYSINFO_CVSTYPE ) {
		case CVSTYPE_GIT:
			safestrncpy(out, "Git", len);
			return;
		case CVSTYPE_SVN:
			safestrncpy(out, "SVN", len);
			return;
		default:
			safestrncpy(out, "Exported", len);
	}
	return;
}

/**
 * Returns the Concurrent Versioning System revision.
 *
 * On platforms other than Windows (MSVC), this information is cached at
 *   compile time for better reliability. On Windows (MSVC), it is cached when
 *   the function is first called (most likely on server startup), so it may
 *   diverge from the actual revision that was compiled.
 *
 * If no CVS is detected (exported), this will return an empty string.
 *
 * @param out a char buffer to write the output value to
 * @param len maximum buffer length
 *
 * Output example: Git: "9128feccf3bddda94a7f8a170305565416815b40", SVN: "17546"
 */
void sysinfo_cvsrevision_src(char *out, size_t len) {
#ifdef WIN32
	if( SYSINFO_CVSTYPE == CVSTYPE_UNKNOWN )
		sysinfo_cvsrevision_src_retrieve();
#endif // WIN32
	if( out )
		safestrncpy(out, SYSINFO_CVSREV, len);
}

/**
 * Returns the Concurrent Versioning System revision.
 *
 * This information is cached during a script reload, so that it matches the
 *   version of the loaded scripts.
 *
 * If no CVS is detected (exported), this will return an empty string.
 *
 * @param out a char buffer to write the output value to
 * @param len maximum buffer length
 *
 * Output example: Git: "9128feccf3bddda94a7f8a170305565416815b40", SVN: "17546"
 */
void sysinfo_cvsrevision_scripts(char *out, size_t len) {
	if( !*SYSINFO_CVSREV_RUNTIME )
		sysinfo_cvsrevision_reload();
	if( out )
		safestrncpy(out, SYSINFO_CVSREV_RUNTIME, len);

}

/**
 * Reloads the run-time (scripts) CVS revision information. To be used during
 *   script reloads to refresh the cached version.
 */
void sysinfo_cvsrevision_reload(void) {
	// Try Git, then SVN
	if( sysinfo_git_get_revision(SYSINFO_CVSREV_RUNTIME, BUFSIZE) ) {
		return;
	}
	if( sysinfo_svn_get_revision(SYSINFO_CVSREV_RUNTIME, BUFSIZE) ) {
		return;
	}
}

