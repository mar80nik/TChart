#pragma once

#include "tChartElements.h"

struct SeriesSearchPattern
{
	CString name;
	SeriesStatus staus;
	int SeriesType;
	int mode;
	int PID;
};

class PointsRgn 
{
public:
	double minX,maxX,minY,maxY;
	PointsRgn() {minX=maxX=minY=maxY=0;}
	PointsRgn(const PointVsError& t);
	void operator=(const PointVsError& t);
	void operator=(const SimplePoint& t);
	void operator+=(const PointVsError& t);
	void operator+=(const SimplePoint& t);
	void operator+=(const PointsRgn& t);
};

class TSeriesArray;
//////////////////////////////////////////////////////////////////////////
template <class PointToDrawType> // C1 - values type
class ChartSeriesStyleTemplate: public _LineStyle, public _SymbolStyle, public _ErrorBarStyle<PointToDrawType>
{
protected:	
	ChartSeriesStyleParams GetStyle() const
	{
		ChartSeriesStyleParams ret;
		*((LineStyleStyleParams*)&ret)=*((LineStyleStyleParams*)this);
		*((SymbolStyleStyleParams*)&ret)=*((SymbolStyleStyleParams*)this);
		*((ErrorBarStyleParams*)&ret)=*((ErrorBarStyleParams*)this);
		return ret;
	}
	
	void _UpdateIcon(BMPanvas& icon)
	{
		if(_ErrorBarStyle<PointToDrawType>::IconDraw!=NULL)
		{
			(this->*_ErrorBarStyle<PointToDrawType>::IconDraw)(icon);
		}	
		if(_LineStyle::IconDraw!=NULL)
		{
			(this->*_LineStyle::IconDraw)(icon);
		}	
		if(_SymbolStyle::Draw!=NULL)
		{
			(this->*_SymbolStyle::IconDraw)(icon);
		}
	}
	virtual void Serialize(CArchive& ar)
	{
		_LineStyle::Serialize(ar); _SymbolStyle::Serialize(ar); _ErrorBarStyle<PointToDrawType>::Serialize(ar);
	}
public:
	ChartSeriesStyleTemplate(ChartSeriesStyleParams style): 
	  _LineStyle(style), _SymbolStyle(style), _ErrorBarStyle<PointToDrawType>(style) 
	{ };
	  	void SetStyle(ChartSeriesStyleParams style)
		{
			_LineStyle::Set(style);
			_SymbolStyle::Set(style);
			_ErrorBarStyle<PointToDrawType>::Set(style);
		}
};

//////////////////////////////////////////////////////////////////////////
enum {CLASS_VALIDATE_ERR,CLASS_VALIDATE_OK};

#define SERIES_MAX -1
#define SERIES_MIN -1

enum TChartImportMsgDestroy {DO_NOT_DELETE_MSG, DELETE_IMPORT_MSG};
//////////////////////////////////////////////////////////////////////////
class TChartSeries: public TAbstractGraphics
{
	virtual int _FindMinMax()=0;
	int PID;
	static int SeriesCounter;
protected:
	bool update;
	int SeriesType, Status;
public:	
	static PointTypes BckgCh0,BckgCh1,Ch0,Ch1, Transmit;
	PointTypes PointType;
	PointsRgn MinMax;	
//-------------------------------------------
	TChartSeries(CString name);
	virtual ~TChartSeries();

	int GetPID() const {return PID;}
	virtual void _UpdateIcon(BMPanvas&) {};
	void UpdateIcon(BMPanvas&);
	void ChangeStyle (ChartSeriesStyleParams &style);
	virtual void Draw(BMPanvas*)=0;
	virtual int FindMinMax();
	int GetStatus();
	int GetSeriesType();
	void SetStatus(int);
	void GetStatus(CString&);
	virtual void RemoveAt(int n)=0;
	virtual ChartSeriesStyleParams GetStyle() const =0 ;

	virtual void ClearAll()=0;	
	virtual int GetSize() const =0;	
	virtual void Serialize(CArchive& ar);
	virtual void FormatElement(int n, CString &t)=0;
	virtual void GetInfo(CStringArray &)=0;	
	virtual int ImportData(MessagesInspectorSubject* pnt,TChartImportMsgDestroy destroy)=0;

	virtual void SetStyle(ChartSeriesStyleParams style)=0;
	virtual void AssignColors(ColorsStyle &style)=0;
	virtual void SetVisible(BYTE t);
	virtual void SetParentUpdateStatus( UpdateStatus sts );
};
//////////////////////////////////////////////////////////////////////////

template <int ID, class TypeOfPoint, class PointStoreType, class PointToDrawType> 
class SeriesTemplate: public TChartSeries, public ChartSeriesStyleTemplate<PointToDrawType>
{	
	PointStoreType Values;
protected:
	int DataImportID;	
public:
	struct DataImportMsg: public MessageForWindow
	{
		SeriesTemplate* DestSeries;
		DataImportMsg() {Name="ChartMessageM1"; Msg=UM_SERIES_DATA_IMPORT; ClassID=ID; DestSeries=NULL;};
		virtual ~DataImportMsg() {};
	};
//-------------------------------------------
protected:
	virtual void _UpdateIcon(BMPanvas& icon) {ChartSeriesStyleTemplate<PointToDrawType>::_UpdateIcon(icon);}
public:
	SeriesTemplate(CString name, ChartSeriesStyleParams style=ChartSeriesStyleParams()):
	  TChartSeries(name), ChartSeriesStyleTemplate<PointToDrawType>(style)	{ SeriesType=ID; DataImportID=ID;};
	virtual ~SeriesTemplate() {};	
	int GetSize() const {return Values.GetSize();};
	virtual ChartSeriesStyleParams GetStyle() const
	{
		return ChartSeriesStyleTemplate<PointToDrawType>::GetStyle();
	};
	virtual void SetStyle(ChartSeriesStyleParams style)
	{
		ChartSeriesStyleTemplate<PointToDrawType>::SetStyle(style);
		PostParentMessage(UM_SERIES_UPDATE);	
	}
	virtual void AssignColors(ColorsStyle &style)
	{
		TAbstractGraphics::AssignColors(style);
		PostParentMessage(UM_SERIES_UPDATE);	
	}
	virtual TypeOfPoint operator[](int i) 
	{
		TypeOfPoint ret; ret=Values[i];
		return ret;
	}
	virtual int GetValues(PointStoreType& dest, int min=SERIES_MIN,int max=SERIES_MAX )
	{
		if(min==SERIES_MIN) min=0;
		if(max==SERIES_MAX) max=GetSize();
		if(min>max) {int t=min; min=max; max=t;}
		if(min<0) return -2;
		if(max>GetSize()) return -3;
		return dest.CopyFrom(Values,max-min,min);
	}
	virtual void Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			TChartSeries::Serialize(ar);
			ChartSeriesStyleTemplate<PointToDrawType>::Serialize(ar);
			int numValues=Values.GetSize();
			ar << numValues;
			for(int i=0;i<numValues;i++) Values[i].Serialize(ar);
		}
		else
		{
			TypeOfPoint t;
			TChartSeries::Serialize(ar);
			ChartSeriesStyleTemplate<PointToDrawType>::Serialize(ar);
			Values.RemoveAll(); int numValues;
			ar >> numValues;
			SetParentUpdateStatus(UPD_OFF);
			for(int i=0;i<numValues;i++) {t.Serialize(ar); Values.Add(t);}
			SetParentUpdateStatus(UPD_ON);
		}
	}
	virtual int AddXY(TypeOfPoint& pnt)
	{
		int ret=Values.Add(pnt); 
		if(ret>=0) 
		{
			update=true; 
			PostParentMessage(UM_SERIES_UPDATE);
		}		
		return ret;
	}
	virtual void RemoveAt(int n)
	{
		Values.RemoveAt(n); 
		update=true; 
		PostParentMessage(UM_SERIES_UPDATE);		
		return;
	}
	virtual int ImportData(MessagesInspectorSubject* _pnt,TChartImportMsgDestroy destroy)
	{
		int ret=0; DataImportMsg* msg=(DataImportMsg*)_pnt;
		if(_pnt->ValidateClass(DataImportID))
		{
			if(msg->DestSeries == this)				
			{
				int num_of_add_points = 0;
				SeriesTemplate* series = (SeriesTemplate*)msg->DestSeries;				
				SetParentUpdateStatus(UPD_OFF);
				for(int i = 0; i < series->Values.GetSize(); i++) 
				{
					AddXY((series->Values)[i]); ret++;
				}
				SetParentUpdateStatus(UPD_ON);
			}
		}
		if(destroy==DELETE_IMPORT_MSG) delete _pnt;
		return ret;
	}
	virtual void GetInfo(CStringArray& strs)
	{
		CString T;
		T.Format("Name=%s",Name); strs.Add(T);
		T.Format("Number of points=%d",Values.GetSize()); strs.Add(T);
	}
	virtual int _FindMinMax();
	virtual void FormatElement(int n, CString &t)
	{
		TypeOfPoint temp; temp=operator[](n); CString T; 
		T.Format("%d",(int)temp.type);				
		t.Format("%s%s%c",temp.Format(),T,9);
	}
	virtual void Draw(BMPanvas* img) {if(GetSize()!=0) TAbstractGraphics::Draw(img);}
	void _Draw(BMPanvas* canvas)
	{
		int size=GetSize(), j;	
		PointToDrawType *pnts=new PointToDrawType[size];
		for(j=0;j<size;j++) pnts[j]=Render->LogicToScreen(operator[](j)); 
		if(_ErrorBarStyle<PointToDrawType>::Draw!=NULL)
			for(j=0;j<size;j++) (this->*_ErrorBarStyle<PointToDrawType>::Draw)(canvas,pnts[j]);
		else canvas->MoveTo(pnts[size-2]);
		if(_LineStyle::Draw!=NULL)
			for(j=size-2;j>=0;j--) (this->*_LineStyle::Draw)(canvas,pnts[j]);
		if(_SymbolStyle::Draw!=NULL)
			for(j=0;j<size;j++) (this->*_SymbolStyle::Draw)(canvas,pnts[j]);
		delete[] pnts;
	}
	void ClearAll() 
	{
		Values.RemoveAll();
		PostParentMessage(UM_SERIES_UPDATE);
	}
	void DispatchDataImportMsg()
	{
		DataImportMsg* ret=new DataImportMsg();
		if (ret != NULL)
		{
			if (Parent != NULL)
			{
				if (Parent->Parent != NULL)
				{
					TChart& chart=*((TChart*)(Parent->Parent));
					ret->Reciver=chart; ret->DestSeries=this;
					ret->Dispatch(); 
				}
			}
		}
	}
	void DispatchDataImportMsg(WindowAddress& chart)
	{
		DataImportMsg* ret=new DataImportMsg();
		if (ret != NULL)
		{
			ret->Reciver=chart; ret->DestSeries=this;		
			ret->Dispatch(); 
		}
	}
};


typedef SeriesTemplate<SIMPLE_POINT, SimplePoint, SimplePointArray, CPoint> TSimplePointSeries;
typedef SeriesTemplate<POINTvsERROR, PointVsError, PointVsErrorArray, CPointVsError> TPointVsErrorSeries;

//**************************************
enum SeriesArrayDeleteSeries {DELETE_SERIES,DO_NOT_DELETE_SERIES};

class TSeriesArray: public TAbstractGraphics
{
	friend class TChart;
	CArray<WORD> ColorsTable;	
private:	
	SeriesArrayDeleteSeries ToDeleteSeries;
public:
	CArray<TChartSeries*,TChartSeries*> Series;
    
	int RegisterSeries(TChartSeries*);	
public:
	TSeriesArray(SeriesArrayDeleteSeries _DeleteSeries=DELETE_SERIES);
	~TSeriesArray();
	int GetSize();
	TChartSeries* operator[](int i);
protected:
	int Add(TChartSeries* t);
public:
	void DeleteItem(int i);
	void ClearAll();    
	void Serialize(CArchive& ar);
	virtual void GetDC(CDC& canvas) {};
	TChartSeries* CreateSeries(int);
	int FindSeries(SeriesSearchPattern patern, TSeriesArray& results);

};

typedef ProtectedObjectX<TSeriesArray> ProtectedSeriesArray;
typedef ProtectorX<TSeriesArray> SeriesProtector;

//++++++++++++++++++++++++++++++


