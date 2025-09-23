#ifndef DRAWPOLYGON_H_
#define DRAWPOLYGON_H_

//	绘制窗体四角多边形
void DrawPolygonBorder(CDC* pDC, CRect rectWnd, bool bLeft = true,  COLORREF clrFill = RGB(234, 23, 23), COLORREF clrBorder = RGB(234, 23, 23));
void GetPolygon(CRect rect, CPoint* pts, int nSize);
#endif

