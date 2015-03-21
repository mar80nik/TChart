#pragma once

#include "TChartSeries.h"
#include "tchart_resources.h"

/////////////////////////////////////////////////////////////////////////////
// SeriesValuesDialog dialog

class SeriesValuesDialog : public CDialog
{
protected:
//	TChartSeries* Series; 
	
public:	
	SeriesValuesDialog(CWnd* pParent = NULL);   // standard constructor
	CStringArray strs,strsDATA;
	ProtectedSeriesArray* Series; int graph_num;

	void InitDialog(TChartSeries* Series1);

// Dialog Data
	//{{AFX_DATA(SeriesValuesDialog)
	enum { IDD = IDD_TCHART_SERIES_VALUES_DLG };
	CListCtrl	ListBox2;
	CListBox	ListBox1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SeriesValuesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SeriesValuesDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnKeydownList2(NMHDR *pNMHDR, LRESULT *pResult);
	void InitDialog2(ProtectedSeriesArray* series, int n);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
