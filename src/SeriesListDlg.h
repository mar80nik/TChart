#pragma once

#include "SeriesListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// DialogBarTab2 dialog
//class SeriesArray;

class SeriesListDlg : public CDialog
{
// Construction
public:
	int cursel;
	SeriesListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DialogBarTab2)
	enum { IDD = IDD_TCHART_SERIES_LIST_DLG };
	SeriesListCtrl	List2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DialogBarTab2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DialogBarTab2)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual void OnOK() {};
	afx_msg void OnLvnColumnclickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnSeriesUpdate(WPARAM wParam, LPARAM lParam );
	afx_msg HBRUSH OnCtlColor( CDC*, CWnd*, UINT );
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMCustomdrawList2(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
