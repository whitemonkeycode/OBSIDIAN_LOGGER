/* LICENCE
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
---------------------------------------------------- */

#define _CRT_SECURE_NO_WARNINGS

#ifndef __OBSIDIAN_TOOL_LOGGER__
#define __OBSIDIAN_TOOL_LOGGER__
#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

/* OPTIONS
---------------------------------------------------- */

#define OBSLOG_FILE_AUTOSAVE	1 /* 0b00000001 */
#define OBSLOG_FILE_REWRITE		2 /* 0b00000010 */

/* DESCRIPTION OF FORMATTING CHARACTERS
----------------------------------------------------

%i 	Signed integer 			-392
%u 	Unsigned integer 			7235
%I 	Signed long long 			-392
%U 	Unsigned long long 			7235
%f 	Floating point 			392.65
%x 	Unsigned hex integer (lowercase) 			7fa
%X 	Unsigned hex integer (uppercase) 			7FA
%c 	Character 			'a'
%s 	String of characters 			"sample"

#n 	TAG "NOTICE" 			NOTICE -->
#i 	TAG "INFO" 			INFO ---->
#w 	TAG "WARNING" 			WARNING ->
#e 	TAG "ERROR" 			ERROR --->
#d 	TAG "DATE" Y-M-D 			2018-08-17
#t 	TAG "TIME" H:M:S 			21:55:35

*/

/* PUBLIC FUNCTIONS AND DEFINITIONS
---------------------------------------------------- */

typedef void* 		OBSLOG;

OBSLOG 				OBSLOG_Init(const char* name);
unsigned int        OBSLOG_SetOptions(OBSLOG log, unsigned int opt);
unsigned int        OBSLOG_SetLogSize(OBSLOG log, size_t size);
unsigned int        OBSLOG_Printf(OBSLOG log, const char* fmt, ...);
unsigned int        OBSLOG_GetLastError(OBSLOG log);
const char*         OBSLOG_GetErrorDescription(unsigned int error);
const char*         OBSLOG_GetLogDump(OBSLOG log);
unsigned int        OBSLOG_Clear(OBSLOG log);
unsigned int        OBSLOG_WriteFile(OBSLOG log);
unsigned int        OBSLOG_Free(OBSLOG log);

#endif /* !__OBSIDIAN_TOOL_LOGGER__ */
