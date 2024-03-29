/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#ifndef PDFLIBCORE_H
#define PDFLIBCORE_H

#include <QFile>
#include <QDataStream>
#include <QPixmap>
#include <QList>
#include <QStack>
#include <string>
#include <vector>

class QImage;
class QRect;
class QString;
class QTextCodec;
class PageItem;
class BookMItem;
class BookMView;
class ScribusDoc;
class ScPage;
class PDFOptions;
class PrefsContext;
class MultiProgressDialog;
class ScLayer;
class ScText;

#include "scribusstructs.h"
#include "scimagestructs.h"

#ifdef HAVE_PODOFO
#include <podofo/podofo.h>
#endif


/**
 * PDFLibCore provides Scribus's implementation of PDF export functionality.
 *
 * This is not public API and is not exported in Scribus's symbol table on supporting
 * platforms. Do not include this header, use pdflib.h instead. Including this header
 * from anywhere except pdflib.cpp may cause linker errors and other problems,
 * and is totally pointless since all the interfaces that are publically
 * visible are presented by PDFlib anyway.
 *
 *@author Franz Schmid
 */

class PDFLibCore : public QObject
{
	Q_OBJECT

public:
	explicit PDFLibCore(ScribusDoc & docu);
	~PDFLibCore();
	bool doExport(const QString& fn, const QString& nam, int Components,
				  const std::vector<int> & pageNs, const QMap<int,QPixmap> & thumbs);

	const QString& errorMessage(void) const;
	bool  exportAborted(void) const;

private:
	struct ShIm
	{
		int ResNum;
		int Width;
		int Height;
		int Page;
		double reso;
		double sxa;
		double sya;
		double xa;
		double ya;
		double origXsc;
		double origYsc;
		QMap<int, ImageLoadRequest> RequestProps;
	};

	bool PDF_Begin_Doc(const QString& fn, SCFonts &AllFonts, QMap<QString, QMap<uint, FPointArray> > DocFonts, BookMView* vi);
	void PDF_Begin_Page(const ScPage* pag, QPixmap pm = 0);
	void PDF_End_Page(int physPage);
	bool PDF_TemplatePage(const ScPage* pag, bool clip = false);
	bool PDF_ProcessPage(const ScPage* pag, uint PNr, bool clip = false);
	bool PDF_ProcessMasterElements(const ScLayer& layer, const ScPage* page, uint PNr);
	bool PDF_ProcessPageElements(const ScLayer& layer, const ScPage* page, uint PNr);
	bool PDF_End_Doc(const QString& PrintPr = "", const QString& Name = "", int Components = 0);
	bool closeAndCleanup();

	void PDF_Error(const QString& errorMsg);
	void PDF_Error_WriteFailure(void);
	void PDF_Error_ImageLoadFailure(const QString& fileName);
	void PDF_Error_ImageWriteFailure(const QString& fileName);
	void PDF_Error_MaskLoadFailure(const QString& fileName);
	void PDF_Error_InsufficientMemory(void);

	QByteArray EncodeUTF16(const QString &in);
	QString    EncStream(const QString & in, int ObjNum);
	QString    EncString(const QString & in, int ObjNum);
	QString    EncStringUTF16(const QString & in, int ObjNum);

	bool       EncodeArrayToStream(const QByteArray& in, int ObjNum);

	int     WriteImageToStream(ScImage& image, int ObjNum, bool cmyk, bool gray, bool precal);
	int     WriteJPEGImageToStream(ScImage& image, const QString& fn, int ObjNum, int quality, bool cmyk, bool gray, bool sameFile, bool precal);
	int     WriteFlateImageToStream(ScImage& image, int ObjNum, bool cmyk, bool gray, bool precal);

	void    CalcOwnerKey(const QString & Owner, const QString & User);
	void    CalcUserKey(const QString & User, int Permission);
	QString FitKey(const QString & pass);

	QString setStrokeMulti(struct SingleLine *sl);
	QString SetClipPathArray(FPointArray *ite, bool poly = true);
	QString SetClipPathImage(PageItem *ite);
	QString SetClipPath(PageItem *ite, bool poly = true);
	QString SetColor(const QString& farbe, double Shade);
	QString SetColor(const ScColor& farbe, double Shade);
	QString SetGradientColor(const QString& farbe, double Shade);
	QString putColor(const QString& color, double Shade, bool fill);
	QString putColorUncached(const QString& color, int Shade, bool fill);
	QString Write_TransparencyGroup(double trans, int blend, QString &data, PageItem *controlItem = 0);
	QString setTextSt(PageItem *ite, uint PNr, const ScPage* pag);
	bool    setTextCh(PageItem *ite, uint PNr, double x, double y, uint d,  QString &tmp, QString &tmp2, const ScText * hl, const ParagraphStyle& pstyle, const ScPage* pag);
	void    getBleeds(const ScPage* page, double &left, double &right);
	void    getBleeds(const ScPage* page, double &left, double &right, double &bottom, double& top);

	// Provide a couple of PutDoc implementations to ease transition away from
	// QString abuse and to provide fast paths for constant strings.
	void PutDoc(const QString & in) { outStream.writeRawData(in.toLatin1(), in.length()); }
	void PutDoc(const QByteArray & in) { outStream.writeRawData(in, in.size()); }
	void PutDoc(const char* in) { outStream.writeRawData(in, strlen(in)); }
	void PutDoc(const std::string & in) { outStream.writeRawData(in.c_str(), in.length()); }

	void       PutPage(const QString & in) { Content += in; }
	void       StartObj(int nr);
	uint       newObject() { return ObjCounter++; }
	uint       WritePDFStream(const QString& cc);
	uint       WritePDFString(const QString& cc);
	void       writeXObject(uint objNr, QString dictionary, QByteArray stream);
	uint       writeObject(QString type, QString dictionary);
	uint       writeGState(QString dictionary) { return writeObject("/ExtGState", dictionary); }
	uint       writeActions(const Annotation&, uint annotationObj);
//	QString    PDFEncode(const QString & in);
	QByteArray ComputeMD5(const QString& in);
	QByteArray ComputeRC4Key(int ObjNum);

	bool    PDF_ProcessItem(QString& output, PageItem* ite, const ScPage* pag, uint PNr, bool embedded = false, bool pattern = false);
	QString PDF_ProcessTableItem(PageItem* ite, const ScPage* pag);
	QString HandleBrushPattern(PageItem* ite, QPainterPath &path, const ScPage* pag, uint PNr);
	QString drawArrow(PageItem *ite, QTransform &arrowTrans, int arrowIndex);
	void    PDF_Bookmark(PageItem *currItem, double ypos);
	bool    PDF_PatternFillStroke(QString& output, PageItem *currItem, int kind = 0, bool forArrow = false);
	quint32 encode32dVal(double val);
	quint16 encode16dVal(double val);
	void    encodeColor(QDataStream &vs, QString colName, int colShade, QStringList &spotColorSet, bool spotMode);
	bool    PDF_MeshGradientFill(QString& output, PageItem *currItem);
	bool	PDF_PatchMeshGradientFill(QString& output, PageItem *c);
	bool    PDF_DiamondGradientFill(QString& output, PageItem *currItem);
	bool    PDF_TensorGradientFill(QString& output, PageItem *currItem);
	bool    PDF_GradientFillStroke(QString& output, PageItem *currItem, bool stroke = false, bool forArrow = false);
	QString PDF_TransparenzFill(PageItem *currItem);
	QString PDF_TransparenzStroke(PageItem *currItem);
	bool    PDF_Annotation(PageItem *ite, uint PNr);
#ifdef HAVE_OSG
	bool    PDF_3DAnnotation(PageItem *ite, uint PNr);
#endif
	void    PDF_Form(const QString& im);
	void    PDF_xForm(uint objNr, double w, double h, QString im);
	bool    PDF_Image(PageItem* c, const QString& fn, double sx, double sy, double x, double y, bool fromAN = false, const QString& Profil = "", bool Embedded = false, eRenderIntent Intent = Intent_Relative_Colorimetric, QString* output = NULL);
	bool    PDF_EmbeddedPDF(PageItem* c, const QString& fn, double sx, double sy, double x, double y, bool fromAN, const QString& Profil, bool Embedded, int Intent, ShIm& imgInfo, QString* output = NULL);
#if HAVE_PODOFO
	void copyPoDoFoObject(const PoDoFo::PdfObject* obj, uint scObjID, QMap<PoDoFo::PdfReference, uint>& importedObjects);
	void copyPoDoFoDirect(const PoDoFo::PdfVariant* obj, QList<PoDoFo::PdfReference>& referencedObjects, QMap<PoDoFo::PdfReference, uint>& importedObjects);
#endif

	void generateXMP(const QString& timeStamp);
	int bytesWritten() { return Spool.pos(); }


	QString Content;
	QString ErrorMessage;
	ScribusDoc & doc;
	const ScPage * ActPageP;
	const PDFOptions & Options;
	BookMView* Bvie;
	QFile Spool;
	int Dokument;
	struct Dest
	{
		QString Name;
		int Seite;
		QString Act;
	};
	struct Cata
	{
		int Outlines;
		int PageTree;
		int Dest;
	}
	Catalog;
	struct PagT
	{
		QMap<int, int> Kids;
		int Count;
	}
	PageTree;
	struct PagL
	{
		int ObjNum;
		int Thumb;
		QMap<QString,int> XObjects;
		QMap<QString,int> ImgObjects;
		QMap<QString,int> FObjects;
		QList<int> AObjects;
		QList<int> FormObjects;
	}
	Seite;
	struct OutL
	{
		int First;
		int Last;
		int Count;
	}
	Outlines;
	struct Bead
	{
		int Parent;
		int Next;
		int Prev;
		int Page;
		QRect Recht;
	};
	struct ICCD
	{
		int ResNum;
		int components;
		QString ResName;
		QString ICCArray;
	};
	struct OCGInfo
	{
		int ObjNum;
		bool visible;
		QString Name;
	};
	struct SpotC
	{
		int ResNum;
		QString ResName;
	};
	struct gData
	{
		int ResNumG;
		int ResNumX;
		QString ResNamG;
		QString ResNamX;
		QString data;
	};
	QMap<QString,ShIm> SharedImages;
	QList<uint> XRef;
	QList<Dest> NamedDest;
	QList<int> Threads;
	QList<Bead> Beads;
	QList<int> CalcFields;
	QMap<QString,int> Patterns;
	QMap<QString,int> Shadings;
	QMap<QString,int> Transpar;
	QMap<QString,ICCD> ICCProfiles;
	QHash<QString, OCGInfo> OCGEntries;
	QTextCodec* ucs2Codec;
	int ObjCounter;
	QString ResNam;
	int ResCount;
	QString NDnam;
	QString Datum;
	int NDnum;
	QMap<QString, QString> UsedFontsP;
	QMap<QString, QString> UsedFontsF;
	QByteArray KeyGen;
	QByteArray OwnerKey;
	QByteArray UserKey;
	QByteArray FileID;
	QByteArray EncryKey;
	int Encrypt;
	int KeyLen;
	QString HTName;
	bool BookMinUse;
	ColorList colorsToUse;
	QMap<QString, SpotC> spotMap;
	QMap<QString, SpotC> spotMapReg;
	QString spotNam;
	int spotCount;
	int inPattern;
	QDataStream outStream;
	QMap<QString, QString> StdFonts;
	MultiProgressDialog* progressDialog;
	bool abortExport;
	bool usingGUI;
	double bleedDisplacementX;
	double bleedDisplacementY;
	QMap<QString, QMap<uint, uint> > Type3Fonts;
	QString xmpPacket;
	QStack<QPointF> groupStackPos;

protected slots:
	void cancelRequested();
};

#endif



