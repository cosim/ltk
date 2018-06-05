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
    eSpriteLast     // 子类从这里开始
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

#define  LUI_DECLARE_MSG_MAP() \
    virtual bool OnEvent( UINT id, void *ev ) override;

#define LUI_BEGIN_MSG_MAP(type) \
    bool type::OnEvent( UINT id, void *ev ) \
    { \
        switch (id) {
          
#define  LUI_END_MSG_MAP() \
        default: \
            return __super::OnEvent(id, ev); \
        } \
    }

#define LUI_HANDLE_PAINT() \
        case ePaint: \
            return OnPaint(static_cast<PaintEvent*>(ev));