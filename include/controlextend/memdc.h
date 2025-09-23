#ifndef _MEMDC_H_
#define _MEMDC_H_

//////////////////////////////////////////////////
// CMemDC - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// This class implements a memory Device Context

#ifdef _DEBUG

class CMemDC : public CDC 
{
public:
	CMemDC(CDC* pDC) : CDC()
    {
        ASSERT(pDC != NULL);

		
        m_pDC = pDC;
		m_hOldBmp = NULL;
		m_bMemDC = FALSE;
		if (NULL != m_pDC)
		{

#ifndef _WIN32_WCE_NO_PRINTING
			m_bMemDC = !m_pDC->IsPrinting();
#else
			m_bMemDC = FALSE;
#endif

			if (m_bMemDC)    // Create a Memory DC
			{
				m_pDC->GetClipBox(&m_rect);
				CreateCompatibleDC(m_pDC);
				m_hBmp = ::CreateCompatibleBitmap(m_pDC->GetSafeHdc(), m_rect.Width(), m_rect.Height());
				m_hOldBmp = (HBITMAP)::SelectObject(GetSafeHdc(), m_hBmp);

#ifndef _WIN32_WCE
				SetWindowOrg(m_rect.left, m_rect.top);
#endif
				// EFW - Bug fix - Fill background in case the user has overridden
				// WM_ERASEBKGND.  We end up with garbage otherwise.
				// CJM - moved to fix a bug in the fix.
				FillSolidRect(m_rect, m_pDC->GetBkColor());
			}
			else        // Make a copy of the relevent parts of the current DC for printing
			{
#if !defined(_WIN32_WCE) || ((_WIN32_WCE > 201) && !defined(_WIN32_WCE_NO_PRINTING))
				m_bPrinting = m_pDC->m_bPrinting;
#endif
				m_hDC       = m_pDC->m_hDC;
				m_hAttribDC = m_pDC->m_hAttribDC;
			}
		}
    }

	CMemDC(CDC* pDC, const CRect& rcBounds) : CDC()
	{
		m_bMemDC = TRUE;
		CreateCompatibleDC(pDC);
		m_hBmp = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), rcBounds.Width(), rcBounds.Height());
		m_hOldBmp = (HBITMAP)::SelectObject(GetSafeHdc(), m_hBmp);
		m_pDC = pDC;
		m_rect = rcBounds;
	}

	// Destructor copies the contents of the mem DC to the original DC
    ~CMemDC()
    {
        if (m_bMemDC)
        {
            // Copy the offscreen bitmap onto the screen.
            m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
                          this, m_rect.left, m_rect.top, SRCCOPY);

            //Swap back the original bitmap.
			::SelectObject(GetSafeHdc(), m_hOldBmp);
			DeleteObject(m_hBmp);
			DeleteDC();
        } 
		else
		{
            // All we need to do is replace the DC with an illegal value,
            // this keeps us from accidently deleting the handles associated with
            // the CDC that was passed to the constructor.
            m_hDC = m_hAttribDC = NULL;
        }
    }


	// 
	CMemDC* operator->() {	return this; }

	// Allow usage as a pointer
    operator CMemDC*() {return this;}

protected:
    HBITMAP			m_hBmp;      // Offscreen bitmap
    HBITMAP			m_hOldBmp;  // bitmap originally found in CMemDC
    CDC*			m_pDC;         // Saves CDC passed in constructor
    CRect			m_rect;        // Rectangle of drawing area.
    BOOL			m_bMemDC;      // TRUE if CDC really is a Memory DC.
};

#else

class CMemDC : public CDC 
{
public:
// 	CMemDC(CDC* pDC) : CDC()
//     {
// 		pDC->GetClipBox(&m_rect);
// 		CreateCompatibleDC(pDC);
// 		m_hBmp = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), m_rect.Width(), m_rect.Height());
// 		m_hOldBmp = (HBITMAP)::SelectObject(GetSafeHdc(),m_hBmp);
// 		m_pDC = pDC;
// 		FillSolidRect(m_rect, pDC->GetBkColor());
//     }

	CMemDC(CDC* pDC, const CRect& rcBounds) : CDC()
	{
		m_rect = rcBounds;
		CreateCompatibleDC(pDC);
		m_hBmp = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), m_rect.Width(), m_rect.Height());
		m_hOldBmp = (HBITMAP)::SelectObject(GetSafeHdc(),m_hBmp);
		m_pDC = pDC;
		FillSolidRect(m_rect, pDC->GetBkColor());

	}

	// Destructor copies the contents of the mem DC to the original DC
    ~CMemDC()
    {

		// Copy the offscreen bitmap onto the screen.
        m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
                      this, m_rect.left, m_rect.top, SRCCOPY);

 		::SelectObject(GetSafeHdc(),m_hOldBmp);
		::DeleteObject(m_hBmp);
		DeleteDC();
    }

	// 
	CMemDC* operator->() {	return this; }

	// Allow usage as a pointer
    operator CMemDC*() {return this;}

protected:
    CDC*     m_pDC;         // Saves CDC passed in constructor
    CRect    m_rect;        // Rectangle of drawing area.
	HBITMAP	 m_hBmp;
	HBITMAP  m_hOldBmp;
};

#endif//_DEBUG

#endif









