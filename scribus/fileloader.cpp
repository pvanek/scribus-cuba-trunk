/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include <QDomElement>
#include <QFile>
#include <QFileInfo>
#include <QtAlgorithms>
#include <QCursor>
#include <QRegExp>
#include <QDir>
#include <QTextCodec>
#include <QCheckBox>
#include <QMessageBox>
#include <QProgressBar>

#include <QList>
#include <cstdlib>
#include <cmath>
#include <QTextStream>

#include "scconfig.h"

#include "commonstrings.h"
#include "fileloader.h"
#include "hyphenator.h"
#include "loadsaveplugin.h"
#include "pagestructs.h"
#include "pluginmanager.h"
#include "prefsmanager.h"
#include "resourcecollection.h"
#include "scclocale.h"
#include "scpage.h"
#include "scribuscore.h"
#include "scribusXml.h"
#include "units.h"
#include "util.h"

#include "plugins/formatidlist.h"
#include "text/nlsconfig.h"
#include "ui/guidemanager.h"
#include "ui/fontreplacedialog.h"
#include "ui/missing.h"

// We need to include the headers for the plugins we support until we start
// using LoadSavePlugin to pick them for us. We only use these headers to
// get the format IDs, NOTHING ELSE.
// #include "plugins/svgimplugin/svgplugin.h"
// #include "plugins/psimport/importpsplugin.h"
// #include "plugins/fileloader/oodraw/oodrawimp.h"
#include <zlib.h>

/*!
 \author Franz Schmid
 \date
 \brief Constructor, sets the variable "FileName" to the input parameter fileName
 \param fileName filename to load
 \retval None
 */
FileLoader::FileLoader(const QString & fileName) :
	QObject(0),
	FileName(fileName),
	FileType(-1),
	formatSLA12x(LoadSavePlugin::getFormatById(FORMATID_SLA12XIMPORT)),
	formatSLA13x(LoadSavePlugin::getFormatById(FORMATID_SLA13XIMPORT)),
	formatSLA134(LoadSavePlugin::getFormatById(FORMATID_SLA134IMPORT)),
	formatSLA150(LoadSavePlugin::getFormatById(FORMATID_SLA150IMPORT)),
	formatPS(LoadSavePlugin::getFormatById(FORMATID_PSIMPORT)),
	formatSVG(LoadSavePlugin::getFormatById(FORMATID_SVGIMPORT)),
	formatSXD(LoadSavePlugin::getFormatById(FORMATID_SXDIMPORT)),
	formatODG(LoadSavePlugin::getFormatById(FORMATID_ODGIMPORT)),
	prefsManager(PrefsManager::instance())
{
}

// FIXME: This static method is here as a temporary transitional
// measure during the process of converting to file loader plugins.
const QString FileLoader::getLoadFilterString()
{
//	return LoadSavePlugin::fileDialogLoadFilter().join(";;");
	QStringList fmts = LoadSavePlugin::fileDialogLoadFilter();
	QString fmtString = QObject::tr("All Supported Formats")+" (";
	int ind = -1;
	for (int a = 0; a < fmts.count()-1; a++)
	{
		QString fmt = fmts[a];
		int s = fmt.indexOf("(");
		int e = fmt.lastIndexOf(")");
		QString f = fmt.mid(s+1, e-s-1);
#ifndef HAVE_POPPLER
		if (f.contains("pdf"))	// for removing PDF from the list
		{
			ind = a;
			continue;
		}
#endif
		fmtString += f + " ";
	}
	fmtString += ");;";
	if (ind != -1)
		fmts.removeAt(ind);
	fmtString += fmts.join(";;");
	return fmtString.simplified();
}

/*!
 \fn int FileLoader::TestFile()
 \author Franz Schmid
 \date
 \brief Tests if the file "FileName" exists and determines the type of the file.
 \retval int -1 if the file doesn't exist or any other error has occurred, 0 for the old Format, 1 for the new Format, 2 for EPS and PS files, 3 for SVG files and 4 for PDF files
 */
int FileLoader::TestFile()
{
	QFileInfo fi = QFileInfo(FileName);
	int ret = -1;
	if (!fi.exists())
		ret = -1;
	QString ext = fi.completeSuffix().toLower();

	QList<FileFormat> fileFormats(LoadSavePlugin::supportedFormats());
	QList<FileFormat>::const_iterator it(fileFormats.constBegin());
	QList<FileFormat>::const_iterator itEnd(fileFormats.constEnd());
	for ( ; it != itEnd ; ++it )
	{
		if ((*it).nameMatch.indexIn("."+ext)!=-1)
		{
//  		qDebug() << QString("Match :%1: :.%2: on %3").arg((*it).nameMatch.pattern()).arg(ext).arg((*it).trName);
			if ((*it).plug!=0)
			{
				if ((*it).plug->fileSupported(0, FileName))
				{
// 					qDebug(QString("File Supported With: %1").arg((*it).trName));
					ret=(*it).formatId;
					break;
				}
			}
		}
// 		else
// 			qDebug() << QString("No Match :%1: :.%2: on %3").arg((*it).nameMatch.pattern()).arg(ext).arg((*it).trName);
	}
	
// 	if (ret==-1)
// 	{
// 		if ((ext.endsWith("sla.gz")) || (ext.endsWith("sla")) || (ext.endsWith("scd.gz")) || (ext.endsWith("scd")))
// 			ret = CheckScribus();
// 		else 
// 		if (((ext.endsWith("ps")) || (ext.endsWith("eps"))) && (formatPS))
// 			ret = FORMATID_PSIMPORT;
// 		else if (((ext.endsWith("svg")) || (ext.endsWith("svgz"))) && (formatSVG))
// 			ret = FORMATID_SVGIMPORT;
// 		else if ((ext.endsWith("sxd")) && (formatSXD))
// 			ret = FORMATID_SXDIMPORT;
// 		else if ((ext.endsWith("odg")) && (formatODG))
// 			ret = FORMATID_ODGIMPORT;
// 	}
/*	if (ext == "pdf")
		ret = 4; */
	FileType = ret;
	return ret;
}
/*
static void replaceFonts(ScribusDoc* currDoc, PageItem *it, QMap<QString, int> UsedFonts, QMap<QString, QString> ReplacedFonts)
{	
	if ((it->asTextFrame()) || (it->asPathText()))
	{
		CharStyle newFontStyle;
		for (uint e = 0; e < it->itemText.nrOfRuns(); ++e)
		{
			int start = it->itemText.startOfRun(e);
			ScFace oldFont = it->itemText.charStyle(start).font();
			if (!UsedFonts.contains(oldFont.scName())) {
				newFontStyle.setFont((*currDoc->AllFonts)[ReplacedFonts[oldFont.scName()]]);
				it->itemText.applyCharStyle(start, it->itemText.endOfRun(e) - start, newFontStyle );
			}
		}
	}
}

*/
bool FileLoader::LoadPage(ScribusDoc* currDoc, int PageToLoad, bool Mpage, QString renamedPageName)
{
	bool ret = false;
// 	newReplacement = false;
	ReplacedFonts = currDoc->AllFonts->getSubstitutions();
// 	dummyScFaces.clear();
	QList<FileFormat>::const_iterator it;
	if (findFormat(FileType, it))
	{
		if (FileType==FORMATID_SLA12XIMPORT)
		{
			(*it).plug->setupTargets(currDoc, currDoc->view(), currDoc->scMW(), currDoc->scMW()->mainWindowProgressBar, &(prefsManager->appPrefs.fontPrefs.AvailFonts));
			ret=(*it).plug->loadPage(FileName, PageToLoad, Mpage, renamedPageName);
// 			if (ret)
// 				(*it).plug->getReplacedFontData(newReplacement, ReplacedFonts, dummyScFaces);
		}
		if (FileType==FORMATID_SLA13XIMPORT || FileType==FORMATID_SLA134IMPORT || FileType==FORMATID_SLA150IMPORT)
		{
			(*it).plug->setupTargets(currDoc, 0, currDoc->scMW(), currDoc->scMW()->mainWindowProgressBar, &(prefsManager->appPrefs.fontPrefs.AvailFonts));
			ret=(*it).plug->loadPage(FileName, PageToLoad, Mpage, renamedPageName);
// 			if (ret)
// 				(*it).plug->getReplacedFontData(newReplacement, ReplacedFonts, dummyScFaces);
		}
	}
	if (ret)
		ret = postLoad(currDoc);  // FIXME: return false if user doesnt want to replace fonts??
	
	/*
	//FIXME: this goes into scribusdoc and should work with substitutes
	if (ReplacedFonts.count() != 0)
	{
		if ((prefsManager->appPrefs.askBeforeSubstituite) || (newReplacement))
		{
			qApp->setOverrideCursor(QCursor(Qt::arrowCursor), true);
			FontReplaceDialog dia(0, &ReplacedFonts);
			if (dia.exec())
			{
				QMap<QString,QString>::Iterator itfsu;
				for (itfsu = ReplacedFonts.begin(); itfsu != ReplacedFonts.end(); ++itfsu)
				{
					if (dia.stickyReplacements->isChecked())
						prefsManager->appPrefs.GFontSub[itfsu.key()] = itfsu.data();
				}
			}
			else
			{
				return false;
			}
		}
		for (uint d = 0; d < currDoc->MasterItems.count(); ++d)
		{
			replaceFonts(currDoc, currDoc->MasterItems.at(d), currDoc->UsedFonts, ReplacedFonts);
		}
		for (uint d = 0; d < currDoc->DocItems.count(); ++d)
		{
			replaceFonts(currDoc, currDoc->DocItems.at(d), currDoc->UsedFonts, ReplacedFonts);
		}
		for (uint d = 0; d < currDoc->FrameItems.count(); ++d)
		{
			replaceFonts(currDoc, currDoc->FrameItems.at(d), currDoc->UsedFonts, ReplacedFonts);
		}
		for (uint a = 0; a < currDoc->paragraphStyles().count(); ++a)
		{
//			if ( !currDoc->paragraphStyles()[a].charStyle().font().isNone() && !currDoc->UsedFonts.contains(currDoc->paragraphStyles()[a].charStyle().font().scName()))
//				currDoc->paragraphStyles()[a].charStyle().setFont
//					((*currDoc->AllFonts)[ReplacedFonts[currDoc->paragraphStyles()[a].charStyle().font().scName()]]);
		}
		QMap<QString,QString>::Iterator itfsu;
		for (itfsu = ReplacedFonts.begin(); itfsu != ReplacedFonts.end(); ++itfsu)
		{
			if (!currDoc->UsedFonts.contains(itfsu.data()))
			{
//				QFont fo = prefsManager->appPrefs.AvailFonts[itfsu.data()]->Font;
//				fo.setPointSize(qRound(ScMW->doc->toolSettings.textSize / 10.0));
				currDoc->AddFont(itfsu.data(), qRound(currDoc->toolSettings.textSize / 10.0));
			}
		}
		if (prefsManager->appPrefs.askBeforeSubstituite)
			ReplacedFonts.clear();
// 		dummyScFaces.clear();
	}
	*/
	return ret;
}

/*!
 \fn bool FileLoader::LoadFile(ScribusDoc* currDoc)
 \author Franz Schmid
 \date
 \brief Loads the file "FileName" as a Scribus document
 \param currDoc the current document
 \retval bool true when loading is succsessful, false otherwise
 */
bool FileLoader::LoadFile(ScribusDoc* currDoc)
{
	newReplacement = false;
	currDoc->guidesPrefs().marginsShown = prefsManager->appPrefs.guidesPrefs.marginsShown;
	currDoc->guidesPrefs().framesShown = prefsManager->appPrefs.guidesPrefs.framesShown;
	currDoc->guidesPrefs().layerMarkersShown = prefsManager->appPrefs.guidesPrefs.layerMarkersShown;
	currDoc->guidesPrefs().gridShown = prefsManager->appPrefs.guidesPrefs.gridShown;
	currDoc->guidesPrefs().guidesShown = prefsManager->appPrefs.guidesPrefs.guidesShown;
	currDoc->guidesPrefs().colBordersShown = prefsManager->appPrefs.guidesPrefs.colBordersShown;
	currDoc->guidesPrefs().baselineGridShown = prefsManager->appPrefs.guidesPrefs.baselineGridShown;
	currDoc->guidesPrefs().linkShown = prefsManager->appPrefs.guidesPrefs.linkShown;
	currDoc->itemToolPrefs().polyCorners = prefsManager->appPrefs.itemToolPrefs.polyCorners;
	currDoc->itemToolPrefs().polyFactor = prefsManager->appPrefs.itemToolPrefs.polyFactor;
	currDoc->itemToolPrefs().polyRotation = prefsManager->appPrefs.itemToolPrefs.polyRotation;
	currDoc->itemToolPrefs().polyCurvature = prefsManager->appPrefs.itemToolPrefs.polyCurvature;
	currDoc->itemToolPrefs().polyOuterCurvature = prefsManager->appPrefs.itemToolPrefs.polyOuterCurvature;
	currDoc->itemToolPrefs().polyInnerRot = prefsManager->appPrefs.itemToolPrefs.polyInnerRot;
	currDoc->itemToolPrefs().polyUseFactor = prefsManager->appPrefs.itemToolPrefs.polyUseFactor;
	currDoc->setAutoSave(prefsManager->appPrefs.docSetupPrefs.AutoSave);
	currDoc->setAutoSaveTime(prefsManager->appPrefs.docSetupPrefs.AutoSaveTime);
	ReplacedFonts = currDoc->AllFonts->getSubstitutions();
	//dummyScFaces.clear();
	bool ret = false;
	QList<FileFormat>::const_iterator it;
	if (findFormat(FileType, it))
	{
//		qDebug("fileloader: type %d plugin %s"),FileType,(*it).trName);
		switch (FileType)
		{
			case FORMATID_SLA12XIMPORT:
				{
					(*it).setupTargets(currDoc, currDoc->view(), currDoc->scMW(), currDoc->scMW()->mainWindowProgressBar, &(prefsManager->appPrefs.fontPrefs.AvailFonts));
					ret=(*it).loadFile(FileName, LoadSavePlugin::lfCreateDoc);
// 					if (ret)
// 						(*it).getReplacedFontData(newReplacement, ReplacedFonts, dummyScFaces);
				}
				break;
			case FORMATID_SLA13XIMPORT:
			case FORMATID_SLA134IMPORT:
			case FORMATID_SLA150IMPORT:
				{
					(*it).setupTargets(currDoc, 0, currDoc->scMW(), currDoc->scMW()->mainWindowProgressBar, &(prefsManager->appPrefs.fontPrefs.AvailFonts));
					ret=(*it).loadFile(FileName, LoadSavePlugin::lfCreateDoc);
// 					if (ret)
// 						(*it).getReplacedFontData(newReplacement, ReplacedFonts, dummyScFaces);
				}
				break;
			default:
				(*it).setupTargets(currDoc, currDoc->view(), currDoc->scMW(), currDoc->scMW()->mainWindowProgressBar, &(prefsManager->appPrefs.fontPrefs.AvailFonts));
				ret = (*it).loadFile(FileName, LoadSavePlugin::lfCreateDoc);
				break;
		}
	}
	return ret;
}

bool FileLoader::SaveFile(const QString& fileName, ScribusDoc *doc, QString *savedFile)
{
	bool ret = false;
	QList<FileFormat>::const_iterator it;
	if (findFormat(FORMATID_SLA150EXPORT, it))
	{
		it->setupTargets(doc, 0, doc->scMW(), doc->scMW()->mainWindowProgressBar, &(prefsManager->appPrefs.fontPrefs.AvailFonts));
		ret = it->saveFile(fileName);
		if (savedFile)
			*savedFile = it->lastSavedFile();
	}
	return ret;
}

bool FileLoader::ReadStyles(const QString& fileName, ScribusDoc* doc, StyleSet<ParagraphStyle> &docParagraphStyles)
{
	QList<FileFormat>::const_iterator it;
	if (findFormat(FileType, it)) {
		(*it).plug->setupTargets(doc, 0, doc->scMW(), doc->scMW()->mainWindowProgressBar, &(prefsManager->appPrefs.fontPrefs.AvailFonts));
		return (*it).readStyles(fileName, doc, docParagraphStyles);
	}
	return false;
}

bool FileLoader::ReadCharStyles(const QString& fileName, ScribusDoc* doc, StyleSet<CharStyle> &docCharStyles)
{
	QList<FileFormat>::const_iterator it;
	if (findFormat(FileType, it)) {
		(*it).plug->setupTargets(doc, 0, doc->scMW(), doc->scMW()->mainWindowProgressBar, &(prefsManager->appPrefs.fontPrefs.AvailFonts));
		return (*it).readCharStyles(fileName, doc, docCharStyles);
	}
	return false;
}

bool FileLoader::ReadColors(const QString& fileName, ColorList & colors)
{
	QList<FileFormat>::const_iterator it;
	if (findFormat(FileType, it))
		return (*it).readColors(fileName, colors);
	return false;
}


bool FileLoader::ReadPageCount(const QString& fileName, int *num1, int *num2, QStringList & masterPageNames)
{
	QList<FileFormat>::const_iterator it;
	if (findFormat(FileType, it))
		return (*it).readPageCount(fileName, num1, num2, masterPageNames);
	return false;
}

bool FileLoader::ReadLineStyles(const QString& fileName, QMap<QString,multiLine> *Sty)
{
	QList<FileFormat>::const_iterator it;
	if (findFormat(FileType, it))
		return (*it).readLineStyles(fileName, Sty);
	return false;
}

void FileLoader::readParagraphStyle(ParagraphStyle& vg, const QDomElement& pg, SCFonts &avail, ScribusDoc *currDoc)
{
	vg.setName(pg.attribute("NAME"));
	vg.setLineSpacingMode(static_cast<ParagraphStyle::LineSpacingMode>(pg.attribute("LINESPMode", "0").toInt()));
	vg.setLineSpacing(ScCLocale::toDoubleC(pg.attribute("LINESP")));
	vg.setLeftMargin(ScCLocale::toDoubleC(pg.attribute("INDENT"), 0.0));
	if (pg.hasAttribute("RMARGIN"))
		vg.setRightMargin(ScCLocale::toDoubleC(pg.attribute("RMARGIN"), 0.0));
	else
		vg.setRightMargin(0);
	vg.setFirstIndent(ScCLocale::toDoubleC(pg.attribute("FIRST"), 0.0));
	vg.setAlignment(static_cast<ParagraphStyle::AlignmentType>(pg.attribute("ALIGN").toInt()));
	vg.setGapBefore(ScCLocale::toDoubleC(pg.attribute("VOR"), 0.0));
	vg.setGapAfter(ScCLocale::toDoubleC(pg.attribute("NACH"), 0.0));
	QString tmpf(pg.attribute("FONT", currDoc->itemToolPrefs().textFont));
	currDoc->AllFonts->findFont(tmpf, currDoc);
		vg.charStyle().setFont((*currDoc->AllFonts)[tmpf]);
		vg.charStyle().setFontSize(qRound(ScCLocale::toDoubleC(pg.attribute("FONTSIZE"), 12.0) * 10.0));
		vg.setHasDropCap(static_cast<bool>(pg.attribute("DROP", "0").toInt()));
		vg.setDropCapLines(pg.attribute("DROPLIN", "2").toInt());
		vg.setDropCapOffset(ScCLocale::toDoubleC(pg.attribute("DROPDIST"), 0.0));
		vg.charStyle().setFeatures(static_cast<StyleFlag>(pg.attribute("EFFECT", "0").toInt()).featureList());
		vg.charStyle().setFillColor(pg.attribute("FCOLOR", currDoc->itemToolPrefs().shapeFillColor));
		vg.charStyle().setFillShade(pg.attribute("FSHADE", "100").toInt());
		vg.charStyle().setStrokeColor(pg.attribute("SCOLOR", currDoc->itemToolPrefs().shapeLineColor));
		vg.charStyle().setStrokeShade(pg.attribute("SSHADE", "100").toInt());
		if (static_cast<bool>(pg.attribute("BASE", "0").toInt()))
			vg.setLineSpacingMode(ParagraphStyle::BaselineGridLineSpacing);
		vg.charStyle().setShadowXOffset(qRound(ScCLocale::toDoubleC(pg.attribute("TXTSHX"), 5.0)) * 10);
		vg.charStyle().setShadowYOffset(qRound(ScCLocale::toDoubleC(pg.attribute("TXTSHY"), -5.0)) * 10);
		vg.charStyle().setOutlineWidth(qRound(ScCLocale::toDoubleC(pg.attribute("TXTOUT"), 1.0)) * 10);
		vg.charStyle().setUnderlineOffset(qRound(ScCLocale::toDoubleC(pg.attribute("TXTULP"), -0.1)) * 10);
		vg.charStyle().setUnderlineWidth(qRound(ScCLocale::toDoubleC(pg.attribute("TXTULW"), -0.1)) * 10);
		vg.charStyle().setStrikethruOffset(qRound(ScCLocale::toDoubleC(pg.attribute("TXTSTP"), -0.1)) * 10);
		vg.charStyle().setStrikethruWidth(qRound(ScCLocale::toDoubleC(pg.attribute("TXTSTW"), -0.1)) * 10);
		vg.charStyle().setScaleH(qRound(ScCLocale::toDoubleC(pg.attribute("SCALEH"), 100.0)) * 10);
		vg.charStyle().setScaleV(qRound(ScCLocale::toDoubleC(pg.attribute("SCALEV"), 100.0)) * 10);
		vg.charStyle().setBaselineOffset(qRound(ScCLocale::toDoubleC(pg.attribute("BASEO"), 0.0)) * 10);
		vg.charStyle().setTracking(qRound(ScCLocale::toDoubleC(pg.attribute("KERN"), 0.0)) * 10);
		if ((pg.hasAttribute("NUMTAB")) && (pg.attribute("NUMTAB", "0").toInt() != 0))
		{
			QList<ParagraphStyle::TabRecord> tbs;
			ParagraphStyle::TabRecord tb;
			QString tmp = pg.attribute("TABS");
			QTextStream tgv(&tmp, QIODevice::ReadOnly);
			QString xf, xf2;
			for (int cxv = 0; cxv < pg.attribute("NUMTAB", "0").toInt(); cxv += 2)
			{
				tgv >> xf;
				tgv >> xf2;
				tb.tabPosition = ScCLocale::toDoubleC(xf2);
				tb.tabType = static_cast<int>(ScCLocale::toDoubleC(xf));
				tb.tabFillChar =  QChar();
				tbs.append(tb);
			}
			vg.setTabValues(tbs);
			tmp = "";
		}
		else
		{
			QList<ParagraphStyle::TabRecord> tbs;
			QDomNode IT = pg.firstChild();
			while(!IT.isNull())
			{
				QDomElement it = IT.toElement();
				if (it.tagName()=="Tabs")
				{
					ParagraphStyle::TabRecord tb;
					tb.tabPosition = ScCLocale::toDoubleC(it.attribute("Pos"));
					tb.tabType = it.attribute("Type").toInt();
					QString tbCh = "";
					tbCh = it.attribute("Fill","");
					if (tbCh.isEmpty())
						tb.tabFillChar = QChar();
					else
						tb.tabFillChar = tbCh[0];
					tbs.append(tb);
				}
				IT=IT.nextSibling();
			}
			vg.setTabValues(tbs);
		}
}

QImage FileLoader::ReadThumbnail(const QString& fileName)
{
	QList<FileFormat>::const_iterator it;
	if (findFormat(FileType, it))
		return (*it).readThumbnail(fileName);
	return QImage();
}

bool FileLoader::postLoad(ScribusDoc* currDoc)
{
	//CB #3749 We have to set these up in post load as each format will load into the doc itself
	//settings. As the hyphenator was created in the doc constructor, it needs to be updated.
	//FIXME: Remove these duplicate settings altogether
	if (currDoc->docHyphenator!=0)
	{
			currDoc->docHyphenator->Automatic=currDoc->hyphAutomatic();
			currDoc->docHyphenator->AutoCheck=currDoc->hyphAutoCheck();
			currDoc->docHyphenator->Language=currDoc->hyphLanguage();
			currDoc->docHyphenator->MinWordLen=currDoc->hyphMinimumWordLength();
			currDoc->docHyphenator->HyCount=currDoc->hyphConsecutiveLines();
	}
/*	for (uint d = 0; d < ScMW->doc->MasterItems.count(); ++d)
	{
		PageItem *it = ScMW->doc->MasterItems.at(d);
		if ((it->itemType() == PageItem::TextFrame) || (it->itemType() == PageItem::PathText))
		{
			for (uint e = 0; e < it->itemText.count(); ++e)
			{
				ScText *hl = it->itemText.at(e);
				if ((hl->ch == QChar(25)) && ((int)hl->cembedded != -1))
					hl->cembedded = ScMW->doc->FrameItems.at((int)hl->cembedded);
				else
					hl->cembedded = 0;
			}
		}
	}
	for (uint d = 0; d < ScMW->doc->DocItems.count(); ++d)
	{
		PageItem *it = ScMW->doc->DocItems.at(d);
		if ((it->itemType() == PageItem::TextFrame) || (it->itemType() == PageItem::PathText))
		{
			for (uint e = 0; e < it->itemText.count(); ++e)
			{
				ScText *hl = it->itemText.at(e);
				if ((hl->ch == QChar(25)) && ((int)hl->cembedded != -1))
					hl->cembedded = ScMW->doc->FrameItems.at((int)hl->cembedded);
				else
					hl->cembedded = 0;
			}
		}
	} */
	//FIXME: this goes into scribusdoc
	//ResourceCollection rsc;
	//currDoc->getNamedResources(rsc);
	//ReplacedFonts.clear();
	//QMap<QString,QString>::ConstIterator it;
	//for (it = rsc.fonts().begin(); it != rsc.fonts().end(); ++it)
	//{
	//	const ScFace& font (rsc.availableFonts->findFont(it.key(), currDoc));
	//	if (font.isReplacement() )
	//		ReplacedFonts[it.key()] = font.replacementName();
	//}
		ReplacedFonts = currDoc->AllFonts->getSubstitutions(ReplacedFonts.keys());
	if (ReplacedFonts.count() != 0)
	{
		if ((prefsManager->appPrefs.fontPrefs.askBeforeSubstitute))
		{
			qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
			FontReplaceDialog dia(0, &ReplacedFonts);
			if (dia.exec())
			{
				QMap<QString,QString>::Iterator itfsu;
				for (itfsu = ReplacedFonts.begin(); itfsu != ReplacedFonts.end(); ++itfsu)
				{
					if (dia.stickyReplacements->isChecked())
						prefsManager->appPrefs.fontPrefs.GFontSub[itfsu.key()] = itfsu.value();
				}
				currDoc->AllFonts->setSubstitutions(ReplacedFonts, currDoc);
				ResourceCollection repl;
				repl.availableFonts = currDoc->AllFonts;
				repl.mapFonts(ReplacedFonts);
				currDoc->replaceNamedResources(repl);
				return true;
			}
			else
			{
				return false;
			}
		}
		/*
		for (uint d = 0; d < currDoc->MasterItems.count(); ++d)
		{
			replaceFonts(currDoc, currDoc->MasterItems.at(d), currDoc->UsedFonts, ReplacedFonts);
		}
		for (uint d = 0; d < currDoc->DocItems.count(); ++d)
		{
			replaceFonts(currDoc, currDoc->DocItems.at(d), currDoc->UsedFonts, ReplacedFonts);
		}
		for (uint d = 0; d < currDoc->FrameItems.count(); ++d)
		{
			replaceFonts(currDoc, currDoc->FrameItems.at(d), currDoc->UsedFonts, ReplacedFonts);
		}
		for (uint a = 0; a < currDoc->paragraphStyles().count(); ++a)
		{
			if ( !currDoc->paragraphStyles()[a].charStyle().font().isNone() 
				 && !currDoc->UsedFonts.contains(currDoc->paragraphStyles()[a].charStyle().font().scName()))
			{
//				currDoc->paragraphStyles()[a].charStyle().setFont
//					((*currDoc->AllFonts)[ReplacedFonts[currDoc->paragraphStyles()[a].charStyle().font().scName()]]);
			}
		}
		QList<QString> tmpList;
		tmpList.clear();
		for (int fe = 0; fe < currDoc->PDF_Options.EmbedList.count(); ++fe)
		{
			if (ReplacedFonts.contains(currDoc->PDF_Options.EmbedList[fe]))
				tmpList.append(ReplacedFonts[currDoc->PDF_Options.EmbedList[fe]]);
			else
				tmpList.append(currDoc->PDF_Options.EmbedList[fe]);
		}
		currDoc->PDF_Options.EmbedList = tmpList;
		tmpList.clear();
		for (int fe = 0; fe < currDoc->PDF_Options.SubsetList.count(); ++fe)
		{
			if (ReplacedFonts.contains(currDoc->PDF_Options.SubsetList[fe]))
				tmpList.append(ReplacedFonts[currDoc->PDF_Options.SubsetList[fe]]);
			else
				tmpList.append(currDoc->PDF_Options.SubsetList[fe]);
		}
		currDoc->PDF_Options.SubsetList = tmpList;
		QMap<QString,QString>::Iterator itfsu;
		for (itfsu = ReplacedFonts.begin(); itfsu != ReplacedFonts.end(); ++itfsu)
		{
			if (!currDoc->UsedFonts.contains(itfsu.data()))
			{
//				QFont fo = prefsManager->appPrefs.AvailFonts[itfsu.data()]->Font;
//				fo.setPointSize(qRound(ScMW->doc->toolSettings.textSize / 10.0));
				currDoc->AddFont(itfsu.data(), qRound(currDoc->toolSettings.textSize / 10.0));
			}
		}
		if (prefsManager->appPrefs.askBeforeSubstituite)
			ReplacedFonts.clear();
		dummyScFaces.clear();
		*/
	}

	return true;
}

void FileLoader::informReplacementFonts()
{
	if (ReplacedFonts.count() != 0)
	{
		qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
		QString mess = tr("Some fonts used by this document have been substituted:")+"\n\n";
		QMap<QString,QString>::Iterator it;
		for (it = ReplacedFonts.begin(); it != ReplacedFonts.end(); ++it)
		{
			mess += it.key() + tr(" was replaced by: ")+ it.value() +"\n";
		}
		QMessageBox::warning(ScCore->primaryMainWindow(), CommonStrings::trWarning, mess, 1, 0, 0);
	}
}

bool FileLoader::findFormat(uint formatId, QList<FileFormat>::const_iterator &it)
{
	QList<FileFormat> fileFormats(LoadSavePlugin::supportedFormats());
	it=fileFormats.constBegin();
	QList<FileFormat>::const_iterator itEnd(fileFormats.constEnd());
	for ( ; it != itEnd ; ++it )
	{
		if (formatId==(*it).formatId)
			return true;
	}
	return false;
}
