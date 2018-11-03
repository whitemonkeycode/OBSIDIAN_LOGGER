#include "OBSLOG.h"

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

/* HIDDEN REALIZATIONS - DEFINITIONS
---------------------------------------------------- */

#define ___OBSLOG_OPT_DEF_SET		0 /* 0b00000000 */
#define ___OBSLOG_OPT_MAX_SET		3 /* 0b00000011 */

#define ___OBSLOG_STG_DEF_LEN		32768
#define ___OBSLOG_STG_MIN_LEN		2048
#define ___OBSLOG_STG_MAX_LEN		8388608

#define ___OBSLOG_FLE_CNT_MIN		1
#define ___OBSLOG_FLE_CNT_MAX		999
#define ___OBSLOG_FLE_CNT_DIG		3

#define ___OBSLOG_NME_WIN_LEN		248
#define ___OBSLOG_NME_MAX_LEN		(___OBSLOG_NME_WIN_LEN - ___OBSLOG_FLE_CNT_DIG - 1)

#define ___OBSLOG_TAG_MAX_VAL		4

#define ___OBSLOG_TAG_NOTICE		0
#define ___OBSLOG_TAG_INFO			1
#define ___OBSLOG_TAG_WARNING		2
#define ___OBSLOG_TAG_ERROR			3

#define ___OBSLOG_ERR_MAX_VAL		9

#define OBSLOG_ERRID_NO_ERROR		0
#define OBSLOG_ERRID_VAL_WRNG		1
#define OBSLOG_ERRID_NME_WRNG		2
#define OBSLOG_ERRID_MSG_SIZE		3
#define OBSLOG_ERRID_MSG_FRMT		4
#define OBSLOG_ERRID_FLE_FAIL		5
#define OBSLOG_ERRID_LOG_FAIL		6
#define OBSLOG_ERRID_MEM_FAIL		7
#define OBSLOG_ERRID_SYS_ERER		8

const char* OBSLOG_error[___OBSLOG_ERR_MAX_VAL] = {
	"Errors not detected.",
	"Warning! The value of the passed argument can not be processed, or it is out of the range of allowed values.",
	"Warning! The name or path to the file (including the name) is longer than the specified value (248 bytes along with the line restriction symbol).",
	"Warning! Message size is larger than the set value. LOGGER API allows you to know the current size of the message buffer or to set it.",
	"Warning! Error in format string. Possible formatting options are described in the documentation for the LOGGER API.",
	"Warning! System error. File output error, occurs when you can not create, modify, or otherwise manipulate a file.",
	"Warning! Error accessing the LOG object. The object is not initialized or the length of the given name is greater than 248 characters including the end-of-line character.",
	"Warning! System error. It is possible if the system fails to allocate the required amount of memory.",
	"Warning! System error. It is not possible to return a description of the error, because the requested code is outside the possible range of values."
};

const char* OBSLOG_tag[___OBSLOG_TAG_MAX_VAL] = {
	" NOTICE --> ",
	" INFO ----> ",
	" WARNING -> ",
	" ERROR ---> "
};

typedef struct {
	char*			name;
	char*			data;
	size_t			data_Size;
	unsigned int	error_Id;
	unsigned int	options;
	unsigned int	needSave_Flag;
	size_t			curPos;
	unsigned int	fileNumCounter;
} OBSLOG_STORAGE;

OBSLOG_STORAGE* storage;

/* HIDDEN REALIZATIONS - FUNCTIONS
---------------------------------------------------- */

unsigned int ___OBSLOG_WriteStorageToFile(OBSLOG_STORAGE* storagePtr) {

	FILE* f;
	size_t tempFileCounter = storagePtr->fileNumCounter;
	size_t storageRealLength = strlen(storagePtr->data);
	size_t nameRealLength = strlen(storagePtr->name);
	size_t tempSize;
	const unsigned int fileCounterNumOfDigits = 4;
	char tempCounterStringBuffer[4] = "";
	char tempNameBuffer[___OBSLOG_NME_MAX_LEN + ___OBSLOG_FLE_CNT_DIG + 1] = "";
	char* lastDotSymbolPtr;

	if (storagePtr->needSave_Flag && storageRealLength && storagePtr->fileNumCounter <= ___OBSLOG_FLE_CNT_MAX) {

		lastDotSymbolPtr = strrchr(storagePtr->name, '.');

		/* IF .extention INCLUDED */

		if (lastDotSymbolPtr != NULL) {

			tempSize = lastDotSymbolPtr - storagePtr->name;

			if (storagePtr->options & OBSLOG_FILE_REWRITE) {
				strcpy(tempNameBuffer, storagePtr->name);
			}
			else {
				strncpy(tempNameBuffer, storagePtr->name, tempSize);
				sprintf(tempCounterStringBuffer, "%03u", storagePtr->fileNumCounter);
				strcat(tempNameBuffer, tempCounterStringBuffer);
				strcat(tempNameBuffer, storagePtr->name + tempSize);
			}
		}

		/* IF NO .extention */

		else {

			if (storagePtr->options & OBSLOG_FILE_REWRITE) {
				strcpy(tempNameBuffer, storagePtr->name);
				strcat(tempNameBuffer, ".txt");
			}
			else {
				strcpy(tempNameBuffer, storagePtr->name);
				sprintf(tempCounterStringBuffer, "%03u", storagePtr->fileNumCounter);
				strcat(tempNameBuffer, tempCounterStringBuffer);
				strcat(tempNameBuffer, ".txt");
			}
		}

		f = fopen(tempNameBuffer, "w");
		if (!f) return storagePtr->error_Id = OBSLOG_ERRID_FLE_FAIL;
		fprintf(f, "%s", storagePtr->data);
		fclose(f);

		if (storagePtr->fileNumCounter < ___OBSLOG_FLE_CNT_MAX) storagePtr->fileNumCounter++;
		else storagePtr->error_Id = OBSLOG_ERRID_FLE_FAIL;
		storagePtr->needSave_Flag = FALSE;
	}

	return OBSLOG_ERRID_NO_ERROR;
}

void ___OBSLOG_RefreshStorage(OBSLOG_STORAGE* storagePtr, size_t curPos_BeforeMessage) {

	size_t tempSizeVar, i;
	char* tempBuffer_PartOfTheMessage;

	tempSizeVar = storagePtr->curPos - curPos_BeforeMessage;
	if (tempSizeVar) {
		tempSizeVar++;
		tempBuffer_PartOfTheMessage = (char*)malloc(tempSizeVar);
		if (tempBuffer_PartOfTheMessage) {
			memset(tempBuffer_PartOfTheMessage, 0, tempSizeVar);
			for (i = 0; i < storagePtr->curPos - curPos_BeforeMessage; i++) {
				tempBuffer_PartOfTheMessage[i] = storagePtr->data[i + curPos_BeforeMessage];
			}
		}
		if (storagePtr->options & OBSLOG_FILE_AUTOSAVE) {
			storagePtr->needSave_Flag = TRUE;
			___OBSLOG_WriteStorageToFile(storagePtr);
		}
		memset(storagePtr->data, 0, storagePtr->data_Size);
		if (tempBuffer_PartOfTheMessage) {
			strcat(storagePtr->data, tempBuffer_PartOfTheMessage);
			storagePtr->curPos = strlen(storagePtr->data);
		}
		else {
			storagePtr->curPos = 0;
		}
		free(tempBuffer_PartOfTheMessage);
	}
	else {
		if (storagePtr->options & OBSLOG_FILE_AUTOSAVE) {
			storagePtr->needSave_Flag = TRUE;
			___OBSLOG_WriteStorageToFile(storagePtr);
		}
		memset(storagePtr->data, 0, storagePtr->data_Size);
		storagePtr->curPos = 0;
	}

}

/* PUBLIC FUNCTIONS
---------------------------------------------------- */

OBSLOG OBSLOG_Init(const char* name) {

	size_t name_Len;

	name_Len = strlen(name);
	if (!name_Len || name_Len >= ___OBSLOG_NME_MAX_LEN) return NULL;

	storage = (OBSLOG_STORAGE*)malloc(sizeof(OBSLOG_STORAGE));
	if (!storage) return NULL;

	storage->name = (char*)malloc(___OBSLOG_NME_MAX_LEN);
	if (!storage->name) return NULL;
	else {
		memset(storage->name, 0, ___OBSLOG_NME_MAX_LEN);
		memcpy(storage->name, name, name_Len);
	}

	storage->data = (char*)malloc(___OBSLOG_STG_DEF_LEN);
	if (!storage->data) return NULL;
	else {
		memset(storage->data, 0, ___OBSLOG_STG_DEF_LEN);
		storage->data_Size = ___OBSLOG_STG_DEF_LEN;
	}

	storage->error_Id = OBSLOG_ERRID_NO_ERROR;
	storage->options = ___OBSLOG_OPT_DEF_SET;
	storage->curPos = 0;
	storage->fileNumCounter = ___OBSLOG_FLE_CNT_MIN;
	storage->needSave_Flag = TRUE;

	return (OBSLOG)storage;
}

/* -------------------------------------------------- */

unsigned int OBSLOG_SetOptions(OBSLOG log, unsigned int opt) {

	OBSLOG_STORAGE* tempLog;

	if (!log) return OBSLOG_ERRID_LOG_FAIL;
	else tempLog = (OBSLOG_STORAGE*)log;

	if (opt > ___OBSLOG_OPT_MAX_SET) return tempLog->error_Id = OBSLOG_ERRID_VAL_WRNG;
	
	tempLog->options ^= opt;

	return OBSLOG_ERRID_NO_ERROR;

}

/* -------------------------------------------------- */

unsigned int OBSLOG_SetLogSize(OBSLOG log, size_t size) {

	OBSLOG_STORAGE* tempLog;
	char* newMemoryBlockPtr;

	if (!log) return OBSLOG_ERRID_LOG_FAIL;

	tempLog = (OBSLOG_STORAGE*)log;
	if (size < ___OBSLOG_STG_MIN_LEN || size > ___OBSLOG_STG_MAX_LEN) return tempLog->error_Id = OBSLOG_ERRID_VAL_WRNG;
	if (tempLog->data_Size == size) return OBSLOG_ERRID_NO_ERROR;

	newMemoryBlockPtr = (char*)malloc(size);
	if (!newMemoryBlockPtr) return tempLog->error_Id = OBSLOG_ERRID_MEM_FAIL;

	memset(newMemoryBlockPtr, 0, size);
	free(tempLog->data);
	tempLog->data = newMemoryBlockPtr;
	tempLog->data_Size = size;
	tempLog->curPos = 0;
	tempLog->needSave_Flag = TRUE;

	return OBSLOG_ERRID_NO_ERROR;
}

/* -------------------------------------------------- */

unsigned int OBSLOG_Printf(OBSLOG log, const char* fmt, ...) {

	OBSLOG_STORAGE*		tempLog;

	char				tempNumBuffer[64];
	size_t				strLength;
	time_t				timer;
	char				timeStamp[64];
	struct tm*			tm_info;
	size_t				last_CurPos;

	va_list				argList;

	int					SIGNED_INT;
	unsigned int		UNSIGNED_INT;
	long long			SIGNED_LL;
	unsigned long long	UNSIGNED_LL;
	size_t				USIZET;
	double				DOUBLEFP;
	char				SYMBOL;
	char*				STRING_PTR;

	if (!log) return OBSLOG_ERRID_LOG_FAIL;
	else tempLog = (OBSLOG_STORAGE*)log;

	last_CurPos = tempLog->curPos;

	va_start(argList, fmt);
	/* >>>>>>>>>> */

	while (*fmt) {
		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
				case 'i': {
					SIGNED_INT = va_arg(argList, int);
					sprintf(tempNumBuffer, "%d", SIGNED_INT);
					strLength = strlen(tempNumBuffer);

					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, tempNumBuffer);
					tempLog->curPos += strLength;
					break;
				}
				case 'I': {
					SIGNED_LL = va_arg(argList, long long);
					sprintf(tempNumBuffer, "%lld", SIGNED_LL);
					strLength = strlen(tempNumBuffer);

					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, tempNumBuffer);
					tempLog->curPos += strLength;
					break;
				}
				case 'u': {
					UNSIGNED_INT = va_arg(argList, unsigned int);
					sprintf(tempNumBuffer, "%u", UNSIGNED_INT);
					strLength = strlen(tempNumBuffer);

					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, tempNumBuffer);
					tempLog->curPos += strLength;
					break;
				}
				case 'U': {
					UNSIGNED_LL = va_arg(argList, unsigned long long);
					sprintf(tempNumBuffer, "%llu", UNSIGNED_LL);
					strLength = strlen(tempNumBuffer);

					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, tempNumBuffer);
					tempLog->curPos += strLength;
					break;
				}
				case 'x': {
					USIZET = va_arg(argList, size_t);
					sprintf(tempNumBuffer, "%zx", USIZET);
					strLength = strlen(tempNumBuffer);

					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, tempNumBuffer);
					tempLog->curPos += strLength;
					break;
				}
				case 'X': {
					USIZET = va_arg(argList, size_t);
					sprintf(tempNumBuffer, "%zX", USIZET);
					strLength = strlen(tempNumBuffer);

					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, tempNumBuffer);
					tempLog->curPos += strLength;
					break;
				}
				case 'f': {
					DOUBLEFP = va_arg(argList, double);
					sprintf(tempNumBuffer, "%f", DOUBLEFP);
					strLength = strlen(tempNumBuffer);

					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, tempNumBuffer);
					tempLog->curPos += strLength;
					break;
				}
				case 'c': {
					SYMBOL = va_arg(argList, char);

					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					tempLog->data[tempLog->curPos] = SYMBOL;
					tempLog->curPos++;
					break;
				}
				case 's': {
					STRING_PTR = va_arg(argList, char*);
					strLength = strlen(STRING_PTR);
					if (strLength >= ((tempLog->data_Size - 1) - (tempLog->curPos - last_CurPos))) break;

					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, STRING_PTR);
					tempLog->curPos += strLength;
					break;
				}
				default: {
					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					tempLog->data[tempLog->curPos] = *fmt;
					tempLog->curPos++;
					break;
				}
			}
		}
		else if (*fmt == '#') {
			fmt++;
			switch (*fmt) {
				case 'n': {
					strLength = strlen(OBSLOG_tag[___OBSLOG_TAG_NOTICE]);
					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, OBSLOG_tag[___OBSLOG_TAG_NOTICE]);
					tempLog->curPos += strLength;
					break;
				}
				case 'i': {
					strLength = strlen(OBSLOG_tag[___OBSLOG_TAG_INFO]);
					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, OBSLOG_tag[___OBSLOG_TAG_INFO]);
					tempLog->curPos += strLength;
					break;
				}
				case 'w': {
					strLength = strlen(OBSLOG_tag[___OBSLOG_TAG_WARNING]);
					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, OBSLOG_tag[___OBSLOG_TAG_WARNING]);
					tempLog->curPos += strLength;
					break;
				}
				case 'e': {
					strLength = strlen(OBSLOG_tag[___OBSLOG_TAG_ERROR]);
					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, OBSLOG_tag[___OBSLOG_TAG_ERROR]);
					tempLog->curPos += strLength;
					break;
				}
				case 'd': {

					time(&timer);
					tm_info = localtime(&timer);
					strftime(timeStamp, 26, " %Y-%m-%d ", tm_info);
					strLength = strlen(timeStamp);
					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, timeStamp);
					tempLog->curPos += strLength;
					break;
				}
				case 't': {
					time(&timer);
					tm_info = localtime(&timer);
					strftime(timeStamp, 26, " %H:%M:%S ", tm_info);
					strLength = strlen(timeStamp);
					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - strLength - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					strcat(tempLog->data, timeStamp);
					tempLog->curPos += strLength;
					break;
				}
				default: {
					/* Check storage free space capacity */
					if (tempLog->curPos >= (tempLog->data_Size - 1)) {
						___OBSLOG_RefreshStorage(tempLog, last_CurPos);
						last_CurPos = 0;
					}

					tempLog->data[tempLog->curPos] = *fmt;
					tempLog->curPos++;
					break;
				}
			}
		}
		else {
			/* Check storage free space capacity */
			if (tempLog->curPos >= (tempLog->data_Size - 1)) {
				___OBSLOG_RefreshStorage(tempLog, last_CurPos);
				last_CurPos = 0;
			}

			tempLog->data[tempLog->curPos] = *fmt;
			tempLog->curPos++;
		}
		fmt++;

	}

	/* <<<<<<<<<< */
	va_end(argList);

	return OBSLOG_ERRID_NO_ERROR;
}

/* -------------------------------------------------- */

unsigned int OBSLOG_GetLastError(OBSLOG log) {

	OBSLOG_STORAGE* tempLog;

	if (!log) return OBSLOG_ERRID_LOG_FAIL;	
	else tempLog = (OBSLOG_STORAGE*)log;

	return tempLog->error_Id;
}

/* -------------------------------------------------- */

const char* OBSLOG_GetErrorDescription(unsigned int error) {

	if (error >= ___OBSLOG_ERR_MAX_VAL) return OBSLOG_error[OBSLOG_ERRID_SYS_ERER];
	return OBSLOG_error[error];
}

/* -------------------------------------------------- */

const char* OBSLOG_GetLogDump(OBSLOG log) {

	OBSLOG_STORAGE* tempLog;

	if (!log) return OBSLOG_error[OBSLOG_ERRID_LOG_FAIL];
	else tempLog = (OBSLOG_STORAGE*)log;

	return tempLog->data;
}

/* -------------------------------------------------- */

unsigned int OBSLOG_Clear(OBSLOG log) {

	OBSLOG_STORAGE* tempLog;

	if (!log) return OBSLOG_ERRID_LOG_FAIL;
	else tempLog = (OBSLOG_STORAGE*)log;
	memset(tempLog->data, 0, tempLog->data_Size);
	tempLog->needSave_Flag = TRUE;

	return OBSLOG_ERRID_NO_ERROR;

}

/* -------------------------------------------------- */

unsigned int OBSLOG_WriteFile(OBSLOG log) {

	OBSLOG_STORAGE* tempLog;
	unsigned int tempErrorValue;

	if (!log) return OBSLOG_ERRID_LOG_FAIL;
	else tempLog = (OBSLOG_STORAGE*)log;

	tempLog->needSave_Flag = TRUE;

	tempErrorValue = ___OBSLOG_WriteStorageToFile(tempLog);

	return tempErrorValue;

}

/* -------------------------------------------------- */

unsigned int OBSLOG_Free(OBSLOG log) {

	OBSLOG_STORAGE* tempLog;
	unsigned int tempErrorValue;

	if (!log) return OBSLOG_ERRID_LOG_FAIL;
	else tempLog = (OBSLOG_STORAGE*)log;

	if (tempLog->options & OBSLOG_FILE_AUTOSAVE) {
		tempLog->needSave_Flag = TRUE;
		___OBSLOG_WriteStorageToFile(tempLog);
	}

	tempErrorValue = tempLog->error_Id;
	free((char*)tempLog->name);
	free((char*)tempLog->data);

	free((OBSLOG_STORAGE*)tempLog);
	log = NULL;

	return tempErrorValue;
}
