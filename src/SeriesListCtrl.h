#pragma once

#include "tchart_resources.h"
#include "SeriesStyleDlg.h"
#include "SeriesValuesDlg.h"

/////////////////////////////////////////////////////////////////////////////
// MyListCtrl window
int CALLBACK CompareFunc1(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
//class SeriesArray;

struct ListItemData
{
	CString Name;
	int Size,Status,PID;
};

class SeriesListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(SeriesListCtrl)
	friend class SeriesListDlg;
protected:
	LV_ITEM _ms_lvi;
	CImageList ImageList1;
	int HitItem; CBrush bckg; DWORD bckg_color;	

	int HitTest(CPoint point);	
	int HitTest();	
	BYTE GetState(int num);
	void SetState(int num, BYTE state);
	void ChangeStatus(int t);
public:
	static CString GetSaveAsPath();
	CArray<ListItemData,const ListItemData&> Items;
	int LastN, SortStatus, SortMask;
	ProtectedSeriesArray* Series;
	CImageList ImageList2;

	SeriesListCtrl();
	virtual ~SeriesListCtrl();
	void UpdateSeriesList();

	// Generated message map functions
protected:
	//{{AFX_MSG(MyListCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);	
	afx_msg void OnDeleteSeries();
	afx_msg void OnSaveAsText();
	afx_msg void OnChangeStatus();
	afx_msg void OnMenuChangeStyle();
	afx_msg void OnChangestatusActive();
	afx_msg void OnChangestatusInactive();
	afx_msg void OnChangestatusCalibrator();
	afx_msg void OnSeriesmenuNormalize();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	LRESULT OnDataUpdate(WPARAM wParam, LPARAM lParam );
	afx_msg HBRUSH OnCtlColor( CDC*, CWnd*, UINT );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSeriesmenuShowvalues();
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

/////////////////////////////////////////////////////////////////////////////