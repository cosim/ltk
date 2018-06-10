#pragma once
#include "Sprite.h"
#include "Delegate.h"

namespace ltk {

class SpriteEx : public Sprite
{
public:
    SpriteEx();
    ~SpriteEx();

    virtual bool OnEvent(Event *ev);

    Delegate<void(PaintEvent *, bool &bHandled)> OnPaintD;
    Delegate<void(MouseEvent *, bool &bHandled)> OnLBtnDownD;
    Delegate<void(MouseEvent *, bool &bHandled)> OnLBtnUpD;
    Delegate<void(MouseEvent *, bool &bHandled)> OnLBtnDbClkD;
    Delegate<void(MouseEvent *, bool &bHandled)> OnRBtnDownD;
    Delegate<void(MouseEvent *, bool &bHandled)> OnRBtnUpD;
    Delegate<void(MouseEvent *, bool &bHandled)> OnMouseMoveD;
    Delegate<void(MouseEvent *, bool &bHandled)> OnMouseEnterD;
    Delegate<void(MouseEvent *, bool &bHandled)> OnMouseLeaveD;
    Delegate<void(MouseEvent *, bool &bHandled)> OnMouseWheelD;
    Delegate<void(KeyEvent *,   bool &bHandled)> OnKeyDownD;
    Delegate<void(KeyEvent *,   bool &bHandled)> OnKeyUpD;
    Delegate<void(KeyEvent *,   bool &bHandled)> OnCharD;
    Delegate<void(SizeEvent *,  bool &bHandled)> OnSizeD;
    Delegate<void(ImeEvent *,   bool &bHandled)> OnImeInputD;
    Delegate<void(FocusEvent *, bool &bHandled)> OnSetFocusD;
    Delegate<void(FocusEvent *, bool &bHandled)> OnKillFocusD;

};

}
