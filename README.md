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

### Example of use

First step.
```C
#include "OBSLOG.h"
```
Each function returns the last error code. 
It makes sense to have a variable for it.
```C
unsigned int err
```
Create log. 
The extension can be **any** or **none** (*.txt added by default).
```C
OBSLOG log = OBSLOG_Init("some_name.txt");
err = OBSLOG_GetLastError(log);
if (err) {
	printf("Last error description: %s\n", OBSLOG_GetErrorDescription(err));
}
```
Set options
  * OBSLOG_FILE_AUTOSAVE - Save file on disk when filling buffer
  * OBSLOG_FILE_REWRITE - Write log to disk with modes (**rewrite** or **new** "000, 001, ...")
Options can be combined (opt_1 || opt_2)
```C
OBSLOG_SetOptions(log, OBSLOG_FILE_AUTOSAVE);
```
Set log buffer size in bytes (min 2048 / max 8388608)
```C
OBSLOG_SetLogSize(log, 2000000);
```
