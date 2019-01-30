#include "stdafx.h"
#include "RTTI.h"

#pragma once

namespace ltk {

enum Events
{
    eSpriteFirst,
    ePaint,

    // type: MouseEvent
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
    eSpriteLast, 

    // type: Notification
    eClicked,
    eValueChanged,
    eDeleted,

    // type: DelegateMouseEvent
    eDelegateMouseEvent,

    eWindowFirst = 1000,
    eButtonFirst = 2000,
    eScrollBarFirst = 3000,
};

struct Event : public RTTI
{
    RTTI_DECLARATIONS(Event, RTTI);
    UINT id;
};

struct SystemEvent : public Event
{
    RTTI_DECLARATIONS(SystemEvent, Event);
    bool bHandled;
};

struct Notification : public Event
{
    RTTI_DECLARATIONS(Notification, Event);
    RTTI *sender;
};

struct MouseEvent : public Event
{
    RTTI_DECLARATIONS(MouseEvent, Event);
    UINT message;
    float x;
    float y;
    UINT flag; // ctrl shift atl
    float delta; // wheel
};

struct DelegateMouseEvent : public Notification
{
    RTTI_DECLARATIONS(DelegateMouseEvent, Notification);
    MouseEvent *data;
    bool bHandled;
};

struct KeyEvent : public Event
{
    RTTI_DECLARATIONS(KeyEvent, Event);
    DWORD keyCode;
    DWORD flag;
};

struct PaintEvent : public Event
{
    RTTI_DECLARATIONS(PaintEvent, Event);
    ID2D1RenderTarget *target;
};

struct SizeEvent : public Event
{
    RTTI_DECLARATIONS(SizeEvent, Event);
    float width;
    float height;
};

struct ImeEvent : public Event
{
    RTTI_DECLARATIONS(ImeEvent, Event);
    LPCWSTR text;
};

class Sprite;

struct FocusEvent : public Event
{
    RTTI_DECLARATIONS(FocusEvent, Event);
    Sprite *oldFocus;
};


}
