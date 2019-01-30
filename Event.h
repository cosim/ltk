#include "stdafx.h"
#include "RTTI.h"

#pragma once

namespace ltk {

enum Events
{
    eNotification,

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
    eSpriteLast,     // 子类从这里开始

    eWindowFirst = 1000,
    eButtonFirst = 2000,
};

struct Event : public RTTI
{
    RTTI_DECLARATIONS(Event, RTTI);
    UINT id;
};

struct Notification : public Event
{
    RTTI_DECLARATIONS(Notification, Event);
    RTTI *sender;
};

struct MouseEvent : public Event
{
    UINT message;
    float x;
    float y;
    UINT flag; // ctrl shift atl
    float delta; // wheel
};

struct ButtonMouseEvent : public Notification
{
    MouseEvent data;
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
