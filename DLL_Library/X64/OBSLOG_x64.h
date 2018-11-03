/*

LICENCE
----------------------------------------------------
This software product is distributed under the license MIT
Copyright (c) 2018 Mogulev Alexey forawhitemonkey@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

DESCRIPTION
----------------------------------------------------

Simple and easy library in C language "LOGGER" from the utility set "OBSIDIAN TOOLS".
Designed for easy messaging when debugging an application.

Key features:

  - Formatted output is simplified (specification differs from "printf ...")
  - All functions return the last error code. 0 if there is no error.
  - For any code you can get a description
  - There is a date tag "#d" and a time "#t". Tags are part of the formatting string
  - Customizable buffer size
  - Write log to disk with modes (rewrite / new "000, 001, ...")
  - Autosave option when filling buffer
  - If the file extension is not specified, the default is "* .txt"

Some limitations and conventions are:

 - Default log buffer size 32768 bytes (min 2048 / max 8388608)
 - Max message size = Current log buffer size
 - Max file number 999, start at 001
 - The log name is the path to save the file
 - The maximum length of the name is limited to 248 characters
   including the file extension

FORMATTING CHARACTERS

	%i			Signed integer                    -123
	%u			Unsigned integer                  456
	%I			Signed long long                  -123456
	%U			Unsigned long long                456789
	%f			Floating point (float / double)   392.65
	%x			Unsigned hex int (lowercase)      7fa
	%X			Unsigned hex int (uppercase)      7FA
	%c			Character                         'a'
	%s			String of characters              "sample"

	#d			TAG "DATE" Y-M-D                  2018-08-17
	#t			TAG "TIME" H:M:S                  21:55:35
	#n			TAG "NOTICE"                      NOTICE -->
	#i			TAG "INFO"                        INFO ---->
	#w			TAG "WARNING"                     WARNING ->
	#e			TAG "ERROR"                       ERROR --->
*/

#ifndef __OBSIDIAN_TOOL_LOGGER__
#define __OBSIDIAN_TOOL_LOGGER__

#pragma once
#include <Windows.h>

typedef void*		OBSLOG;

static HMODULE		___OBSLOG_DLL = NULL;

typedef OBSLOG(*___OBSLOG_INIT_PROC)(const char*);
___OBSLOG_INIT_PROC ___OBSLOG_Init;
typedef unsigned int(*___OBSLOG_SETOPTIONS_PROC)(OBSLOG, unsigned int);
___OBSLOG_SETOPTIONS_PROC ___OBSLOG_SetOptions;
typedef unsigned int(*___OBSLOG_SETLOGSIZE_PROC)(OBSLOG, size_t);
___OBSLOG_SETLOGSIZE_PROC ___OBSLOG_SetLogSize;
typedef unsigned int(*___OBSLOG_GETLASTERROR_PROC)(OBSLOG);
___OBSLOG_GETLASTERROR_PROC ___OBSLOG_GetLastError;
typedef const char* (*___OBSLOG_GETERRORDESCRIPTION_PROC)(unsigned int);
___OBSLOG_GETERRORDESCRIPTION_PROC ___OBSLOG_GetErrorDescription;
typedef unsigned int(*___OBSLOG_PRINTF_PROC)(OBSLOG, const char*, ...);
___OBSLOG_PRINTF_PROC ___OBSLOG_Printf;
typedef const char* (*___OBSLOG_GETLOGDUMP_PROC)(OBSLOG);
___OBSLOG_GETLOGDUMP_PROC ___OBSLOG_GetLogDump;
typedef unsigned int(*___OBSLOG_CLEAR_PROC)(OBSLOG);
___OBSLOG_CLEAR_PROC ___OBSLOG_Clear;
typedef unsigned int(*___OBSLOG_WRITEFILE_PROC)(OBSLOG);
___OBSLOG_WRITEFILE_PROC ___OBSLOG_WriteFile;
typedef unsigned int(*___OBSLOG_FREE_PROC)(OBSLOG);
___OBSLOG_FREE_PROC ___OBSLOG_Free;

/* PUBLIC
---------------------------------------------------- */

#define OBSLOG_FILE_AUTOSAVE      1       /* 0b00000001 */
#define OBSLOG_FILE_REWRITE       2       /* 0b00000010 */

#define OBSLOG_DLL_ERRORCODE      999     /* Specific code for work with dll */

/* DLL SPRCIFIC FUNCTION
---------------------------------------------------- */

unsigned int OBSLOG_DLL_LOAD(void) {

	if (___OBSLOG_DLL == NULL) {

		___OBSLOG_DLL = LoadLibrary("OBSLOG_x64.dll");
		if (___OBSLOG_DLL == NULL) return OBSLOG_DLL_ERRORCODE;

		___OBSLOG_Init = (___OBSLOG_INIT_PROC)GetProcAddress(___OBSLOG_DLL, "OBSLOG_Init");
		___OBSLOG_SetOptions = (___OBSLOG_SETOPTIONS_PROC)GetProcAddress(___OBSLOG_DLL, "OBSLOG_SetOptions");
		___OBSLOG_SetLogSize = (___OBSLOG_SETLOGSIZE_PROC)GetProcAddress(___OBSLOG_DLL, "OBSLOG_SetLogSize");
		___OBSLOG_GetLastError = (___OBSLOG_GETLASTERROR_PROC)GetProcAddress(___OBSLOG_DLL, "OBSLOG_GetLastError");
		___OBSLOG_GetErrorDescription = (___OBSLOG_GETERRORDESCRIPTION_PROC)GetProcAddress(___OBSLOG_DLL, "OBSLOG_GetErrorDescription");
		___OBSLOG_Printf = (___OBSLOG_PRINTF_PROC)GetProcAddress(___OBSLOG_DLL, "OBSLOG_Printf");
		___OBSLOG_GetLogDump = (___OBSLOG_GETLOGDUMP_PROC)GetProcAddress(___OBSLOG_DLL, "OBSLOG_GetLogDump");
		___OBSLOG_Clear = (___OBSLOG_CLEAR_PROC)GetProcAddress(___OBSLOG_DLL, "OBSLOG_Clear");
		___OBSLOG_WriteFile = (___OBSLOG_WRITEFILE_PROC)GetProcAddress(___OBSLOG_DLL, "OBSLOG_WriteFile");
		___OBSLOG_Free = (___OBSLOG_FREE_PROC)GetProcAddress(___OBSLOG_DLL, "OBSLOG_Free");

	}

	return 0;

}

void OBSLOG_DLL_FREE(void) {

	if (___OBSLOG_DLL) {

		FreeLibrary(___OBSLOG_DLL);
		___OBSLOG_DLL = NULL;

	}

}

/* OBSLOG FUNCTIONS
---------------------------------------------------- */

OBSLOG OBSLOG_Init(const char* name) {

	if (!___OBSLOG_DLL) return NULL;
	return ___OBSLOG_Init(name);

}

unsigned int OBSLOG_SetOptions(OBSLOG log, unsigned int opt) {

	if (!___OBSLOG_DLL) return OBSLOG_DLL_ERRORCODE;
	return ___OBSLOG_SetOptions(log, opt);

}

unsigned int OBSLOG_SetLogSize(OBSLOG log, size_t size) {

	if (!___OBSLOG_DLL) return OBSLOG_DLL_ERRORCODE;
	return ___OBSLOG_SetLogSize(log, size);

}

unsigned int OBSLOG_Printf(OBSLOG log, const char* fmt, ...) {

	if (!___OBSLOG_DLL) return OBSLOG_DLL_ERRORCODE;
	return ___OBSLOG_Printf(log, fmt);

}

unsigned int OBSLOG_GetLastError(OBSLOG log) {

	if (!___OBSLOG_DLL) return OBSLOG_DLL_ERRORCODE;
	return ___OBSLOG_GetLastError(log);

}

const char* OBSLOG_GetErrorDescription(unsigned int error) {

	if (!___OBSLOG_DLL) return NULL;
	return ___OBSLOG_GetErrorDescription(error);

}

const char* OBSLOG_GetLogDump(OBSLOG log) {

	if (!___OBSLOG_DLL) return NULL;
	return ___OBSLOG_GetLogDump(log);

}

unsigned int OBSLOG_Clear(OBSLOG log) {

	if (!___OBSLOG_DLL) return OBSLOG_DLL_ERRORCODE;
	return ___OBSLOG_Clear(log);

}

unsigned int OBSLOG_WriteFile(OBSLOG log) {

	if (!___OBSLOG_DLL) return OBSLOG_DLL_ERRORCODE;
	return ___OBSLOG_WriteFile(log);

}

unsigned int OBSLOG_Free(OBSLOG log) {

	if (!___OBSLOG_DLL) return 1;
	return FreeLibrary(___OBSLOG_DLL);

}

/* ---------------------------------------------------- */
#endif /* !__OBSIDIAN_TOOL_LOGGER__ */
