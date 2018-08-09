/*
** LTK -- a lightweight GUI toolkit based on Direct2D
**
** Copyright (C) 2015-2016 Chen Shi. All rights reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** [ MIT license: http://www.opensource.org/licenses/mit-license.php ]
*/

#ifdef LTK_EXPORTS
#define LTK_API __declspec(dllexport)
#else
#define LTK_API __declspec(dllimport)
#endif

namespace ltk {
    void LtkInitialize();
    void LtkUninitialize();
    ID2D1Factory *GetD2DFactory();
    IWICImagingFactory *GetWICFactory();
    IDWriteFactory *GetDWriteFactory();
    void TranslateTransform(ID2D1RenderTarget *target, float dx, float dy);
    D2D1_RECT_F GdipRectF2D2D1RectF(const RectF &rc);
	HRESULT LoadBitmapFromFile(ID2D1RenderTarget * target, LPCWSTR path, ID2D1Bitmap ** bitmap);
    lua_State *GetGlobalLuaState();
}
