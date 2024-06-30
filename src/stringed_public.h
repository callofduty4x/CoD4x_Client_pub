#ifndef COD4X_SRC_STRINGED_PUBLIC_H
#define COD4X_SRC_STRINGED_PUBLIC_H

enum msgLocErrType_t {
    LOCMSG_SAFE = 0x0,
    LOCMSG_NOERR = 0x1
};

unsigned int  SEH_DecodeLetter(unsigned char firstChar, unsigned char secondChar, int *usedCount);
const char*   SEH_LocalizeTextMessage(const char *pszInputBuffer, const char *pszMessageType, enum msgLocErrType_t errType);
const char*   SEH_StringEd_GetString(const char *pszReference);
int           SEH_GetCurrentLanguage();
int           SEH_PrintStrlen(const char *string);

#endif // COD4X_SRC_STRINGED_PUBLIC_H
