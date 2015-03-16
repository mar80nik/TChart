#include "stdafx.h"
#include "TChartSeries.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int TChartSeries::SeriesCounter=0;
CArray<WORD> ColorsStyle::ColorsTable;

TChartSeries::TChartSeries(CString name)
	: TAbstractGraphics(name)
{
	Status=SER_ACTIVE;	
	update=true; 
	SeriesType=0; PID = SeriesCounter++; Parent=NULL;
	PointType.Set(GenericPnt);	
}

TChartSeries::~TChartSeries() {}

int TChartSeries::GetStatus() {return Status;}
void TChartSeries::SetStatus(int t) 
{
	if(Status!=SER_LOCKED) Status=t;
}
int TChartSeries::GetSeriesType() {return SeriesType;}

void TChartSeries::GetStatus(CString& T)
{
    switch(Status)
	{
	case SER_ACTIVE: T.Format("Active"); break;
	case SER_INACTIVE: T.Format("InActive"); break;
	case SER_CALIBRATOR: T.Format("Calibrator"); break;
	case SER_LOCKED: T.Format("Locked"); break;
	case SER_ERROR: 
	default: T.Format("Error"); break;        	
	}
}
void TChartSeries::UpdateIcon(BMPanvas& icon)
{
	HGDIOBJ temp1,temp2;
	temp1=icon.SelectObject(pen); temp2=icon.SelectObject(brush);
	_UpdateIcon(icon);
	icon.SelectObject(temp2); icon.SelectObject(temp1); 
}

void TChartSeries::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	
		ar << SeriesType;
		ar << SeriesType << visible << Status << Name;
		TAbstractGraphics::Serialize(ar);		
	}
	else
	{		
		ar >> SeriesType >> visible >> Status >> Name;
		TAbstractGraphics::Serialize(ar);		
		AssignColors(*this);
	}
}

int TChartSeries::FindMinMax()
{	
	int ret=0;
	if(IsVisible() && GetSize()!=0)
	{
		ret=1;
		if(update) 
		{
			ret=_FindMinMax(); 
			update=false;
		}
		
	}
	return ret;
}

void TChartSeries::SetVisible( BYTE t )
{
	TAbstractGraphics::SetVisible(t);
	PostParentMessage(UM_SERIES_UPDATE);
}

void TChartSeries::SetParentUpdateStatus( UpdateStatus sts )
{
	upd=sts;
	if(upd==UPD_ON) 
		PostParentMessage(UM_SERIES_UPDATE);
}

PointsRgn::PointsRgn(const PointVsError& t)
{
	double d=fabs(t.dy);
	minX=maxX=t.x; minY=t.y-d; maxY=t.y+d;
};
void PointsRgn::operator=(const PointVsError& t)
{
	double d=fabs(t.dy);
	minX=maxX=t.x; minY=t.y-d; maxY=t.y+d;	
}
void PointsRgn::operator=(const SimplePoint& t)
{
	minX=maxX=t.x; minY=t.y; maxY=t.y;	
}
void PointsRgn::operator+=(const PointVsError& t)
{
	double d=fabs(t.dy);
	if(t.x>maxX) maxX=t.x; 
	else {if(t.x<=minX) minX=t.x;}
	if((t.y+d)>maxY) maxY=t.y+d;
	if((t.y-d)<=minY) minY=t.y-d;
};
void PointsRgn::operator+=(const SimplePoint& t)
{
	if(t.x>maxX) maxX=t.x; 
	else {if(t.x<=minX) minX=t.x;}
	if((t.y)>maxY) maxY=t.y;
	if((t.y)<=minY) minY=t.y;
};
void PointsRgn::operator+=(const PointsRgn& t)
{
	if(t.minX<minX) minX=t.minX;	
	if(t.minY<minY) minY=t.minY;	
	if(t.maxX>maxX) maxX=t.maxX;	
	if(t.maxY>maxY) maxY=t.maxY;			
};

TSeriesArray::TSeriesArray(SeriesArrayDeleteSeries _DeleteSeries)
{
	for(WORD i=0;i<360;i++) ColorsTable.Add(i);
	Parent=0; 
	ToDeleteSeries=_DeleteSeries;
}

TSeriesArray::~TSeriesArray() 
{
	SetParentUpdateStatus(UPD_OFF);
	ClearAll(); 
}
int TSeriesArray::GetSize() {return Series.GetSize();}
TChartSeries* TSeriesArray::operator[](int i) {return Series[i];}
int TSeriesArray::Add(TChartSeries* t) 
{
	RegisterSeries(t); 
	int ret = Series.Add(t);
	t->SetParentUpdateStatus(UPD_ON);
	return ret;
}
void TSeriesArray::DeleteItem(int i) 
{
	if(i>=0 && i<Series.GetSize()) 
	{
		delete Series[i]; Series.RemoveAt(i);
		PostParentMessage(UM_SERIES_UPDATE);	
	}
}
void TSeriesArray::ClearAll()
{
	if( ToDeleteSeries==DELETE_SERIES ) 
		for(int i=0;i<Series.GetSize();i++) delete Series[i];
	Series.RemoveAll();    
	PostParentMessage(UM_SERIES_UPDATE);	
}

void TSeriesArray::Serialize(CArchive& ar)
{
	int SeriesSize, SeriesType; TChartSeries* temp=0;
	
	if (ar.IsStoring())
	{			
		SeriesSize=Series.GetSize();
		ar << SeriesSize;
		for(int i=0;i<SeriesSize;i++) 
			Series[i]->Serialize(ar);
	}
	else
	{		
		ar >> SeriesSize;
		SetParentUpdateStatus(UPD_OFF);
		
		for(int i=0;i<SeriesSize;i++)
		{
			ar >> SeriesType;
			temp=CreateSeries(SeriesType);
			if(temp)
			{				
				Add(temp);			
				temp->Serialize(ar);				
			}
			else
			{
				ErrorInspector Insp; ErrorsArray arr; Insp.AttachErrors(&arr);
				CString T; T.Format("���������� ����� ������������ ���� (%d)",SeriesType);
				ErrorMessage* t=new ErrorMessage(); Insp.RegisterError(t->Create(CString("Loader"),T));	
				Insp.DispatchErrors();
				Insp.DetachErrors();
			}
		}		
		SetParentUpdateStatus(UPD_ON);
	}
}


int TSeriesArray::RegisterSeries(TChartSeries* s)
{
	CString t; t.Format("%s_%d",s->Name,s->GetPID()); 
	s->Name=t; s->Parent=this;
	s->SetRender(SERIES_RENDER);	
	return s->GetPID();
}

TChartSeries* TSeriesArray::CreateSeries(int type)
{
	TChartSeries* ret=NULL;
	
	switch(type)
	{
	case SIMPLE_POINT: ret=new TSimplePointSeries(CString("")); break;
	case POINTvsERROR: ret=new TPointVsErrorSeries(CString("")); break;
	}
	if(ret) ret->AssignColors(ColorsStyle(clWHITE,GetRandomColor()));
	return ret;
}

int TSeriesArray::FindSeries( SeriesSearchPattern patern, TSeriesArray& results )
{
	BOOL found; int ret=0; TChartSeries* t;
	results.Series.RemoveAll();

	for(int i=0;i<Series.GetSize();i++)
	{
		found=TRUE;
		if(patern.mode & SERIES_NAME)
		{
			if(Series[i]->Name.Find(patern.name)>=0) found&=TRUE;
			else found&=FALSE;
		}
		if(patern.mode & SERIES_STATUS)
		{
			if(Series[i]->GetStatus() == patern.staus) found&=TRUE;
			else found&=FALSE;
		}
		if(patern.mode & SERIES_TYPE)
		{
			if(Series[i]->GetSeriesType() == patern.SeriesType)	found&=TRUE;
			else found&=FALSE;
		}
		if(patern.mode & SERIES_PID)
		{
			if(Series[i]->GetPID() == patern.PID)	found&=TRUE;
			else found&=FALSE;
		}

		if(found==TRUE) 
		{
			t=Series[i];
			ret=results.Series.Add(t);
		}
	}	
	return results.Series.GetSize();
}

//////////////////////////////////////////////////////////////////////////
int SeriesTemplate<SIMPLE_POINT, SimplePoint, SimplePointArray, CPoint>::_FindMinMax()
{
	SimplePoint t; BOOL FoundFirst=false;

	for(int j=0;j<Values.GetSize();j++) 
	{
		t=operator[](j);
		if(t.IsVisible())
		{
			if(FoundFirst==false) {MinMax=t; FoundFirst=true;}
			else MinMax+=t;
		}
	}
	return 1;
}
int SeriesTemplate<POINTvsERROR, PointVsError, PointVsErrorArray, CPointVsError>::_FindMinMax()
{
	PointVsError t; SimplePoint t2; BOOL FoundFirst=false;

	for(int j=0;j<Values.GetSize();j++) 
	{
		t=operator[](j);
		if(t.IsVisible())
		{
			if(_ErrorBarStyle<CPointVsError>::Draw!=NULL)
			{
				if(FoundFirst==false) {MinMax=t; FoundFirst=true;}
				else MinMax+=t;
			}
			else
			{
				t2=t;
				if(FoundFirst==false) {MinMax=t2; FoundFirst=true;}
				else MinMax+=t2;
			}
		}
	}
	return 1;
}
