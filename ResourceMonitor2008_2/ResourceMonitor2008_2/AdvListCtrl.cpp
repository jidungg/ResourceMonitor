// AdvListCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ResourceMonitor2008_2.h"
#include "AdvListCtrl.h"


// CAdvListCtrl

IMPLEMENT_DYNAMIC(CAdvListCtrl, CListCtrl)

CAdvListCtrl::CAdvListCtrl()
{
    m_nColumn = -1; 
}

CAdvListCtrl::~CAdvListCtrl()
{
}


BEGIN_MESSAGE_MAP(CAdvListCtrl, CListCtrl)
	ON_WM_HSCROLL()  
    ON_WM_VSCROLL()  
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, &CAdvListCtrl::OnLvnEndlabeledit)  
    ON_WM_LBUTTONDOWN()  
    ON_WM_LBUTTONUP() 
END_MESSAGE_MAP()



// CAdvListCtrl 메시지 처리기입니다.

void CAdvListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)  
{  
    if(GetFocus() != this) SetFocus();  
    CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);  
}  
  
void CAdvListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)  
{  
    if(GetFocus() != this) SetFocus();  
    CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);  
}  
  
void CAdvListCtrl::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)  
{  
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);  
    LVITEMW* pItem = &pDispInfo->item;  
  
    if(pItem->pszText != NULL)  
    {  
        SetItemText(pItem->iItem, pItem->iSubItem, pItem->pszText);  
    }  
  
    *pResult = 0;  
}  
  
void CAdvListCtrl::OnLButtonDown(UINT nFlags, CPoint point)  
{  
    int nIndex;  
    CListCtrl::OnLButtonDown(nFlags, point);  
  
    int nColumn;  
    if((nIndex = HitTestEx(point, &nColumn)) != -1)  
    {  
        UINT flag = LVIS_FOCUSED;  
        if((GetItemState(nIndex, flag) & flag) == flag && m_nColumn == nColumn)  
        {  
            if(!(GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS))  
            {   
  
                CString strGet = this->GetItemText(nIndex, m_nColumn);  
                ShowAdvComboBox(nIndex, m_nColumn, lstItems, 0);  
                SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);  
            }  
        }  
        else  
        {  
            SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);  
        }  
    }  
  
  CListCtrl::OnLButtonDown(nFlags, point);  
}  
  
void CAdvListCtrl::OnLButtonUp(UINT nFlags, CPoint point)  
{    
	CListCtrl::OnLButtonUp(nFlags, point);  
}  
  
int CAdvListCtrl::HitTestEx(CPoint& point, int* nCol) const  
{  
    int nColumn = 0;  
    int nRow = HitTest(point, NULL);  
  
    if(nCol) *nCol = 0;  
  
    // LVS_REPORT Check  
    if((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)  
        return nRow;  
  
    nRow = GetTopIndex();  
    int nBottom = nRow + GetCountPerPage();  
  
    if(nBottom > GetItemCount())  
        nBottom = GetItemCount();  
  
    CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);  
    int nColumnCount = pHeader->GetItemCount();  
  
    for(; nRow <= nBottom; ++nRow)  
    {  
        CRect rect;  
        GetItemRect(nRow, &rect, LVIR_BOUNDS);  
  
        if(rect.PtInRect(point))  
        {  
            for(nColumn = 0; nColumn < nColumnCount; ++nColumn)  
            {  
                int nColWidth = GetColumnWidth(nColumn);  
                if(point.x >= rect.left && point.x <= (rect.left + nColWidth))  
                {  
                    if(nCol) *nCol = nColumn;  
                    return nRow;  
                }  
  
                rect.left += nColWidth;  
            }  
        }  
    }  
  
    return -1;  
}  
  
void CAdvListCtrl::SetColumnCombo(int nColumn)  
{  
    m_nColumn = nColumn;  
}  
  
CComboBox* CAdvListCtrl::ShowAdvComboBox(int nItem, int nCol, CStringList& lstItems, int nSel)  
{  
    //if(!EnsureVisible(nItem, TRUE)) return NULL;  
  
    CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);  
    int nColumnCount = pHeader->GetItemCount();  
    if(nCol >= nColumnCount || GetColumnWidth(nCol) < 10)  
        return NULL;  
  
    int offset = 0;  
    for(int i = 0; i < nCol; ++i)  
    {  
        offset += GetColumnWidth(i);  
    }  
  
    CRect rect;  
    GetItemRect(nItem, &rect, LVIR_BOUNDS);  
  
    CRect rcClient;  
    GetClientRect(&rcClient);  
    if(offset + rect.left < 0 || offset + rect.left > rcClient.right)  
    {  
        CSize size;  
        size.cx = offset + rect.left;  
        size.cy = 0;  
        Scroll(size);  
        rect.left -= size.cx;  
    }  
  
    rect.left += offset + 4;  
    rect.right = rect.left + GetColumnWidth(nCol) - 3;  
    int nHeight = rect.bottom - rect.top;  
    rect.bottom += 5 * nHeight;  
    if(rect.right > rcClient.right) rect.right = rcClient.right;  
  
    DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE |  
        CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;  
    CComboBox* pList = new CAdvComboBox(nItem, nCol, &lstItems, nSel);  
    pList->Create(dwStyle, rect, this,1);  
    pList->SetItemHeight(-1, nHeight);  
    pList->SetHorizontalExtent(GetColumnWidth(nCol));  
  
    return pList;  
}  


