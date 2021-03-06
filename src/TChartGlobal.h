#pragma  once

#include <afxtempl.h>
#include <afxmt.h>
#include <math.h>
#include <memory.h>
#include "BMPanvas.h"
#include "MyThread.h"
#include "type_array.h"
#include "my_color.h"

#define SET(byte,mask) byte|=(mask)
#define RST(byte,mask) byte&=(~(mask))
#define GET(byte,mask) (byte & (mask))
#define MEAN_CALC 1<<0
#define SD_CALC 1<<1

enum SeriesType {TCHART_MSG_TYPE_FIRST = 0xbbbbbbbb,
	SCATER_LINE, SCATER_ERR, LINE_ERR, SCATER_LINE_ERR, 
	SCATER_LINE_ERR_BCKG, SLEB, SLEB_MULTI,
	SCATER_ERR_T,SCATER_ERR_T2, SCATER_LINE_ERR_T, SCATER_PVE, SIMPLE_LINE,
	SIMPLE_POINT, POINTvsERROR,
	TCHART_MSG_TYPE_LAST
};

enum TChartMessages
{
	UM_SERIES_DATA_IMPORT=WM_USER + 4000, UM_CHART_BUFFER_UPDATE, UM_CHART_SHOWALL, UM_SERIES_UPDATE, UM_UPDATE_STATUS_CHANGED
};

#define clWHITE RGB(255,255,255)
#define clBLACK RGB(0,0,0)
#define clRED RGB(255,0,0)
#define clGREEN RGB(0,255,0)
#define clBLUE RGB(0,0,255)
enum ChartColor {RANDOM_COLOR};

struct ColorsStyle 
{
	COLORREF BColor,PColor; 
	static CArray<WORD> ColorsTable;

	explicit ColorsStyle(COLORREF bColor=clWHITE, COLORREF pColor=clBLACK) 
	{
		BColor=bColor; PColor=pColor;
	}		
	explicit ColorsStyle(COLORREF bColor, ChartColor pColor) 
	{
		BColor=bColor; PColor=GetRandomColor();
	}		
	explicit ColorsStyle(ChartColor bColor, COLORREF pColor=clBLACK) 
	{
		BColor=GetRandomColor(); PColor=pColor;
	}		
	static COLORREF GetRandomColor();
	void Serialize(CArchive& ar);
};

enum ChartSerieStyles {
	MIN_LINE_STYLE = 1000,
		NO_LINE, STRAIGHT,							//Line styles
	MAX_LINE_STYLE,
	MIN_SYMBOL_STYLE,
		NO_SYMBOL, CIRCLE, CROSS45, VERT_LINE,		//Symbol styles
	MAX_SYMBOL_STYLE,
	MIN_EBAR_STYLE,
		NO_BARS, POINTvsERROR_BAR,					//ErrorBar styles
	MAX_EBAR_STYLE,
	SYMBOL_DX, SYMBOL_DY, EBAR_CAP,
};

enum {ID_CHART=1234567,ID_CHART_FRAME};

enum ChartAxisTickStyle {TICK_NONE,TICK_IN,TICK_OUT,TICK_INOUT};
enum ChartAxisLayoutStyle {AXIS_NONE,LEFT_AXIS,RIGHT_AXIS,TOP_AXIS,BOTTOM_AXIS};

enum ChartRendersID {CHARTRENDER_DEFAULT,SERIES_RENDER,FRAME_RENDER};

struct TChartInterval
{
	double min,max;

	TChartInterval(){min=max=0;}
	TChartInterval(double _min,double _max) {min=_min; max=_max;}
	double Length() {return max-min;}
	void Offset(double d) {min+=d;max+=d;}
	BOOL IsNull() {return (min==max ? TRUE:FALSE);}
	BOOL operator==(TChartInterval& t) 	
	{
		return (min==t.min && max==t.max);
	}
	void Normalize() { if(min>max) {double t; t=max; max=min; min=t;} } 
};

enum {SIMPLE_POINT_MSG=0xabcdea,POINTvsERROR_MSG};


