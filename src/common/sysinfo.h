// Copyright (c) Hercules Dev Team, licensed under GNU GPL.
// See the LICENSE file
// Base Author: Haru @ http://hercules.ws

#ifndef	_COMMON_SYSINFO_H_
#define _COMMON_SYSINFO_H_

/**
 * Provides various bits of information about the system Hercules is running on
 * (note: on unix systems, to avoid runtime detection, most of the data is]
 * cached at compile time)
 */

#include "../common/cbasetypes.h"

void sysinfo_platform(char *out, size_t len);
void sysinfo_osversion(char *out, size_t len);
void sysinfo_cpu(char *out, size_t len);
void sysinfo_arch(char *out, size_t len);
bool sysinfo_is64bit(void);
void sysinfo_compiler(char *out, size_t len);
void sysinfo_cflags(char *out, size_t len);
void sysinfo_cvstype(char *out, size_t len);
void sysinfo_cvsrevision_src(char *out, size_t len);
void sysinfo_cvsrevision_scripts(char *out, size_t len);
void sysinfo_cvsrevision_reload(void);

#endif /* _COMMON_SYSINFO_H_ */
