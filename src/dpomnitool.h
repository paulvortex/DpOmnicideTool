// dpspr32m.h
#ifndef F_DPOMNITOOL_H
#define F_DPOMNITOOL_H

#define MAX_FPATH 8192
#define DPOMNITOOL_VERSION_MAJOR "1"
#define DPOMNITOOL_VERSION_MINOR "0"

// utils
#include "mem.h"
#include "cmd.h"

// fourCC
#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
        ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | \
        ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif

// parameters
#ifdef F_DPOMNITOOL_C
	#define MAIN_EXTERN 
#else
	#define MAIN_EXTERN extern
#endif

// general globals
MAIN_EXTERN bool waitforkey;
MAIN_EXTERN bool memstats;
MAIN_EXTERN bool errorlog;
MAIN_EXTERN int  numthreads;
MAIN_EXTERN char progname[MAX_FPATH];
MAIN_EXTERN char progpath[MAX_FPATH];

#endif