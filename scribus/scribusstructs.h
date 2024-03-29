/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef SCRIBUSSTRUCTS_H
#define SCRIBUSSTRUCTS_H

#ifdef HAVE_CONFIG_H
#include "scconfig.h"
#endif

#include <QAction>
#include <QByteArray>
#include <QDebug>
#include <QKeySequence>
#include <QList>
#include <QPolygon>
#include <QStack>
#include <QString>
#include <QMap>
#include <QVector>

#include <vector>

#include "scribusapi.h"
#include "colormgmt/sccolormgmtstructs.h"
#include "scimagestructs.h"
#include "sctextstruct.h"
#include "scfonts.h"
#include "fpointarray.h"
#include "vgradient.h"
#include "annotation.h"
#include "pageitem.h"

extern bool SCRIBUS_API compareDouble(double, double);

typedef struct
{
	int r;
	int g;
	int b;
	void getValues(int& vr, int& vg, int& vb) {vr = r; vg = g; vb = b;}
} RGBColor;

typedef struct
{
	int c;
	int m;
	int y;
	int k;
	void getValues(int& vc, int& vm, int& vy, int& vk) {vc = c; vm = m; vy = y; vk = k;}
} CMYKColor;

struct CopyContentsBuffer
{
	PageItem::ItemType sourceType;
	QString contentsFileName;
	double LocalScX;
	double LocalScY;
	double LocalX;
	double LocalY;
	double LocalRot;
	double ItemX;
	double ItemY;
	ScImageEffectList effects;
	QString inputProfile;
	bool   useEmbedded;
	eRenderIntent renderingIntent;
};

/** \brief Pagemargins and bleeds*/
class MarginStruct
{
	public:
		MarginStruct() : Top(0), Left(0), Bottom(0), Right(0) {}
		MarginStruct(double top, double left, double bottom, double right) :
			Top(top), Left(left), Bottom(bottom), Right(right) {}
		MarginStruct(const MarginStruct& rhs) {Top=rhs.Top;Bottom=rhs.Bottom;Left=rhs.Left;Right=rhs.Right;}
		void set(double top, double left, double bottom, double right) {Top=top;Bottom=bottom;Left=left;Right=right;}
		void resetToZero() {Top=0.0;Bottom=0.0;Left=0.0;Right=0.0;}
		bool hasNonZeroValue() { return Top!=0.0 || Bottom!=0.0 || Left!=0.0 || Right!=0.0;}
		void print() {qDebug() << Top << Left << Bottom << Right;}
		double Top;
		double Left;
		double Bottom;
		double Right;
};

struct DocPagesSetup
{
	DocPagesSetup() : pageArrangement(0), firstPageLocation(0), firstPageNumber(0), orientation(0), autoTextFrames(false), columnDistance(0), columnCount(1) {}
	DocPagesSetup(int pa, int fpl, int fpn, int o, bool atf, double cd, double cc) :
		pageArrangement(pa), firstPageLocation(fpl), firstPageNumber(fpn), orientation(o), autoTextFrames(atf),
		columnDistance(cd), columnCount(cc) {}
	int pageArrangement;
	int firstPageLocation;
	int firstPageNumber;
	int orientation;
	bool autoTextFrames;
	double columnDistance;
	double columnCount;
};

struct PageSet
{
	QString Name;
	int FirstPage;
	int Rows;
	int Columns;
//	double GapHorizontal;
//	double GapVertical;
//	double GapBelow;
	QStringList pageNames;
};

struct CMSData
{
	QString DefaultMonitorProfile;
	QString DefaultPrinterProfile;
	QString DefaultImageRGBProfile;
	QString DefaultImageCMYKProfile;
	QString DefaultSolidColorRGBProfile;
	QString DefaultSolidColorCMYKProfile;
	int ComponentsInput2;
	int ComponentsInput3;
	int ComponentsPrinter;
	eRenderIntent DefaultIntentImages;
	eRenderIntent DefaultIntentColors;
	bool CMSinUse;
	bool SoftProofOn;
	bool SoftProofFullOn;
	bool GamutCheck;
	bool BlackPoint;
} ;

struct SingleLine
{
	double Width;
	int Dash;
	int LineEnd;
	int LineJoin;
	QString Color;
	int Shade;
	// setter necessary for use with serializer/digester
	void setLineWidth(double value) { Width = value; }
	void setDash(int value)         { Dash = value; }
	void setLineEnd(int value)      { LineEnd = value; }
	void setLineJoin(int value)     { LineJoin = value; }
	void setColor(const QString& name) { Color = name; }
	void setShade(int value)        { Shade = value; }
	bool operator==(const SingleLine& other) const
	{
		if (!compareDouble(Width, other.Width) )
			return false;
		if ((Dash != other.Dash)  || (LineEnd != other.LineEnd) || (LineJoin != other.LineJoin) ||
			(Color != other.Color)|| (Shade != other.Shade))
			return false;
		return true;
	}
	bool operator!=(const SingleLine& other) const
	{
		return !(*this == other);
	}
};

struct ArrowDesc
{
	QString name;
	bool userArrow;
	FPointArray points;
};

struct PDFPresentationData
{
	int pageEffectDuration;
	int pageViewDuration;
	int effectType;
	int Dm;
	int M;
	int Di;
};

struct LPIData
{
	int Frequency;
	int Angle;
	int SpotFunc;
};

struct Keys
{
	QString actionName;
	QString cleanMenuText;
	QKeySequence keySequence;
	int tableRow;
	QString menuName;
	int menuPos;
};

enum PrintEngine
{
	PostScript1 = 1,
	PostScript2 = 2,
	PostScript3 = 3,
	WindowsGDI  = 4
};
typedef QMap<QString, PrintEngine> PrintEngineMap;

struct PrintOptions
{
	bool firstUse;
	bool toFile;
	bool useAltPrintCommand;
	bool outputSeparations;
	bool useSpotColors;
	bool useColor;
	bool mirrorH;
	bool mirrorV;
	bool useICC;
	bool doGCR;
	bool doClip;
	bool setDevParam;
	bool useDocBleeds;
	bool cropMarks;
	bool bleedMarks;
	bool registrationMarks;
	bool colorMarks;
	bool includePDFMarks;
	int  copies;
	PrintEngine prnEngine;
	double markOffset;
	MarginStruct bleeds;
	std::vector<int> pageNumbers;
	QString printerOptions;
	QString printer;
	QString filename;
	QString separationName;
	QStringList allSeparations;
	QString printerCommand;
	QByteArray devMode; // printer specific options on Windows
};

typedef QMap<QString,QString> ProfilesL;
// typedef QValueVector<SingleLine> multiLine;

class multiLine : public QList<SingleLine> {
public:
	QString shortcut;
	bool operator!=(const multiLine& other) const
	{
		return !(this->operator ==(other));
	}
};

typedef enum {
	MissingGlyph=1,
	TextOverflow=2,
	ObjectNotOnPage=3,
	MissingImage=4,
	ImageDPITooLow=5,
	Transparency=6,
	PDFAnnotField=7,
	PlacedPDF=8,
	ImageDPITooHigh=9,
	ImageIsGIF=10,
	BlendMode=11,
	WrongFontInAnnotation=12,
	NotCMYKOrSpot=13,
	DeviceColorAndOutputIntend=14,
	FontNotEmbedded=15,
	EmbeddedFontIsOpenType=16,
	OffConflictLayers=17,
	PartFilledImageFrame= 18
} PreflightError;

typedef QMap<PreflightError, int> errorCodes;

enum AppMode
{
	modeNormal,
	modeDrawShapes,
	modeStoryEditor,
	modeDrawImage,
	modeDrawText,
	modeMagnifier,
	modeEdit,
	modeDrawLine,
	modeRotation,
	modeLinkFrames,
	modeUnlinkFrames,
	modeDrawRegularPolygon,
	modeDrawBezierLine,
	modeInsertPDFButton,
	modeInsertPDFTextfield,
	modeInsertPDFCheckbox,
	modeInsertPDFCombobox,
	modeInsertPDFListbox,
	modeInsertPDFTextAnnotation,
	modeInsertPDFLinkAnnotation,
	modeDrawFreehandLine,
	modeDrawTable,
	modeDrawTable2, // Temporary name for new tables.
	modePanning,
	modeMeasurementTool,
	modeEditGradientVectors,
	modeEyeDropper,
	modeCopyProperties,
	modeEditClip,
	modeDrawLatex,
	modeImportObject,
	modeInsertPDF3DAnnotation,
	modeEditMeshGradient,
	modeDrawCalligraphicLine,
	modeDrawArc,
	modeEditArc,
	modeEditPolygon,
	modeDrawSpiral,
	modeEditSpiral,
	modeEditTable,
	modeEditMeshPatch
};

struct AlignObjs
{
	int ObjNr;
	int Group;
	double x1;
	double y1;
	double x2;
	double y2;
	double width;
	double height;
	QList<PageItem*> Objects;
};

/*! \brief Human readable orientations */
enum PageOrientation
{
	portraitPage = 0,
	landscapePage = 1,
	customPage = 30
};

enum UpdateRequests
{
	reqColorsUpdate = 1,
	reqCharStylesUpdate = 2,
	reqParaStylesUpdate = 4,
	reqTextStylesUpdate = 6,
	reqArrowStylesUpdate = 8,
	reqLineStylesUpdate  = 16,
	reqSymbolsUpdate     = 32,
	reqDefFontListUpdate = 64,
	reqDocFontListUpdate = 128,
	reqStyleComboDocUpdate = 256,
	reqCmsOptionsUpdate  = 512,
	reqCustomShapeUpdate = 1024,
	reqUpdateAll = 65535
};

//! \brief Common type for guides list
typedef QList<double> Guides;

//! \brief from ols scribusXml
struct Linked
{
	int Start;
	int StPag;
};

// this is a quick hack to combine runs until I've thought of something better -- AV
class LastStyles {
public:
	CharStyle Style;
	int StyleStart;
	QString ParaStyle;
	LastStyles() {
		StyleStart = 0;
		ParaStyle = "";
	}
};

#endif



