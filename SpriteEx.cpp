#include "stdafx.h"
#include "SpriteEx.h"

namespace ltk {

SpriteEx::SpriteEx()
{
}


SpriteEx::~SpriteEx()
{
}

bool SpriteEx::OnEvent(Event *ev)
{
    bool bHandled = false;
    UINT id = ev->id;

    switch (id)
    {
    case ePaint:
        this->OnPaintD.Invoke(static_cast<PaintEvent*>(ev), std::ref(bHandled));
        break;
    case eMouseMove:
        this->OnMouseMoveD.Invoke(static_cast<MouseEvent*>(ev), std::ref(bHandled));
        break;
    case eSizeChanged:
        this->OnSizeD.Invoke(static_cast<SizeEvent*>(ev), std::ref(bHandled));
        break;
    case eMouseEnter:
        this->OnMouseEnterD.Invoke(static_cast<MouseEvent*>(ev), std::ref(bHandled));
        break;
    case eMouseLeave:
        this->OnMouseLeaveD.Invoke(static_cast<MouseEvent*>(ev), std::ref(bHandled));
        break;
    case eLBtnDown:
        this->OnLBtnDownD.Invoke(static_cast<MouseEvent*>(ev), std::ref(bHandled));
        break;
    case eLBtnUp:
        this->OnLBtnUpD.Invoke(static_cast<MouseEvent*>(ev), std::ref(bHandled));
        break;
    case eRBtnDown:
        this->OnRBtnDownD.Invoke(static_cast<MouseEvent*>(ev), std::ref(bHandled));
        break;
    case eRBtnUp:
        this->OnRBtnUpD.Invoke(static_cast<MouseEvent*>(ev), std::ref(bHandled));
        break;
    case eMouseWheel:
        this->OnMouseWheelD.Invoke(static_cast<MouseEvent*>(ev), std::ref(bHandled));
        break;
    case eKeyDown:
        this->OnKeyDownD.Invoke(static_cast<KeyEvent*>(ev), std::ref(bHandled));
        break;
    case eKeyUp:
        this->OnKeyUpD.Invoke(static_cast<KeyEvent*>(ev), std::ref(bHandled));
        break;
    case eCharInput:
        this->OnCharD.Invoke(static_cast<KeyEvent*>(ev), std::ref(bHandled));
        break;
    case eImeInput:
        this->OnImeInputD.Invoke(static_cast<ImeEvent*>(ev), std::ref(bHandled));
        break;
    case eSetFocus:
        this->OnSetFocusD.Invoke(static_cast<FocusEvent*>(ev), std::ref(bHandled));
        break;
    case eKillFocus:
        this->OnKillFocusD.Invoke(static_cast<FocusEvent*>(ev), std::ref(bHandled));
        break;
    default:
        bHandled = false;
    }
    return bHandled;
}

}
