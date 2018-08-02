#pragma once

/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/


CStringW Utf8ToUtf16(LPCSTR strA, int len = -1);
CStringA Utf16ToUtf8(LPCTSTR strW, int len);

static void DebugOutputLine(CString line);

#define LOG(msg) do\
{\
	std::stringstream ss;\
	ss << __FUNCTION__ << "() " msg << std::endl;\
	OutputDebugStringA(ss.str().c_str());\
} while (0)

#define LOGW(msg) do\
{\
	std::wstringstream ss;\
	ss << __FUNCTIONW__ << L"() " msg << std::endl;\
	OutputDebugString(ss.str().c_str());\
} while (0)

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);             \
	void operator=(const TypeName&)

// https://stackoverflow.com/questions/3060006/is-it-worth-setting-pointers-to-null-in-a-destructor
#ifndef INVALID_POINTER
#define INVALID_POINTER(type) reinterpret_cast<type *>(0xDEADBEEF)
#endif

class DukStackChecker
{
public:
    DukStackChecker(duk_context *ctx_)
    {
        ctx = ctx_;
        init_top = duk_get_top(ctx);
        num_ret = 0;
    }

    ~DukStackChecker()
    {
        if (duk_get_top(ctx) - num_ret != init_top)
        {
            __debugbreak();
        }
    }

    duk_idx_t Return(duk_idx_t num)
    {
        num_ret = num;
        return num_ret;
    }

private:
    duk_context *ctx;
    duk_idx_t init_top;
    duk_idx_t num_ret;
};