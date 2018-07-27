#pragma once
#include "SpriteEx.h"

namespace ltk {

struct SlidePage
{
	ID2D1Bitmap *bmp = nullptr;
	std::wstring path;
	Delegate<void()> OnClickD;

	~SlidePage() { SAFE_RELEASE(bmp); }
};

class SlideCard : public Sprite
{
public:
	~SlideCard() {}
	void SetPage(SlidePage *page) { m_page = page; }
	D2D1_SIZE_F GetImageSize();

	virtual bool OnPaint(PaintEvent *ev) override;
	virtual bool OnLBtnDown(MouseEvent *ev) override;

	Delegate<void()> OnClickD;

private:
	SlidePage * m_page = nullptr;
};

class Slide : public Sprite
{
public:
    Slide();
    virtual ~Slide();

	DelegateTracker AddPage(LPCWSTR path, std::function<void()> onClick);
	DelegateTracker AttachPageOnClick(UINT idx, std::function<void()> onClick);

protected:
	virtual void RecreateResouce(ID2D1RenderTarget *target) override;

private:
	UINT GetLeftIdx();
	UINT GetRightIdx();
	void UpdateCards();
	void MoveCards();

	enum AniState {
		statNone, statTurnLeft, statTurnRight, statSuffer
	};

private:
	std::vector<std::unique_ptr<SlidePage>> m_vecPages;
	SlideCard *m_spLeft = nullptr;
	SlideCard *m_spMid = nullptr;
	SlideCard *m_spRight = nullptr;
	int m_idx = 0;
	AniState m_state = statNone;
	DWORD m_lastTick = 0;
	int m_counter = 0;
	static const UINT AniDuration = 500;
	static const float UpMargin;
};

}
