/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <commdlg.h>

#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <memory>

using std::wstring;
using std::wstringstream;
using std::stringstream;
using std::unique_ptr;

#include <new> // for `placement new`
#include <cassert>
#include <atlstr.h>

#include <GdiPlus.h>
using Gdiplus::RectF;
using Gdiplus::SizeF;
using Gdiplus::Color;

#include <ShlObj.h>
#include <vld.h>
#include <atlconv.h>

#include <d2d1.h>
#include <Wincodec.h>
#include <Dwrite.h>

#include <functional>

#define SAFE_RELEASE(p) if (p) { p->Release(); p = NULL; }

#include "lua.h"
#include "lauxlib.h"
