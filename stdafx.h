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

#include <new> // 没这个 placement new 就不能编译
#include <cassert>
#include <atlstr.h>

#include <GdiPlus.h>
#include <ShlObj.h>
#include <vld.h>
#include <atlconv.h>

#include <d2d1.h>
#include <Wincodec.h>
#include <Dwrite.h>

#include <functional>

#define SAFE_RELEASE(p) if (p) { p->Release(); p = NULL; }
#define DEF_SMART_PTR(type) typedef std::shared_ptr<type> type##Ptr; typedef std::weak_ptr<type> type##Weak;

#include "duktape.h"

//#include <boost/signals2.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/enable_shared_from_this.hpp>
//
//using boost::shared_ptr;
//using boost::weak_ptr;
