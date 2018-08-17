#include "stdafx.h"

#pragma once

namespace ltk {

enum Events
{
    eSpriteFirst,
    ePaint,

    eMouseFirst,
    eMouseMove,
    eMouseEnter,
    eMouseLeave,
    eMouseWheel,
    eLBtnDown,
    eLBtnUp,
    eRBtnDown,
    eRBtnUp,
    eMouseLast,

    eKeyDown,
    eKeyUp,
    eCharInput,
    eImeInput,
    eSetFocus,
    eKillFocus,
    eSizeChanged,
    eRecreateResource,
    eSpriteLast     // ��������￪ʼ
};

struct Event
{
    UINT id;
};

struct MouseEvent : public Event
{
    UINT message;
    float x;
    float y;
    UINT flag; // ctrl shift atl
    float delta; // wheel
};

struct KeyEvent : public Event
{
    DWORD keyCode;
    DWORD flag;
};

struct PaintEvent : public Event
{
    ID2D1RenderTarget *target;
};

struct SizeEvent : public Event
{
    float width;
    float height;
};

struct ImeEvent : public Event
{
    LPCWSTR text;
};

class Sprite;

struct FocusEvent : public Event
{
    Sprite *oldFocus;
};


}
