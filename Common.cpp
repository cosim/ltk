/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/

#include "stdafx.h"
#include "Common.h"
#include "UniConversion.h"

CStringW Utf8ToUtf16(LPCSTR strA, int len)
{
	if(len < 0)
	{
		len = strlen(strA);
	}
	CStringW strW;
	int lenW = UTF16Length(strA, len);
	wchar_t *pbuff = strW.GetBuffer(lenW + 1);
	int ret = UTF16FromUTF8(strA, len, pbuff, lenW);
	strW.ReleaseBuffer(ret);
	return strW;
}

CStringA Utf16ToUtf8(LPCTSTR strW, int len)
{
	CStringA strA;
	int lenA = UTF8Length(strW, len);
	char *pbuff = strA.GetBuffer(lenA + 1);
	UTF8FromUTF16(strW, len, pbuff, lenA);
	strA.ReleaseBuffer(lenA);
	return strA;
}

// 网上说有点不安全 希望能找到更好的版本 比如gtk内部带的  TODO fltk里面有 (2016-08-23)
///Returns -1 if string is valid. Invalid character is put to ch.
int GetInvalidUtf8SymbolPosition(const char *input, char &ch) {
	int                 nb = 0, na = 0;
	const char *c = input;

	for (c = input; *c; c += (nb + 1)) {
		if (!(*c & 0x80))
			nb = 0;
		else if ((*c & 0xc0) == 0x80)
		{
			ch = *c;
			return (int)c - (int)input;
		}
		else if ((*c & 0xe0) == 0xc0)
			nb = 1;
		else if ((*c & 0xf0) == 0xe0)
			nb = 2;
		else if ((*c & 0xf8) == 0xf0)
			nb = 3;
		else if ((*c & 0xfc) == 0xf8)
			nb = 4;
		else if ((*c & 0xfe) == 0xfc)
			nb = 5;
		na = nb;
		while (na-- > 0)
			if ((*(c + nb) & 0xc0) != 0x80)
			{
				ch = *(c + nb);
				return (int)(c + nb) - (int)input;
			}
	}

	return -1;
}

