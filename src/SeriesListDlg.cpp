#include "stdafx.h"
#include "SeriesListDlg.h"

/////////////////////////////////////////////////////////////////////////////
// DialogBarTab2 dialog


SeriesListDlg::SeriesListDlg(CWnd* pParent /*=NULL*/)
	: CDialog()
{
	//{{AFX_DATA_INIT(DialogBarTab2)
	//}}AFX_DATA_INIT
	cursel=LB_ERR;
}


void SeriesListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DialogBarTab2)
	DDX_Control(pDX, IDC_TCHART_LIST2, List2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SeriesListDlg, CDialog)
	//{{AFX_MSG_MAP(DialogBarTab2)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_TCHART_LIST2, OnLvnColumnclickList2)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TCHART_LIST2, OnLvnEndlabeleditList2)
	ON_MESSAGE(UM_SERIES_UPDATE,OnSeriesUpdate)	
	ON_WM_CREATE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TCHART_LIST2, OnNMCustomdrawList2)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DialogBarTab2 message handlers

void SeriesListDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	if(bShow) 
	{
		List2.UpdateSeriesList();
	}
	
}

BOOL SeriesListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog(); int n=2;
	
	List2.InsertColumn(n++,"Series",LVCFMT_LEFT,195);
	List2.InsertColumn(n++,"Pnts",LVCFMT_LEFT,40);
	List2.InsertColumn(n++,"Status",LVCFMT_LEFT,64);
	List2.InsertColumn(n++,"PID",LVCFMT_LEFT,32);
	List2.SetImageList(&List2.ImageList2,LVSIL_STATE);	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void SeriesListDlg::OnLvnColumnclickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int& mask=List2.SortMask; int &Status=List2.SortStatus;

	mask=1<<pNMLV->iSubItem; 
	if(Status&mask) Status&=(~mask);
	else Status|=mask;	

	List2.SortItems(CompareFunc1,(LPARAM) (&List2));

	*pResult = 0;
}


void SeriesListDlg::OnLvnEndlabeleditList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LVITEM& item=pDispInfo->item,lvi; CString T; void *x;
	
	if((x=List2.Series->GainAcsess(READ_EX))!=0)
	{
		SeriesProtector Protector(x); TSeriesArray& Series(Protector);
		TChartSeries* series=Series[item.lParam]; series->Name=item.pszText;
		T=series->Name; List2.Items[item.lParam].Name=T;
	}
	else
	{
		T=List2.Items[item.lParam].Name;
	}

	lvi.mask = LVIF_TEXT; lvi.iItem = item.iItem; lvi.iSubItem=0;	
	List2.SetItemText(lvi.iItem,lvi.iSubItem++,(char*)LPCTSTR(T));
	List2.SortItems(CompareFunc1,(LPARAM) (&List2));
	
	*pResult = 0;
}

int SeriesListDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowPos(0,100,100,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);

	return 0;
}

LRESULT SeriesListDlg::OnSeriesUpdate(WPARAM wParam, LPARAM lParam )
{
	if(lParam!=0) 
	{
		List2.Series=(ProtectedSeriesArray*)lParam;
		List2.UpdateSeriesList();
		if(wParam==SW_SHOW) ShowWindow(wParam);
	}
	return 0;
}

void SeriesListDlg::OnNMCustomdrawList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	CRect rect(pNMCD->rc); 
	HGDIOBJ brsh=SelectObject(pNMCD->hdc,List2.bckg);
	PatBlt(pNMCD->hdc,rect.left,rect.top,rect.Width(),rect.Height(),PATCOPY);
	SelectObject(pNMCD->hdc,brsh);	
	List2.SetTextBkColor(List2.bckg_color);
	List2.SetBkColor (List2.bckg_color);
	*pResult = 0;
}
