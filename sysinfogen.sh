#!/bin/bash

# Copyright (c) Hercules Dev Team, licensed under GNU GPL.
# See the LICENSE file
# Base Author: Haru @ http://hercules.ws

function do_fail {
	echo 'Error writing output file'
	exit -1
}

function cleanstring {
	echo "$@" | sed -e 's/"/ /g' -e 's/[ \t][ \t]*/ /g' -e 's/^[ \t]*//g' -e 's/[ \t]*$//g'
}

if [ -z "$1" ]; then
	echo 'No output file specified'
	exit -1
fi

OUTFILE="$1"
shift

if ! touch "$OUTFILE"; then
	echo 'Cannot create output file'
	exit -1
fi

cat > "$OUTFILE" << EOF
// Copyright (c) Hercules Dev Team, licensed under GNU GPL.
// See the LICENSE file

// This file was automatically generated. Any edit to it will be lost.

EOF
[ $? -eq 0 ] || do_fail

HERC_PLATFORM="$( uname -s )"

case $HERC_PLATFORM in
	Linux)
		DIST=''
		DESCRIPTION=''
		REV=''

		if type lsb_release >/dev/null 2>&1; then
			LSBINFO="$( lsb_release -a 2>/dev/null )"
			DIST="$( cleanstring "$( echo "$LSBINFO" | grep '^Distributor ID:' | cut -d: -f2- )" )"
			DESCRIPTION="$( echo "$LSBINFO" | grep '^Description:' | cut -d: -f2- )"

			DESCRIPTION="${DESCRIPTION/Enterprise Linux Enterprise Linux/Oracle Linux}"
		fi
		
		if [ -z "$DIST" -o "$DIST" == "Gentoo" ]; then
			if [ -f /etc/gentoo-release ]; then
				# "Gentoo Base System release 2.2"
				DIST='Gentoo'
				DESCRIPTION="$( head -n 1 /etc/gentoo-release )"
				REV=''
			elif [ -f /etc/arch-release ]; then
				# empty release file
				DIST='ArchLinux'
				DESCRIPTION=''
				REV=''
			elif [ -f /etc/vmware-release ]; then
				# "VMware ESX Server 3" or "VMware ESX 4.0 (Kandinsky)"
				DIST="VMWare"
				DESCRIPTION="$( head -n 1 /etc/vmware-release )"
				REV=''
			elif [ -f /etc/debian_version ]; then
				# "wheezy/sid"
				DIST="Debian"
				DESCRIPTION="Debian GNU/Linux $( head -n 1 /etc/debian_version )"
				REV=''
			elif [ -f /etc/fedora-release ]; then
				# "Fedora release 9 (Sulphur)"
				DIST="Fedora"
				DESCRIPTION="$( head -n 1 /etc/fedora-release )"
				REV=''
			elif [ -f /etc/knoppix_version ]; then
				# "3.2 2003-04-15"
				DIST="Knoppix"
				REV="Knoppix GNU/Linux $( head -n 1 /etc/knoppix_version )"
				DESCRIPTION=''
			elif [ -f /etc/mandriva-release ]; then
				# "Mandriva Linux release 2010.1 (Official) for x86_64"
				DIST="Mandriva"
				DESCRIPTION="$( head -n 1 /etc/mandriva-release )"
				REV=''
			elif [ -f /etc/mandrake-release ]; then
				# "Mandrakelinux release 10.1 (Community) for i586"
				DIST="Mandrake"
				DESCRIPTION="$( head -n 1 /etc/mandrake-release )"
				REV=''
			elif [ -f /etc/oracle-release ]; then
				# "Oracle Linux Server release 6.3"
				DIST="Oracle"
				DESCRIPTION="$( head -n 1 /etc/oracle-release )"
				REV=''
			elif [ -f /etc/centos-release ]; then
				# "CentOS Linux release 6.0 (Final)"
				DIST="CentOS"
				DESCRIPTION="$( head -n 1 /etc/centos-release )"
				REV=''
			elif [ -f /etc/redhat-release ]; then
				# "Red Hat Enterprise Linux Server release 5 (Tikanga)"
				DIST="RedHat"
				DESCRIPTION="$( head -n 1 /etc/redhat-release )"
				REV=''
			elif [ -f /etc/slackware-version ]; then
				DIST="Slackware"
				DESCRIPTION="$( head -n 1 /etc/slackware-version )"
				REV=''
			elif [ -f /etc/slackware-release ]; then
				DIST="Slackware"
				DESCRIPTION="$( head -n 1 /etc/slackware-release )"
				REV=''
			elif [ -f /etc/SuSE-release ]; then
				# "SUSE Linux Enterprise Server 11 (x86_64)"
				# Note: it may contain several extra lines
				DIST="SuSE"
				DESCRIPTION="$( head -n 1 /etc/SuSE-release )"
				REV=''
			elif [ -f /etc/trustix-release ]; then
				# "Trustix Secure Linux release 2.0 (Cloud)"
				DIST="Trustix"
				DESCRIPTION="$( head -n 1 /etc/trustix-release )"
				REV=''
			else
				DIST='Unknown'
				DESCRIPTION=''
				REV=''
			fi
		fi
		if [ -n "$DESCRIPTION" ]; then
			DIST="$DESCRIPTION"
		fi
		HERC_OSVERSION="$DIST"

		HWDATA_CPU="$( cat /proc/cpuinfo | grep "model name" | head -n 1 | cut -d: -f2- )"
		HWDATA_CPUCORES="$( cleanstring "$( grep '^processor' /proc/cpuinfo | wc -l )" )"
		HERC_CPU="${HWDATA_CPU} [${HWDATA_CPUCORES}]"
		;;
	Darwin)
		HERC_PLATFORM="Mac OS X"
		if type sw_vers >/dev/null 2>&1; then
			HERC_OSVERSION="$( sw_vers -productName ) $( sw_vers -productVersion ) $( sw_vers -buildVersion )"
		else
			HERC_OSVERSION="Unknown"
		fi
		if type system_profiler >/dev/null 2>&1; then
			HWDATA="$( system_profiler SPHardwareDataType )"
			HWDATA_CPU="$( echo "$HWDATA" | grep "Processor Name:" | cut -d: -f2- )"
			HWDATA_CPUSPEED="$( cleanstring "$( echo "$HWDATA" | grep "Processor Speed:" | cut -d: -f2- )" )"
			HWDATA_CPUCORES="$( cleanstring "$( echo "$HWDATA" | grep "Total Number of Cores:" | cut -d: -f2- )" )"
			HERC_CPU="${HWDATA_CPU} (${HWDATA_CPUSPEED}) [${HWDATA_CPUCORES}]"
		else
			HERC_CPU="Unknown"
		fi
		;;
	SunOS)
		HERC_PLATFORM="Solaris"
		HERC_OSVERSION="${HERC_PLATFORM} $( uname -r )($( uname -p) $(uname -v))"
		HERC_CPU="Unknown"
		;;
	AIX)
		HERC_OSVERSION="AIX $( oslevel ) ($(`oslevel -r`))"
		HERC_CPU="Unknown"
		;;
	*)
		HERC_OSVERSION="Unknown"
		HERC_CPU="Unknown"
		;;
esac

cat >> "$OUTFILE" << EOF
// Platform (uname -s)
#define SYSINFO_PLATFORM "$( cleanstring "${HERC_PLATFORM}" )"

// Operating System version (Platform-dependent)
#define SYSINFO_OSVERSION "$( cleanstring "${HERC_OSVERSION}" )"

// CPU Model (Platform-dependent)
#define SYSINFO_CPU "$( cleanstring "${HERC_CPU}" )"

EOF
[ $? -eq 0 ] || do_fail

HERC_ARCH="$( uname -m )"

cat >> "$OUTFILE" << EOF
// OS Architecture (uname -m)
#define SYSINFO_ARCH "$( cleanstring "${HERC_ARCH}" )"

EOF
[ $? -eq 0 ] || do_fail

HERC_CFLAGS="$@"
HERC_CFLAGS="${HERC_CFLAGS//\"/}"

cat >> "$OUTFILE" << EOF
// Compiler Flags
#define SYSINFO_CFLAGS "$( cleanstring "${HERC_CFLAGS}" )"

EOF
[ $? -eq 0 ] || do_fail

HERC_CVSREV=""
if [ -d .git ]; then
	HERC_CVSTYPE="CVSTYPE_GIT"
	if type git >/dev/null 2>&1; then
		HERC_CVSREV="$( git rev-parse HEAD )"
	else
		HERC_CVSREV="Unknown"
	fi
elif [ -d .svn ]; then
	HERC_CVSTYPE="CVSTYPE_SVN"
	if type svnversion >/dev/null 2>&1; then
		HERC_CVSREV="$( svnversion )"
	else
		HERC_CVSREV="Unknown"
	fi
else
	HERC_CVSTYPE="CVSTYPE_NONE"
fi

cat >> "$OUTFILE" << EOF
// CVS Type
#define SYSINFO_CVSTYPE ${HERC_CVSTYPE}

// CVS Revision
#define SYSINFO_CVSREV "$( cleanstring "${HERC_CVSREV}" )"

EOF
[ $? -eq 0 ] || do_fail

