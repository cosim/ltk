/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/

#pragma once

#include <new> // for `placement new`
#include <cassert>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <memory>
#include <algorithm>
#include <functional>
#include <thread>

using std::wstring;
using std::wstringstream;
using std::stringstream;
using std::unique_ptr;

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <commdlg.h>

#include <tchar.h>
#include <strsafe.h>
#include <atlstr.h>
#include <atlconv.h>

#include <GdiPlus.h>
using Gdiplus::RectF;
using Gdiplus::SizeF;
using Gdiplus::Color;

#include <ShlObj.h>

#include <d2d1.h>
#include <Wincodec.h>
#include <Dwrite.h>

#include "luajit\lua.hpp"

#include "tinyxml/tinyxml2.h"

#include <vld.h>

