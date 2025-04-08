// CameraPictureControl.cpp : implementation file
//
#include "stdafx.h"
#include "CameraPictureControl.h"

BEGIN_MESSAGE_MAP(CameraPictureControl, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL CameraPictureControl::OnEraseBkgnd(CDC* pDC)
{
	HBRUSH hBrush = (HBRUSH)GetClassLongPtr(this->GetSafeHwnd(), GCLP_HBRBACKGROUND);
	
	if (hBrush == NULL) 
	{
		hBrush = GetSysColorBrush(COLOR_BTNFACE);	// RGB(240, 240, 240)
	}

	// Получаем цвет кисти
	LOGBRUSH logBrush;
	GetObject(hBrush, sizeof(LOGBRUSH), &logBrush);
	COLORREF bgColor = logBrush.lbColor;

	CRect rect;
	GetClientRect(&rect);

	pDC->FillSolidRect(rect, bgColor);	
	return TRUE;							// Возвращаем TRUE, чтобы предотвратить стандартную очистку	
}

// CameraPictureControl message handlers
void CameraPictureControl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_isSelecting = true;
	m_startX = point.x;
	m_startY = point.y;

	m_selectionRect.SetRect(m_startX, m_startY, m_startX, m_startY);

	CStatic::OnLButtonDown(nFlags, point);
}

void CameraPictureControl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_isSelecting) 
	{
		m_endX = point.x;
		m_endY = point.y;

		 m_selectionRect.SetRect(m_startX, m_startY, m_endX, m_endY);

		Invalidate();
	}
	CStatic::OnMouseMove(nFlags, point);
}

void CameraPictureControl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_isSelecting) 								
	{
		m_selectionRect.NormalizeRect();
		m_isSelecting = false;
		Invalidate();

		// Здесь можно использовать координаты selectionRect
	}
	CStatic::OnLButtonUp(nFlags, point);
}

void CameraPictureControl::OnPaint() 
{
	CPaintDC dc(this);
	//CStatic::OnPaint();

	if (m_isSelecting || !m_selectionRect.IsRectEmpty() && !m_isClearControl)
	{
		CPen greenPen(PS_SOLID, 2, RGB(0, 255, 0));
		CPen* pOldPen = dc.SelectObject(&greenPen);

		CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);

		dc.Rectangle(m_selectionRect);

		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);
	}
}