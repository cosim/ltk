/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "stdafx.h"
#include "Common.h"
#include "UniConversion.h"


void LuaLog(LPCSTR strLogA)
{
	// �ж��Ƿ��LOG
	static int log = -1;
	if (log < 0)
	{
		log = ::PathFileExists(L"C:\\luaui_log");
		// CsDebugA("<LuaUI> PathFileExists:%d\r\n", log);
	}
	if (log == 0)
	{
		return;
	}
	// OutPutDebugStringA ��Ҫgbk ListView��Ҫutf16 ��lua������utf8 sqlite���и��ֿ�Դ�⻹ǿ��ʹ��utf8
	CString logW = Utf8ToUtf16(strLogA, strlen(strLogA)); // �����utf8תutf16
	CString line;
	int start = 0;
	int end = logW.Find(L'\n', start);
	//bool bMultiLine = false;

	while (end != -1)
	{
		//bMultiLine = true;
		line = logW.Mid(start, end - start);
		DebugOutputLine(line);
		start = end + 1;
		end = logW.Find(L'\n', start);
	}
	line = logW.Mid(start);
	DebugOutputLine(line);

	//if (bMultiLine)
	//{
	//	DebugOutputLine(L"----------");
	//}

	// ��������ԭ�� �������� ��һ����ʱ�� 1�����һ�� ������� eclipse logcat 
}

static void DebugOutputLine(CString line)
{
	line.Replace(L"\t", L"  ");
	line.Insert(0, L"<LuaUI> ");
	line.Append(L"\r\n");
	::OutputDebugStringW(line);
	// line.Replace(L"\r\n", L"\\r\\n"); // ���溯���ѷֹ�����
}

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

// ����˵�е㲻��ȫ ϣ�����ҵ����õİ汾 ����gtk�ڲ�����  TODO fltk������ (2016-08-23)
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

