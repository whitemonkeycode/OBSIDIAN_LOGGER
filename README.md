# OBSIDIAN_LOGGER

### Simple and easy library in C language "LOGGER" from the utility set "OBSIDIAN TOOLS". Designed for easy messaging when debugging an application.


### Key features:

  - Formatted output is simplified (specification differs from "printf ...")
  - All functions return the last error code. 0 if there is no error
  - For any code you can get a description
  - There is a date tag "#d" and a time "#t". Tags are part of the formatting string
  - Customizable buffer size
  - Write log to disk with modes (rewrite / new "000, 001, ...")
  - Autosave option when filling buffer
  - Forced file write
  - If the file extension is not specified, the default is "* .txt"

### Some limitations and conventions are:

 - Default log buffer size 32768 bytes (min 2048 / max 8388608)
 - Max message size = Current log buffer size
 - Max file number 999, start at 001
 - The log name is the path to save the file
 - The maximum length of the name is limited to 248 characters including the file extension
 
 
### Formatting characters

Char | Description | Output
---- | ----------- | ------
%i | Signed integer (lowercase) | -123
%u | Unsigned integer (lowercase) | 456
%I | Signed long long (uppercase) | -123456
%U | Unsigned long long (uppercase) | 456789
%f | Floating point (float / double) | 392.65
%x | Unsigned hex int (lowercase) | 7fa
%X | Unsigned hex int (uppercase) | 7FA
%c | Character | 'a'
%s | String of characters | "sample"
#d | TAG "DATE" Y-M-D | 2018-08-17
#t | TAG "TIME" H:M:S | 21:55:35
#n | TAG "NOTICE" | NOTICE -->
#i | TAG "INFO" | INFO ---->
#w | TAG "WARNING" | WARNING ->
#e | TAG "ERROR" | ERROR --->


## Beginning of work

### If you want to use the **source code**:
```C
#include "OBSLOG.h"
```
### If you want to use the **DLL**:
The OBSLOG_DLL_LOAD function returns an integer. (0 - Success) or (another number - Error).

```C
#include "OBSLOG_x32.h"
...
OBSLOG_DLL_LOAD();
```
or
```C
#include "OBSLOG_x64.h"
...
OBSLOG_DLL_LOAD();
```

## Example of use


Create log. 
The extension can be **any** or **none** (*.txt added by default).
```C

unsigned int err

int i = -777;
unsigned int u = 555;
long long ll = -777777;
unsigned long long ull = 555555;
float f = -123.0f;
double d = -456.789;
char c = 'Q';
char s[] = "Hello world!";

OBSLOG log = OBSLOG_Init("some_name.txt");
err = OBSLOG_GetLastError(log);
if (err) {
	printf("Last error description: %s\n", OBSLOG_GetErrorDescription(err));
}

/* Set options (can be combined (opt_1 | opt_2))
 - OBSLOG_FILE_AUTOSAVE - Save file on disk when filling buffer
 - OBSLOG_FILE_REWRITE - Write log to disk with modes (**rewrite** or **new** "000, 001, ...") */

OBSLOG_SetOptions(log, OBSLOG_FILE_AUTOSAVE | OBSLOG_FILE_REWRITE);
OBSLOG_SetLogSize(log, 65536);

OBSLOG_Printf(log, "\n---------------------------------------\n");
OBSLOG_Printf(log, "Integer: %i\n", i);
OBSLOG_Printf(log, "Unsigned: %u\n", u);
OBSLOG_Printf(log, "Long long: %I\n", ll);
OBSLOG_Printf(log, "Unsigned long long: %U\n", ull);
OBSLOG_Printf(log, "Hex lower case: %x\n", i);
OBSLOG_Printf(log, "Hex upper case: %X\n", i);
OBSLOG_Printf(log, "Floating point: %f\n", f);
OBSLOG_Printf(log, "Double: %f\n", d);
OBSLOG_Printf(log, "Char: %c\n", c);
OBSLOG_Printf(log, "String: %s\n", s);

OBSLOG_Printf(log, "Tag 'n': #n\n");
OBSLOG_Printf(log, "Tag 'i': #i\n");
OBSLOG_Printf(log, "Tag 'w': #w\n");
OBSLOG_Printf(log, "Tag 'e': #e\n");
OBSLOG_Printf(log, "Tag 'd': #d\n");
OBSLOG_Printf(log, "Tag 't': #t\n");

OBSLOG_WriteFile(log);
OBSLOG_Free(log);

```

Output
```
Integer: -777
Unsigned: 555
Long long: -777777
Unsigned long long: 555555
Hex lower case: fffffcf7
Hex upper case: FFFFFCF7
Floating point: -123.000000
Double: -456.789000
Char: Q
String: Hello world!
Tag 'n':  NOTICE --> 
Tag 'i':  INFO ----> 
Tag 'w':  WARNING -> 
Tag 'e':  ERROR ---> 
Tag 'd':  2018-10-20 
Tag 't':  23:08:00
```

### All OBSIDIAN LOGGER function

```C
OBSLOG				OBSLOG_Init(const char* name);
unsigned int			OBSLOG_SetOptions(OBSLOG log, unsigned int opt);
unsigned int			OBSLOG_SetLogSize(OBSLOG log, size_t size);
unsigned int			OBSLOG_Printf(OBSLOG log, const char* fmt, ...);
unsigned int			OBSLOG_GetLastError(OBSLOG log);
const char*			OBSLOG_GetErrorDescription(unsigned int error);
const char*			OBSLOG_GetLogDump(OBSLOG log);
unsigned int			OBSLOG_Clear(OBSLOG log);
unsigned int			OBSLOG_WriteFile(OBSLOG log);
unsigned int			OBSLOG_Free(OBSLOG log);
```

### License

> This software product is distributed under the license MIT
> Copyright (c) 2018 Mogulev Alexey forawhitemonkey@gmail.com

> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:
> 
> The above copyright notice and this permission notice shall be included in all
> copies or substantial portions of the Software.
> 
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
> SOFTWARE.
