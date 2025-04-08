#pragma once
#include "afxwin.h"
#include <algorithm>

// CameraPictureControl dialog

class CameraPictureControl : public CStatic
{
private:
	CRect m_selectionRect;
	bool m_isSelecting;
	bool m_isClearControl;
	int m_startX;
	int m_startY;
	int m_endX;
	int m_endY;

public:
	CameraPictureControl(): m_isSelecting(false)
		, m_startX(0)
		, m_startY(0)
		, m_endX(0)
		, m_endY(0)
		, m_isClearControl(false)
	{

	};

	~CameraPictureControl()
	{

	};

	void ClearControl()
	{
		m_isClearControl = true;
		Invalidate();
		UpdateWindow();
		m_isClearControl = false;
	}

	DECLARE_MESSAGE_MAP()

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};
