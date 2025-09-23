#ifndef _MEMPARTDC_H_
#define _MEMPARTDC_H_


class CMemPartDC : public CDC 
{
public:
	CMemPartDC(CDC* pDC, const CRect& rcPartBounds) : CDC()
	{
		m_rect = rcPartBounds;
		CreateCompatibleDC(pDC);
		CRect rcDCBounds(0,0,0,0);
		CBitmap *pSrcDBBmp = pDC->GetCurrentBitmap();
		if ( NULL!=pSrcDBBmp )
		{
			BITMAP stBmp = {0};
			if ( pSrcDBBmp->GetBitmap(&stBmp) )
			{
				rcDCBounds.right	 = stBmp.bmWidth;
				rcDCBounds.bottom	 = stBmp.bmHeight;
			}
		}
		if ( rcDCBounds.IsRectEmpty() )
		{
			rcDCBounds.right = pDC->GetDeviceCaps(HORZRES);
			rcDCBounds.bottom = pDC->GetDeviceCaps(VERTRES);
		}

		m_hBmp = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), rcDCBounds.Width(), rcDCBounds.Height());
		m_hOldBmp = (HBITMAP)::SelectObject(GetSafeHdc(),m_hBmp);
		m_pDC = pDC;
		
		BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
			m_pDC, m_rect.left, m_rect.top, SRCCOPY
			);
	}
	
	// Destructor copies the contents of the mem DC to the original DC
    ~CMemPartDC()
    {
		
		// Copy the offscreen bitmap onto the screen.
        m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
			this, m_rect.left, m_rect.top, SRCCOPY);
		
		::SelectObject(GetSafeHdc(),m_hOldBmp);
		::DeleteObject(m_hBmp);
		DeleteDC();
    }
	
	// 
	CMemPartDC* operator->() {	return this; }
	
	// Allow usage as a pointer
    operator CMemPartDC*() {return this;}
	
protected:
    CDC*     m_pDC;         // Saves CDC passed in constructor
    CRect    m_rect;        // Rectangle of drawing area.
	HBITMAP	 m_hBmp;
	HBITMAP  m_hOldBmp;
};



class CMemSwapDC : public CDC 
{
public:
	CMemSwapDC() : CDC()
	{
		m_rect.SetRectEmpty();
		m_hBmp = m_hOldBmp = NULL;
	}

	BOOL	CreateSwapDC(CDC* pDC, const CRect& rcPartBounds, const CRect &rcDCBounds)
	{
		ASSERT( NULL!=pDC );
		if (NULL != pDC)
		{
			DestroySwapDC();

			m_rect = rcPartBounds;
			CreateCompatibleDC(pDC);

			m_hBmp = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), rcDCBounds.Width(), rcDCBounds.Height());
			m_hOldBmp = (HBITMAP)::SelectObject(GetSafeHdc(),m_hBmp);

			return BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				pDC, m_rect.left, m_rect.top, SRCCOPY
				);

		}

		return FALSE;

		
	}

	void	DestroySwapDC()
	{
		if ( NULL!=GetSafeHdc() )
		{
			::SelectObject(GetSafeHdc(),m_hOldBmp);
			::DeleteObject(m_hBmp);
			DeleteDC();
		}
	}

	BOOL	Swap(CDC *pDC, const CRect &rcSwap)
	{
		if ( NULL!=pDC && NULL!=GetSafeHdc() )
		{
			return pDC->BitBlt(rcSwap.left, rcSwap.top, rcSwap.Width(), rcSwap.Height(),
							this, rcSwap.left, rcSwap.top, SRCCOPY);
		}
		return FALSE;
	}

	BOOL	Swap(CDC *pDC)
	{
		ASSERT( pDC );
		return Swap(pDC, m_rect);
	}
	
	// Destructor copies the contents of the mem DC to the original DC
    ~CMemSwapDC()
    {
		DestroySwapDC();
    }
	
protected:
    CRect    m_rect;        // Rectangle of drawing area.
	HBITMAP	 m_hBmp;
	HBITMAP  m_hOldBmp;
};

#endif //!_MEMPARTDC_H_