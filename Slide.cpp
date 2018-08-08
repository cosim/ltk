#include "stdafx.h"
#include "Slide.h"
#include "ltk.h"

namespace ltk {

const float Slide::UpMargin = 10.0f;

Slide::Slide()
{
	using namespace std::placeholders;
	m_spLeft = new SlideCard;
	m_spMid = new SlideCard;
	m_spRight = new SlideCard;

	//m_spLeft->OnLBtnDownD.Attach([this](MouseEvent *ev, bool &bHandled) {
	//	// FIXME 当Window删除的时候 会不会有消息进来呢？
	//	m_vecPages[this->GetLeftIdx()]->OnClickD.Invoke();
	//});
	//m_spLeft->OnPaintD.Attach(std::bind(Slide::OnPagePaint, this, _1, _2, ))
}


Slide::~Slide()
{
}

DelegateTracker Slide::AddPage(LPCWSTR path, std::function<void()> onClick)
{
	SlidePage *page = new SlidePage;
	page->path = path;
	m_vecPages.emplace_back(page);
	UpdateCards();
	return page->OnClickD.Attach(onClick);
}

DelegateTracker Slide::AttachPageOnClick(UINT idx, std::function<void()> onClick)
{
	return m_vecPages[idx]->OnClickD.Attach(onClick);
}

void Slide::UpdateCards()
{
	if (m_vecPages.size() < 3)
		return;
	m_spLeft->SetPage(m_vecPages[this->GetLeftIdx()].get());
	m_spMid->SetPage(m_vecPages[m_idx].get());
	m_spRight->SetPage(m_vecPages[this->GetRightIdx()].get());
}

void Slide::MoveCards()
{
	auto rc = this->GetClientRect();
	auto szLeft = m_spLeft->GetImageSize();
	auto szMid = m_spMid->GetImageSize();
	auto szRight = m_spRight->GetImageSize();
	if (m_state == statNone)
	{
		m_spLeft->SetRect(Gdiplus::RectF(0.0f, UpMargin, szLeft.width, szLeft.height));
		m_spMid->SetRect(Gdiplus::RectF((rc.Width - szMid.width) / 2.0f, 0.0f, szMid.width, szMid.height));
		m_spRight->SetRect(Gdiplus::RectF(rc.Width - szRight.width, UpMargin, szRight.width, szRight.height));
	}
	else if (m_state == statTurnRight)
	{
		float moveLeft = (rc.Width - szMid.width) / 2.0f;
		m_spLeft->SetRect(Gdiplus::RectF(moveLeft * m_counter / AniDuration,
			UpMargin * m_counter / AniDuration, szLeft.width, szLeft.height));
		//m_spMid->SetRect(Gdiplus::RectF())
	}
}

void Slide::RecreateResouce(ID2D1RenderTarget * target)
{
	HRESULT hr;
	for (auto &page : m_vecPages)
	{
		SAFE_RELEASE(page->bmp);
		hr = LoadBitmapFromFile(target, page->path.c_str(), &page->bmp);
		if (FAILED(hr))
		{
			LOGW(L"load bmp failed: " << page->path);
		}
	}
}

UINT Slide::GetLeftIdx()
{
	UINT idx = 0;
	if (m_idx < 1)
	{
		idx = (UINT)m_vecPages.size() - 1;
	}
	else
	{
		idx = m_idx - 1;
	}
	return idx;
}

UINT Slide::GetRightIdx()
{
	UINT idx = 0;
	if (m_idx >= (int)m_vecPages.size() - 1)
	{
		idx = 0;
	}
	else
	{
		idx = m_idx + 1;
	}
	return idx;
}

D2D1_SIZE_F SlideCard::GetImageSize()
{
	if (m_page)
	{
		return m_page->bmp->GetSize();
	}
	D2D1_SIZE_F sz = { 0.0f, 0.0f };
	return sz;
}

bool SlideCard::OnPaint(PaintEvent * ev)
{
	return true;
}

bool SlideCard::OnLBtnDown(MouseEvent * ev)
{
	return false;
}

}
