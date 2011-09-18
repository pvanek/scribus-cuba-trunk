/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          scribus.cpp  -  description
                             -------------------
    begin                : Fre Apr  6 21:09:31 CEST 2001
    copyright            : (C) 2001 by Franz Schmid
    email                : Franz.Schmid@altmuehlnet.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QAction>
#include <QApplication>
#include <QByteArray>
#include <QCloseEvent>
#include <QColor>
#include <QColorDialog>
#include <QCursor>
#include <QDesktopWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QEventLoop>
#include <QFileDialog>
#include <QFrame>
#include <QFont>
#include <QIcon>
#include <QInputDialog>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QList>
#include <QLocale>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPixmap>
#include <QProgressBar>
#include <QRegExp>
#include <QStyleFactory>
#include <QTableWidget>
#include <QTextCodec>
#include <QToolButton>
#include <QTranslator>
#include <QWheelEvent>

#ifdef DEBUG_LOAD_TIMES
#include <QDebug>
#include <QTime>
#include <sys/times.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include "scconfig.h"

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <iostream>
#include <sstream>
#include <signal.h>
#include <string>


#include "actionmanager.h"
#include "canvasmode.h"
#include "commonstrings.h"
#include "desaxe/digester.h"
#include "desaxe/saxXML.h"
#include "desaxe/simple_actions.h"
#include "docinfo.h"
#include "documentchecker.h"
#include "documentinformation.h"
#include "fileloader.h"
#include "filewatcher.h"
#include "fpoint.h"
#include "fpointarray.h"
#include "gtgettext.h"
#include "hyphenator.h"
#include "langmgr.h"
#include "pageitem_imageframe.h"
#include "pageitem_latexframe.h"
#include "pageitem_textframe.h"
#include "pagesize.h"
#include "pdflib.h"
#include "pdfoptions.h"
#include "pluginmanager.h"
#include "plugins/formatidlist.h"
#include "prefscontext.h"
#include "prefsfile.h"
#include "prefsmanager.h"
#include "prefstable.h"
#include "pslib.h"
#include "resourcecollection.h"
#include "sccolorengine.h"
#include "scgtplugin.h"
#include "scmimedata.h"
#include "scpage.h"
#include "scpaths.h"
#include "scprintengine_ps.h"
#include "scraction.h"
#include "scribus.h"
#include "scribusdoc.h"
#include "scribusXml.h"
#include "scribusapp.h"
#include "scribuscore.h"
#include "scribusview.h"
#include "scribuswin.h"
#include "selection.h"
#include "serializer.h"
#include "ui/about.h"
#include "ui/aboutplugins.h"
#include "ui/aligndistribute.h"
#include "ui/annot.h"
#include "ui/annota.h"
#include "ui/applytemplatedialog.h"
#include "ui/arrowchooser.h"
#include "ui/autoform.h"
#include "ui/basepointwidget.h"
#include "ui/bookmarkpalette.h"
#include "ui/charselect.h"
#include "ui/checkDocument.h"
#include "ui/collectforoutput_ui.h"
#include "ui/colorcombo.h"
#include "ui/contextmenu.h"
#include "ui/cpalette.h"
#include "ui/customfdialog.h"
#include "ui/delpages.h"
#include "ui/effectsdialog.h"
#include "ui/fontcombo.h"
#include "ui/guidemanager.h"
#include "ui/helpbrowser.h"
#include "ui/hruler.h"
#include "ui/imageinfodialog.h"
#include "ui/insertaframe.h"
#include "ui/inspage.h"
#include "ui/javadocs.h"
#include "langmgr.h"
#include "ui/layers.h"
#include "ui/loremipsum.h"
#include "ui/marginwidget.h"
#include "ui/margindialog.h"
#include "ui/masterpagepalette.h"
#include "ui/mergedoc.h"
#include "ui/movepage.h"
#include "ui/multipleduplicate.h"
#include "ui/newfile.h"
#include "ui/newtemp.h"
#include "nfttemplate.h"
#include "ui/nftdialog.h"
#include "ui_nftdialog.h"
#include "ui/nftwidget.h"
#include "ui/nodeeditpalette.h"
#ifdef HAVE_OSG
	#include "ui/osgeditor.h"
#endif
#include "ui/outlinepalette.h"
#include "ui/pageitemattributes.h"
#include "ui/pagelayout.h"
#include "ui/pagepalette.h"
#include "ui/pageselector.h"
#include "ui/paintmanager.h"
#include "ui/pdfopts.h"
#include "ui/picstatus.h"
#include "ui/polygonwidget.h"
#include "ui/preferencesdialog.h"
#include "ui/preview.h"
#include "ui/printdialog.h"
#include "ui/propertiespalette.h"
#include "ui/propertiespalette_image.h"
#include "ui/propertiespalette_line.h"
#include "ui/propertiespalette_shape.h"
#include "ui/propertiespalette_text.h"
#include "ui/propertiespalette_xyz.h"
#include "ui/query.h"
#include "ui/replacecolors.h"
#include "ui/sccombobox.h"
#include "ui/scmessagebox.h"
#include "ui/scrapbookpalette.h"
#include "ui/scmwmenumanager.h"
#include "ui/selectobjects.h"
#include "ui/search.h"
#include "ui/smlinestyle.h"
#include "ui/smtextstyles.h"
#include "ui/splash.h"
#include "ui/storyeditor.h"
#include "ui/stylemanager.h"
#include "ui/symbolpalette.h"
#include "ui/tabmanager.h"
#include "text/nlsconfig.h"
#include "tocgenerator.h"
#include "ui/transformdialog.h"
#include "ui/transparencypalette.h"
#include "ui/copypagetomasterpagedialog.h"
#include "ui/edittoolbar.h"
#include "ui/filetoolbar.h"
#include "ui/modetoolbar.h"
#include "ui/pdftoolbar.h"
#include "undogui.h"
#include "undomanager.h"
#include "undostate.h"
#include "units.h"
#include "urllauncher.h"
#include "util.h"
#include "util_formats.h"
#include "util_ghostscript.h"
#include "util_icon.h"
#include "util_math.h"
#include "ui/vruler.h"
#include "loadsaveplugin.h"
#include "plugins/formatidlist.h"
#include "scimagecachemanager.h"

#if defined(_WIN32)
#include "scdocoutput_ps2.h"
#include "scprintengine_gdi.h"
#endif

#include "sclimits.h"

using namespace std;

bool previewDinUse;
bool printDinUse;

QString DocDir;


extern ScribusQApp* ScQApp;
extern bool emergencyActivated;


ScribusMainWindow::ScribusMainWindow()
{
	actionManager=0;
	scrMenuMgr=0;
	prefsManager=0;
	formatsManager=0;
	UrlLauncher::instance();
	mainWindowStatusLabel=0;
	ExternalApp=0;
#ifdef Q_WS_MAC
	noIcon = loadIcon("noicon.xpm");
#endif
}

/*
static QCoreApplication::EventFilter origEventFilter = 0;

bool reportFocusChanges(void *message, long *result)
{
	unsigned* data = static_cast<unsigned*>(message);
	if (QApplication::focusWidget())
		qDebug() << QApplication::applicationFilePath() << reinterpret_cast<void*>(QApplication::focusWidget()) << typeid(*QApplication::focusWidget()).name() << QApplication::focusWidget()->objectName() << message << data[0] << data[1] << data[2] << data[3] << data[4] << data[5] << data[6] << data[7];
	else
		qDebug() << QApplication::applicationFilePath() << "no focus" << message << data[0] << data[1] << data[2] << data[3] << data[4] << data[5] << data[6] << data[7];
	return origEventFilter && origEventFilter(message, result);
}
*/

/*
 * retval 0 - ok, 1 - no fonts, ...
 */
int ScribusMainWindow::initScMW(bool primaryMainWindow)
{
	int retVal=0;

	QByteArray stylesheet;
	if (loadRawText(ScPaths::getApplicationDataDir() + "/stylesheet.css", stylesheet))
	{
		qApp->setStyleSheet(QString(stylesheet));
	}

//	origEventFilter = qApp->setEventFilter(reportFocusChanges);

	previewDinUse = false;
	printDinUse = false;
	internalCopy = false;
	m_doc = new ScribusDoc();
	m_doc->setup(0, 1, 1, 1, 1, "Custom", "Custom");
	m_doc->setPage(100, 100, 0, 0, 0, 0, 0, 0, false, false);
	m_doc->addPage(0);
	m_doc->setGUI(false, this, 0);
	CurrStED = NULL;
	setWindowTitle( tr("Scribus " VERSION));
	setAttribute(Qt::WA_KeyCompression, false);
	setAttribute(Qt::WA_InputMethodEnabled, true);
	setWindowIcon(loadIcon("AppIcon.png"));
	setObjectName("MainWindow");
	scrActionGroups.clear();
	scrActions.clear();
	scrRecentFileActions.clear();
	scrRecentPasteActions.clear();
	scrWindowsActions.clear();
	scrLayersActions.clear();
	scrScrapActions.clear();
	scrMenuMgr = new ScMWMenuManager(menuBar());
	prefsManager = PrefsManager::instance();
	formatsManager = FormatsManager::instance();
	objectSpecificUndo = false;

	undoManager = UndoManager::instance();
	PrefsContext *undoPrefs = prefsManager->prefsFile->getContext("undo");
	undoManager->setUndoEnabled(undoPrefs->getBool("enabled", true));
	tocGenerator = new TOCGenerator();


	initDefaultValues();

	initStatusBar();

	qApp->processEvents();

	actionManager = new ActionManager(this);
	actionManager->init(this);
//	if (primaryMainWindow)
//		ScCore->setSplashStatus( tr("Applying User Shortcuts") );
//	prefsManager->applyLoadedShortCuts();
//	initKeyboardShortcuts();
	initMenuBar();
	initToolBars();
 	ScCore->pluginManager->setupPluginActions(this);
	ScCore->pluginManager->enableOnlyStartupPluginActions(this);
 	ScCore->pluginManager->languageChange();
	if (primaryMainWindow)
		ScCore->setSplashStatus( tr("Applying User Shortcuts") );
	prefsManager->applyLoadedShortCuts();
	initKeyboardShortcuts();

	resize(610, 600);
	mdiArea = new QMdiArea(this);
	mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	if (prefsManager->appPrefs.uiPrefs.useTabs)
		mdiArea->setViewMode(QMdiArea::TabbedView);
	else
		mdiArea->setViewMode(QMdiArea::SubWindowView);
	setCentralWidget( mdiArea );
	connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(newActWin(QMdiSubWindow *)));
	//Connect windows cascade and tile actions to the workspace after its created. Only depends on mdiArea created.
	connect( scrActions["windowsCascade"], SIGNAL(triggered()) , mdiArea, SLOT(cascadeSubWindows()) );
	connect( scrActions["windowsTile"], SIGNAL(triggered()) , mdiArea, SLOT(tileSubWindows()) );
	initPalettes();

	prefsManager->setupMainWindow(this);

	if (primaryMainWindow)
		ScCore->setSplashStatus( tr("Initializing Story Editor") );
	storyEditor = new StoryEditor(this);

	DocDir = prefsManager->documentDir();

	if (primaryMainWindow)
		ScCore->setSplashStatus( tr("Initializing Hyphenator") );
	QString preLang(prefsManager->appPrefs.hyphPrefs.Language);
	initHyphenator();
	if (!LanguageManager::instance()->getHyphFilename( preLang, false ).isEmpty() )
		prefsManager->appPrefs.hyphPrefs.Language = preLang;
	if (primaryMainWindow)
		ScCore->setSplashStatus( tr("Reading Scrapbook") );
	initScrapbook();

	scrActions["helpTooltips"]->setChecked(prefsManager->appPrefs.displayPrefs.showToolTips);
	scrActions["showMouseCoordinates"]->setChecked(prefsManager->appPrefs.displayPrefs.showMouseCoordinates);
	scrActions["stickyTools"]->setChecked(prefsManager->appPrefs.uiPrefs.stickyTools);
	ToggleTips();
	ToggleMouseTips();
	propertiesPalette->setFontSize();
	if (scrActions["SaveAsDocumentTemplate"])
		scrActions["SaveAsDocumentTemplate"]->setEnabled(false);

	connect(ScCore->fileWatcher, SIGNAL(fileDeleted(QString )), this, SLOT(removeRecentFromWatcher(QString)));
//	connect(this, SIGNAL(TextStyle(const ParagraphStyle&)), propertiesPalette, SLOT(updateStyle(const ParagraphStyle&)));
//	connect(this, SIGNAL(TextEffects(int)), propertiesPalette, SLOT(setStil(int)));
	connect(ClipB, SIGNAL(dataChanged()), this, SLOT(ClipChange()));
//	connect(ClipB, SIGNAL(selectionChanged()), this, SLOT(ClipChange()));
	setAcceptDrops(true);
	QCoreApplication::instance()->installEventFilter(this);
	scrActions["toolsSelect"]->setChecked(true);
	ColorSetManager csm;
	csm.findPaletteLocations();
	csm.findPalettes();
	csm.findUserPalettes();
	QString Cpfad = QDir::toNativeSeparators(ScPaths::getApplicationDataDir())+"DefaultColors.xml";
	QFile fc(Cpfad);
	if (fc.exists())
		csm.loadPalette(Cpfad, m_doc, prefsManager->appPrefs.colorPrefs.DColors, prefsManager->appPrefs.defaultGradients, prefsManager->appPrefs.defaultPatterns, false);
	else
	{
		if (prefsManager->appPrefs.colorPrefs.DColorSet != "Scribus Small")
		{
			QStringList CustomColorSets = csm.userPaletteNames();
			if (CustomColorSets.contains(prefsManager->appPrefs.colorPrefs.DColorSet))
				Cpfad = csm.userPaletteFileFromName(prefsManager->appPrefs.colorPrefs.DColorSet);
			else
				Cpfad = csm.paletteFileFromName(prefsManager->appPrefs.colorPrefs.DColorSet);
			csm.loadPalette(Cpfad, m_doc, prefsManager->appPrefs.colorPrefs.DColors, prefsManager->appPrefs.defaultGradients, prefsManager->appPrefs.defaultPatterns, false);
		}
	}
	return retVal;
}


ScribusMainWindow::~ScribusMainWindow()
{
	delete m_doc;
}

void ScribusMainWindow::addScToolBar(ScToolBar *tb, QString name)
{
	if (!scrToolBars.contains(name))
		scrToolBars.insert(name, tb);
	addToolBar(tb);
}

void ScribusMainWindow::initToolBars()
{
	fileToolBar = new FileToolBar(this);
	editToolBar = new EditToolBar(this);
	UndoWidget* uWidget = new UndoWidget(editToolBar, "uWidget");
	undoManager->registerGui(uWidget);
	modeToolBar = new ModeToolBar(this);
	pdfToolBar = new PDFToolBar(this);

	addScToolBar(fileToolBar, fileToolBar->objectName());
	addScToolBar(editToolBar, editToolBar->objectName());
	addScToolBar(modeToolBar, modeToolBar->objectName());
	addScToolBar(pdfToolBar, pdfToolBar->objectName());
	connect(modeToolBar, SIGNAL(visibilityChanged(bool)), scrActions["toolsToolbarTools"], SLOT(setChecked(bool)));
	connect(scrActions["toolsToolbarPDF"], SIGNAL(toggled(bool)), pdfToolBar, SLOT(setVisible(bool)));
	connect(pdfToolBar, SIGNAL(visibilityChanged(bool)), scrActions["toolsToolbarPDF"], SLOT(setChecked(bool)));
	connect(scrActions["toolsToolbarTools"], SIGNAL(toggled(bool)), modeToolBar, SLOT(setVisible(bool)) );
}


void ScribusMainWindow::initDefaultValues()
{
	HaveDoc = false;
	ScriptRunning = 0;
	view = NULL;
	doc = NULL;
	DocNr = 1;
	PrinterUsed = false;
	PDef.Pname = "";
	PDef.Dname = "";
	PDef.Command = "";
	keyrep = false;
	_arrowKeyDown = false;
	ClipB = QApplication::clipboard();
	palettesStatus[0] = false;
	guidesStatus[0] = false;
}


void ScribusMainWindow::initKeyboardShortcuts()
{
	for( QMap<QString, QPointer<ScrAction> >::Iterator it = scrActions.begin(); it!=scrActions.end(); ++it )
	{
		if ((ScrAction*)(it.value())!=NULL)
		{
			QString accelerator=it.value()->shortcut();
			prefsManager->setKeyEntry(it.key(), it.value()->cleanMenuText(), accelerator,0);
		}
		//else
		//	qDebug() << it.key();
		//qDebug() << QString("|-\n|%1||%2||%3").arg(it.key()).arg(it.value()->cleanMenuText()).arg(QString(it.data()->accel()));
	}
}


void ScribusMainWindow::initPalettes()
{
	//CB TODO hide the publicly available members of some palettes
	// these must be filtered too as they take control of the palettes events
	outlinePalette = new OutlinePalette(this);
	outlinePalette->setMainWindow(this);
	connect( scrActions["toolsOutline"], SIGNAL(toggled(bool)) , outlinePalette, SLOT(setPaletteShown(bool)) );
	connect( outlinePalette, SIGNAL(paletteShown(bool)), scrActions["toolsOutline"], SLOT(setChecked(bool)));

	propertiesPalette = new PropertiesPalette(this);
	propertiesPalette->setMainWindow(this);
	connect( scrActions["toolsProperties"], SIGNAL(toggled(bool)) , propertiesPalette, SLOT(setPaletteShown(bool)) );
	connect( propertiesPalette, SIGNAL(paletteShown(bool)), scrActions["toolsProperties"], SLOT(setChecked(bool)));
	//CB dont need this until we have a doc...
	//propertiesPalette->Cpal->setColors(prefsManager->colorSet());
	emit UpdateRequest(reqDefFontListUpdate);
	propertiesPalette->installEventFilter(this);
	nodePalette = new NodePalette(this);
	nodePalette->installEventFilter(this);
	layerPalette = new LayerPalette(this);
	guidePalette = new GuideManager(this);
	charPalette = new CharSelect(this);
	connect( scrActions["toolsLayers"], SIGNAL(toggled(bool)) , layerPalette, SLOT(setPaletteShown(bool)) );
	connect( layerPalette, SIGNAL(paletteShown(bool)), scrActions["toolsLayers"], SLOT(setChecked(bool)));
	layerPalette->installEventFilter(this);
	layerPalette->Table->installEventFilter(this);
	scrapbookPalette = new Biblio(this);
	connect( scrActions["toolsScrapbook"], SIGNAL(toggled(bool)) , scrapbookPalette, SLOT(setPaletteShown(bool)) );
	connect( scrapbookPalette, SIGNAL(paletteShown(bool)), scrActions["toolsScrapbook"], SLOT(setChecked(bool)));
	connect( scrapbookPalette, SIGNAL(pasteToActualPage(QString)), this, SLOT(pasteFromScrapbook(QString)));
	scrapbookPalette->installEventFilter(this);
	pagePalette = new PagePalette(this);
	connect( scrActions["toolsPages"], SIGNAL(toggled(bool)) , pagePalette, SLOT(setPaletteShown(bool)) );
	connect( scrActions["toolsPages"], SIGNAL(toggled(bool)) , this, SLOT(setPagePalette(bool)) );
	connect( pagePalette, SIGNAL(paletteShown(bool)), scrActions["toolsPages"], SLOT(setChecked(bool)));
	pagePalette->installEventFilter(this);
	bookmarkPalette = new BookPalette(this);
	connect( scrActions["toolsBookmarks"], SIGNAL(toggled(bool)) , bookmarkPalette, SLOT(setPaletteShown(bool)) );
	connect( bookmarkPalette, SIGNAL(paletteShown(bool)), scrActions["toolsBookmarks"], SLOT(setChecked(bool)));
	bookmarkPalette->installEventFilter(this);
//	measurementPalette = new Measurements(this);
//	connect( scrActions["toolsMeasurements"], SIGNAL(toggled(bool)) , measurementPalette, SLOT(setPaletteShown(bool)) );
	connect( scrActions["toolsMeasurements"], SIGNAL(toggledData(bool, int)) , this, SLOT(setAppModeByToggle(bool, int)) );
//	connect( measurementPalette, SIGNAL(paletteShown(bool)), scrActions["toolsMeasurements"], SLOT(setChecked(bool)));
//	measurementPalette->installEventFilter(this);
//	measurementPalette->hide();
	docCheckerPalette = new CheckDocument(this, false);
	connect( scrActions["toolsPreflightVerifier"], SIGNAL(toggled(bool)) , docCheckerPalette, SLOT(setPaletteShown(bool)) );
	connect( scrActions["toolsPreflightVerifier"], SIGNAL(toggled(bool)) , this, SLOT(docCheckToggle(bool)) );
	connect( docCheckerPalette, SIGNAL(paletteShown(bool)), scrActions["toolsPreflightVerifier"], SLOT(setChecked(bool)));
	connect( docCheckerPalette, SIGNAL(paletteShown(bool)), this, SLOT(docCheckToggle(bool)));
	docCheckerPalette->installEventFilter(this);
	docCheckerPalette->hide();

	alignDistributePalette = new AlignDistributePalette(this, "AlignDistributePalette", false);
	connect( scrActions["toolsAlignDistribute"], SIGNAL(toggled(bool)) , alignDistributePalette, SLOT(setPaletteShown(bool)) );
	connect( alignDistributePalette, SIGNAL(paletteShown(bool)), scrActions["toolsAlignDistribute"], SLOT(setChecked(bool)));
	connect( alignDistributePalette, SIGNAL(documentChanged()), this, SLOT(slotDocCh()));
	alignDistributePalette->installEventFilter(this);

	symbolPalette = new SymbolPalette(this);
	symbolPalette->setMainWindow(this);
	connect(scrActions["toolsSymbols"], SIGNAL(toggled(bool)), symbolPalette, SLOT(setPaletteShown(bool)));
	connect(symbolPalette, SIGNAL(paletteShown(bool)), scrActions["toolsSymbols"], SLOT(setChecked(bool)));
	connect(symbolPalette, SIGNAL(startEdit(QString)), this, SLOT(editSymbolStart(QString)));
	connect(symbolPalette, SIGNAL(endEdit()), this, SLOT(editSymbolEnd()));
	symbolPalette->installEventFilter(this);
	symbolPalette->hide();
	

	undoPalette = new UndoPalette(this, "undoPalette");
	undoPalette->installEventFilter(this);
	undoManager->registerGui(undoPalette);
	connect(undoPalette, SIGNAL(paletteShown(bool)), this, SLOT(setUndoPalette(bool)));
	connect(undoPalette, SIGNAL(objectMode(bool)), this, SLOT(setUndoMode(bool)));

	// initializing style manager here too even it's not strictly a palette
	styleManager = new StyleManager(this, "styleManager");
	styleManager->addStyle(new SMLineStyle());
	SMCharacterStyle *tmpCS = new SMCharacterStyle();
	styleManager->addStyle(new SMParagraphStyle(tmpCS->tmpStyles()));
	styleManager->addStyle(tmpCS);
	connect( scrActions["editStyles"], SIGNAL(toggled(bool)), styleManager, SLOT(setPaletteShown(bool)) );
	connect( styleManager, SIGNAL(paletteShown(bool)), scrActions["editStyles"], SLOT(setChecked(bool)));
	styleManager->installEventFilter(this);

//	connect(docCheckerPalette, SIGNAL(selectElement(int, int)), this, SLOT(selectItemsFromOutlines(int, int)));
	connect(docCheckerPalette, SIGNAL(selectElementByItem(PageItem *, bool)), this, SLOT(selectItemsFromOutlines(PageItem *, bool)));
	connect(docCheckerPalette, SIGNAL(selectPage(int)), this, SLOT(selectPagesFromOutlines(int)));
	connect(docCheckerPalette, SIGNAL(selectMasterPage(QString)), this, SLOT(manageMasterPages(QString)));
//	connect(outlinePalette, SIGNAL(selectElement(int, int, bool)), this, SLOT(selectItemsFromOutlines(int, int, bool)));
	connect(outlinePalette, SIGNAL(selectElementByItem(PageItem *, bool)), this, SLOT(selectItemsFromOutlines(PageItem *, bool)));
	connect(outlinePalette, SIGNAL(selectPage(int)), this, SLOT(selectPagesFromOutlines(int)));
	connect(outlinePalette, SIGNAL(selectMasterPage(QString)), this, SLOT(manageMasterPages(QString)));
	connect(propertiesPalette->textPal->paraStyleCombo, SIGNAL(newStyle(const QString&)), this, SLOT(setNewParStyle(const QString&)));
	connect(propertiesPalette->textPal->charStyleCombo, SIGNAL(newStyle(const QString&)), this, SLOT(setNewCharStyle(const QString&)));
//	connect(propertiesPalette, SIGNAL(EditLSt()), this, SLOT(slotEditLineStyles()));
//	connect(nodePalette, SIGNAL(paletteClosed()), propertiesPalette, SLOT(endEdit2()));
	connect(nodePalette, SIGNAL(paletteClosed()), this, SLOT(slotSelect()));
	connect(nodePalette, SIGNAL(DocChanged()), this, SLOT(slotDocCh()));
	connect(layerPalette, SIGNAL(LayerChanged()), this, SLOT(showLayer()));

	connect(bookmarkPalette->BView, SIGNAL(MarkMoved()), this, SLOT(StoreBookmarks()));
	connect(bookmarkPalette->BView, SIGNAL(changed()), this, SLOT(slotDocCh()));
	connect(bookmarkPalette->BView, SIGNAL(SelectElement(PageItem *, bool)), this, SLOT(selectItemsFromOutlines(PageItem *, bool)));
	// guides
	connect(scrActions["pageManageGuides"], SIGNAL(toggled(bool)), guidePalette, SLOT(setPaletteShown(bool)));
	connect(guidePalette, SIGNAL(paletteShown(bool)), scrActions["pageManageGuides"], SLOT(setChecked(bool)));
	// char palette
	connect(scrActions["insertGlyph"], SIGNAL(toggled(bool)), charPalette, SLOT(setPaletteShown(bool)));
	connect(charPalette, SIGNAL(paletteShown(bool)), scrActions["insertGlyph"], SLOT(setChecked(bool)));
}


void ScribusMainWindow::initScrapbook()
{
	QString scrapbookFileO = QDir::toNativeSeparators(prefsManager->preferencesLocation()+"/scrap13.scs");
	QFileInfo scrapbookFileInfoO = QFileInfo(scrapbookFileO);
	if (scrapbookFileInfoO.exists())
	{
		scrapbookPalette->readOldContents(scrapbookFileO, QDir::toNativeSeparators(prefsManager->preferencesLocation()+"/scrapbook/main"));
		QDir d = QDir();
		d.rename(scrapbookFileO, QDir::toNativeSeparators(prefsManager->preferencesLocation()+"/scrap13.backup"));
	}
	QString scrapbookTemp = QDir::toNativeSeparators(prefsManager->preferencesLocation()+"/scrapbook/tmp");
	QFileInfo scrapbookTempInfo = QFileInfo(scrapbookTemp);
	if (scrapbookTempInfo.exists())
		scrapbookPalette->readTempContents(scrapbookTemp);
	QString scrapbookFile = QDir::toNativeSeparators(prefsManager->preferencesLocation()+"/scrapbook/main");
	QFileInfo scrapbookFileInfo = QFileInfo(scrapbookFile);
	if (scrapbookFileInfo.exists())
		scrapbookPalette->readContents(scrapbookFile);
	scrapbookPalette->setScrapbookFileName(scrapbookFile);
	ScCore->fileWatcher->addDir(scrapbookFile, true);
	scrapbookPalette->setOpenScrapbooks(prefsManager->appPrefs.scrapbookPrefs.RecentScrapbooks);
	rebuildRecentPasteMenu();
	connect(scrapbookPalette, SIGNAL(updateRecentMenue()), this, SLOT(rebuildRecentPasteMenu()));
	connect(ScCore->fileWatcher, SIGNAL(dirChanged(QString )), scrapbookPalette, SLOT(reloadLib(QString )));
	connect(ScCore->fileWatcher, SIGNAL(dirDeleted(QString )), scrapbookPalette, SLOT(closeOnDel(QString )));
}


bool ScribusMainWindow::warningVersion(QWidget *parent)
{
	bool retval = false;
	int t = ScMessageBox::warning(parent, QObject::tr("Scribus Development Version"), "<qt>" +
								 QObject::tr("You are running a development version of Scribus 1.3.x. The document you are working with was created in Scribus 1.2.x.  Saving the current file under 1.3.x renders it unable to be edited in Scribus 1.2.x versions. To preserve the ability to edit in 1.2.x, save this file under a different name and further edit the newly named file and the original will be untouched. Are you sure you wish to proceed with this operation?") + "</qt>",
								 QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if (t == QMessageBox::Ok)
		retval = true;
	return retval;
}


void ScribusMainWindow::initMenuBar()
{
	RecentDocs.clear();
	scrMenuMgr->createMenu("File", ActionManager::defaultMenuNameEntryTranslated("File"));
	scrMenuMgr->addMenuItem(scrActions["fileNew"], "File", true);
	scrMenuMgr->addMenuItem(scrActions["fileNewFromTemplate"], "File", true);
	scrMenuMgr->addMenuItem(scrActions["fileOpen"], "File", true);
	scrMenuMgr->createMenu("FileOpenRecent", tr("Open &Recent"), "File");
	scrMenuMgr->addMenuSeparator("File");
	scrMenuMgr->addMenuItem(scrActions["fileClose"], "File", false);
	scrMenuMgr->addMenuItem(scrActions["fileSave"], "File", false);
	scrMenuMgr->addMenuItem(scrActions["fileSaveAs"], "File", false);
	scrMenuMgr->addMenuItem(scrActions["fileRevert"], "File", false);
	scrMenuMgr->addMenuItem(scrActions["fileCollect"], "File", false);
	scrMenuMgr->addMenuSeparator("File");
	scrMenuMgr->createMenu("FileImport", tr("&Import"), "File");
	scrMenuMgr->addMenuItem(scrActions["fileImportText"], "FileImport", false);
// 	scrMenuMgr->addMenuItem(scrActions["fileImportText2"], "FileImport", false);
	scrMenuMgr->addMenuItem(scrActions["fileImportAppendText"], "FileImport", false);
	scrMenuMgr->addMenuItem(scrActions["fileImportImage"], "FileImport", false);
	scrMenuMgr->addMenuItem(scrActions["fileImportVector"], "FileImport", true);

	scrMenuMgr->createMenu("FileExport", tr("&Export"), "File");
	scrMenuMgr->addMenuItem(scrActions["fileExportText"], "FileExport", false);
	scrMenuMgr->addMenuItem(scrActions["fileExportAsEPS"], "FileExport", false);
	scrMenuMgr->addMenuItem(scrActions["fileExportAsPDF"], "FileExport", false);
	scrMenuMgr->addMenuSeparator("File");
//	scrMenuMgr->addMenuItem(scrActions["fileDocSetup"], "File", false);
	scrMenuMgr->addMenuItem(scrActions["fileDocSetup150"], "File", false);
//	scrMenuMgr->addMenuItem(scrActions["filePreferences"], "File", true);
	scrMenuMgr->addMenuItem(scrActions["filePreferences150"], "File", true);
	scrMenuMgr->addMenuSeparator("File");
	scrMenuMgr->addMenuItem(scrActions["filePrint"], "File", false);
	scrMenuMgr->addMenuItem(scrActions["PrintPreview"], "File", false);
	scrMenuMgr->addMenuSeparator("File");
	scrMenuMgr->addMenuItem(scrActions["fileQuit"], "File", true);

	scrMenuMgr->setMenuEnabled("FileImport", false);
	scrMenuMgr->setMenuEnabled("FileExport", false);

	scrMenuMgr->createMenu("Edit", ActionManager::defaultMenuNameEntryTranslated("Edit"));
	scrMenuMgr->addMenuItem(scrActions["editUndoAction"], "Edit", false);
	scrMenuMgr->addMenuItem(scrActions["editRedoAction"], "Edit", false);
	scrMenuMgr->addMenuItem(scrActions["editActionMode"], "Edit", true);
	scrMenuMgr->addMenuSeparator("Edit");
	scrMenuMgr->addMenuItem(scrActions["editCut"], "Edit", false);
	scrMenuMgr->addMenuItem(scrActions["editCopy"], "Edit", false);
	scrMenuMgr->addMenuItem(scrActions["editPaste"], "Edit", false);
	scrMenuMgr->createMenu("EditPasteRecent", tr("Paste Recent"), "Edit");
	scrMenuMgr->createMenu("EditContents", tr("Contents"), "Edit");
	scrMenuMgr->addMenuItem(scrActions["editCopyContents"], "EditContents", false);
	scrMenuMgr->addMenuItem(scrActions["editPasteContents"], "EditContents", false);
	scrMenuMgr->addMenuItem(scrActions["editPasteContentsAbs"], "EditContents", false);
	scrMenuMgr->addMenuItem(scrActions["editClearContents"], "EditContents", false);
	scrMenuMgr->addMenuSeparator("Edit");
	scrMenuMgr->addMenuItem(scrActions["editSelectAll"], "Edit", false);
	scrMenuMgr->addMenuItem(scrActions["editSelectAllOnLayer"], "Edit", false);
	scrMenuMgr->addMenuItem(scrActions["editDeselectAll"], "Edit", false);
	scrMenuMgr->addMenuSeparator("Edit");
	scrMenuMgr->addMenuItem(scrActions["editSearchReplace"], "Edit" , false);
	scrMenuMgr->addMenuItem(scrActions["toolsEditWithStoryEditor"], "Edit", false);
	scrMenuMgr->addMenuItem(scrActions["editEditWithImageEditor"], "Edit", false);
	scrMenuMgr->addMenuItem(scrActions["editEditRenderSource"], "Edit", false);
	scrMenuMgr->addMenuSeparator("Edit");
	scrMenuMgr->addMenuItem(scrActions["editColors"], "Edit", true);
	scrMenuMgr->addMenuItem(scrActions["editReplaceColors"], "Edit", false);
//	scrMenuMgr->addMenuItem(scrActions["editGradients"], "Edit", false);
//	scrMenuMgr->addMenuItem(scrActions["editPatterns"], "Edit", false);
	scrMenuMgr->addMenuItem(scrActions["editStyles"], "Edit", false);
	scrMenuMgr->addMenuItem(scrActions["editMasterPages"], "Edit", false);
	scrMenuMgr->addMenuItem(scrActions["editJavascripts"], "Edit", false);
	scrMenuMgr->setMenuEnabled("EditPasteRecent", false);
	scrMenuMgr->setMenuEnabled("EditContents", false);



//	scrActions["itemDuplicate"]->setEnabled(false);
//	scrActions["itemMulDuplicate"]->setEnabled(false);
//	scrActions["itemDelete"]->setEnabled(false);
//	scrActions["itemRaise"]->setEnabled(false);
//	scrActions["itemLower"]->setEnabled(false);
//	scrActions["itemRaiseToTop"]->setEnabled(false);
//	scrActions["itemLowerToBottom"]->setEnabled(false);
//	scrActions["itemSendToScrapbook"]->setEnabled(false);
//	scrActions["itemSendToPattern"]->setEnabled(false);
//	scrActions["itemAdjustFrameToImage"]->setEnabled(false);
//	scrActions["itemAdjustImageToFrame"]->setEnabled(false);
//	scrActions["itemExtendedImageProperties"]->setEnabled(false);
//	scrActions["itemUpdateImage"]->setEnabled(false);
//	scrActions["itemPreviewLow"]->setEnabled(false);
//	scrActions["itemPreviewNormal"]->setEnabled(false);
//	scrActions["itemPreviewFull"]->setEnabled(false);
//	scrActions["itemAttributes"]->setEnabled(false);
//	scrActions["itemPreviewLow"]->setEnabled(false);


	//Item Menu
	scrMenuMgr->createMenu("Item", ActionManager::defaultMenuNameEntryTranslated("Item"));
	scrMenuMgr->addMenuItem(scrActions["itemDuplicate"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["itemMulDuplicate"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["itemTransform"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["itemDelete"], "Item", false);
	scrMenuMgr->addMenuSeparator("Item");
	scrMenuMgr->addMenuItem(scrActions["itemGroup"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["itemUngroup"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["itemLock"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["itemLockSize"], "Item", false);
	scrMenuMgr->addMenuSeparator("Item");
	scrMenuMgr->createMenu("ItemLevel", tr("Level"));
	scrMenuMgr->addMenuToMenu("ItemLevel", "Item");
	scrMenuMgr->addMenuItem(scrActions["itemRaise"], "ItemLevel", false);
	scrMenuMgr->addMenuItem(scrActions["itemLower"], "ItemLevel", false);
	scrMenuMgr->addMenuItem(scrActions["itemRaiseToTop"], "ItemLevel", false);
	scrMenuMgr->addMenuItem(scrActions["itemLowerToBottom"], "ItemLevel", false);
	scrMenuMgr->createMenu("ItemLayer", tr("Send to La&yer"));
	scrMenuMgr->addMenuToMenu("ItemLayer", "Item");
	scrMenuMgr->createMenu("itemSendToScrapbook", tr("Send to Scrapbook"));
	scrMenuMgr->addMenuToMenu("itemSendToScrapbook", "Item");
//	scrMenuMgr->addMenuItem(scrActions["itemSendToScrapbook"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["itemSendToPattern"], "Item", false);
	scrMenuMgr->addMenuSeparator("Item");
	scrMenuMgr->addMenuItem(scrActions["itemAdjustFrameToImage"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["itemAdjustImageToFrame"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["itemUpdateImage"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["styleImageEffects"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["itemExtendedImageProperties"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["itemToggleInlineImage"], "Item", false);
	scrMenuMgr->createMenu("ItemPreviewSettings", tr("Preview Settings"), "Item");
	scrMenuMgr->addMenuItem(scrActions["itemImageIsVisible"], "ItemPreviewSettings", false);
	scrMenuMgr->addMenuSeparator("ItemPreviewSettings");
	scrMenuMgr->addMenuItem(scrActions["itemPreviewLow"], "ItemPreviewSettings", false);
	scrMenuMgr->addMenuItem(scrActions["itemPreviewNormal"], "ItemPreviewSettings", false);
	scrMenuMgr->addMenuItem(scrActions["itemPreviewFull"], "ItemPreviewSettings", false);
	scrMenuMgr->addMenuSeparator("Item");
	scrMenuMgr->addMenuItem(scrActions["itemAttributes"], "Item", false);
	scrMenuMgr->createMenu("ItemPDFOptions", tr("&PDF Options"));
	scrMenuMgr->addMenuToMenu("ItemPDFOptions", "Item");
	scrMenuMgr->addMenuItem(scrActions["itemPDFIsAnnotation"], "ItemPDFOptions", false);
	scrMenuMgr->addMenuItem(scrActions["itemPDFIsBookmark"], "ItemPDFOptions", false);
	scrMenuMgr->addMenuItem(scrActions["itemPDFAnnotationProps"], "ItemPDFOptions", false);
	scrMenuMgr->addMenuItem(scrActions["itemPDFFieldProps"], "ItemPDFOptions", false);
	scrMenuMgr->createMenu("ItemConvertTo", tr("C&onvert To"), "Item");
	scrMenuMgr->addMenuItem(scrActions["itemConvertToBezierCurve"], "ItemConvertTo", false);
	scrMenuMgr->addMenuItem(scrActions["itemConvertToImageFrame"], "ItemConvertTo", false);
	scrMenuMgr->addMenuItem(scrActions["itemConvertToOutlines"], "ItemConvertTo", false);
	scrMenuMgr->addMenuItem(scrActions["itemConvertToPolygon"], "ItemConvertTo", false);
	scrMenuMgr->addMenuItem(scrActions["itemConvertToTextFrame"], "ItemConvertTo", false);
	scrMenuMgr->addMenuSeparator("Item");
	scrMenuMgr->addMenuItem(scrActions["toolsLinkTextFrame"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["toolsUnlinkTextFrame"], "Item", false);
	scrMenuMgr->addMenuSeparator("Item");
	scrMenuMgr->addMenuItem(scrActions["itemAttachTextToPath"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["itemDetachTextFromPath"], "Item", false);
//	scrMenuMgr->createMenu("ItemPathOps", tr("Path Tools"), "Item");
	scrMenuMgr->addMenuItem(scrActions["itemCombinePolygons"], "Item", false);
	scrMenuMgr->addMenuItem(scrActions["itemSplitPolygons"], "Item", false);

	scrActions["itemPrintingEnabled"]->setEnabled(false);
	scrMenuMgr->setMenuEnabled("ItemConvertTo", false);


	//Insert menu
	scrMenuMgr->createMenu("Insert", ActionManager::defaultMenuNameEntryTranslated("Insert"));
	scrMenuMgr->addMenuItem(scrActions["insertFrame"], "Insert", false);
	scrMenuMgr->addMenuSeparator("Insert");
	scrMenuMgr->addMenuItem(scrActions["toolsInsertTextFrame"], "Insert", false);
	scrMenuMgr->addMenuItem(scrActions["toolsInsertImageFrame"], "Insert", false);
	scrMenuMgr->addMenuItem(scrActions["toolsInsertRenderFrame"], "Insert", false);
	scrMenuMgr->addMenuItem(scrActions["toolsInsertTableFrame"], "Insert", false);
	scrMenuMgr->addMenuItem(scrActions["toolsInsertShape"], "Insert", false);
	scrMenuMgr->addMenuItem(scrActions["toolsInsertPolygon"], "Insert", false);
	scrMenuMgr->addMenuItem(scrActions["toolsInsertArc"], "Insert", false);
	scrMenuMgr->addMenuItem(scrActions["toolsInsertSpiral"], "Insert", false);
	scrMenuMgr->addMenuItem(scrActions["toolsInsertLine"], "Insert", false);
	scrMenuMgr->addMenuItem(scrActions["toolsInsertBezier"], "Insert", false);
	scrMenuMgr->addMenuItem(scrActions["toolsInsertFreehandLine"], "Insert", false);
	scrMenuMgr->addMenuItem(scrActions["toolsInsertCalligraphicLine"], "Insert", false);
	scrMenuMgr->addMenuSeparator("Insert");
	scrMenuMgr->addMenuItem(scrActions["stickyTools"], "Insert", true);
	scrMenuMgr->addMenuSeparator("Insert");
	scrMenuMgr->addMenuItem(scrActions["insertGlyph"], "Insert", false);

	scrMenuMgr->createMenu("InsertChar", tr("&Character"), "Insert");
	scrMenuMgr->addMenuItem(scrActions["unicodePageNumber"], "InsertChar", false);
	scrMenuMgr->addMenuItem(scrActions["unicodePageCount"], "InsertChar", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeSoftHyphen"], "InsertChar", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeNonBreakingHyphen"], "InsertChar", false);
	scrMenuMgr->addMenuSeparator("InsertChar");
	scrMenuMgr->addMenuItem(scrActions["unicodeCopyRight"], "InsertChar", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeRegdTM"], "InsertChar", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeTM"], "InsertChar", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeSolidus"], "InsertChar", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeBullet"], "InsertChar", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeMidpoint"], "InsertChar", false);
	scrMenuMgr->addMenuSeparator("InsertChar");
	scrMenuMgr->addMenuItem(scrActions["unicodeDashEm"], "InsertChar", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeDashEn"], "InsertChar", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeDashFigure"], "InsertChar", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeDashQuotation"], "InsertChar", false);

	scrMenuMgr->createMenu("InsertQuote", tr("&Quote"), "Insert");
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteApostrophe"], "InsertQuote", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteStraight"], "InsertQuote", false);
	scrMenuMgr->addMenuSeparator("InsertQuote");
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteSingleLeft"], "InsertQuote", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteSingleRight"], "InsertQuote", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteDoubleLeft"], "InsertQuote", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteDoubleRight"], "InsertQuote", false);
	scrMenuMgr->addMenuSeparator("InsertQuote");
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteSingleReversed"], "InsertQuote", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteDoubleReversed"], "InsertQuote", false);
	scrMenuMgr->addMenuSeparator("InsertQuote");
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteLowSingleComma"], "InsertQuote", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteLowDoubleComma"], "InsertQuote", false);
	scrMenuMgr->addMenuSeparator("InsertQuote");
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteSingleLeftGuillemet"], "InsertQuote", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteSingleRightGuillemet"], "InsertQuote", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteDoubleLeftGuillemet"], "InsertQuote", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteDoubleRightGuillemet"], "InsertQuote", false);
	scrMenuMgr->addMenuSeparator("InsertQuote");
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteCJKSingleLeft"], "InsertQuote", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteCJKSingleRight"], "InsertQuote", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteCJKDoubleLeft"], "InsertQuote", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeQuoteCJKDoubleRight"], "InsertQuote", false);

	scrMenuMgr->createMenu("InsertSpace", tr("S&paces && Breaks"), "Insert");
	scrMenuMgr->addMenuItem(scrActions["unicodeNonBreakingSpace"], "InsertSpace", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeSpaceEN"], "InsertSpace", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeSpaceEM"], "InsertSpace", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeSpaceThin"], "InsertSpace", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeSpaceThick"], "InsertSpace", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeSpaceMid"], "InsertSpace", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeSpaceHair"], "InsertSpace", false);
	scrMenuMgr->addMenuSeparator("InsertSpace");
	scrMenuMgr->addMenuItem(scrActions["unicodeNewLine"], "InsertSpace", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeFrameBreak"], "InsertSpace", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeColumnBreak"], "InsertSpace", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeZerowidthSpace"], "InsertSpace", false);

	scrMenuMgr->createMenu("InsertLigature", tr("Liga&ture"), "Insert");
	scrMenuMgr->addMenuItem(scrActions["unicodeLigature_ff"], "InsertLigature", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeLigature_fi"], "InsertLigature", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeLigature_fl"], "InsertLigature", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeLigature_ffi"], "InsertLigature", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeLigature_ffl"], "InsertLigature", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeLigature_ft"], "InsertLigature", false);
	scrMenuMgr->addMenuItem(scrActions["unicodeLigature_st"], "InsertLigature", false);

	scrMenuMgr->addMenuSeparator("Insert");
	scrMenuMgr->addMenuItem(scrActions["insertSampleText"], "Insert", false);

	//Page menu
	scrMenuMgr->createMenu("Page", ActionManager::defaultMenuNameEntryTranslated("Page"));
	scrMenuMgr->addMenuItem(scrActions["pageInsert"], "Page", false);
	scrMenuMgr->addMenuItem(scrActions["pageImport"], "Page", false);
	scrMenuMgr->addMenuItem(scrActions["pageDelete"], "Page", false);
	scrMenuMgr->addMenuItem(scrActions["pageCopy"], "Page", false);
	scrMenuMgr->addMenuItem(scrActions["pageMove"], "Page", false);
	scrMenuMgr->addMenuSeparator("Page");
	scrMenuMgr->addMenuItem(scrActions["pageApplyMasterPage"], "Page", false);
	scrMenuMgr->addMenuItem(scrActions["pageCopyToMasterPage"], "Page", false);
	scrMenuMgr->addMenuItem(scrActions["pageManageGuides"], "Page", false);
	scrMenuMgr->addMenuItem(scrActions["pageManageMargins"], "Page", false);
	scrMenuMgr->addMenuSeparator("Page");
	scrMenuMgr->addMenuItem(scrActions["viewSnapToGrid"], "Page", true);
	scrMenuMgr->addMenuItem(scrActions["viewSnapToGuides"], "Page", true);

	//View menu
	scrMenuMgr->createMenu("View", ActionManager::defaultMenuNameEntryTranslated("View"));
	scrMenuMgr->addMenuItem(scrActions["viewFitInWindow"], "View", false);
	scrMenuMgr->addMenuItem(scrActions["viewFitWidth"], "View", false);
	scrMenuMgr->addMenuItem(scrActions["viewFit50"], "View", false);
	scrMenuMgr->addMenuItem(scrActions["viewFit75"], "View", false);
	scrMenuMgr->addMenuItem(scrActions["viewFit100"], "View", false);
	scrMenuMgr->addMenuItem(scrActions["viewFit200"], "View", false);
	scrMenuMgr->addMenuItem(scrActions["viewFit400"], "View", false);
	scrMenuMgr->addMenuSeparator("View");
	scrMenuMgr->addMenuItem(scrActions["viewFitPreview"], "View", true);
	scrMenuMgr->addMenuSeparator("View");
	scrMenuMgr->addMenuItem(scrActions["viewShowMargins"], "View", true);
	scrMenuMgr->addMenuItem(scrActions["viewShowBleeds"], "View", true);
	scrMenuMgr->addMenuItem(scrActions["viewShowFrames"], "View", true);
	scrMenuMgr->addMenuItem(scrActions["viewShowLayerMarkers"], "View", true);
	scrMenuMgr->addMenuItem(scrActions["viewShowImages"], "View", true);
	scrMenuMgr->addMenuItem(scrActions["viewShowGrid"], "View", true);
	scrMenuMgr->addMenuItem(scrActions["viewShowGuides"], "View", true);
	scrMenuMgr->addMenuItem(scrActions["viewShowColumnBorders"], "View", true);
	scrMenuMgr->addMenuItem(scrActions["viewShowBaseline"], "View", true);
	scrMenuMgr->addMenuItem(scrActions["viewShowTextChain"], "View", true);
	scrMenuMgr->addMenuItem(scrActions["viewShowTextControls"], "View", true);
	scrMenuMgr->addMenuItem(scrActions["viewShowRulers"], "View", false);
	scrMenuMgr->addMenuItem(scrActions["viewRulerMode"], "View", true);
	scrMenuMgr->addMenuItem(scrActions["showMouseCoordinates"], "View", true);

	//CB If this is viewNewView imeplemented, it should be on the windows menu
//	scrMenuMgr->addMenuItem(scrActions["viewNewView"], "View");

	//Tool menu
	/*
	scrMenuMgr->createMenu("Tools", tr("&Tools"));
	scrMenuMgr->addMenuItem(scrActions["toolsProperties"], "Tools");
	scrMenuMgr->addMenuItem(scrActions["toolsOutline"], "Tools");
	scrMenuMgr->addMenuItem(scrActions["toolsScrapbook"], "Tools");
	scrMenuMgr->addMenuItem(scrActions["toolsLayers"], "Tools");
	scrMenuMgr->addMenuItem(scrActions["toolsPages"], "Tools");
	scrMenuMgr->addMenuItem(scrActions["toolsBookmarks"], "Tools");
	scrMenuMgr->addMenuItem(scrActions["toolsMeasurements"], "Tools");
	scrMenuMgr->addMenuItem(scrActions["toolsActionHistory"], "Tools");
	scrMenuMgr->addMenuItem(scrActions["toolsPreflightVerifier"], "Tools");
	scrMenuMgr->addMenuItem(scrActions["toolsAlignDistribute"], "Tools");
	scrMenuMgr->addMenuSeparator("Tools");
	scrMenuMgr->addMenuItem(scrActions["toolsToolbarTools"], "Tools");
	scrMenuMgr->addMenuItem(scrActions["toolsToolbarPDF"], "Tools");
	//scrActions["toolsPreflightVerifier"]->setEnabled(false);*/

	//Extra menu
	scrMenuMgr->createMenu("Extras", ActionManager::defaultMenuNameEntryTranslated("Extras"));
	scrMenuMgr->addMenuItem(scrActions["extrasManageImages"], "Extras", false);
	scrMenuMgr->addMenuItem(scrActions["extrasHyphenateText"], "Extras", false);
	scrMenuMgr->addMenuItem(scrActions["extrasDeHyphenateText"], "Extras", false);
	scrMenuMgr->addMenuItem(scrActions["extrasGenerateTableOfContents"], "Extras", false);
	scrMenuMgr->setMenuEnabled("Extras", false);
	connect(scrMenuMgr->getLocalPopupMenu("Extras"), SIGNAL(aboutToShow()), this, SLOT(extrasMenuAboutToShow()));

	//Window menu
	scrMenuMgr->createMenu("Windows", ActionManager::defaultMenuNameEntryTranslated("Windows"), QString::null, true);
	connect(scrMenuMgr->getLocalPopupMenu("Windows"), SIGNAL(aboutToShow()), this, SLOT(windowsMenuAboutToShow()));
	addDefaultWindowMenuItems();

	//Help menu
	scrMenuMgr->createMenu("Help", ActionManager::defaultMenuNameEntryTranslated("Help"));
	scrMenuMgr->addMenuItem(scrActions["helpManual"], "Help", true);
	scrMenuMgr->addMenuItem(scrActions["helpManual2"], "Help", true);
	scrMenuMgr->addMenuSeparator("Help");
	scrMenuMgr->addMenuItem(scrActions["helpTooltips"], "Help", true);
	scrMenuMgr->addMenuSeparator("Help");
	scrMenuMgr->addMenuItem(scrActions["helpOnlineWWW"], "Help", true);
	scrMenuMgr->addMenuItem(scrActions["helpOnlineDocs"], "Help", true);
	scrMenuMgr->addMenuItem(scrActions["helpOnlineWiki"], "Help", true);
	scrMenuMgr->createMenu("HelpOnlineTutorials", tr("Online &Tutorials"), "Help");
	scrMenuMgr->addMenuItem(scrActions["helpOnlineTutorial1"], "HelpOnlineTutorials", true);
	scrMenuMgr->addMenuSeparator("Help");
	scrMenuMgr->addMenuItem(scrActions["helpCheckUpdates"], "Help", true);
	scrMenuMgr->addMenuSeparator("Help");
	scrMenuMgr->addMenuItem(scrActions["helpAboutScribus"], "Help", true);
	scrMenuMgr->addMenuItem(scrActions["helpAboutPlugins"], "Help", true);
	scrMenuMgr->addMenuItem(scrActions["helpAboutQt"], "Help", true);

	scrMenuMgr->addMenuToMenuBar("File");
	scrMenuMgr->addMenuToMenuBar("Edit");
	scrMenuMgr->addMenuToMenuBar("Item");
	scrMenuMgr->addMenuToMenuBar("Insert");
	scrMenuMgr->setMenuEnabled("Insert", false);
	scrMenuMgr->addMenuToMenuBar("Page");
	scrMenuMgr->addMenuToMenuBar("View");
	scrMenuMgr->addMenuToMenuBar("Extras");
	scrMenuMgr->setMenuEnabled("Extras", false);
	scrMenuMgr->addMenuToMenuBar("Windows");
	menuBar()->addSeparator();
	scrMenuMgr->addMenuToMenuBar("Help");
}


void ScribusMainWindow::addDefaultWindowMenuItems()
{
	scrMenuMgr->clearMenu("Windows");
	scrMenuMgr->addMenuItem(scrActions["windowsCascade"], "Windows", true);
	scrMenuMgr->addMenuItem(scrActions["windowsTile"], "Windows", true);
	scrMenuMgr->addMenuSeparator("Windows");

	scrMenuMgr->addMenuItem(scrActions["toolsProperties"], "Windows", true);
	scrMenuMgr->addMenuItem(scrActions["toolsOutline"], "Windows", true);
	scrMenuMgr->addMenuItem(scrActions["toolsScrapbook"], "Windows", true);
	scrMenuMgr->addMenuItem(scrActions["toolsLayers"], "Windows", true);
	scrMenuMgr->addMenuItem(scrActions["toolsPages"], "Windows", true);
	scrMenuMgr->addMenuItem(scrActions["toolsBookmarks"], "Windows", true);
	scrMenuMgr->addMenuItem(scrActions["toolsMeasurements"], "Windows", true);
	scrMenuMgr->addMenuItem(scrActions["toolsActionHistory"], "Windows", true);
	scrMenuMgr->addMenuItem(scrActions["toolsPreflightVerifier"], "Windows", true);
	scrMenuMgr->addMenuItem(scrActions["toolsAlignDistribute"], "Windows", true);
	scrMenuMgr->addMenuItem(scrActions["toolsSymbols"], "Windows", true);
	scrMenuMgr->addMenuSeparator("Windows");
	scrMenuMgr->addMenuItem(scrActions["toolsToolbarTools"], "Windows", true);
	scrMenuMgr->addMenuItem(scrActions["toolsToolbarPDF"], "Windows", true);
}


void ScribusMainWindow::initStatusBar()
{
	mainWindowStatusLabel = new QLabel( "           ", statusBar());
	mainWindowProgressBar = new QProgressBar(statusBar());
	mainWindowProgressBar->setAlignment(Qt::AlignHCenter);
	mainWindowProgressBar->setFixedWidth( 100 );
	mainWindowProgressBar->reset();
	mainWindowXPosLabel = new QLabel( tr("X-Pos:"), statusBar());
	mainWindowYPosLabel = new QLabel( tr("Y-Pos:"), statusBar());
	mainWindowXPosDataLabel = new QLabel( "        ", statusBar());
	mainWindowYPosDataLabel = new QLabel( "        ", statusBar());

	statusBar()->addPermanentWidget(mainWindowStatusLabel, 6);
	statusBar()->addPermanentWidget(mainWindowProgressBar, 0);
	statusBar()->addPermanentWidget(mainWindowXPosLabel, 0);
	statusBar()->addPermanentWidget(mainWindowXPosDataLabel, 1);
	statusBar()->addPermanentWidget(mainWindowYPosLabel, 0);
	statusBar()->addPermanentWidget(mainWindowYPosDataLabel, 1);
	connect(statusBar(), SIGNAL(messageChanged(const QString &)), this, SLOT(setTempStatusBarText(const QString &)));
}


void ScribusMainWindow::setStatusBarMousePosition(double xp, double yp)
{
	double xn = xp;
	double yn = yp;
	if (doc->guidesPrefs().rulerMode)
	{
		xn -= doc->currentPage()->xOffset();
		yn -= doc->currentPage()->yOffset();
	}
	xn -= doc->rulerXoffset;
	yn -= doc->rulerYoffset;
	mainWindowXPosDataLabel->setText(value2String(xn, doc->unitIndex(), true, true));
	mainWindowYPosDataLabel->setText(value2String(yn, doc->unitIndex(), true, true));
}

void ScribusMainWindow::setStatusBarTextPosition(double base, double xp)
{
	mainWindowXPosDataLabel->setText(base + xp >= 0? value2String(xp, doc->unitIndex(), true, true): QString("-"));
	mainWindowYPosDataLabel->setText("-");
}

void ScribusMainWindow::setTempStatusBarText(const QString &text)
{
	if (mainWindowStatusLabel)
	{
		if (text.isEmpty())
			mainWindowStatusLabel->setText(statusLabelText);
		else
			mainWindowStatusLabel->setText(text);
	}
}

void ScribusMainWindow::setStatusBarInfoText(QString newText)
{
	if (mainWindowStatusLabel)
		mainWindowStatusLabel->setText(newText);
	statusLabelText = newText;
}


//AV to be replaced with Selection::update and listener in PropertiesPalette
void ScribusMainWindow::setTBvals(PageItem *currItem)
{
	if (currItem->itemText.length() != 0)
	{
//		int ChPos = qMin(currItem->CPos, static_cast<int>(currItem->itemText.length()-1));
		const ParagraphStyle& currPStyle( (doc->appMode == modeEdit) ? currItem->currentStyle() : currItem->itemText.defaultStyle());
		setAlignmentValue(currPStyle.alignment());
		propertiesPalette->textPal->displayParStyle(currPStyle.parent());
		propertiesPalette->textPal->displayCharStyle(currItem->currentCharStyle().parent());
		doc->currentStyle = currItem->currentStyle();
		// #8112 : do not use operator= here as it does not update style features
		doc->currentStyle.charStyle().setStyle( currItem->currentCharStyle() );
		emit TextStyle(doc->currentStyle);
		// to go: (av)
		propertiesPalette->textPal->updateStyle(doc->currentStyle);
	}
}

//Special keys assigned to actions are stolen by the action and not passed to
//keyPressEvent so process them here.
void ScribusMainWindow::specialActionKeyEvent(const QString& actionName, int unicodevalue)
{
	if (HaveDoc)
	{
		if (doc->appMode==modeEdit)
		{
			if (doc->m_Selection->count() == 1)
			{
				PageItem *currItem = doc->m_Selection->itemAt(0);
				if (currItem!=NULL)
				{
					if (unicodevalue!=-1)
					{
						if (currItem->HasSel && currItem->itemType()==PageItem::TextFrame)
							currItem->asTextFrame()->deleteSelectedTextFromFrame();

						currItem->itemText.insertChars(QString(QChar(unicodevalue)), true);
//						currItem->Tinput = true;
						currItem->update();
					}
					else if (actionName=="unicodeSoftHyphen") //ignore the char as we use an attribute if the text item, for now.
					{
						// this code is currently dead since unicodeSoftHyphen
						// doesnt have unicodevalue == -1 any more
						if (currItem->itemText.cursorPosition() > 1)
						{
#if 0
							StyleFlag fl = currItem->itemText.item(qMax(currItem->CPos-1,0))->effects();
							fl |= ScStyle_HyphenationPossible;
							currItem->itemText.item(qMax(currItem->CPos-1,0))->setEffects(fl);
#else
							currItem->itemText.insertChars(QString(SpecialChars::SHYPHEN), true);
#endif
//							currItem->Tinput = true;
							currItem->update();
						}
					}
				}
			}
		}
	}
}

bool ScribusMainWindow::eventFilter( QObject* /*o*/, QEvent *e )
{
	bool retVal;
	if (e->type() == QEvent::ToolTip)
	{
		return (!prefsManager->appPrefs.displayPrefs.showToolTips);
	}
	if ( e->type() == QEvent::KeyPress )
	{
		QKeyEvent *k = (QKeyEvent *)e;
		int keyMod=0;
		if (k->modifiers() & Qt::ShiftModifier)
			keyMod |= Qt::SHIFT;
		if (k->modifiers() & Qt::ControlModifier)
			keyMod |= Qt::CTRL;
		if (k->modifiers() & Qt::AltModifier)
			keyMod |= Qt::ALT;

		QKeySequence currKeySeq = QKeySequence(k->key() | keyMod);
		if (QString(currKeySeq).isNull())
			return false;
		retVal=true;
		//Palette actions
		if (currKeySeq == scrActions["specialToggleAllPalettes"]->shortcut())
			scrActions["specialToggleAllPalettes"]->activate(QAction::Trigger);
		else
		if (currKeySeq == scrActions["specialToggleAllGuides"]->shortcut())
			scrActions["specialToggleAllGuides"]->activate(QAction::Trigger);
		else
// CB These were moved to ActionManager via the setShortcutContext(Qt::ApplicationShortcut) calls, leaving for notes for now
// 		if (currKeySeq == scrActions["toolsProperties"]->accel())
// 			scrActions["toolsProperties"]->toggle();
// 		else
// 		if (currKeySeq == scrActions["toolsOutline"]->accel())
// 			scrActions["toolsOutline"]->toggle();
// 		else
// 		if (currKeySeq == scrActions["toolsScrapbook"]->accel())
// 			scrActions["toolsScrapbook"]->toggle();
// 		else
// 		if (currKeySeq == scrActions["toolsLayers"]->accel())
// 			scrActions["toolsLayers"]->toggle();
// 		else
// 		if (currKeySeq == scrActions["toolsPages"]->accel())
// 			scrActions["toolsPages"]->toggle();
// 		else
// 		if (currKeySeq == scrActions["toolsBookmarks"]->accel())
// 			scrActions["toolsBookmarks"]->toggle();
// 		else
// 		if (currKeySeq == scrActions["toolsActionHistory"]->accel())
// 			scrActions["toolsActionHistory"]->toggle();
// 		else
// 		if (currKeySeq == scrActions["toolsPreflightVerifier"]->accel())
// 			scrActions["toolsPreflightVerifier"]->toggle();
// 		else
// 		if (currKeySeq == scrActions["toolsAlignDistribute"]->accel())
// 			scrActions["toolsAlignDistribute"]->toggle();
// 		else
		//Edit actions
/*		if (currKeySeq == scrActions["editStyles"]->shortcut())
			scrActions["editStyles"]->toggle();
		else
		if (currKeySeq == scrActions["editUndoAction"]->shortcut() && scrActions["editUndoAction"]->isEnabled())
			scrActions["editUndoAction"]->activate(QAction::Trigger);
		else
		if (currKeySeq == scrActions["editRedoAction"]->shortcut() && scrActions["editRedoAction"]->isEnabled())
			scrActions["editRedoAction"]->activate(QAction::Trigger);
		else
		//Other actions
		if (currKeySeq == scrActions["fileQuit"]->shortcut())
			scrActions["fileQuit"]->activate(QAction::Trigger);
		else
		//Zoom actions
		if (currKeySeq == scrActions["toolsZoomIn"]->shortcut())
			scrActions["toolsZoomIn"]->activate(QAction::Trigger);
		else
		if (currKeySeq == scrActions["toolsZoomOut"]->shortcut())
			scrActions["toolsZoomOut"]->activate(QAction::Trigger);
		else */
		retVal=false;
	}
	else
		retVal=false;
	//Return false to pass event to object
	return retVal;
}


void ScribusMainWindow::inputMethodEvent ( QInputMethodEvent * event )
{
	qDebug() << "IMEmw" << event->commitString() << event->preeditString() << "attributes:" << event->attributes().count();
}

QVariant ScribusMainWindow::inputMethodQuery ( Qt::InputMethodQuery query ) const
{
	qDebug() << "IMQmw" << query;
	return QVariant();
}

//AV -> CanvasMode
void ScribusMainWindow::keyPressEvent(QKeyEvent *k)
{
	QList<QMdiSubWindow *> windows;
	QMdiSubWindow* w = NULL;
	int kk = k->key();
	QString uc = k->text();
// 	QString cr, Tcha, Twort;
	if (HaveDoc)
	{
		if ((doc->appMode == modeMagnifier) && (kk == Qt::Key_Shift))
		{
			qApp->changeOverrideCursor(QCursor(loadIcon("LupeZm.xpm")));
			return;
		}
	}
	if (keyrep)
		return;
	keyrep = true;
	int keyMod=0;
	if (k->modifiers() & Qt::ShiftModifier)
		keyMod |= Qt::SHIFT;
	if (k->modifiers() & Qt::ControlModifier)
		keyMod |= Qt::CTRL;
	if (k->modifiers() & Qt::AltModifier)
		keyMod |= Qt::ALT;
	//User presses escape and we have a doc open, and we have an item selected
	if ((kk == Qt::Key_Escape) && (HaveDoc))
	{
		keyrep = false;
		PageItem *currItem;
		if (doc->m_Selection->count() != 0)
		{
			currItem = doc->m_Selection->itemAt(0);
			switch (doc->appMode)
			{
				case modeNormal:
				case modeEditClip:
					currItem->Sizing = false;
					if (doc->SubMode != -1)
					{
						view->Deselect(false);
						doc->Items->removeAt(currItem->ItemNr);
					}
					else
						view->Deselect(false);
					view->cancelGroupTransaction();
					break;
				case modeEdit:
					view->cancelGroupTransaction();
					break;
				case modeLinkFrames:
				case modeUnlinkFrames:
				case modeRotation:
				case modeEditGradientVectors:
				case modeEditMeshGradient:
				case modeCopyProperties:
					view->Deselect(false);
				case modeEyeDropper:
				case modeImportObject:
				case modePanning:
					view->requestMode(modeNormal);
					break;
				case modeDrawBezierLine:
					currItem->PoLine.resize(qMax(0, static_cast<int>(currItem->PoLine.size())-2));
					if (currItem->PoLine.size() < 4)
					{
						view->Deselect(false);
						doc->Items->removeAt(currItem->ItemNr);
					}
					else
					{
						doc->SizeItem(currItem->PoLine.WidthHeight().x(), currItem->PoLine.WidthHeight().y(), currItem->ItemNr, false, false);
						currItem->setPolyClip(qRound(qMax(currItem->lineWidth() / 2.0, 1.0)));
						doc->AdjustItemSize(currItem);
						currItem->ContourLine = currItem->PoLine.copy();
						currItem->ClipEdited = true;
						currItem->FrameType = 3;
						slotDocCh();
					}
					view->FirstPoly = true;
					break;
				default:
					if (currItem->Sizing)
					{
						view->Deselect(false);
						doc->Items->removeAt(currItem->ItemNr);
					}
					break;
			}
		}
		doc->DragP = false;
		doc->leaveDrag = false;
		view->stopAllDrags();
		doc->SubMode = -1;
		doc->ElemToLink = NULL;
		slotSelect();
		if (doc->m_Selection->count() == 0)
			HaveNewSel(-1);
		prefsManager->appPrefs.uiPrefs.stickyTools = false;
		scrActions["stickyTools"]->setChecked(false);
		return;
	}
	Qt::KeyboardModifiers buttonModifiers = k->modifiers();
	/**If we have a doc and we are not changing the page or zoom level in the status bar */
	if ((HaveDoc) && (!view->zoomSpinBox->hasFocus()) && (!view->pageSelector->hasFocus()))
	{
		//Show our context menu
		QKeySequence currKeySeq = QKeySequence(kk | keyMod);
		if (currKeySeq.matches(scrActions["viewShowContextMenu"]->shortcut()) == QKeySequence::ExactMatch)
		{
			ContextMenu* cmen=NULL;
			qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
			if (doc->m_Selection->count() == 0)
			{
				//CB We should be able to get this calculated by the canvas.... it is already in m_canvas->globalToCanvas(m->globalPos());
				QPoint p(QCursor::pos() - mapToGlobal(QPoint(0,0)));
				FPoint fp(p.x() / view->scale() + doc->minCanvasCoordinate.x(),
				p.y() / view->scale() + doc->minCanvasCoordinate.y());
				cmen = new ContextMenu(this, doc, fp.x(), fp.y());
			}
			else
				cmen = new ContextMenu(*(doc->m_Selection), this, doc);
			if (cmen)
			{
				setUndoMode(true);
				cmen->exec(QCursor::pos());
				setUndoMode(false);
			}
			delete cmen;
		}


		/**
		 * With no item selected we can:
		 * - With space, get into panning mode (modePanning)
		 * - With PageUp, scroll up
		 * - With PageDown, scroll down
		 * - With Tab, change active document windowActivated
		 */

		if ((doc->appMode != modeEdit) && (doc->m_Selection->count() == 0))
		{
			int pg;
			int wheelVal = prefsManager->mouseWheelJump();
			if ((buttonModifiers & Qt::ShiftModifier) && !(buttonModifiers & Qt::ControlModifier) && !(buttonModifiers & Qt::AltModifier))
				wheelVal = qMax(qRound(wheelVal / 10.0), 1);
			switch (kk)
			{
			case Qt::Key_Space:
				keyrep = false;
				if (doc->appMode == modePanning)
					view->requestMode(modeNormal);
				else
					view->requestMode(modePanning);
				return;
				break;
			case Qt::Key_PageUp:
				if (doc->masterPageMode() || doc->symbolEditMode())
					view->scrollBy(0, -prefsManager->mouseWheelJump());
				else
				{
					pg = doc->currentPageNumber();
					if ((buttonModifiers & Qt::ShiftModifier) && !(buttonModifiers & Qt::ControlModifier) && !(buttonModifiers & Qt::AltModifier))
						pg--;
					else
						pg -= doc->pageSets()[doc->pagePositioning()].Columns;
					if (pg > -1)
						view->GotoPage(pg);
				}
				keyrep = false;
				return;
				break;
			case Qt::Key_PageDown:
				if (doc->masterPageMode() || doc->symbolEditMode())
					view->scrollBy(0, prefsManager->mouseWheelJump());
				else
				{
					pg = doc->currentPageNumber();
					if ((buttonModifiers & Qt::ShiftModifier) && !(buttonModifiers & Qt::ControlModifier) && !(buttonModifiers & Qt::AltModifier))
						pg++;
					else
						pg += doc->pageSets()[doc->pagePositioning()].Columns;
					if (pg < static_cast<int>(doc->Pages->count()))
						view->GotoPage(pg);
				}
				keyrep = false;
				return;
				break;
			case Qt::Key_Left:
				view->scrollBy(-wheelVal, 0);
				keyrep = false;
				return;
				break;
			case Qt::Key_Right:
				view->scrollBy(wheelVal, 0);
				keyrep = false;
				return;
				break;
			case Qt::Key_Up:
				view->scrollBy(0, -wheelVal);
				keyrep = false;
				return;
				break;
			case Qt::Key_Down:
				view->scrollBy(0, wheelVal);
				keyrep = false;
				return;
				break;
			case Qt::Key_Tab:
				if (buttonModifiers == Qt::ControlModifier)
				{
					keyrep = false;
					windows = mdiArea->subWindowList();
					if (windows.count() > 1)
					{
						for (int i = 0; i < static_cast<int>(windows.count()); ++i)
						{
							if (mdiArea->activeSubWindow() == windows.at(i))
							{
								if (i == static_cast<int>(windows.count()-1))
									w = windows.at(0);
								else
									w = windows.at(i+1);
								break;
							}
						}
						outlinePalette->buildReopenVals();
						docCheckerPalette->clearErrorList();
						if ( w )
							w->showNormal();
						newActWin(w);
					}
					return;
				}
				break;
			}
		}
		/** Now if we have an item selected
		 * - In normal mode we can:
		 * -- Use backspace or delete to delete the item
		 * -- In edit mode of an image frame, use the arrow keys to resize the image:
		 		(flows to pageitem_imageframe for control)
		 		Alt right arrow, move right side of image outwards (expand)
		 		Alt left arrow, move right side inwards (shrink)
		 		Alt down arrow, move bottom side downwards (expand)
		 		Alt up arrow, move top side inwards (shrink)
		 */
		if (doc->m_Selection->count() != 0)
		{
			PageItem *currItem = doc->m_Selection->itemAt(0);
			if (doc->appMode == modeEdit)
			{
				if (currItem->asImageFrame() && !currItem->locked())
				{
					currItem->handleModeEditKey(k, keyrep);
				}
//FIXME:av		view->oldCp = currItem->CPos;
				if (currItem->itemType() == PageItem::TextFrame)
				{
					bool kr=keyrep;
					view->canvasMode()->keyPressEvent(k); //Hack for 1.4.x for stopping the cursor blinking while moving about
					currItem->handleModeEditKey(k, keyrep);
					keyrep=kr;
				}
				slotDocCh(false);
			}
		}
	}
	switch(kk)
	{
		case Qt::Key_Left:
		case Qt::Key_Right:
		case Qt::Key_Up:
		case Qt::Key_Down:
			_arrowKeyDown = true;
	}
	keyrep = false;
}

void ScribusMainWindow::keyReleaseEvent(QKeyEvent *k)
{
	//Exit out of panning mode if Control is release while the right mouse button is pressed
	if (HaveDoc)
	{
		if ((doc->appMode == modePanning) && (k->key() == Qt::Key_Control) && (QApplication::mouseButtons() & Qt::RightButton))
			view->requestMode(modeNormal);
	}
	if (HaveDoc)
	{
		if (doc->appMode == modeMagnifier)
			qApp->changeOverrideCursor(QCursor(loadIcon("LupeZ.xpm")));
	}
	if (k->isAutoRepeat() || !_arrowKeyDown)
		return;
	switch(k->key())
	{
		case Qt::Key_Left:
		case Qt::Key_Right:
		case Qt::Key_Up:
		case Qt::Key_Down:
			_arrowKeyDown = false;
			if ((HaveDoc) && (!view->zoomSpinBox->hasFocus()) && (!view->pageSelector->hasFocus()))
			{
				int docSelectionCount=doc->m_Selection->count();
				if ((docSelectionCount != 0) && (doc->appMode == modeEditClip) && (doc->nodeEdit.hasNodeSelected()))
				{
					PageItem *currItem = doc->m_Selection->itemAt(0);
					double xposOrig = currItem->xPos();
					double yposOrig = currItem->yPos();
					doc->AdjustItemSize(currItem);
					if (!doc->nodeEdit.isContourLine)
						currItem->ContourLine.translate(xposOrig - currItem->xPos(),yposOrig - currItem->yPos());
					currItem->update();
				}
				for (int i = 0; i < docSelectionCount; ++i)
					doc->m_Selection->itemAt(i)->checkChanges(true);
				if (docSelectionCount > 1 && view->groupTransactionStarted())
					view->endGroupTransaction();
			}
			break;
	}
}

void ScribusMainWindow::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::LanguageChange)
	{
		languageChange();
	}
	else
		QWidget::changeEvent(e);
}

void ScribusMainWindow::closeEvent(QCloseEvent *ce)
{
	//Do not quit if Preferences or new doc window is open
//	Preferences *prefsWin = findChild<Preferences *>(QString::fromLocal8Bit("PreferencesWindow"));
	PreferencesDialog *prefsDialog = findChild<PreferencesDialog *>(QString::fromLocal8Bit("PreferencesDialog"));
	NewDoc *newDocWin = findChild<NewDoc *>(QString::fromLocal8Bit("NewDocumentWindow"));
//	if (prefsWin!=NULL || prefsDialog!=NULL || newDocWin!=NULL)
	if (prefsDialog!=NULL || newDocWin!=NULL)
	{
		ce->ignore();
		return;
	}
	QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
	ScribusWin* tw;
	disconnect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(newActWin(QMdiSubWindow *)));
	if (!windows.isEmpty())
	{
		uint windowCount=windows.count();
		for ( uint i = 0; i < windowCount; ++i )
		{
			tw = (ScribusWin *)(windows.at(i));
			QMdiSubWindow *tws = windows.at(i);
			ScribusWin* scw = dynamic_cast<ScribusWin *>(tws->widget());
			if (scw)
			{
				newActWin(windows.at(i));
				tw = ActWin;
				slotSelect();
				ActWin->close();
				if (tw == ActWin)
				{
					ce->ignore();
					connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(newActWin(QMdiSubWindow *)));
					return;
				}
			}
		}
	}
	fileToolBar->connectPrefsSlot(false);
	editToolBar->connectPrefsSlot(false);
	modeToolBar->connectPrefsSlot(false);
	pdfToolBar->connectPrefsSlot(false);
	propertiesPalette->hide();
	outlinePalette->hide();
	scrapbookPalette->hide();
	bookmarkPalette->hide();
	layerPalette->hide();
	pagePalette->hide();
//	measurementPalette->hide();
	docCheckerPalette->hide();
	undoPalette->hide();
	alignDistributePalette->hide();
	guidePalette->hide();
	charPalette->hide();
	symbolPalette->hide();

	// Clean up plugins, THEN save prefs to disk
	ScCore->pluginManager->cleanupPlugins();
	if (!prefsManager->appPrefs.scrapbookPrefs.persistentScrapbook)
		scrapbookPalette->CleanUpTemp();
	prefsManager->appPrefs.scrapbookPrefs.RecentScrapbooks.clear();
	prefsManager->appPrefs.scrapbookPrefs.RecentScrapbooks = scrapbookPalette->getOpenScrapbooks();
	if (!emergencyActivated)
		prefsManager->SavePrefs();
	UndoManager::deleteInstance();
	PrefsManager::deleteInstance();
	FormatsManager::deleteInstance();
	UrlLauncher::deleteInstance();
	qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
	qApp->exit(0);
}

void ScribusMainWindow::requestUpdate(int val)
{
	emit UpdateRequest(val);
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////


bool ScribusMainWindow::arrowKeyDown()
{
	return _arrowKeyDown;
}

void ScribusMainWindow::startUpDialog()
{
	bool docSet = false;
	PrefsContext* docContext = prefsManager->prefsFile->getContext("docdirs", false);
	NewDoc* dia = new NewDoc(this, RecentDocs, true, ScCore->getGuiLanguage());
	if (dia->exec())
	{
		if (dia->tabSelected() == NewDoc::NewDocumentTab)
		{
			int facingPages = dia->choosenLayout();
			int firstPage = dia->firstPage->currentIndex();
			docSet = dia->startDocSetup->isChecked();
			double topMargin = dia->marginGroup->top();
			double bottomMargin = dia->marginGroup->bottom();
			double leftMargin = dia->marginGroup->left();
			double rightMargin = dia->marginGroup->right();
			double columnDistance = dia->distance();
			double pageWidth = dia->pageWidth();
			double pageHeight = dia->pageHeight();
			double numberCols = dia->numberOfCols->value();
			bool autoframes = dia->autoTextFrame->isChecked();
			int orientation = dia->orientation();
			int pageCount=dia->pageCountSpinBox->value();
			QString pagesize;
			if (dia->pageSizeComboBox->currentText() == CommonStrings::trCustomPageSize)
				pagesize = CommonStrings::customPageSize;
			else
			{
				PageSize ps2(dia->pageSizeComboBox->currentText());
				pagesize = ps2.name();
			}
			doFileNew(pageWidth, pageHeight, topMargin, leftMargin, rightMargin, bottomMargin, columnDistance, numberCols, autoframes, facingPages, dia->unitOfMeasureComboBox->currentIndex(), firstPage, orientation, 1, pagesize, true, pageCount, true, dia->marginGroup->getMarginPreset());
			doc->setPageSetFirstPage(facingPages, firstPage);
			doc->bleeds()->set(dia->bleedTop(), dia->bleedLeft(), dia->bleedBottom(), dia->bleedRight());
			HaveNewDoc();
			doc->reformPages(true);
			// Don's disturb user with "save?" dialog just after new doc
			// doc changing should be rewritten maybe... maybe later...
			doc->setModified(false);
			updateActiveWindowCaption(doc->DocName);
		}
		else if (dia->tabSelected() == NewDoc::NewFromTemplateTab)
		{
			QString fileName = QDir::cleanPath(dia->selectedFile());
			if (!fileName.isEmpty() && loadDoc(fileName))
			{
				doc->hasName = false;
				UndoManager::instance()->renameStack(dia->nftGui->currentDocumentTemplate->name);
				doc->DocName = dia->nftGui->currentDocumentTemplate->name;
				updateActiveWindowCaption(QObject::tr("Document Template: ") + dia->nftGui->currentDocumentTemplate->name);
				QDir::setCurrent(PrefsManager::instance()->documentDir());
				removeRecent(fileName);
			}
		}
		else if (dia->tabSelected() == NewDoc::OpenExistingTab)
		{
			QString fileName = dia->selectedFile();
			if (!fileName.isEmpty())
			{
				QFileInfo fi(fileName);
				docContext->set("docsopen", fi.absolutePath());
				loadDoc(fileName);
			}
		}
		else // NewDoc::OpenRecentTab
		{
			QString fileName = dia->selectedFile();
			if (!fileName.isEmpty())
				loadRecent(fileName);
		}
	}
	prefsManager->setShowStartupDialog(!dia->startUpDialog->isChecked());
	delete dia;
	mainWindowStatusLabel->setText( tr("Ready"));
	if (docSet)
		slotDocSetup150();
}

bool ScribusMainWindow::slotFileNew()
{
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	bool retVal = false;
	bool docSet = false;
	NewDoc* dia = new NewDoc(this, RecentDocs);
	if (dia->exec())
	{
		int facingPages = dia->choosenLayout();
		int firstPage = dia->firstPage->currentIndex();
		docSet = dia->startDocSetup->isChecked();
		double topMargin = dia->marginGroup->top();
		double bottomMargin = dia->marginGroup->bottom();
		double leftMargin = dia->marginGroup->left();
		double rightMargin = dia->marginGroup->right();
		double columnDistance = dia->distance();
		double pageWidth = dia->pageWidth();
		double pageHeight = dia->pageHeight();
		double numberCols = dia->numberOfCols->value();
		bool autoframes = dia->autoTextFrame->isChecked();
		int orientation = dia->orientation();
		int pageCount=dia->pageCountSpinBox->value();
		QString pagesize;
		if (dia->pageSizeComboBox->currentText() == CommonStrings::trCustomPageSize)
			pagesize = CommonStrings::customPageSize;
		else
		{
			PageSize ps2(dia->pageSizeComboBox->currentText());
			pagesize = ps2.name();
		}
		if (doFileNew(pageWidth, pageHeight, topMargin, leftMargin, rightMargin, bottomMargin, columnDistance, numberCols, autoframes, facingPages, dia->unitOfMeasureComboBox->currentIndex(), firstPage, orientation, 1, pagesize, true, pageCount, true, dia->marginGroup->getMarginPreset()))
		{
			doc->setPageSetFirstPage(facingPages, firstPage);
			doc->bleeds()->set(dia->bleedTop(), dia->bleedLeft(), dia->bleedBottom(), dia->bleedRight());
			mainWindowStatusLabel->setText( tr("Ready"));
			HaveNewDoc();
			doc->reformPages(true);
			retVal = true;
			// Don's disturb user with "save?" dialog just after new doc
			// doc changing should be rewritten maybe... maybe later...
			doc->setModified(false);
			updateActiveWindowCaption(doc->DocName);
		}
	}
	delete dia;
	if (docSet)
		slotDocSetup150();
	return retVal;
}

//TODO move to core, assign doc to doc list, optionally create gui for it
ScribusDoc *ScribusMainWindow::newDoc(double width, double height, double topMargin, double leftMargin, double rightMargin, double bottomMargin, double columnDistance, double columnCount, bool autoTextFrames, int pageArrangement, int unitIndex, int firstPageLocation, int orientation, int firstPageNumber, const QString& defaultPageSize, bool requiresGUI, int pageCount, bool showView, int marginPreset)
{
	return doFileNew(width, height, topMargin, leftMargin, rightMargin, bottomMargin, columnDistance, columnCount, autoTextFrames, pageArrangement, unitIndex, firstPageLocation, orientation, firstPageNumber, defaultPageSize, requiresGUI, pageCount, showView, marginPreset);
	/* TODO CB finish later this week.
	if (HaveDoc)
		doc->OpenNodes = outlinePalette->buildReopenVals();
	MarginStruct margins(topMargin, leftMargin, bottomMargin, rightMargin);
	DocPagesSetup pagesSetup(pageArrangement, firstPageLocation, firstPageNumber, orientation, autoTextFrames, columnDistance, columnCount);
	QString newDocName(tr("Document")+"-"+QString::number(DocNr));
	doc = new ScribusDoc(newDocName, unitindex, pagesize, margins, pagesSetup);
		doc->setLoading(true);
	doc->setup(unitIndex, pageArrangement, firstPageLocation, orientation, firstPageNumber, defaultPageSize, newDocName);
	HaveDoc++;
	DocNr++;
	if (ScCore->haveCMS() && doc->CMSSettings.CMSinUse)
		recalcColors();
	//CB NOTE should be all done now
	doc->setPage(width, height, topMargin, leftMargin, rightMargin, bottomMargin, columnDistance, columnCount, autoTextFrames, pageArrangement);
	doc->setMasterPageMode(false);
	doc->addMasterPage(0, CommonStrings::masterPageNormal);
	int createCount=qMax(pageCount,1);
	for (int i = 0; i < createCount; ++i)
		doc->addPage(i, CommonStrings::masterPageNormal, true);
	doc->addSection();
	doc->setFirstSectionFromFirstPageNumber();
	doc->setModified(false);
	doc->OpenNodes.clear();
	actionManager->disconnectNewDocActions();
	actionManager->connectNewDocActions(doc);
	//<<View and window code
	ScribusWin* w = new ScribusWin(mdiArea, doc);
	w->setMainWindow(this);
	if (view!=NULL)
	{
		actionManager->disconnectNewViewActions();
		disconnect(view, SIGNAL(signalGuideInformation(int, double)), alignDistributePalette, SLOT(setGuide(int, double)));
	}
	view = new ScribusView(w, this, doc);
	doc->setCurrentPage(doc->Pages->at(0));
	doc->setGUI(this, view);
	doc->setLoading(false);
	//run after setGUI to set up guidepalette ok

	view->setScale(prefsManager->displayScale());
	actionManager->connectNewViewActions(view);
	alignDistributePalette->setDoc(doc);
	docCheckerPalette->clearErrorList();
	w->setView(view);
	ActWin = w;
	doc->WinHan = w;
	w->setCentralWidget(view);
	doc->connectDocSignals(); //Must be before the first reformpages
	view->reformPages(true);
	//>>

	connect(undoManager, SIGNAL(undoRedoDone()), view, SLOT(DrawNew()));
	//connect(w, SIGNAL(Schliessen()), this, SLOT(DoFileClose()));
	connect(view, SIGNAL(signalGuideInformation(int, double)), alignDistributePalette, SLOT(setGuide(int, double)));
	//	connect(w, SIGNAL(SaveAndClose()), this, SLOT(DoSaveClose()));

	//Independent finishing tasks after doc setup
	if (showView)
	{
		if ( mdiArea->windowList().isEmpty() )
			w->showMaximized();
		else
			w->show();
		view->show();
	}
	connect(w, SIGNAL(AutoSaved()), this, SLOT(slotAutoSaved()));
	connect(ScCore->fileWatcher, SIGNAL(fileChanged(QString)), doc, SLOT(updatePict(QString)));
	connect(ScCore->fileWatcher, SIGNAL(fileDeleted(QString)), doc, SLOT(removePict(QString)));
	scrActions["fileSave"]->setEnabled(false);
	undoManager->switchStack(doc->DocName);
	styleManager->currentDoc(doc);
	tocGenerator->setDoc(doc);

	return doc;
	*/
}

ScribusDoc *ScribusMainWindow::doFileNew(double width, double height, double topMargin, double leftMargin, double rightMargin, double bottomMargin, double columnDistance, double columnCount, bool autoTextFrames, int pageArrangement, int unitIndex, int firstPageLocation, int orientation, int firstPageNumber, const QString& defaultPageSize, bool requiresGUI, int pageCount, bool showView, int marginPreset)
{
	if (HaveDoc)
		outlinePalette->buildReopenVals();
	undoManager->setUndoEnabled(false);
	MarginStruct margins(topMargin, leftMargin, bottomMargin, rightMargin);
	DocPagesSetup pagesSetup(pageArrangement, firstPageLocation, firstPageNumber, orientation, autoTextFrames, columnDistance, columnCount);
	QString newDocName( tr("Document")+"-"+QString::number(DocNr));
	ScribusDoc *tempDoc = new ScribusDoc();
	if (requiresGUI)
		doc=tempDoc;
	tempDoc->setLoading(true);
	outlinePalette->setDoc(tempDoc);
	ColorSetManager csm;
	csm.findPaletteLocations();
	csm.findPalettes();
	csm.findUserPalettes();
	ColorList colorList;
	QMap<QString, VGradient> gradientsList;
	QMap<QString, ScPattern> patternsList;
	QString Cpfad = QDir::toNativeSeparators(ScPaths::getApplicationDataDir())+"DefaultColors.xml";
	QFile fc(Cpfad);
	if (fc.exists())
	{
		csm.loadPalette(Cpfad, doc, colorList, gradientsList, patternsList, false);
		doc->PageColors = colorList;
		doc->docGradients = gradientsList;
		doc->docPatterns = patternsList;
	}
	else
	{
		if (prefsManager->appPrefs.colorPrefs.DColorSet != "Scribus Small")
		{
			QStringList CustomColorSets = csm.userPaletteNames();
			if (CustomColorSets.contains(prefsManager->appPrefs.colorPrefs.DColorSet))
				Cpfad = csm.userPaletteFileFromName(prefsManager->appPrefs.colorPrefs.DColorSet);
			else
				Cpfad = csm.paletteFileFromName(prefsManager->appPrefs.colorPrefs.DColorSet);
			csm.loadPalette(Cpfad, doc, colorList, gradientsList, patternsList, false);
			doc->PageColors = colorList;
			doc->docGradients = gradientsList;
			doc->docPatterns = patternsList;
		}
		else
			doc->PageColors = prefsManager->appPrefs.colorPrefs.DColors;
	}
	tempDoc->PageColors.ensureDefaultColors();
	tempDoc->setup(unitIndex, pageArrangement, firstPageLocation, orientation, firstPageNumber, defaultPageSize, newDocName);
	if (requiresGUI)
	{
		HaveDoc++;
		DocNr++;
	}
	if (ScCore->haveCMS() && tempDoc->cmsSettings().CMSinUse)
		recalcColors();
	//CB NOTE should be all done now
	tempDoc->setPage(width, height, topMargin, leftMargin, rightMargin, bottomMargin, columnCount, columnDistance, autoTextFrames, pageArrangement);
	tempDoc->setMarginPreset(marginPreset);
	tempDoc->setMasterPageMode(false);
	tempDoc->createDefaultMasterPages();
	tempDoc->createNewDocPages(pageCount);
	tempDoc->addSection();
	tempDoc->setFirstSectionFromFirstPageNumber();
	tempDoc->setModified(false);
	tempDoc->OpenNodes.clear();
	if (requiresGUI)
	{
		actionManager->disconnectNewDocActions();
		actionManager->connectNewDocActions(tempDoc);
	}
	//<<View and window code
	QMdiArea* qwsp = 0;
	if (requiresGUI)
		qwsp = mdiArea;
	ScribusWin* w = new ScribusWin(qwsp, tempDoc);
	w->setMainWindow(this);
	if (requiresGUI && view!=NULL)
	{
		actionManager->disconnectNewViewActions();
		disconnect(view, SIGNAL(signalGuideInformation(int, qreal)), alignDistributePalette, SLOT(setGuide(int, qreal)));
	}
	ScribusView* tempView = new ScribusView(w, this, tempDoc);
	if (requiresGUI)
		view = tempView;
	tempDoc->setCurrentPage(tempDoc->Pages->at(0));
	tempDoc->setGUI(requiresGUI, this, tempView);
	if (requiresGUI)
	{
		tempDoc->docHyphenator->ignoredWords = prefsManager->appPrefs.hyphPrefs.ignoredWords;
		tempDoc->docHyphenator->specialWords = prefsManager->appPrefs.hyphPrefs.specialWords;
	}
	tempDoc->setLoading(false);
	//run after setGUI to set up guidepalette ok

	tempView->setScale(prefsManager->displayScale());
	if (requiresGUI)
	{
		actionManager->connectNewViewActions(tempView);
		alignDistributePalette->setDoc(tempDoc);
		docCheckerPalette->clearErrorList();
		symbolPalette->setDoc(tempDoc);
	}
	w->setView(tempView);
	ActWin = w;
	tempDoc->WinHan = w;
	if (requiresGUI)
		tempDoc->connectDocSignals(); //Must be before the first reformpages
	tempView->reformPages(true);
	//>>
	if (requiresGUI)
	{
		w->setSubWin(mdiArea->addSubWindow(w));
		//#9250 : transfered to newActWin()
		//connect(undoManager, SIGNAL(undoRedoBegin()), tempDoc, SLOT(undoRedoBegin()));
		//connect(undoManager, SIGNAL(undoRedoDone()), tempDoc, SLOT(undoRedoDone()));
		//connect(undoManager, SIGNAL(undoRedoDone()), tempView, SLOT(DrawNew()));
		connect(tempView, SIGNAL(signalGuideInformation(int, qreal)), alignDistributePalette, SLOT(setGuide(int, qreal)));
	}
	//Independent finishing tasks after tempDoc setup
	if (showView)
	{
		if ( mdiArea->subWindowList().count() == 1)
			w->showMaximized();
		else
			w->show();
		tempView->show();
		// Seems to fix crash on loading
		ActWin = NULL;
		newActWin(w->getSubWin());
	}
	if (requiresGUI)
	{
		connect(w, SIGNAL(AutoSaved()), this, SLOT(slotAutoSaved()));
		connect(ScCore->fileWatcher, SIGNAL(fileChanged(QString)), tempDoc, SLOT(updatePict(QString)));
		connect(ScCore->fileWatcher, SIGNAL(fileDeleted(QString)), tempDoc, SLOT(removePict(QString)));
		connect(ScCore->fileWatcher, SIGNAL(dirChanged(QString )), tempDoc, SLOT(updatePictDir(QString )));
		//scrActions["fileSave"]->setEnabled(false);
		tempView->cmsToolbarButton->setChecked(tempDoc->HasCMS);
		undoManager->switchStack(tempDoc->DocName);
		styleManager->setDoc(tempDoc);
		tocGenerator->setDoc(tempDoc);
	}
	undoManager->setUndoEnabled(true);
	return tempDoc;
}

void ScribusMainWindow::newFileFromTemplate()
{
	nftdialog* nftdia = new nftdialog(this, ScCore->getGuiLanguage());
	if (nftdia->exec())
	{
		if (nftdia->nftGui->currentDocumentTemplate)
		{
			qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
			if (loadDoc(QDir::cleanPath(nftdia->nftGui->currentDocumentTemplate->file)))
			{
				doc->hasName = false;
				UndoManager::instance()->renameStack(nftdia->nftGui->currentDocumentTemplate->name);
				doc->DocName = nftdia->nftGui->currentDocumentTemplate->name;
				updateActiveWindowCaption(QObject::tr("Document Template: ") + nftdia->nftGui->currentDocumentTemplate->name);
				QDir::setCurrent(PrefsManager::instance()->documentDir());
				removeRecent(QDir::cleanPath(nftdia->nftGui->currentDocumentTemplate->file));
			}
			qApp->changeOverrideCursor(Qt::ArrowCursor);
		}
	}
	delete nftdia;
}

void ScribusMainWindow::newView()
{
	ScribusWin* w = new ScribusWin(mdiArea, doc);
	w->setMainWindow(this);
	view = new ScribusView(w, this, doc);
	view->setScale(prefsManager->displayScale());
	w->setView(view);
	ActWin = w;
	w->setCentralWidget(view);
	actionManager->connectNewViewActions(view);
	alignDistributePalette->setDoc(doc);
	connect(undoManager, SIGNAL(undoRedoDone()), view, SLOT(DrawNew()));
	view->show();
}

void ScribusMainWindow::windowsMenuAboutToShow()
{
	for( QMap<QString, QPointer<ScrAction> >::Iterator it = scrWindowsActions.begin(); it!=scrWindowsActions.end(); ++it )
		scrMenuMgr->removeMenuItem((*it), "Windows");
	scrWindowsActions.clear();
	addDefaultWindowMenuItems();
	QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
	bool windowsListNotEmpty=!windows.isEmpty();
	scrActions["windowsCascade"]->setEnabled(windowsListNotEmpty);
	scrActions["windowsTile"]->setEnabled(windowsListNotEmpty);
	if (windowsListNotEmpty)
	{
		int windowCount=static_cast<int>(windows.count());
		if (windowCount>1)
			scrMenuMgr->addMenuSeparator("Windows");
		for ( int i = 0; i < windowCount; ++i )
		{
			QString docInWindow(windows.at(i)->windowTitle());
			scrWindowsActions.insert(docInWindow, new ScrAction( ScrAction::Window, QPixmap(), QPixmap(), docInWindow, QKeySequence(), this, i));
			scrWindowsActions[docInWindow]->setToggleAction(true);
			connect( scrWindowsActions[docInWindow], SIGNAL(triggeredData(int)), this, SLOT(windowsMenuActivated(int)) );
			if (windowCount>1)
				scrMenuMgr->addMenuItem(scrWindowsActions[docInWindow], "Windows", true);
			scrWindowsActions[docInWindow]->setChecked(mdiArea->activeSubWindow() == windows.at(i));
		}
	}
}

void ScribusMainWindow::extrasMenuAboutToShow()
{
	// There is only Picture Manager handled now.
	// As it can be opened all the time of the document life.
	// This is only check for availability of any ImageFrame
	// in the doc.
	bool enablePicManager = false;
	if (HaveDoc)
	{
		QList<PageItem*> allItems;
		for (int i = 0; i < doc->Items->count(); ++i)
		{
			PageItem *currItem = doc->Items->at(i);
			if (currItem->isGroup())
				allItems = currItem->getItemList();
			else
				allItems.append(currItem);
			for (int ii = 0; ii < allItems.count(); ii++)
			{
				PageItem* item = allItems.at(ii);
#ifdef HAVE_OSG
				if ((item->itemType() == PageItem::ImageFrame) && (!((item->asLatexFrame()) || (item->asOSGFrame()))))
#else
				if ((item->itemType() == PageItem::ImageFrame) && (!(item->asLatexFrame())))
#endif
				{
					enablePicManager = true;
					break;
				}
			}
			allItems.clear();
		}
	}
	scrActions["extrasManageImages"]->setEnabled(enablePicManager);
}

void ScribusMainWindow::newActWin(QMdiSubWindow *w)
{
	if (w == NULL)
	{
		if ( mdiArea->subWindowList().count() == 0)
			ActWin = NULL;
		return;
	}
	if (w->widget() == NULL)
		return;
	if (doc!=0 && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	ScribusWin* scw = dynamic_cast<ScribusWin *>(w->widget());
	if (!scw)
		return;
	if (scw && scw->doc())
	{
		if (!scw->doc()->hasGUI())
			return;
	}
	if(scw == ActWin)
		return;
	ActWin = scw;
	if (ActWin->doc()==NULL)
		return;

	if (doc != NULL)
	{
		if ((HaveDoc) && (doc != ActWin->doc()))
			outlinePalette->buildReopenVals();
	}
	docCheckerPalette->clearErrorList();

	if (HaveDoc && (doc != NULL) && doc->hasGUI())
	{
		disconnect(undoManager, SIGNAL(undoRedoBegin()), doc, SLOT(undoRedoBegin()));
		disconnect(undoManager, SIGNAL(undoRedoDone()) , doc, SLOT(undoRedoDone()));
		disconnect(undoManager, SIGNAL(undoRedoDone()) , doc->view(), SLOT(DrawNew()));
	}

	doc = ActWin->doc();
	undoManager->switchStack(doc->DocName);

	if ((doc != NULL) && doc->hasGUI())
	{
		connect(undoManager, SIGNAL(undoRedoBegin()), doc, SLOT(undoRedoBegin()));
		connect(undoManager, SIGNAL(undoRedoDone()) , doc, SLOT(undoRedoDone()));
		connect(undoManager, SIGNAL(undoRedoDone()) , doc->view(), SLOT(DrawNew()));
	}

	if (view!=NULL)
	{
		actionManager->disconnectNewViewActions();
		disconnect(view, SIGNAL(signalGuideInformation(int, qreal)), alignDistributePalette, SLOT(setGuide(int, qreal)));
		if (ScCore->usingGUI())
		{
			disconnect(doc->m_Selection, SIGNAL(selectionIsMultiple(bool)), 0, 0);
			//disconnect(doc->m_Selection, SIGNAL(empty()), 0, 0);
		}
	}

	view = ActWin->view();
	actionManager->connectNewViewActions(view);
	actionManager->disconnectNewDocActions();
	actionManager->connectNewDocActions(doc);
	connect(view, SIGNAL(signalGuideInformation(int, qreal)), alignDistributePalette, SLOT(setGuide(int, qreal)));
	if (ScCore->usingGUI())
	{
		//connect(doc->m_Selection, SIGNAL(selectionIsMultiple(bool)), propertiesPalette, SLOT( setMultipleSelection(bool)));
		connect(doc->m_Selection, SIGNAL(selectionIsMultiple(bool)), actionManager, SLOT( handleMultipleSelections(bool)));
		//connect(doc->m_Selection, SIGNAL(empty()), propertiesPalette, SLOT( unsetItem()));
	}

	pagePalette->setView(view);
	alignDistributePalette->setDoc(doc);
	if (!doc->isLoading())
	{
//		scanDocument();
//		docCheckerPalette->buildErrorList(doc);
		SwitchWin();
		QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
		ScribusWin* swin;
		for ( int i = 0; i < static_cast<int>(windows.count()); ++i )
		{
			swin = dynamic_cast<ScribusWin *>(windows.at(i)->widget());
			if (swin)
			{
				if (swin==ActWin && doc->masterPageMode())
					swin->setMasterPagesPaletteShown(true);
				else
					swin->setMasterPagesPaletteShown(false);
			}
		}
		//if (doc->masterPageMode())
		//	ActWin->setMasterPagesPaletteShown(true);
		view->requestMode(doc->appMode);
	}
	view->setFocus();
//	mdiArea->setScrollBarsEnabled(!(w->isMaximized()));
	scrActions["viewShowMargins"]->setChecked(doc->guidesPrefs().marginsShown);
	scrActions["viewShowBleeds"]->setChecked(doc->guidesPrefs().showBleed);
	scrActions["viewShowFrames"]->setChecked(doc->guidesPrefs().framesShown);
	scrActions["viewShowLayerMarkers"]->setChecked(doc->guidesPrefs().layerMarkersShown);
	scrActions["viewShowGrid"]->setChecked(doc->guidesPrefs().gridShown);
	scrActions["viewShowGuides"]->setChecked(doc->guidesPrefs().guidesShown);
	scrActions["viewShowColumnBorders"]->setChecked(doc->guidesPrefs().colBordersShown);
	scrActions["viewShowBaseline"]->setChecked(doc->guidesPrefs().baselineGridShown);
	scrActions["viewShowImages"]->setChecked(doc->guidesPrefs().showPic);
	scrActions["viewShowTextChain"]->setChecked(doc->guidesPrefs().linkShown);
	scrActions["viewShowTextControls"]->setChecked(doc->guidesPrefs().showControls);
	scrActions["viewShowRulers"]->setChecked(doc->guidesPrefs().rulersShown);
	scrActions["viewRulerMode"]->setChecked(doc->guidesPrefs().rulerMode);
	scrActions["extrasGenerateTableOfContents"]->setEnabled(doc->hasTOCSetup());
	if (!doc->masterPageMode())
		pagePalette->Rebuild();
	outlinePalette->setDoc(doc);
	if (outlinePalette->isVisible())
	{
		outlinePalette->BuildTree(false);
		outlinePalette->reopenTree();
	}
	RestoreBookMarks();
	if (!doc->isLoading())
	{
		if (doc->m_Selection->count() != 0)
		{
			HaveNewSel(doc->m_Selection->itemAt(0)->itemType());
			doc->m_Selection->itemAt(0)->emitAllToGUI();
		}
		else
			HaveNewSel(-1);
	}
	tocGenerator->setDoc(doc);
	styleManager->setDoc(doc);
	symbolPalette->setDoc(doc);
	modeToolBar->Angle->setValue(doc->itemToolPrefs().calligrapicPenAngle);
	modeToolBar->PWidth->setValue(doc->itemToolPrefs().calligrapicPenWidth);
	// Give plugins a chance to react on changing the current document
	PluginManager& pluginManager(PluginManager::instance());
	QStringList pluginNames(pluginManager.pluginNames(false));
	ScPlugin* plugin;
	QString pName;
	for (int i = 0; i < pluginNames.count(); ++i)
	{
		pName = pluginNames.at(i);
		plugin = pluginManager.getPlugin(pName, true);
		Q_ASSERT(plugin); // all the returned names should represent loaded plugins
		plugin->setDoc(doc);
	}
}

void ScribusMainWindow::windowsMenuActivated( int id )
{
	QMdiSubWindow* windowWidget = mdiArea->subWindowList().at( id );
	if ( windowWidget )
		windowWidget->showNormal();
	newActWin(windowWidget);
}


void ScribusMainWindow::SwitchWin()
{
	updateActiveWindowCaption(doc->DocName);
// 	scrActions["shade100"]->setChecked(true);
	propertiesPalette->setDoc(doc);
	//propertiesPalette->Cpal->displayGradient(0);
	pagePalette->setView(view);
	layerPalette->setDoc(doc);
	guidePalette->setDoc(doc);
	charPalette->setDoc(doc);
	outlinePalette->setDoc(doc);
	symbolPalette->setDoc(doc);
	rebuildLayersList();
	view->updateLayerMenu();
	view->setLayerMenuText(doc->activeLayerName());
	//Do not set this!, it doesnt get valid pointers unless its in EditClip mode and its not
	//if we are switching windows #4357
	//nodePalette->setDoc(doc, view);
	slotChangeUnit(doc->unitIndex(), false);
/*	FIXME: check if this is really superflous now
	if (doc->appMode == modeEditClip)
	{
		doc->appMode = modeNormal;
		view->requestMode(submodeEndNodeEdit);
	} */
	scrActions["fileClose"]->setEnabled(true);
	if (doc->masterPageMode() || doc->symbolEditMode())
	{
		scrActions["pageInsert"]->setEnabled(false);
		scrActions["pageDelete"]->setEnabled(false);
		scrActions["pageCopy"]->setEnabled(false);
		scrActions["pageMove"]->setEnabled(false);
		scrActions["pageApplyMasterPage"]->setEnabled(false);
		scrActions["pageCopyToMasterPage"]->setEnabled(false);
		scrActions["editMasterPages"]->setEnabled(false);
		scrActions["fileNew"]->setEnabled(false);
		scrActions["fileNewFromTemplate"]->setEnabled(false);
		if (doc->symbolEditMode())
		{
			scrActions["fileCollect"]->setEnabled(false);
			scrActions["fileSaveAs"]->setEnabled(false);
			scrMenuMgr->setMenuEnabled("FileExport", false);
			scrActions["fileExportAsEPS"]->setEnabled(false);
			scrActions["fileExportAsPDF"]->setEnabled(false);
			scrActions["fileSave"]->setEnabled(false);
		}
		else
			scrActions["fileSave"]->setEnabled(true);
		scrActions["fileOpen"]->setEnabled(false);
		scrActions["fileRevert"]->setEnabled(false);
		scrMenuMgr->setMenuEnabled("FileOpenRecent", false);
		pagePalette->enablePalette(false);
		scrActions["toolsPDFPushButton"]->setEnabled(false);
		scrActions["toolsPDFTextField"]->setEnabled(false);
		scrActions["toolsPDFCheckBox"]->setEnabled(false);
		scrActions["toolsPDFComboBox"]->setEnabled(false);
		scrActions["toolsPDFListBox"]->setEnabled(false);
		scrActions["toolsPDFAnnotText"]->setEnabled(false);
#ifdef HAVE_OSG
		scrActions["toolsPDFAnnot3D"]->setEnabled(false);
#endif
	}
	else
	{
		scrActions["editMasterPages"]->setEnabled(true);
		scrActions["fileNew"]->setEnabled(true);
		scrActions["fileNewFromTemplate"]->setEnabled(true);
		scrActions["fileOpen"]->setEnabled(true);
		scrActions["fileClose"]->setEnabled(true);
		scrActions["fileSave"]->setEnabled(true);
		scrActions["fileRevert"]->setEnabled(false);
		scrMenuMgr->setMenuEnabled("FileOpenRecent", true);

		if (doc->isModified())
			slotDocCh(false);
		else
		{
			bool setter = doc->DocPages.count() > 1 ? true : false;
			scrActions["pageDelete"]->setEnabled(setter);
			scrActions["pageMove"]->setEnabled(setter);
		}
//		scrMenuMgr->setMenuEnabled("Page", true);
		scrActions["pageInsert"]->setEnabled(true);
		scrActions["pageImport"]->setEnabled(true);
		scrActions["pageCopy"]->setEnabled(true);
		scrActions["pageApplyMasterPage"]->setEnabled(true);
		scrActions["pageCopyToMasterPage"]->setEnabled(true);
		scrActions["pageManageGuides"]->setEnabled(true);
		scrActions["pageManageMargins"]->setEnabled(true);

		scrActions["fileSaveAs"]->setEnabled(true);
		scrActions["fileCollect"]->setEnabled(true);
		scrActions["toolsPDFPushButton"]->setEnabled(true);
		scrActions["toolsPDFTextField"]->setEnabled(true);
		scrActions["toolsPDFCheckBox"]->setEnabled(true);
		scrActions["toolsPDFComboBox"]->setEnabled(true);
		scrActions["toolsPDFListBox"]->setEnabled(true);
		scrActions["toolsPDFAnnotText"]->setEnabled(true);
#ifdef HAVE_OSG
		scrActions["toolsPDFAnnot3D"]->setEnabled(true);
#endif
		pagePalette->enablePalette(true);
	}
	scrMenuMgr->setMenuEnabled("ItemLayer", doc->layerCount() > 1);
}

void ScribusMainWindow::HaveNewDoc()
{
	scrActions["filePrint"]->setEnabled(true);
 	scrActions["fileSave"]->setEnabled(true);
	scrActions["fileClose"]->setEnabled(true);
//	scrActions["fileDocSetup"]->setEnabled(true);
	scrActions["fileDocSetup150"]->setEnabled(true);
	scrActions["fileRevert"]->setEnabled(false);
	scrActions["fileCollect"]->setEnabled(true);
	scrActions["fileSaveAs"]->setEnabled(true);
	scrMenuMgr->setMenuEnabled("FileImport", true);
	scrMenuMgr->setMenuEnabled("FileExport", true);
	scrActions["fileExportAsEPS"]->setEnabled(true);
	scrActions["fileExportAsPDF"]->setEnabled(true);
	scrActions["pageImport"]->setEnabled(true);
	//scrActions["toolsPreflightVerifier"]->setEnabled(true);

	if ( ScCore->haveGS() || ScCore->isWinGUI() )
		scrActions["PrintPreview"]->setEnabled(true);

	if (scrActions["SaveAsDocumentTemplate"])
		scrActions["SaveAsDocumentTemplate"]->setEnabled(true);

	scrActions["editCut"]->setEnabled(false);
	scrActions["editCopy"]->setEnabled(false);
	scrActions["editPaste"]->setEnabled((ScMimeData::clipboardHasScribusData()) || (scrapbookPalette->tempBView->objectMap.count() > 0));
	scrMenuMgr->setMenuEnabled("EditPasteRecent", scrapbookPalette->tempBView->objectMap.count() > 0);
	scrMenuMgr->setMenuEnabled("EditContents", false);
	scrActions["editCopyContents"]->setEnabled(false);
	scrActions["editPasteContents"]->setEnabled(false);
	scrActions["editPasteContentsAbs"]->setEnabled(false);
	scrActions["editSelectAll"]->setEnabled(true);
	scrActions["editSelectAllOnLayer"]->setEnabled(true);
	scrActions["editDeselectAll"]->setEnabled(false);
	scrActions["editReplaceColors"]->setEnabled(true);
//	scrActions["editPatterns"]->setEnabled(true);
//	scrActions["editGradients"]->setEnabled(true);
 	scrActions["editStyles"]->setEnabled(true);
	scrActions["editMasterPages"]->setEnabled(true);
	scrActions["editJavascripts"]->setEnabled(true);

//	scrMenuMgr->setMenuEnabled("View", true);
	scrActions["viewFitInWindow"]->setEnabled(true);
	scrActions["viewFitWidth"]->setEnabled(true);
	scrActions["viewFit50"]->setEnabled(true);
	scrActions["viewFit75"]->setEnabled(true);
	scrActions["viewFit100"]->setEnabled(true);
	scrActions["viewFit200"]->setEnabled(true);
	scrActions["viewFit400"]->setEnabled(true);

	scrActions["viewSnapToGrid"]->setChecked(doc->useRaster);
	scrActions["viewSnapToGuides"]->setChecked(doc->SnapGuides);
	scrActions["viewShowRulers"]->setEnabled(true);

	scrMenuMgr->setMenuEnabled("Insert", true);
	scrActions["insertFrame"]->setEnabled(true);
	//scrMenuMgr->setMenuEnabled("Windows", true);
//	scrMenuMgr->setMenuEnabled("Page", true);
	scrMenuMgr->setMenuEnabled("Extras", true);

	scrActions["toolsSelect"]->setEnabled(true);
	scrActions["toolsZoom"]->setEnabled(true);
	scrActions["toolsInsertTextFrame"]->setEnabled(true);
	scrActions["toolsInsertImageFrame"]->setEnabled(true);
	scrActions["toolsInsertTableFrame"]->setEnabled(true);
	scrActions["toolsInsertShape"]->setEnabled(true);
	scrActions["toolsInsertLine"]->setEnabled(true);
	scrActions["toolsInsertBezier"]->setEnabled(true);
	scrActions["toolsInsertFreehandLine"]->setEnabled(true);
	scrActions["toolsInsertCalligraphicLine"]->setEnabled(true);
	scrActions["toolsInsertPolygon"]->setEnabled(true);
	scrActions["toolsInsertArc"]->setEnabled(true);
	scrActions["toolsInsertSpiral"]->setEnabled(true);
	scrActions["toolsInsertRenderFrame"]->setEnabled(true);
	scrActions["toolsMeasurements"]->setEnabled(true);
	scrActions["toolsEyeDropper"]->setEnabled(true);
	scrActions["toolsPDFPushButton"]->setEnabled(true);
	scrActions["toolsPDFTextField"]->setEnabled(true);
	scrActions["toolsPDFCheckBox"]->setEnabled(true);
	scrActions["toolsPDFComboBox"]->setEnabled(true);
	scrActions["toolsPDFListBox"]->setEnabled(true);
	scrActions["toolsPDFAnnotText"]->setEnabled(true);
	scrActions["toolsPDFAnnotLink"]->setEnabled(true);
#ifdef HAVE_OSG
	scrActions["toolsPDFAnnot3D"]->setEnabled(true);
#endif

	bool setter = doc->DocPages.count() > 1 ? true : false;
	scrActions["pageDelete"]->setEnabled(setter);
	scrActions["pageMove"]->setEnabled(setter);
	scrActions["pageInsert"]->setEnabled(true);
	scrActions["pageImport"]->setEnabled(true);
	scrActions["pageCopy"]->setEnabled(true);
	scrActions["pageApplyMasterPage"]->setEnabled(true);
	scrActions["pageCopyToMasterPage"]->setEnabled(true);
	scrActions["pageManageGuides"]->setEnabled(true);
	scrActions["pageManageMargins"]->setEnabled(true);
	scrMenuMgr->setMenuEnabled("ItemLayer", doc->layerCount() > 1);

	//Update palettes
	updateActiveWindowCaption(doc->DocName);
// 	scrActions["shade100"]->setChecked(true);
	propertiesPalette->setDoc(doc);
	symbolPalette->setDoc(doc);
//	propertiesPalette->Cpal->displayGradient(0);
//	propertiesPalette->updateColorList();
	pagePalette->setView(view);
	layerPalette->setDoc(doc);
	guidePalette->setDoc(doc);
	charPalette->setDoc(doc);
	outlinePalette->setDoc(doc);
	if (outlinePalette->isVisible())
		outlinePalette->BuildTree();
	rebuildLayersList();
	view->updateLayerMenu();
	view->setLayerMenuText(doc->activeLayerName());
	slotChangeUnit(doc->unitIndex());
	windowsMenuAboutToShow();

	// #9275 : scripter must call HaveNewDoc() in case new doc has been created in a script
	// We may consequently have to call HaveNewDoc several times for the same doc.
	// Use Qt::UniqueConnection here to avoid multiple identical signal connections
	connect(view, SIGNAL(changeUN(int)), this, SLOT(slotChangeUnit(int)), Qt::UniqueConnection);
	connect(view, SIGNAL(changeLA(int)), layerPalette, SLOT(markActiveLayer(int)), Qt::UniqueConnection);
	connect(view->horizRuler, SIGNAL(MarkerMoved(double, double)), this, SLOT(setStatusBarTextPosition(double, double)), Qt::UniqueConnection);
	connect(view->horizRuler, SIGNAL(DocChanged(bool)), this, SLOT(slotDocCh(bool)), Qt::UniqueConnection);
	connect(view, SIGNAL(ClipPo(double, double)), nodePalette, SLOT(SetXY(double, double)), Qt::UniqueConnection);
	connect(view, SIGNAL(PolyOpen()), nodePalette, SLOT(IsOpen()), Qt::UniqueConnection);
	connect(view, SIGNAL(PStatus(int, uint)), nodePalette, SLOT(PolyStatus(int, uint)), Qt::UniqueConnection);
	connect(view, SIGNAL(ItemGeom()), propertiesPalette->xyzPal, SLOT(handleSelectionChanged()), Qt::UniqueConnection);
	connect(view, SIGNAL(ChBMText(PageItem *)), this, SLOT(BookMarkTxT(PageItem *)), Qt::UniqueConnection);
	connect(view, SIGNAL(HaveSel(int)), this, SLOT(HaveNewSel(int)), Qt::UniqueConnection);
//	connect(view, SIGNAL(SetAngle(double)), propertiesPalette, SLOT(setR(double)), Qt::UniqueConnection);
//	connect(view, SIGNAL(SetSizeValue(double)), propertiesPalette, SLOT(setLineWidth(double)), Qt::UniqueConnection);
//	connect(view, SIGNAL(SetLocalValues(double, double, double, double)), propertiesPalette, SLOT(setScaleAndOffset(double, double, double, double)), Qt::UniqueConnection);
//	connect(view, SIGNAL(SetLineArt(Qt::PenStyle, Qt::PenCapStyle, Qt::PenJoinStyle)), propertiesPalette, SLOT( setLIvalue(Qt::PenStyle, Qt::PenCapStyle, Qt::PenJoinStyle)), Qt::UniqueConnection);
//	connect(view, SIGNAL(ItemTextAttr(double)), propertiesPalette, SLOT(setLsp(double)), Qt::UniqueConnection);
//	connect(view, SIGNAL(ItemTextCols(int, double)), propertiesPalette, SLOT(setCols(int, double)), Qt::UniqueConnection);
//	connect(view, SIGNAL(SetDistValues(double, double, double, double)), propertiesPalette, SLOT(setTextToFrameDistances(double, double, double, double)), Qt::UniqueConnection);
	//connect(view, SIGNAL(ItemRadius(double)), propertiesPalette, SLOT(setRR(double)), Qt::UniqueConnection);
//	connect(view, SIGNAL(Amode(int)), this, SLOT(setAppMode(int)), Qt::UniqueConnection);
	connect(view, SIGNAL(PaintingDone()), this, SLOT(slotSelect()), Qt::UniqueConnection);
	connect(view, SIGNAL(DocChanged()), this, SLOT(slotDocCh()), Qt::UniqueConnection);
	//connect(view, SIGNAL(HavePoint(bool, bool)), nodePalette, SLOT(HaveNode(bool, bool)), Qt::UniqueConnection);
	connect(view, SIGNAL(MousePos(double, double)), this, SLOT(setStatusBarMousePosition(double, double)), Qt::UniqueConnection);
	//connect(view, SIGNAL(ItemRadius(double)), propertiesPalette, SLOT(setRR(double)), Qt::UniqueConnection);
	connect(view, SIGNAL(ItemCharStyle(const CharStyle&)), propertiesPalette->textPal, SLOT(updateCharStyle(const CharStyle&)), Qt::UniqueConnection);
//	connect(view, SIGNAL(ItemTextEffects(int)), propertiesPalette, SLOT(setStil(int)), Qt::UniqueConnection);
	connect(view, SIGNAL(ItemTextEffects(int)), this, SLOT(setStyleEffects(int)), Qt::UniqueConnection);
	connect(view, SIGNAL(ItemTextAlign(int))  , this, SLOT(setAlignmentValue(int)), Qt::UniqueConnection);
	connect(view, SIGNAL(HasTextSel()), this, SLOT(EnableTxEdit()), Qt::UniqueConnection);
	connect(view, SIGNAL(HasNoTextSel()), this, SLOT(DisableTxEdit()), Qt::UniqueConnection);
	connect(view, SIGNAL(CopyItem()), this, SLOT(slotEditCopy()), Qt::UniqueConnection);
	connect(view, SIGNAL(CutItem()), this, SLOT(slotEditCut()), Qt::UniqueConnection);
	connect(view, SIGNAL(LoadPic()), this, SLOT(slotGetContent()), Qt::UniqueConnection);
	connect(view, SIGNAL(StatusPic()), this, SLOT(StatusPic()), Qt::UniqueConnection);
	connect(view, SIGNAL(AppendText()), this, SLOT(slotFileAppend()), Qt::UniqueConnection);
	connect(view, SIGNAL(AnnotProps()), this, SLOT(ModifyAnnot()), Qt::UniqueConnection);
	connect(view, SIGNAL(LoadElem(QString, double ,double, bool, bool, ScribusDoc *, ScribusView*)), this, SLOT(slotElemRead(QString, double, double, bool, bool, ScribusDoc *, ScribusView*)), Qt::UniqueConnection);
	connect(view, SIGNAL(AddBM(PageItem *)), this, SLOT(AddBookMark(PageItem *)), Qt::UniqueConnection);
	connect(view, SIGNAL(DelBM(PageItem *)), this, SLOT(DelBookMark(PageItem *)), Qt::UniqueConnection);
	connect(view, SIGNAL(DoGroup()), this, SLOT(GroupObj()), Qt::UniqueConnection);
//	connect(view, SIGNAL(EndNodeEdit()), this, SLOT(ToggleFrameEdit()), Qt::UniqueConnection);
//	connect(view, SIGNAL(LevelChanged(uint )), propertiesPalette, SLOT(setLevel(uint)), Qt::UniqueConnection);
	connect(view, SIGNAL(callGimp()), this, SLOT(callImageEditor()), Qt::UniqueConnection);
}

void ScribusMainWindow::HaveNewSel(int SelectedType)
{
	bool isRaster = false;
	PageItem *currItem = NULL;
	const uint docSelectionCount=doc->m_Selection->count();
	if (SelectedType != -1)
	{
		if (docSelectionCount != 0)
		{
			PageItem *lowestItem = doc->m_Selection->itemAt(0);
			for (uint a=0; a < docSelectionCount; ++a)
			{
				currItem = doc->m_Selection->itemAt(a);
				if (currItem->ItemNr < lowestItem->ItemNr)
					lowestItem = currItem;
			}
			currItem = lowestItem;
			if ((docSelectionCount == 1) && currItem && currItem->asImageFrame())
				isRaster = currItem->isRaster;
//			doc->m_Selection->removeItem(currItem);
//			doc->m_Selection->prependItem(currItem);
//			currItem = doc->m_Selection->itemAt(0);
			assert(currItem);
//			if (!currItem)
//				SelectedType=-1;
		}
		else
			SelectedType = -1;
	}
	else if (docSelectionCount > 0)
	{
		currItem = doc->m_Selection->itemAt(0);
	}
	assert (docSelectionCount == 0 || currItem != NULL); // help coverity analysis
	if (docSelectionCount == 0)
		setStatusBarInfoText("");
	else if (docSelectionCount == 1)
		setStatusBarInfoText( tr("1 Object selected"));
	else
		setStatusBarInfoText( tr("%1 Objects selected").arg(docSelectionCount));
	actionManager->disconnectNewSelectionActions();
	scrActions["editSelectAllOnLayer"]->setEnabled(true);
	scrActions["editDeselectAll"]->setEnabled(SelectedType != -1);
	scrActions["itemDetachTextFromPath"]->setEnabled(false);
	charPalette->setEnabled(false, 0);
	scrActions["itemUpdateImage"]->setEnabled(SelectedType==PageItem::ImageFrame && (currItem->PictureIsAvailable || currItem->asLatexFrame()));
	scrActions["itemAdjustFrameToImage"]->setEnabled(SelectedType==PageItem::ImageFrame && currItem->PictureIsAvailable && !currItem->isTableItem);
	scrActions["itemAdjustImageToFrame"]->setEnabled(SelectedType==PageItem::ImageFrame && currItem->PictureIsAvailable);
	scrActions["itemExtendedImageProperties"]->setEnabled(SelectedType==PageItem::ImageFrame && currItem->PictureIsAvailable && currItem->pixm.imgInfo.valid);
	scrActions["itemToggleInlineImage"]->setEnabled(SelectedType==PageItem::ImageFrame && currItem->PictureIsAvailable);
	scrMenuMgr->setMenuEnabled("ItemPreviewSettings", SelectedType==PageItem::ImageFrame);
	scrActions["itemImageIsVisible"]->setEnabled(SelectedType==PageItem::ImageFrame);
	scrActions["itemPreviewLow"]->setEnabled(SelectedType==PageItem::ImageFrame);
	scrActions["itemPreviewNormal"]->setEnabled(SelectedType==PageItem::ImageFrame);
	scrActions["itemPreviewFull"]->setEnabled(SelectedType==PageItem::ImageFrame);
	scrActions["styleImageEffects"]->setEnabled(SelectedType==PageItem::ImageFrame && isRaster);
	scrActions["editCopyContents"]->setEnabled(SelectedType==PageItem::ImageFrame && currItem->PictureIsAvailable);
	scrActions["editPasteContents"]->setEnabled(SelectedType==PageItem::ImageFrame);
	scrActions["editPasteContentsAbs"]->setEnabled(SelectedType==PageItem::ImageFrame);
	scrActions["editEditWithImageEditor"]->setEnabled(SelectedType==PageItem::ImageFrame && currItem->PictureIsAvailable && currItem->isRaster);
#ifdef HAVE_OSG
	scrActions["editEditRenderSource"]->setEnabled(SelectedType==PageItem::ImageFrame && currItem && (currItem->asLatexFrame() || currItem->asOSGFrame()));
#else
	scrActions["editEditRenderSource"]->setEnabled(SelectedType==PageItem::ImageFrame && currItem && (currItem->asLatexFrame()));
#endif
	if (SelectedType!=PageItem::ImageFrame)
	{
		scrActions["itemImageIsVisible"]->setChecked(false);
		scrActions["itemPreviewLow"]->setChecked(false);
		scrActions["itemPreviewNormal"]->setChecked(false);
		scrActions["itemPreviewFull"]->setChecked(false);
	}
	if ((SelectedType==-1) || (SelectedType!=-1 && !currItem->asTextFrame()))
		enableTextActions(&scrActions, false);
	scrActions["insertSampleText"]->setEnabled(false);

	view->horizRuler->textMode(false);
	view->horizRuler->update();
	switch (SelectedType)
	{
	case -1: // None
		scrActions["fileImportText"]->setEnabled(false);
		scrActions["fileImportText2"]->setEnabled(false);
		scrActions["fileImportImage"]->setEnabled(false);
		scrActions["fileImportAppendText"]->setEnabled(false);
		scrActions["fileExportText"]->setEnabled(false);
		scrActions["itemDuplicate"]->setEnabled(false);
		scrActions["itemMulDuplicate"]->setEnabled(false);
		scrActions["itemTransform"]->setEnabled(false);
		scrActions["itemDelete"]->setEnabled(false);
		scrActions["itemRaise"]->setEnabled(false);
		scrActions["itemLower"]->setEnabled(false);
		scrActions["itemRaiseToTop"]->setEnabled(false);
		scrActions["itemLowerToBottom"]->setEnabled(false);
//		scrActions["itemSendToScrapbook"]->setEnabled(false);
		scrMenuMgr->setMenuEnabled("itemSendToScrapbook", false);
		scrActions["itemSendToPattern"]->setEnabled(false);
		scrActions["itemAdjustFrameToImage"]->setEnabled(false);
		scrActions["itemAdjustImageToFrame"]->setEnabled(false);
		scrActions["itemExtendedImageProperties"]->setEnabled(false);
		scrActions["itemUpdateImage"]->setEnabled(false);
		scrActions["itemPreviewLow"]->setEnabled(false);
		scrActions["itemPreviewNormal"]->setEnabled(false);
		scrActions["itemPreviewFull"]->setEnabled(false);
		scrActions["itemAttributes"]->setEnabled(false);
		scrActions["itemPreviewLow"]->setEnabled(false);
//		scrMenuMgr->setMenuEnabled("Item", false);
		//scrMenuMgr->setMenuEnabled("ItemShapes", false);
//		scrMenuMgr->setMenuEnabled("ItemConvertTo", false);
		scrMenuMgr->setMenuEnabled("EditContents", false);
		scrActions["itemConvertToBezierCurve"]->setEnabled(false);
		scrActions["itemConvertToImageFrame"]->setEnabled(false);
		scrActions["itemConvertToOutlines"]->setEnabled(false);
		scrActions["itemConvertToPolygon"]->setEnabled(false);
		scrActions["itemConvertToTextFrame"]->setEnabled(false);
		scrActions["itemLock"]->setEnabled(false);
		scrActions["itemLockSize"]->setEnabled(false);
		scrActions["itemPrintingEnabled"]->setEnabled(false);
		scrActions["editCut"]->setEnabled(false);
		scrActions["editCopy"]->setEnabled(false);
		scrActions["editCopyContents"]->setEnabled(false);
		scrActions["editSearchReplace"]->setEnabled(false);
		scrActions["extrasHyphenateText"]->setEnabled(false);
		scrActions["extrasDeHyphenateText"]->setEnabled(false);

		scrActions["toolsUnlinkTextFrame"]->setEnabled(false);
		scrActions["toolsLinkTextFrame"]->setEnabled(false);
		scrActions["toolsEditContents"]->setEnabled(false);
		scrActions["toolsEditWithStoryEditor"]->setEnabled(false);
		scrActions["toolsRotate"]->setEnabled(false);
		scrActions["toolsCopyProperties"]->setEnabled(false);
		//CB 061005 moved to cpalette choosegrad
		//propertiesPalette->Cpal->gradientQCombo->setCurrentItem(0);
		outlinePalette->slotShowSelect(doc->currentPageNumber(), -1);
		propertiesPalette->setGradientEditMode(false);
		break;
	case PageItem::ImageFrame: //Image Frame
		scrActions["fileImportAppendText"]->setEnabled(false);
		scrActions["fileImportText"]->setEnabled(false);
		scrActions["fileImportText2"]->setEnabled(false);
		scrActions["fileImportImage"]->setEnabled(true);
		scrActions["editCut"]->setEnabled(true);
		scrActions["editCopy"]->setEnabled(true);
		scrMenuMgr->setMenuEnabled("EditContents", true);
		scrActions["editClearContents"]->setEnabled(true);
		scrActions["editSearchReplace"]->setEnabled(false);
		scrActions["extrasHyphenateText"]->setEnabled(false);
		scrActions["extrasDeHyphenateText"]->setEnabled(false);
//		scrMenuMgr->setMenuEnabled("Item", true);
		scrActions["itemDuplicate"]->setEnabled(true);
		scrActions["itemMulDuplicate"]->setEnabled(true);
		scrActions["itemTransform"]->setEnabled(true);
		scrActions["itemDelete"]->setEnabled(true);
		scrActions["itemRaise"]->setEnabled(true);
		scrActions["itemLower"]->setEnabled(true);
		scrActions["itemRaiseToTop"]->setEnabled(true);
		scrActions["itemLowerToBottom"]->setEnabled(true);
//		scrActions["itemSendToScrapbook"]->setEnabled(true);
		scrMenuMgr->setMenuEnabled("itemSendToScrapbook", true);
		scrActions["itemSendToPattern"]->setEnabled(true);
		scrActions["itemAdjustFrameToImage"]->setEnabled(true);
		scrActions["itemAdjustImageToFrame"]->setEnabled(true);
		scrActions["itemExtendedImageProperties"]->setEnabled(true);
		scrActions["itemUpdateImage"]->setEnabled(true);
		scrActions["itemPreviewLow"]->setEnabled(true);
		scrActions["itemPreviewNormal"]->setEnabled(true);
		scrActions["itemPreviewFull"]->setEnabled(true);
		scrActions["itemAttributes"]->setEnabled(true);
		scrActions["itemPreviewLow"]->setEnabled(true);
		//scrMenuMgr->setMenuEnabled("ItemShapes", !(currItem->isTableItem && currItem->isSingleSel));
//		scrMenuMgr->setMenuEnabled("ItemConvertTo", true);
		scrActions["itemConvertToBezierCurve"]->setEnabled(false);
		scrActions["itemConvertToImageFrame"]->setEnabled(false);
		scrActions["itemConvertToOutlines"]->setEnabled(false);
		scrActions["itemConvertToPolygon"]->setEnabled(!currItem->isTableItem && doc->appMode != modeEdit);
		scrActions["itemConvertToTextFrame"]->setEnabled(doc->appMode != modeEdit);
		scrActions["toolsUnlinkTextFrame"]->setEnabled(false);
		scrActions["toolsLinkTextFrame"]->setEnabled(false);
		scrActions["toolsEditContents"]->setEnabled(currItem->ScaleType);
		scrActions["toolsEditWithStoryEditor"]->setEnabled(false);
		scrActions["toolsRotate"]->setEnabled(true);
		scrActions["toolsCopyProperties"]->setEnabled(true);
		scrActions["itemImageIsVisible"]->setChecked(currItem->imageShown());
		scrActions["itemToggleInlineImage"]->setChecked(currItem->isImageInline());
		scrActions["itemPreviewLow"]->setChecked(currItem->pixm.imgInfo.lowResType==scrActions["itemPreviewLow"]->actionInt());
		scrActions["itemPreviewNormal"]->setChecked(currItem->pixm.imgInfo.lowResType==scrActions["itemPreviewNormal"]->actionInt());
		scrActions["itemPreviewFull"]->setChecked(currItem->pixm.imgInfo.lowResType==scrActions["itemPreviewFull"]->actionInt());

		break;
	case PageItem::TextFrame: //Text Frame
		scrActions["fileImportText"]->setEnabled(true);
		scrActions["fileImportText2"]->setEnabled(true);
		scrActions["fileImportImage"]->setEnabled(false);
		scrActions["fileImportAppendText"]->setEnabled(true);
		scrActions["fileExportText"]->setEnabled(true);
		scrActions["editCut"]->setEnabled(true);
		scrActions["editCopy"]->setEnabled(true);
		scrMenuMgr->setMenuEnabled("EditContents", true);
		scrActions["editClearContents"]->setEnabled(true);
		scrActions["editSearchReplace"]->setEnabled(currItem->itemText.length() != 0);
		scrActions["extrasHyphenateText"]->setEnabled(true);
		scrActions["extrasDeHyphenateText"]->setEnabled(true);
		//		scrMenuMgr->setMenuEnabled("Item", true);
		scrActions["itemDuplicate"]->setEnabled(true);
		scrActions["itemMulDuplicate"]->setEnabled(true);
		scrActions["itemTransform"]->setEnabled(true);
		scrActions["itemDelete"]->setEnabled(true);
		scrActions["itemRaise"]->setEnabled(true);
		scrActions["itemLower"]->setEnabled(true);
		scrActions["itemRaiseToTop"]->setEnabled(true);
		scrActions["itemLowerToBottom"]->setEnabled(true);
//		scrActions["itemSendToScrapbook"]->setEnabled(true);
		scrMenuMgr->setMenuEnabled("itemSendToScrapbook", true);
		scrActions["itemSendToPattern"]->setEnabled(true);
		scrActions["itemAdjustFrameToImage"]->setEnabled(true);
		scrActions["itemAdjustImageToFrame"]->setEnabled(true);
		scrActions["itemExtendedImageProperties"]->setEnabled(true);
		scrActions["itemUpdateImage"]->setEnabled(true);
		scrActions["itemPreviewLow"]->setEnabled(true);
		scrActions["itemPreviewNormal"]->setEnabled(true);
		scrActions["itemPreviewFull"]->setEnabled(true);
		scrActions["itemAttributes"]->setEnabled(true);
		scrActions["itemPreviewLow"]->setEnabled(true);
		//scrMenuMgr->setMenuEnabled("ItemShapes", !(currItem->isTableItem && currItem->isSingleSel));
//		scrMenuMgr->setMenuEnabled("ItemConvertTo", true);
		scrActions["itemConvertToBezierCurve"]->setEnabled(false);
		scrActions["itemConvertToImageFrame"]->setEnabled(doc->appMode != modeEdit);
		scrActions["itemConvertToOutlines"]->setEnabled(!currItem->isTableItem && doc->appMode != modeEdit);
		scrActions["itemConvertToPolygon"]->setEnabled(!currItem->isTableItem && doc->appMode != modeEdit);
		scrActions["itemConvertToTextFrame"]->setEnabled(false);

		scrActions["toolsRotate"]->setEnabled(true);
		scrActions["toolsCopyProperties"]->setEnabled(true);
		scrActions["toolsEditWithStoryEditor"]->setEnabled(true);
		scrActions["insertSampleText"]->setEnabled(true);
		if ((currItem->nextInChain() != 0) || (currItem->prevInChain() != 0))
		{
			scrActions["itemConvertToBezierCurve"]->setEnabled(false);
			scrActions["itemConvertToImageFrame"]->setEnabled(false);
//			scrActions["itemConvertToOutlines"]->setEnabled(false);
			scrActions["itemConvertToPolygon"]->setEnabled(false);
			scrActions["itemConvertToTextFrame"]->setEnabled(false);
			scrActions["toolsUnlinkTextFrame"]->setEnabled(true);
			// FIXME: once there's one itemtext per story, always enable editcontents
			if ((currItem->prevInChain() != 0) && (currItem->itemText.length() == 0))
				scrActions["toolsEditContents"]->setEnabled(false);
			else
				scrActions["toolsEditContents"]->setEnabled(true);
		}
		else
		{
			scrActions["toolsEditContents"]->setEnabled(true);
			scrActions["toolsUnlinkTextFrame"]->setEnabled(false);
		}
		if (currItem->nextInChain() == 0)
			scrActions["toolsLinkTextFrame"]->setEnabled(true);
//		if (doc->masterPageMode())
//			scrActions["toolsLinkTextFrame"]->setEnabled(false);
		if (doc->appMode == modeEdit)
		{
			setTBvals(currItem);
			scrActions["editSelectAll"]->setEnabled(true);
			scrActions["editSelectAllOnLayer"]->setEnabled(false);
			charPalette->setEnabled(true, currItem);
			if (currItem->asTextFrame())
				enableTextActions(&scrActions, true, currItem->currentStyle().charStyle().font().scName());
			view->horizRuler->setItem(currItem);
			view->horizRuler->update();
		}
		else
		{
			doc->currentStyle = currItem->itemText.defaultStyle();
			propertiesPalette->textPal->displayParStyle(doc->currentStyle.parent());
			propertiesPalette->textPal->displayCharStyle(doc->currentStyle.charStyle().parent());
			emit TextStyle(doc->currentStyle);
			// to go: (av)
			propertiesPalette->textPal->updateStyle(doc->currentStyle);
			setStyleEffects(doc->currentStyle.charStyle().effects());
		}

//		doc->docParagraphStyles[0].setLineSpacingMode(static_cast<ParagraphStyle::LineSpacingMode>(currItem->lineSpacingMode()));
//		doc->docParagraphStyles[0].setLineSpacing(currItem->lineSpacing());
//		doc->docParagraphStyles[0].setAlignment(currItem->textAlignment);

		break;
	case PageItem::PathText: //Path Text
		scrActions["fileImportText"]->setEnabled(true);
		scrActions["fileImportText2"]->setEnabled(true);
		scrActions["fileImportImage"]->setEnabled(false);
		scrActions["fileImportAppendText"]->setEnabled(true);
		scrActions["fileExportText"]->setEnabled(true);
		scrActions["editCut"]->setEnabled(true);
		scrActions["editCopy"]->setEnabled(true);
		scrActions["editClearContents"]->setEnabled(false);
		scrActions["editSearchReplace"]->setEnabled(false);
		scrActions["extrasHyphenateText"]->setEnabled(false);
		scrActions["extrasDeHyphenateText"]->setEnabled(false);
		//		scrMenuMgr->setMenuEnabled("Item", true);
		scrActions["itemDuplicate"]->setEnabled(true);
		scrActions["itemMulDuplicate"]->setEnabled(true);
		scrActions["itemTransform"]->setEnabled(true);
		scrActions["itemDelete"]->setEnabled(true);
		scrActions["itemRaise"]->setEnabled(true);
		scrActions["itemLower"]->setEnabled(true);
		scrActions["itemRaiseToTop"]->setEnabled(true);
		scrActions["itemLowerToBottom"]->setEnabled(true);
//		scrActions["itemSendToScrapbook"]->setEnabled(true);
		scrMenuMgr->setMenuEnabled("itemSendToScrapbook", true);
		scrActions["itemSendToPattern"]->setEnabled(true);
		scrActions["itemAdjustFrameToImage"]->setEnabled(true);
		scrActions["itemAdjustImageToFrame"]->setEnabled(true);
		scrActions["itemExtendedImageProperties"]->setEnabled(true);
		scrActions["itemUpdateImage"]->setEnabled(true);
		scrActions["itemPreviewLow"]->setEnabled(true);
		scrActions["itemPreviewNormal"]->setEnabled(true);
		scrActions["itemPreviewFull"]->setEnabled(true);
		scrActions["itemAttributes"]->setEnabled(true);
		scrActions["itemPreviewLow"]->setEnabled(true);
		//scrMenuMgr->setMenuEnabled("ItemShapes", false);
		scrActions["itemDetachTextFromPath"]->setEnabled(true);
//		scrMenuMgr->setMenuEnabled("ItemConvertTo", true);
		scrActions["itemConvertToBezierCurve"]->setEnabled(false);
		scrActions["itemConvertToImageFrame"]->setEnabled(false);
		scrActions["itemConvertToOutlines"]->setEnabled(true);
		scrActions["itemConvertToPolygon"]->setEnabled(false);
		scrActions["itemConvertToTextFrame"]->setEnabled(false);

		scrActions["toolsRotate"]->setEnabled(true);
		scrActions["toolsCopyProperties"]->setEnabled(true);
		scrActions["toolsEditContents"]->setEnabled(false);
		scrActions["toolsEditWithStoryEditor"]->setEnabled(true);
		scrActions["toolsLinkTextFrame"]->setEnabled(false);
		scrActions["toolsUnlinkTextFrame"]->setEnabled(false);
		if (doc->appMode == modeEdit)
			setTBvals(currItem);
		else
		{
			doc->currentStyle = currItem->itemText.defaultStyle();
			propertiesPalette->textPal->displayParStyle(doc->currentStyle.parent());
			propertiesPalette->textPal->displayCharStyle(doc->currentStyle.charStyle().parent());
			emit TextStyle(doc->currentStyle);
			// to go: (av)
			propertiesPalette->textPal->updateStyle(doc->currentStyle);
			setStyleEffects(doc->currentStyle.charStyle().effects());
		}
		break;
	default:
		scrActions["fileImportText"]->setEnabled(false);
		scrActions["fileImportText2"]->setEnabled(false);
		scrActions["fileImportImage"]->setEnabled(false);
		scrActions["fileImportAppendText"]->setEnabled(false);
		scrActions["fileExportText"]->setEnabled(false);
		scrActions["editCut"]->setEnabled(true);
		scrActions["editCopy"]->setEnabled(true);
		scrMenuMgr->setMenuEnabled("EditContents", false);
		scrActions["editClearContents"]->setEnabled(false);
		scrActions["editSearchReplace"]->setEnabled(false);

		scrActions["extrasHyphenateText"]->setEnabled(false);
		scrActions["extrasDeHyphenateText"]->setEnabled(false);
		//		scrMenuMgr->setMenuEnabled("Item", true);
		scrActions["itemDuplicate"]->setEnabled(true);
		scrActions["itemMulDuplicate"]->setEnabled(true);
		scrActions["itemTransform"]->setEnabled(true);
		scrActions["itemDelete"]->setEnabled(true);
		scrActions["itemRaise"]->setEnabled(true);
		scrActions["itemLower"]->setEnabled(true);
		scrActions["itemRaiseToTop"]->setEnabled(true);
		scrActions["itemLowerToBottom"]->setEnabled(true);
//		scrActions["itemSendToScrapbook"]->setEnabled(true);
		scrMenuMgr->setMenuEnabled("itemSendToScrapbook", true);
		scrActions["itemSendToPattern"]->setEnabled(true);
		scrActions["itemAdjustFrameToImage"]->setEnabled(false);
		scrActions["itemAdjustImageToFrame"]->setEnabled(false);
		scrActions["itemExtendedImageProperties"]->setEnabled(false);
		scrActions["itemUpdateImage"]->setEnabled(false);
		scrActions["itemPreviewLow"]->setEnabled(false);
		scrActions["itemPreviewNormal"]->setEnabled(false);
		scrActions["itemPreviewFull"]->setEnabled(true);
		scrActions["itemAttributes"]->setEnabled(true);
		scrActions["itemPreviewLow"]->setEnabled(true);
		if (SelectedType == PageItem::Polygon) //Polygon
		{
			scrMenuMgr->setMenuEnabled("ItemConvertTo", true);
			scrActions["itemConvertToBezierCurve"]->setEnabled(doc->appMode != modeEdit);
			scrActions["itemConvertToImageFrame"]->setEnabled(doc->appMode != modeEdit);
			scrActions["itemConvertToOutlines"]->setEnabled(false);
			scrActions["itemConvertToPolygon"]->setEnabled(false);
			scrActions["itemConvertToTextFrame"]->setEnabled(doc->appMode != modeEdit);
		}
		else if ((SelectedType == PageItem::RegularPolygon) || (SelectedType == PageItem::Arc)) // Regular Polygon + Arc
		{
			scrMenuMgr->setMenuEnabled("ItemConvertTo", true);
			scrActions["itemConvertToBezierCurve"]->setEnabled(doc->appMode != modeEdit);
			scrActions["itemConvertToImageFrame"]->setEnabled(doc->appMode != modeEdit);
			scrActions["itemConvertToOutlines"]->setEnabled(false);
			scrActions["itemConvertToPolygon"]->setEnabled(doc->appMode != modeEdit);
			scrActions["itemConvertToTextFrame"]->setEnabled(doc->appMode != modeEdit);
		}
		else if (SelectedType == PageItem::PolyLine) //Polyline
		{
			scrMenuMgr->setMenuEnabled("ItemConvertTo", true);
			scrActions["itemConvertToBezierCurve"]->setEnabled(false);
			scrActions["itemConvertToImageFrame"]->setEnabled(false);
			scrActions["itemConvertToOutlines"]->setEnabled(false);
			scrActions["itemConvertToPolygon"]->setEnabled(doc->appMode != modeEdit);
			scrActions["itemConvertToTextFrame"]->setEnabled(false);
		}
		else if ((SelectedType == PageItem::Line) || (SelectedType == PageItem::Spiral)) // Line
		{
			scrMenuMgr->setMenuEnabled("ItemConvertTo", true);
			scrActions["itemConvertToBezierCurve"]->setEnabled(true);
			scrActions["itemConvertToImageFrame"]->setEnabled(false);
			scrActions["itemConvertToOutlines"]->setEnabled(false);
			if (SelectedType == PageItem::Spiral)
				scrActions["itemConvertToPolygon"]->setEnabled(doc->appMode != modeEdit);
			else
				scrActions["itemConvertToPolygon"]->setEnabled(false);
			scrActions["itemConvertToTextFrame"]->setEnabled(false);
		}
		else if (SelectedType == PageItem::Symbol)
			scrMenuMgr->setMenuEnabled("ItemConvertTo", false);
		scrActions["toolsEditContents"]->setEnabled(false);
		scrActions["toolsEditWithStoryEditor"]->setEnabled(false);
		scrActions["toolsUnlinkTextFrame"]->setEnabled(false);
		scrActions["toolsLinkTextFrame"]->setEnabled(false);
//		if (SelectedType != 5)
			scrActions["toolsRotate"]->setEnabled(true);
//		else
//			scrActions["toolsRotate"]->setEnabled(false);
		scrActions["toolsCopyProperties"]->setEnabled(true);
		break;
	}
	doc->CurrentSel = SelectedType;
	propertiesPalette->xyzPal->basePointWidget->setCheckedId(doc->RotMode());
	if (docSelectionCount > 1)
	{
		if (!doc->m_Selection->itemsAreSameType())
		{
			scrActions["itemConvertToBezierCurve"]->setEnabled(false);
			scrActions["itemConvertToImageFrame"]->setEnabled(false);
			//scrActions["itemConvertToOutlines"]->setEnabled(false);
			scrActions["itemConvertToPolygon"]->setEnabled(false);
			scrActions["itemConvertToTextFrame"]->setEnabled(false);
		}
		scrActions["editSearchReplace"]->setEnabled(false);

		bool hPoly = false;
		for (uint bx=0; bx < docSelectionCount; ++bx)
		{
			PageItem* bxi=doc->m_Selection->itemAt(bx);
			if ((bxi->asPolygon()) || (bxi->asPolyLine()))
				hPoly = true;
		}
		// It is possible to select objects on different layer using
		// document outline palette. We need to check selected objects
		// are on a common layer before allowing user to group them
		bool objectsOnSameLayer = (doc->m_Selection->objectsLayer() != -1);
		scrActions["itemGroup"]->setEnabled(objectsOnSameLayer);
		scrActions["itemCombinePolygons"]->setEnabled(hPoly);
		if (docSelectionCount == 2)
		{
			//CB swap bx around if currItem is not at 0 index from the lastItem loop at start of havenewsel
			PageItem* bx=doc->m_Selection->itemAt(1);
			if (currItem==bx)
				bx=doc->m_Selection->itemAt(0);

			if ((currItem->asTextFrame() && (bx->asPolygon() || bx->asPolyLine())) || (bx->asTextFrame() && (currItem->asPolygon() || currItem->asPolyLine())))
			{
				if ((currItem->nextInChain() == 0) && (currItem->prevInChain() == 0) && (bx->nextInChain() == 0) && (bx->prevInChain() == 0) && (!currItem->isGroup()) && (!bx->isGroup()))
					scrActions["itemAttachTextToPath"]->setEnabled(true);
			}

		}
	}
	else
	{
		scrActions["itemGroup"]->setEnabled(false);
		scrActions["itemAttachTextToPath"]->setEnabled(false);
		scrActions["itemCombinePolygons"]->setEnabled(false);
	}

	if (docSelectionCount != 0)
	{
		actionManager->setPDFActions(view);
		updateItemLayerList();
		QStringList scrapNames = scrapbookPalette->getOpenScrapbooksNames();
		scrapNames.removeAt(1);
		for( QMap<QString, QPointer<ScrAction> >::Iterator it0s = scrScrapActions.begin(); it0s != scrScrapActions.end(); ++it0s )
			scrMenuMgr->removeMenuItem((*it0s), "itemSendToScrapbook");
		scrScrapActions.clear();
		for (int i = 0; i < scrapNames.count(); i++)
		{
			ScrAction *act = new ScrAction( ScrAction::DataInt, QPixmap(), QPixmap(), scrapNames[i], QKeySequence(), this, i);
			scrScrapActions.insert(scrapNames[i], act);
			scrMenuMgr->addMenuItem(act, "itemSendToScrapbook", true);
			connect(act, SIGNAL(triggeredData(int)), this, SLOT(PutScrap(int)));
		}
		//propertiesPalette->textFlowsAroundFrame->setChecked(currItem->textFlowsAroundFrame());
		propertiesPalette->setTextFlowMode(currItem->textFlowMode());
		scrActions["itemLock"]->setEnabled(true);
		scrActions["itemLockSize"]->setEnabled(true);
		scrActions["itemPrintingEnabled"]->setEnabled(true);
		if (currItem->isGroup())
			scrActions["itemUngroup"]->setEnabled(true);
		else
		{
			scrActions["itemUngroup"]->setEnabled(false);
			scrActions["itemSplitPolygons"]->setEnabled( (currItem->asPolygon()) && (currItem->Segments.count() != 0) );
		}
		if (currItem->locked())
		{
			//scrMenuMgr->setMenuEnabled("ItemShapes", false);
			scrMenuMgr->setMenuEnabled("ItemConvertTo", false);
			scrActions["itemConvertToBezierCurve"]->setEnabled(false);
			scrActions["itemConvertToImageFrame"]->setEnabled(false);
			scrActions["itemConvertToOutlines"]->setEnabled(false);
			scrActions["itemConvertToPolygon"]->setEnabled(false);
			scrActions["itemConvertToTextFrame"]->setEnabled(false);
			scrActions["itemSplitPolygons"]->setEnabled(false);
			scrActions["itemAttachTextToPath"]->setEnabled(false);
			scrActions["itemDetachTextFromPath"]->setEnabled(false);
			scrActions["itemCombinePolygons"]->setEnabled(false);
			scrActions["itemDelete"]->setEnabled(false);
			scrMenuMgr->setMenuEnabled("ItemLevel", false);
			scrActions["itemLowerToBottom"]->setEnabled(false);
			scrActions["itemRaiseToTop"]->setEnabled(false);
			scrActions["itemRaise"]->setEnabled(false);
			scrActions["itemLower"]->setEnabled(false);
//			scrActions["itemSendToScrapbook"]->setEnabled(!(currItem->isTableItem && currItem->isSingleSel));
			scrMenuMgr->setMenuEnabled("itemSendToScrapbook", !(currItem->isTableItem && currItem->isSingleSel));
			scrActions["itemSendToPattern"]->setEnabled(!(currItem->isTableItem && currItem->isSingleSel));
			scrActions["editCut"]->setEnabled(false);
			scrActions["editClearContents"]->setEnabled(false);
			scrActions["toolsRotate"]->setEnabled(false);
		}
		else
		{
			bool setter=!(currItem->isSingleSel && (currItem->isTableItem  || currItem->isGroup()));
			scrMenuMgr->setMenuEnabled("ItemLevel", setter);
			scrActions["itemDuplicate"]->setEnabled(setter);
			scrActions["itemMulDuplicate"]->setEnabled(setter);
			scrActions["itemTransform"]->setEnabled(setter);
			scrActions["itemDelete"]->setEnabled(true);
			scrActions["itemLowerToBottom"]->setEnabled(setter);
			scrActions["itemRaiseToTop"]->setEnabled(setter);
			scrActions["itemRaise"]->setEnabled(setter);
			scrActions["itemLower"]->setEnabled(setter);
//			scrActions["itemSendToScrapbook"]->setEnabled(setter);
			scrMenuMgr->setMenuEnabled("itemSendToScrapbook", setter);
			scrActions["itemSendToPattern"]->setEnabled(setter);
		}
		scrActions["itemLock"]->setChecked(currItem->locked());
		scrActions["itemLockSize"]->setChecked(currItem->sizeLocked());
		scrActions["itemPrintingEnabled"]->setChecked(currItem->printEnabled());
	}

	if (SelectedType != -1)
	{
		//propertiesPalette->setCurrentItem(currItem);
		outlinePalette->slotShowSelect(currItem->OwnPage, currItem->ItemNr);
		actionManager->connectNewSelectionActions(view, doc);
// 		propertiesPalette->handleSelectionChanged();
	}
//	else
//		propertiesPalette->handleSelectionChanged();

	PluginManager& pluginManager(PluginManager::instance());
	QStringList pluginNames(pluginManager.pluginNames(false));
	ScPlugin* plugin;
	ScActionPlugin* ixplug;
	ScrAction* pluginAction = 0;
	QString pName;
	for (int i = 0; i < pluginNames.count(); ++i)
	{
		pName = pluginNames.at(i);
		plugin = pluginManager.getPlugin(pName, true);
		Q_ASSERT(plugin); // all the returned names should represent loaded plugins
		if (plugin->inherits("ScActionPlugin"))
		{
			ixplug = dynamic_cast<ScActionPlugin*>(plugin);
			Q_ASSERT(ixplug);
			ScActionPlugin::ActionInfo ai(ixplug->actionInfo());
			pluginAction = ScCore->primaryMainWindow()->scrActions[ai.name];
			if (pluginAction != 0)
				pluginAction->setEnabled(ixplug->handleSelection(doc, SelectedType));
		}
	}
}

void ScribusMainWindow::slotDocCh(bool /*reb*/)
{
//	if (!doc->isLoading() && docCheckerPalette->isVisible())
//	{
//		scanDocument();
//		docCheckerPalette->buildErrorList(doc);
//	}
	if (!doc->isModified())
		doc->setModified(true);
	updateActiveWindowCaption(doc->DocName + "*");
// 	scrActions["fileSave"]->setEnabled(true);
// 	scrActions["fileSaveAs"]->setEnabled(true);
	if (!doc->masterPageMode())
	{
		if (!doc->symbolEditMode())
		{
			if (doc->hasName)
				scrActions["fileRevert"]->setEnabled(true);
			bool multiPages = doc->DocPages.count() > 1;
			scrActions["pageDelete"]->setEnabled(multiPages);
			scrActions["pageMove"]->setEnabled(multiPages);
			scrActions["fileCollect"]->setEnabled(true);
		}
		scrActions["fileClose"]->setEnabled(true);
	}

	if (outlinePalette->isVisible())
		outlinePalette->BuildTree();
	// Give plugins a chance to react on changes in the current document
	PluginManager& pluginManager(PluginManager::instance());
	QStringList pluginNames(pluginManager.pluginNames(false));
	ScPlugin* plugin;
	QString pName;
	for (int i = 0; i < pluginNames.count(); ++i)
	{
		pName = pluginNames.at(i);
		plugin = pluginManager.getPlugin(pName, true);
		Q_ASSERT(plugin); // all the returned names should represent loaded plugins
		plugin->changedDoc(doc);
	}
}

void ScribusMainWindow::updateRecent(QString fn)
{
	if (RecentDocs.indexOf(fn) == -1)
	{
		RecentDocs.prepend(fn);
		//#9845, why are we tracking doc changes, see also removeRecent:
		//ScCore->fileWatcher->addFile(fn);
	}
	else
	{
		RecentDocs.removeAll(fn);
		RecentDocs.prepend(fn);
	}
	rebuildRecentFileMenu();
}

void ScribusMainWindow::removeRecent(QString fn, bool fromFileWatcher)
{
	if (RecentDocs.indexOf(fn) != -1)
	{
		RecentDocs.removeAll(fn);
		//#9845: if (!fromFileWatcher)
		//#9845:	ScCore->fileWatcher->removeFile(fn);
	}
	rebuildRecentFileMenu();
}

void ScribusMainWindow::removeRecentFromWatcher(QString fn)
{
	removeRecent(fn, true);
}

void ScribusMainWindow::loadRecent(QString fn)
{
	QFileInfo fd(fn);
	if (!fd.exists())
	{
		removeRecent(fn);
		return;
	}
	loadDoc(fn);
}

void ScribusMainWindow::rebuildRecentFileMenu()
{
	for( QMap<QString, QPointer<ScrAction> >::Iterator it = scrRecentFileActions.begin(); it!=scrRecentFileActions.end(); ++it )
		scrMenuMgr->removeMenuItem((*it), "FileOpenRecent");

	scrRecentFileActions.clear();
	uint max = qMin(prefsManager->appPrefs.uiPrefs.recentDocCount, RecentDocs.count());
	QString strippedName, localName;
	for (uint m = 0; m < max; ++m)
	{
		strippedName = localName = QDir::toNativeSeparators(RecentDocs[m]);
		strippedName.remove(QDir::separator());
		scrRecentFileActions.insert(strippedName, new ScrAction(ScrAction::RecentFile, QPixmap(), QPixmap(), QString("&%1 %2").arg(m+1).arg(localName.replace("&","&&")), QKeySequence(), this, 0,0.0,RecentDocs[m]));
		connect( scrRecentFileActions[strippedName], SIGNAL(triggeredData(QString)), this, SLOT(loadRecent(QString)) );
		scrMenuMgr->addMenuItem(scrRecentFileActions[strippedName], "FileOpenRecent", true);
	}
}

void ScribusMainWindow::rebuildRecentPasteMenu()
{
	for( QMap<QString, QPointer<ScrAction> >::Iterator it = scrRecentPasteActions.begin(); it!=scrRecentPasteActions.end(); ++it )
		scrMenuMgr->removeMenuItem((*it), "EditPasteRecent");

	scrRecentPasteActions.clear();
	int max = qMin(prefsManager->appPrefs.scrapbookPrefs.numScrapbookCopies, scrapbookPalette->tempBView->objectMap.count());
	if (max > 0)
	{
		QMap<QString,BibView::Elem>::Iterator it;
		it = scrapbookPalette->tempBView->objectMap.end();
		it--;
		QString strippedName;
		for (int m = 0; m < max; ++m)
		{
			strippedName = it.key();
			QPixmap pm = it.value().Preview;
			scrRecentPasteActions.insert(strippedName, new ScrAction(ScrAction::RecentPaste, pm, QPixmap(), QString("&%1 %2").arg(m+1).arg(strippedName), QKeySequence(), this, 0,0.0,it.key()));
			connect( scrRecentPasteActions[strippedName], SIGNAL(triggeredData(QString)), this, SLOT(pasteRecent(QString)) );
			scrMenuMgr->addMenuItem(scrRecentPasteActions[strippedName], "EditPasteRecent", true);
			it--;
		}
	}
}

void ScribusMainWindow::pasteFromScrapbook(QString fn)
{
	doPasteRecent(scrapbookPalette->activeBView->objectMap[fn].Data);
}

void ScribusMainWindow::pasteRecent(QString fn)
{
	doPasteRecent(scrapbookPalette->tempBView->objectMap[fn].Data);
}

void ScribusMainWindow::doPasteRecent(QString data)
{
	if (HaveDoc)
	{
		QFileInfo fi(data);
		QString formatD(FormatsManager::instance()->extensionListForFormat(FormatsManager::RASTORIMAGES, 1));
		QStringList rasterFiles = formatD.split("|");
		QStringList vectorFiles = LoadSavePlugin::getExtensionsForPreview(FORMATID_ODGIMPORT);
		if (vectorFiles.contains(fi.suffix().toLower()))
		{
			FileLoader *fileLoader = new FileLoader(data);
			int testResult = fileLoader->TestFile();
			delete fileLoader;
			if ((testResult != -1) && (testResult >= FORMATID_ODGIMPORT))
			{
				const FileFormat * fmt = LoadSavePlugin::getFormatById(testResult);
				if( fmt )
				{
					fmt->loadFile(data, LoadSavePlugin::lfUseCurrentPage|LoadSavePlugin::lfInteractive|LoadSavePlugin::lfScripted);
				}
			}
			if (doc->m_Selection->count() > 0)
			{
				double x2, y2, w, h;
				doc->m_Selection->getGroupRect(&x2, &y2, &w, &h);
				doc->moveGroup(doc->currentPage()->xOffset() - x2, doc->currentPage()->yOffset() - y2);
				emit UpdateRequest(reqColorsUpdate|reqTextStylesUpdate|reqLineStylesUpdate);
			}
		}
		else if (rasterFiles.contains(fi.suffix().toLower()))
		{
			int z = doc->itemAdd(PageItem::ImageFrame, PageItem::Unspecified, doc->currentPage()->xOffset(), doc->currentPage()->yOffset(), 1, 1, doc->itemToolPrefs().shapeLineWidth, doc->itemToolPrefs().imageFillColor, CommonStrings::None, true);
			PageItem *b = doc->Items->at(z);
			b->LayerID = doc->activeLayer();
			doc->LoadPict(data, b->ItemNr);
			b->setWidth(static_cast<double>(b->OrigW * 72.0 / b->pixm.imgInfo.xres));
			b->setHeight(static_cast<double>(b->OrigH * 72.0 / b->pixm.imgInfo.yres));
			b->OldB2 = b->width();
			b->OldH2 = b->height();
			b->updateClip();
			b->AdjustPictScale();
		}
		else
		{
			UndoTransaction pasteAction(undoManager->beginTransaction(Um::SelectionGroup, Um::IGroup, Um::Create,"",Um::ICreate));
			view->Deselect(true);
			uint ac = doc->Items->count();
			bool savedAlignGrid = doc->useRaster;
			bool savedAlignGuides = doc->SnapGuides;
			doc->useRaster = false;
			doc->SnapGuides = false;
			slotElemRead(data, doc->currentPage()->xOffset(), doc->currentPage()->yOffset(), true, true, doc, view);
			doc->useRaster = savedAlignGrid;
			doc->SnapGuides = savedAlignGuides;
			Selection tmpSelection(this, false);
			tmpSelection.copy(*doc->m_Selection, true);
			for (int as = ac; as < doc->Items->count(); ++as)
			{
				PageItem* currItem = doc->Items->at(as);
				doc->setRedrawBounding(currItem);
				tmpSelection.addItem(currItem, true);
				if (currItem->isBookmark)
					AddBookMark(currItem);
			}
			doc->m_Selection->copy(tmpSelection, false);
			pasteAction.commit();
		}
		slotDocCh(false);
		doc->regionsChanged()->update(QRectF());
	}
}

void ScribusMainWindow::importVectorFile()
{
	QString fileName = "";
	QStringList formats;
	QString allFormats = tr("All Supported Formats")+" (";
	int fmtCode = FORMATID_ODGIMPORT;
	const FileFormat *fmt = LoadSavePlugin::getFormatById(fmtCode);
	while (fmt != 0)
	{
		if (fmt->load)
		{
			formats.append(fmt->filter);
			int an = fmt->filter.indexOf("(");
			int en = fmt->filter.indexOf(")");
			while (an != -1)
			{
				allFormats += fmt->filter.mid(an+1, en-an-1)+" ";
				an = fmt->filter.indexOf("(", en);
				en = fmt->filter.indexOf(")", an);
			}
		}
		fmtCode++;
		fmt = LoadSavePlugin::getFormatById(fmtCode);
	}
	allFormats += "*.sce *.SCE);;";
	formats.append("Scribus Objects (*.sce *.SCE)");
	qSort(formats);
	allFormats += formats.join(";;");
	PrefsContext* dirs = PrefsManager::instance()->prefsFile->getContext("dirs");
	QString wdir = dirs->get("pastefile", ".");
	CustomFDialog dia(this, wdir, tr("Open"), allFormats, fdExistingFiles);
	if (dia.exec() == QDialog::Accepted)
		fileName = dia.selectedFile();
	else
		return;
	if (!fileName.isEmpty())
	{
		PrefsManager::instance()->prefsFile->getContext("dirs")->set("pastefile", fileName.left(fileName.lastIndexOf("/")));
		QFileInfo fi(fileName);
		QString suffix = fi.suffix().toLower();
		if ((suffix == "sce") || (suffix == "shape"))
		{
			QList<QUrl> urls;
			QMimeData* md = new QMimeData();
			urls.append( QUrl::fromLocalFile(fileName) );
			md->setUrls(urls);
			QDrag* dr = new QDrag(this);
			dr->setMimeData(md);
			const QPixmap& dragCursor = loadIcon("DragPix.xpm");
			dr->setPixmap(dragCursor);
			dr->exec();
		}
		else
		{
			FileLoader *fileLoader = new FileLoader(fileName);
			int testResult = fileLoader->TestFile();
			delete fileLoader;
			if ((testResult != -1) && (testResult >= FORMATID_ODGIMPORT))
			{
				const FileFormat * fmt = LoadSavePlugin::getFormatById(testResult);
				if( fmt )
				{
					fmt->loadFile(fileName, LoadSavePlugin::lfUseCurrentPage|LoadSavePlugin::lfInteractive);
				}
			}
		}
	}
}

void ScribusMainWindow::rebuildLayersList()
{
	if (HaveDoc)
	{
		for( QMap<QString, QPointer<ScrAction> >::Iterator it0 = scrLayersActions.begin(); it0 != scrLayersActions.end(); ++it0 )
			scrMenuMgr->removeMenuItem((*it0), "ItemLayer");
		scrLayersActions.clear();
		ScLayers::iterator it;
		if (doc->Layers.count()!= 0)
		{
			for (it = doc->Layers.begin(); it != doc->Layers.end(); ++it)
			{
				scrLayersActions.insert(QString("%1").arg((*it).ID), new ScrAction( ScrAction::Layer, QPixmap(), QPixmap(), (*it).Name, QKeySequence(), this, (*it).ID));
				scrLayersActions[QString("%1").arg((*it).ID)]->setToggleAction(true);
				QPixmap pm(20,15);
				pm.fill((*it).markerColor);
				scrLayersActions[QString("%1").arg((*it).ID)]->setIcon(pm);
			}
		}
		int currActiveLayer=doc->activeLayer();
		bool found=false;
		for (it = doc->Layers.begin(); it != doc->Layers.end(); ++it)
		{
			if ((*it).ID == currActiveLayer)
			{
				found=true;
				break;
			}
		}
		Q_ASSERT(found);
		scrLayersActions[QString("%1").arg((*it).ID)]->setChecked(true);

		for( QMap<QString, QPointer<ScrAction> >::Iterator it = scrLayersActions.begin(); it!=scrLayersActions.end(); ++it )
		{
			scrMenuMgr->addMenuItem((*it), "ItemLayer", true);
			connect( (*it), SIGNAL(triggeredData(int)), doc, SLOT(itemSelection_SendToLayer(int)) );
		}
	}
}

void ScribusMainWindow::updateItemLayerList()
{
	if (HaveDoc)
	{
		QMap<QString, QPointer<ScrAction> >::Iterator itend=scrLayersActions.end();
		for( QMap<QString, QPointer<ScrAction> >::Iterator it = scrLayersActions.begin(); it!=itend; ++it )
		{
			disconnect( (*it), SIGNAL(triggeredData(int)), 0, 0 );
			(*it)->setChecked(false);
		}
		if (doc->m_Selection->count()>0 && doc->m_Selection->itemAt(0))
			scrLayersActions[QString("%1").arg(doc->m_Selection->itemAt(0)->LayerID)]->setChecked(true);
		for( QMap<QString, QPointer<ScrAction> >::Iterator it = scrLayersActions.begin(); it!=itend; ++it )
			connect( (*it), SIGNAL(triggeredData(int)), doc, SLOT(itemSelection_SendToLayer(int)) );
	}
}

void ScribusMainWindow::updateColorLists()
{
	if (HaveDoc)
	{
		requestUpdate(reqColorsUpdate | reqLineStylesUpdate);
		styleManager->updateColorList();
	}
}

bool ScribusMainWindow::slotDocOpen()
{
	PrefsContext* docContext = prefsManager->prefsFile->getContext("docdirs", false);
	QString docDir(".");
	QString prefsDocDir(prefsManager->documentDir());
	if (!prefsDocDir.isEmpty())
		docDir = docContext->get("docsopen", prefsDocDir);
	else
		docDir = docContext->get("docsopen", ".");
	QString formats(FileLoader::getLoadFilterString());
//	formats.remove("PDF (*.pdf *.PDF);;");
	QString fileName(CFileDialog( docDir, tr("Open"), formats));
	if (fileName.isEmpty()) // User cancelled
		return false;
	QFileInfo fi(fileName);
	docContext->set("docsopen", fi.absolutePath());
	return loadDoc(fileName);
}

bool ScribusMainWindow::slotPageImport()
{
	Q_ASSERT(!doc->masterPageMode());
	bool ret = false;
	MergeDoc *dia = new MergeDoc(this, false, doc->DocPages.count(), doc->currentPage()->pageNr() + 1);
	if (dia->exec())
	{
		mainWindowStatusLabel->setText( tr("Importing Pages..."));
		qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
		std::vector<int> pageNs;
		parsePagesString(dia->getPageNumbers(), &pageNs, dia->getPageCounter());
		int startPage=0, nrToImport=pageNs.size();
		bool doIt = true;
		if (doc->masterPageMode())
		{
			if (nrToImport > 1)
				loadPage(dia->getFromDoc(), pageNs[0] - 1, false);
			doIt = false;
		}
		else if (dia->getCreatePageChecked())
		{
			int importWhere=dia->getImportWhere();
			if (importWhere == 0)
				startPage = dia->getImportWherePage();
			else if (importWhere == 1)
				startPage = dia->getImportWherePage() + 1;
			else
				startPage = doc->DocPages.count() + 1;
			addNewPages(dia->getImportWherePage(), importWhere, nrToImport, doc->pageHeight(), doc->pageWidth(), doc->pageOrientation(), doc->pageSize(), true);
		}
		else
		{
			startPage = doc->currentPage()->pageNr() + 1;
			if (nrToImport > (doc->DocPages.count() - doc->currentPage()->pageNr()))
			{
				qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
				int scmReturn=ScMessageBox::information(this, tr("Import Page(s)"), "<qt>" +
				QObject::tr("<p>You are trying to import more pages than there are available in the current document counting from the active page.</p>Choose one of the following:<br>"
				"<ul><li><b>Create</b> missing pages</li>"
				"<li><b>Import</b> pages until the last page</li>"
				"<li><b>Cancel</b></li></ul>") + "</qt>",
				QObject::tr("C&reate"),
				QObject::tr("&Import"),
				CommonStrings::tr_Cancel, 2, 2);
				switch( scmReturn )
				{
					case 0:
						addNewPages(doc->DocPages.count(), 2,
									nrToImport - (doc->DocPages.count() - doc->currentPage()->pageNr()),
									doc->pageHeight(), doc->pageWidth(), doc->pageOrientation(), doc->pageSize(), true);
						break;
					case 1:
						nrToImport = doc->DocPages.count() - doc->currentPage()->pageNr();
						break;
					case 2:
						doIt = false;
						mainWindowStatusLabel->setText("");
						break;
				}
				qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
			}
		}
		if (doIt)
		{
			if (nrToImport > 0)
			{
				mainWindowProgressBar->reset();
				mainWindowProgressBar->setMaximum(nrToImport);
				int counter = startPage;
				for (int i = 0; i < nrToImport; ++i)
				{
					view->GotoPa(counter);
					loadPage(dia->getFromDoc(), pageNs[i] - 1, false);
					counter++;
					mainWindowProgressBar->setValue(i + 1);
				}
				view->GotoPa(startPage);
				mainWindowProgressBar->reset();
				mainWindowStatusLabel->setText( tr("Import done"));
			}
			else
			{
				mainWindowStatusLabel->setText( tr("Found nothing to import"));
				doIt = false;
			}
		}
		qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
		ret = doIt;
	}
	delete dia;
	return ret;
}

bool ScribusMainWindow::loadPage(QString fileName, int Nr, bool Mpa, const QString& renamedPageName)
{
	bool ret = false;
	if (!fileName.isEmpty())
	{
		FileLoader *fl = new FileLoader(fileName);
		if (fl->TestFile() == -1)
		{
			delete fl;
			return false;
		}
		doc->setLoading(true);
		uint oldItemsCount = doc->Items->count();
		if(!fl->LoadPage(doc, Nr, Mpa, renamedPageName))
		{
			delete fl;
			doc->setLoading(false);
			return false;
		}
		delete fl;
		if (ScCore->haveCMS() && doc->cmsSettings().CMSinUse)
		{
			recalcColors();
			doc->RecalcPictures(&ScCore->InputProfiles, &ScCore->InputProfilesCMYK);
		}
		uint docItemsCount=doc->Items->count();
		for (uint i = oldItemsCount; i < docItemsCount; ++i)
		{
			PageItem *ite = doc->Items->at(i);
//			if ((docItemsCount - oldItemsCount) > 1)
//				ite->Groups.push(doc->GroupCounter);
//	#5386: allow locked imported items to remain locked
// 			if (ite->locked())
// 				ite->setLocked(false);
			if ((ite->asTextFrame()) && (ite->isBookmark))
				AddBookMark(ite);
		}
//		if ((docItemsCount - oldItemsCount) > 1)
//			doc->GroupCounter++;
		propertiesPalette->updateColorList();
		emit UpdateRequest(reqArrowStylesUpdate | reqLineStylesUpdate | reqStyleComboDocUpdate);
		symbolPalette->updateSymbolList();
//		if (!Mpa)
//		{
//			scanDocument();
//			docCheckerPalette->buildErrorList(doc);
//		}
		slotDocCh();
		rebuildLayersList();
		view->updateLayerMenu();
		layerPalette->rebuildList();
		doc->setLoading(false);
		ret = true;
	}
	if (!Mpa)
		pagePalette->Rebuild();
	view->reformPages();
	view->DrawNew();
	return ret;
}

bool ScribusMainWindow::loadDoc(QString fileName)
{
#ifdef DEBUG_LOAD_TIMES
	QTime t;
	struct tms tms1, tms2;
	t.start();
	times(&tms1);
#endif
	undoManager->setUndoEnabled(false);
	QFileInfo fi(fileName);
	if (!fi.exists())
	{
		QMessageBox::warning(this, CommonStrings::trWarning, tr("File does not exist on the specified path :\n%1").arg(QDir::toNativeSeparators(fileName)), 
		                           CommonStrings::tr_OK);
		return false;
	}
	qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
	if (HaveDoc)
		outlinePalette->buildReopenVals();
	bool ret = false;
	QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
	ScribusWin* ActWinOld = NULL;
	if (windows.count() != 0)
		ActWinOld = ActWin;

	// PV - 5780: Scribus doesn't track what documents are already opened
	// The goal of this part of code is to disallow user to open one
	// doc multiple times.
	QString platfName(QDir::toNativeSeparators(fileName));
	uint windowCount = windows.count();
	for ( uint i = 0; i < windowCount; ++i )
	{
		QString docNameUnmodified(windows.at(i)->windowTitle());
		ScribusWin * mx = qobject_cast<ScribusWin*>(windows.at(i)->widget());
		if (mx && mx->doc()->isModified() && docNameUnmodified.endsWith("*"))
			docNameUnmodified.resize(docNameUnmodified.length() - 1);

		if (docNameUnmodified == platfName)
		{
			qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
			QMessageBox::information(this, tr("Document is already opened"),
			                         tr("This document is already in use."
			                            "You'll be switched into its window now."));
			windowsMenuActivated(i);
			return true;
		}
	}

	if (!fileName.isEmpty())
	{
		QString FName = fi.absoluteFilePath();
		FileLoader *fileLoader = new FileLoader(FName);
		int testResult=fileLoader->TestFile();
		if (testResult == -1)
		{
			delete fileLoader;
			qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
			QString title = tr("Fatal Error") ;
			QString msg = "<qt>"+ tr("File %1 is not in an acceptable format").arg(FName)+"</qt>";
			QString infoMsg = "<qt>" + tr("The file may be damaged or may have been produced in a later version of Scribus.") + "</qt>";
			QMessageBox msgBox(QMessageBox::Critical, title, msg, QMessageBox::Ok | QMessageBox::Help, this);
			msgBox.setInformativeText(infoMsg);
			int i=msgBox.exec();
			if (i==QMessageBox::Help)
					slotOnlineHelp("", "fileproblems.html");
			return false;
		}
		bool is12doc=false;
		if (testResult == 0)
		{
			qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
			//Scribus 1.3.x warning, remove at a later stage
			is12doc=true;
		}

		ScCore->getCMSProfilesDir(fi.absolutePath()+"/", false, false);
		prefsManager->appPrefs.fontPrefs.AvailFonts.AddScalableFonts(fi.absolutePath()+"/", FName);
		prefsManager->appPrefs.fontPrefs.AvailFonts.updateFontMap();
		doc=new ScribusDoc();
		doc->is12doc=is12doc;
		doc->appMode = modeNormal;
		doc->HasCMS = false;
		//doc->setActiveLayer(0); //CB should not need this, the file load process sets it to ALAYER from the doc
		doc->OpenNodes.clear();
		doc->setLoading(true);
		mainWindowStatusLabel->setText( tr("Loading..."));
		mainWindowProgressBar->reset();
		ScribusWin* w = new ScribusWin(mdiArea, doc);
		w->setMainWindow(this);
		view = new ScribusView(w, this, doc);
		doc->setGUI(true, this, view);
		view->setScale(prefsManager->displayScale());
		w->setView(view);
		alignDistributePalette->setDoc(doc);
		ActWin = w;
		doc->WinHan = w;
		w->setSubWin(mdiArea->addSubWindow(w));
		w->setUpdatesEnabled(false);
		view->updatesOn(false);
		doc->SoftProofing = false;
		doc->Gamut = false;
		setScriptRunning(true);
		bool loadSuccess=fileLoader->LoadFile(doc);
		//Do the font replacement check from here, when we have a GUI. TODO do this also somehow without the GUI
		//This also gives the user the opportunity to cancel the load when finding theres a replacement required.
		if (loadSuccess && ScCore->usingGUI())
			loadSuccess=fileLoader->postLoad(doc);
		if(!loadSuccess)
		{
			view->close();
			delete fileLoader;
// 			delete view;
			delete doc;
			delete w;
			view=NULL;
			doc=NULL;
			setScriptRunning(false);
			qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
			mainWindowStatusLabel->setText("");
			mainWindowProgressBar->reset();
			ActWin = NULL;
			undoManager->setUndoEnabled(true);
			if (windows.count() != 0)
				newActWin(ActWinOld->getSubWin());
			return false;
		}
		symbolPalette->setDoc(doc);
		outlinePalette->setDoc(doc);
		fileLoader->informReplacementFonts();
		setCurrentComboItem(view->unitSwitcher, unitGetStrFromIndex(doc->unitIndex()));
		view->unitChange();
		setScriptRunning(false);
		view->Deselect(true);
		mainWindowStatusLabel->setText("");
		mainWindowProgressBar->reset();
		HaveDoc++;
		if (doc->checkerProfiles().count() == 0)
		{
			prefsManager->initDefaultCheckerPrefs(&(doc->checkerProfiles()));
			doc->setCurCheckProfile(CommonStrings::PostScript);
		}
		if (doc->pdfOptions().LPISettings.count() == 0)
		{
			struct LPIData lpo;
			lpo.Frequency = 133;
			lpo.SpotFunc = 3;
			lpo.Angle = 105;
			doc->pdfOptions().LPISettings.insert("Cyan", lpo);
			lpo.Angle = 75;
			doc->pdfOptions().LPISettings.insert("Magenta", lpo);
			lpo.Angle = 90;
			doc->pdfOptions().LPISettings.insert("Yellow", lpo);
			lpo.Angle = 45;
			doc->pdfOptions().LPISettings.insert("Black", lpo);
		}

		if (!doc->cmsSettings().CMSinUse)
			doc->HasCMS = false;
		if ((ScCore->haveCMS()) && (doc->cmsSettings().CMSinUse))
		{
			bool cmsWarning = false;
			QStringList missing;
			QStringList replacement;
			if (!ScCore->InputProfiles.contains(doc->cmsSettings().DefaultImageRGBProfile))
			{
				cmsWarning = true;
				missing.append(doc->cmsSettings().DefaultImageRGBProfile);
				replacement.append(prefsManager->appPrefs.colorPrefs.DCMSset.DefaultImageRGBProfile);
				doc->cmsSettings().DefaultImageRGBProfile = prefsManager->appPrefs.colorPrefs.DCMSset.DefaultImageRGBProfile;
			}
			if (!ScCore->InputProfilesCMYK.contains(doc->cmsSettings().DefaultImageCMYKProfile))
			{
				cmsWarning = true;
				missing.append(doc->cmsSettings().DefaultImageCMYKProfile);
				replacement.append(prefsManager->appPrefs.colorPrefs.DCMSset.DefaultImageCMYKProfile);
				doc->cmsSettings().DefaultImageCMYKProfile = prefsManager->appPrefs.colorPrefs.DCMSset.DefaultImageCMYKProfile;
			}
			if (!ScCore->InputProfiles.contains(doc->cmsSettings().DefaultSolidColorRGBProfile))
			{
				cmsWarning = true;
				missing.append(doc->cmsSettings().DefaultSolidColorRGBProfile);
				replacement.append(prefsManager->appPrefs.colorPrefs.DCMSset.DefaultSolidColorRGBProfile);
				doc->cmsSettings().DefaultSolidColorRGBProfile = prefsManager->appPrefs.colorPrefs.DCMSset.DefaultSolidColorRGBProfile;
			}
			if (!ScCore->InputProfilesCMYK.contains(doc->cmsSettings().DefaultSolidColorCMYKProfile))
			{
				cmsWarning = true;
				missing.append(doc->cmsSettings().DefaultSolidColorCMYKProfile);
				replacement.append(prefsManager->appPrefs.colorPrefs.DCMSset.DefaultSolidColorCMYKProfile);
				doc->cmsSettings().DefaultSolidColorCMYKProfile = prefsManager->appPrefs.colorPrefs.DCMSset.DefaultSolidColorCMYKProfile;
			}
			if (!ScCore->MonitorProfiles.contains(doc->cmsSettings().DefaultMonitorProfile))
			{
				cmsWarning = true;
				missing.append(doc->cmsSettings().DefaultMonitorProfile);
				replacement.append(prefsManager->appPrefs.colorPrefs.DCMSset.DefaultMonitorProfile);
				doc->cmsSettings().DefaultMonitorProfile = prefsManager->appPrefs.colorPrefs.DCMSset.DefaultMonitorProfile;
			}
			if (!ScCore->PrinterProfiles.contains(doc->cmsSettings().DefaultPrinterProfile))
			{
				cmsWarning = true;
				missing.append(doc->cmsSettings().DefaultPrinterProfile);
				replacement.append(prefsManager->appPrefs.colorPrefs.DCMSset.DefaultPrinterProfile);
				doc->cmsSettings().DefaultPrinterProfile = prefsManager->appPrefs.colorPrefs.DCMSset.DefaultPrinterProfile;
			}
			if (!ScCore->PrinterProfiles.contains(doc->pdfOptions().PrintProf))
			{
				cmsWarning = true;
				missing.append(doc->pdfOptions().PrintProf);
				replacement.append(prefsManager->appPrefs.colorPrefs.DCMSset.DefaultPrinterProfile);
				doc->pdfOptions().PrintProf = doc->cmsSettings().DefaultPrinterProfile;
			}
			if (!ScCore->InputProfiles.contains(doc->pdfOptions().ImageProf))
			{
				cmsWarning = true;
				missing.append(doc->pdfOptions().ImageProf);
				replacement.append(prefsManager->appPrefs.colorPrefs.DCMSset.DefaultImageRGBProfile);
				doc->pdfOptions().ImageProf = doc->cmsSettings().DefaultImageRGBProfile;
			}
			if (!ScCore->InputProfiles.contains(doc->pdfOptions().SolidProf))
			{
				cmsWarning = true;
				missing.append(doc->pdfOptions().SolidProf);
				replacement.append(prefsManager->appPrefs.colorPrefs.DCMSset.DefaultSolidColorRGBProfile);
				doc->pdfOptions().SolidProf = doc->cmsSettings().DefaultSolidColorRGBProfile;
			}
			if (cmsWarning)
			{
				qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
				QString mess = tr("Some color profiles used by this document are not installed:")+"\n\n";
				for (int m = 0; m < missing.count(); ++m)
				{
					mess += missing[m] + tr(" was replaced by: ")+replacement[m]+"\n";
				}
				QMessageBox::warning(this, CommonStrings::trWarning, mess, 1, 0, 0);
			}
			doc->SoftProofing = doc->cmsSettings().SoftProofOn;
			doc->Gamut        = doc->cmsSettings().GamutCheck;
			doc->IntentColors = doc->cmsSettings().DefaultIntentColors;
			doc->IntentImages = doc->cmsSettings().DefaultIntentImages;
			if (doc->OpenCMSProfiles(ScCore->InputProfiles, ScCore->InputProfilesCMYK, ScCore->MonitorProfiles, ScCore->PrinterProfiles))
			{
				doc->HasCMS = true;
				doc->pdfOptions().SComp = doc->cmsSettings().ComponentsInput2;
			}
			else
			{
				doc->SetDefaultCMSParams();
				doc->HasCMS = false;
			}
			if (doc->HasCMS)
			{
				recalcColors();
				doc->RecalcPictures(&ScCore->InputProfiles, &ScCore->InputProfilesCMYK);
			}
		}
		else
		{
			doc->cmsSettings().CMSinUse = false;
		}
//		propertiesPalette->updateColorList();
//		propertiesPalette->Cpal->displayGradient(0);
		if (fileLoader->FileType > FORMATID_NATIVEIMPORTEND)
		{
			doc->setName(FName+ tr("(converted)"));
			QFileInfo fi(doc->DocName);
			doc->setName(fi.fileName());
		}
		else
			doc->setName(FName);
		doc->setMasterPageMode(false);
		doc->setHyphLanguage(GetLang(doc->hyphLanguage()));
		HaveNewDoc();
//		propertiesPalette->Cpal->displayGradient(0);
//		propertiesPalette->updateCList();
		doc->hasName = true;
		if (doc->MasterPages.count() == 0)
			doc->addMasterPage(0, CommonStrings::masterPageNormal);
		//Add doc sections if we have none
		if (doc->sections().count()==0)
		{
			doc->addSection(-1);
			doc->setFirstSectionFromFirstPageNumber();
		}
		doc->RePos = true;
		doc->setMasterPageMode(true);
		doc->reformPages();
		doc->setLoading(false);
		for (int azz=0; azz<doc->MasterItems.count(); ++azz)
		{
			PageItem *ite = doc->MasterItems.at(azz);
			// TODO fix that for Groups on Masterpages
//			if (ite->Groups.count() != 0)
//				view->GroupOnPage(ite);
//			qDebug() << QString("load M: %1 %2 %3").arg(azz).arg((uint)ite).arg(ite->itemType());
			ite->layout();
		}
//		RestoreBookMarks();
		doc->setMasterPageMode(false);
		int docItemsCount=doc->Items->count();
		for (int azz=0; azz<docItemsCount; ++azz)
		{
			PageItem *ite = doc->Items->at(azz);
			if(ite->nextInChain() == NULL)
				ite->layout();
/*			if (doc->OldBM)
			{
				if ((ite->itemType() == PageItem::TextFrame) && (ite->isBookmark))
					bookmarkPalette->BView->AddPageItem(ite);
			}
			else
			{
				if ((ite->itemType() == PageItem::TextFrame) && (ite->isBookmark))
					bookmarkPalette->BView->ChangeItem(ite->BMnr, ite->ItemNr);
			} */
		}
		for (int azz=0; azz<doc->FrameItems.count(); ++azz)
		{
			PageItem *ite = doc->FrameItems.at(azz);
//			qDebug() << QString("load F: %1 %2 %3").arg(azz).arg((uint)ite).arg(ite->itemType());
			if(ite->nextInChain() == NULL)
				ite->layout();
		}
//		if (doc->OldBM)
//			StoreBookmarks();
		doc->RePos = false;
		doc->setModified(false);
		updateRecent(FName);
		mainWindowStatusLabel->setText( tr("Ready"));
		ret = true;
		doc->setLoading(true);
		for (int p = 0; p < doc->DocPages.count(); ++p)
		{
			Apply_MasterPage(doc->DocPages.at(p)->MPageNam, p, false);
		}
		view->reformPages(false);
		doc->setLoading(false);
/*		if (fileLoader->FileType > FORMATID_NATIVEIMPORTEND)
		{
			doc->hasName = false;
			slotFileSaveAs();
		} */
		delete fileLoader;
		view->updatesOn(true);
		w->setUpdatesEnabled(true);
		disconnect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(newActWin(QMdiSubWindow *)));
		if ((mdiArea->subWindowList().isEmpty()) || (mdiArea->subWindowList().count() == 1))
			w->showMaximized();
		else
			w->show();
		view->show();
		// Seems to fix crash on loading
		ActWin = NULL;
		newActWin(w->getSubWin());
		doc->setCurrentPage(doc->DocPages.at(0));
		view->cmsToolbarButton->setChecked(doc->HasCMS);
		view->zoom();
		view->GotoPage(0);
		connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(newActWin(QMdiSubWindow *)));
		connect(w, SIGNAL(AutoSaved()), this, SLOT(slotAutoSaved()));
		connect(ScCore->fileWatcher, SIGNAL(fileChanged(QString )), doc, SLOT(updatePict(QString)));
		connect(ScCore->fileWatcher, SIGNAL(fileDeleted(QString )), doc, SLOT(removePict(QString)));
		connect(ScCore->fileWatcher, SIGNAL(dirChanged(QString )), doc, SLOT(updatePictDir(QString )));
		connect(undoManager, SIGNAL(undoRedoBegin()), doc, SLOT(undoRedoBegin()));
		connect(undoManager, SIGNAL(undoRedoDone()), doc, SLOT(undoRedoDone()));
		connect(undoManager, SIGNAL(undoRedoDone()), view, SLOT(DrawNew()));
		doc->connectDocSignals();
		if (doc->autoSave())
			doc->autoSaveTimer->start(doc->autoSaveTime());
// 		scrActions["fileSave"]->setEnabled(false);
		doc->NrItems = bookmarkPalette->BView->NrItems;
		doc->First = bookmarkPalette->BView->First;
		doc->Last = bookmarkPalette->BView->Last;
	}
	else
	{
		pagePalette->setView(0);
	}
	undoManager->switchStack(doc->DocName);
	pagePalette->Rebuild();
	qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
	undoManager->setUndoEnabled(true);
	doc->setModified(false);
#ifdef DEBUG_LOAD_TIMES
	times(&tms2);
	double ticks = sysconf(_SC_CLK_TCK);
	double user  = (tms2.tms_utime - tms1.tms_utime)/ticks;
	double sys   = (tms2.tms_stime - tms1.tms_stime)/ticks;
	double cuser = (tms2.tms_cutime - tms1.tms_cutime)/ticks;
	double csys  = (tms2.tms_cstime - tms1.tms_cstime)/ticks;
	qDebug("loaded document in %.3f seconds (%.3f user + %.3f sys = %.3f sec, child %.3f user + %.3f sys = %.3f sec)",
		t.elapsed()/1000.0, user, sys, user + sys, cuser, csys, cuser + csys);
#endif
	return ret;
}

bool ScribusMainWindow::postLoadDoc()
{
	//FIXME Just return for now, if we arent using the GUI
	if (!ScCore->usingGUI())
		return false;
	return true;
}

// This method was once named slotFileOpen(...) but it hasn't had anything to
// do with file->open for a LONG time. It's used for get text / get picture.
void ScribusMainWindow::slotGetContent()
{
	if (doc->m_Selection->count() != 0)
	{
		PageItem *currItem = doc->m_Selection->itemAt(0);
		if (currItem->itemType() == PageItem::ImageFrame)
		{
			QString formatD(FormatsManager::instance()->fileDialogFormatList(FormatsManager::IMAGESIMGFRAME));

			QString docDir = ".";
			QString prefsDocDir=prefsManager->documentDir();
			if (!prefsDocDir.isEmpty())
				docDir = prefsManager->prefsFile->getContext("dirs")->get("images", prefsDocDir);
			else
				docDir = prefsManager->prefsFile->getContext("dirs")->get("images", ".");
			QString fileName = CFileDialog( docDir, tr("Open"), formatD, "", fdShowPreview | fdExistingFiles);
			if (!fileName.isEmpty())
			{
				prefsManager->prefsFile->getContext("dirs")->set("images", fileName.left(fileName.lastIndexOf("/")));
				currItem->EmProfile = "";
				currItem->pixm.imgInfo.isRequest = false;
				currItem->UseEmbedded = true;
				currItem->IProfile = doc->cmsSettings().DefaultImageRGBProfile;
				currItem->IRender = doc->cmsSettings().DefaultIntentImages;
				qApp->changeOverrideCursor( QCursor(Qt::WaitCursor) );
				qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
				doc->LoadPict(fileName, currItem->ItemNr, false, true);
				propertiesPalette->imagePal->displayScaleAndOffset(currItem->imageXScale(), currItem->imageYScale(), currItem->imageXOffset(), currItem->imageYOffset());
				qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
				view->DrawNew();
				emit UpdateRequest(reqColorsUpdate | reqCmsOptionsUpdate);
				currItem->emitAllToGUI();
			}
		}
		if (currItem->asTextFrame())
		{
			gtGetText* gt = new gtGetText(doc);
			ImportSetup impsetup=gt->run();
			if (impsetup.runDialog)
			{
				if (currItem->itemText.length() != 0)
				{
					int t = QMessageBox::warning(this, CommonStrings::trWarning, tr("Do you really want to clear all your text?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
					if (t == QMessageBox::No)
						return;
				}
				gt->launchImporter(impsetup.importer, impsetup.filename, impsetup.textOnly, impsetup.encoding, false);
			}
			delete gt;
			if (doc->docHyphenator->AutoCheck)
				doc->docHyphenator->slotHyphenate(currItem);
			for (int a = 0; a < doc->Items->count(); ++a)
			{
				if (doc->Items->at(a)->isBookmark)
					bookmarkPalette->BView->ChangeText(doc->Items->at(a));
			}
			view->DrawNew();
			slotDocCh();
			styleManager->setDoc(doc);
		}
	}
}

void ScribusMainWindow::slotGetContent2() // kk2006
{
	if (doc->m_Selection->count() == 0)
		return; // nothing to do, no selection

	PageItem *currItem = doc->m_Selection->itemAt(0);

	if (!currItem->asTextFrame())
		return; // not a text frame

	ScGTPluginManager::instance()->run();
	if (doc->docHyphenator->AutoCheck)
		doc->docHyphenator->slotHyphenate(currItem);
	for (int a = 0; a < doc->Items->count(); ++a)
	{
		if (doc->Items->at(a)->isBookmark)
			bookmarkPalette->BView->ChangeText(doc->Items->at(a));
	}
	view->DrawNew();
	slotDocCh();
}

void ScribusMainWindow::slotGetClipboardImage()
{
	if (HaveDoc && (doc->m_Selection->count() != 0) && (QApplication::clipboard()->mimeData()->hasImage()))
	{
		PageItem *currItem = doc->m_Selection->itemAt(0);
		if (currItem->itemType() == PageItem::ImageFrame)
		{
			int t = QMessageBox::Yes;
			if (currItem->PictureIsAvailable)
				t = QMessageBox::warning(this, CommonStrings::trWarning, tr("Do you really want to replace your existing image?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (t == QMessageBox::Yes)
			{
				QImage img = QApplication::clipboard()->image();
				if (!img.isNull())
				{
					currItem->EmProfile = "";
					currItem->pixm.imgInfo.isRequest = false;
					currItem->UseEmbedded = true;
					currItem->IProfile = doc->cmsSettings().DefaultImageRGBProfile;
					currItem->IRender = doc->cmsSettings().DefaultIntentImages;
					qApp->changeOverrideCursor( QCursor(Qt::WaitCursor) );
					qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
					currItem->tempImageFile = new QTemporaryFile(QDir::tempPath() + "/scribus_temp_XXXXXX.png");
					currItem->tempImageFile->open();
					QString fileName = getLongPathName(currItem->tempImageFile->fileName());
					currItem->tempImageFile->close();
					currItem->isInlineImage = true;
					currItem->Pfile = fileName;
					img.save(fileName, "PNG");
					doc->LoadPict(fileName, currItem->ItemNr, false, true);
					propertiesPalette->imagePal->displayScaleAndOffset(currItem->imageXScale(), currItem->imageYScale(), currItem->imageXOffset(), currItem->imageYOffset());
					qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
					view->DrawNew();
					emit UpdateRequest(reqColorsUpdate | reqCmsOptionsUpdate);
					currItem->emitAllToGUI();
				}
			}
		}
	}
}

void ScribusMainWindow::toogleInlineState()
{
	if (doc->m_Selection->count() != 0)
	{
		PageItem *currItem = doc->m_Selection->itemAt(0);
		if (currItem->itemType() == PageItem::ImageFrame)
		{
			if (currItem->PictureIsAvailable)
			{
				if (currItem->isImageInline())
				{
					QFileInfo fiB(currItem->Pfile);
					QString fna = fiB.fileName();
					PrefsContext* docContext = prefsManager->prefsFile->getContext("docdirs", false);
					QString wdir = ".";
					if (doc->hasName)
					{
						QFileInfo fi(doc->DocName);
						wdir = QDir::fromNativeSeparators( fi.path() );
					}
					else
					{
						QString prefsDocDir = prefsManager->documentDir();
						if (!prefsDocDir.isEmpty())
							wdir = docContext->get("place_as", prefsDocDir);
						else
							wdir = docContext->get("place_as", ".");
						wdir = QDir::fromNativeSeparators( wdir );
					}
					QString fileName = CFileDialog(wdir, tr("Filename and Path for Image"), tr("All Files (*)"), fna, fdHidePreviewCheckBox);
					if (!fileName.isEmpty())
					{
						if (ScCore->fileWatcher->files().contains(currItem->Pfile) != 0)
							ScCore->fileWatcher->removeFile(currItem->Pfile);
						docContext->set("place_as", fileName.left(fileName.lastIndexOf("/")));
						if (overwrite(this, fileName))
						{
							currItem->makeImageExternal(fileName);
							ScCore->fileWatcher->addFile(currItem->Pfile);
							bool fho = currItem->imageFlippedH();
							bool fvo = currItem->imageFlippedV();
							doc->loadPict(currItem->Pfile, currItem, true);
							currItem->setImageFlippedH(fho);
							currItem->setImageFlippedV(fvo);
						}
					}
				}
				else
				{
					if (ScCore->fileWatcher->files().contains(currItem->Pfile) != 0)
						ScCore->fileWatcher->removeFile(currItem->Pfile);
					currItem->makeImageInline();
					ScCore->fileWatcher->addFile(currItem->Pfile);
					bool fho = currItem->imageFlippedH();
					bool fvo = currItem->imageFlippedV();
					doc->loadPict(currItem->Pfile, currItem, true);
					currItem->setImageFlippedH(fho);
					currItem->setImageFlippedV(fvo);
				}
				scrActions["itemToggleInlineImage"]->setChecked(currItem->isImageInline());
			}
		}
	}
}

void ScribusMainWindow::slotFileAppend()
{
	if (doc->m_Selection->count() != 0)
	{
		gtGetText* gt = new gtGetText(doc);
		ImportSetup impsetup=gt->run();
		if (impsetup.runDialog)
		{
			gt->launchImporter(impsetup.importer, impsetup.filename, impsetup.textOnly, impsetup.encoding, true);
		}
		delete gt;
		//CB Hyphenating now emits doc changed, plus we change lang as appropriate
		if (doc->docHyphenator->AutoCheck)
			doc->itemSelection_DoHyphenate();
			//doc->docHyphenator->slotHyphenate(doc->m_Selection->itemAt(0));
		view->DrawNew();
		//slotDocCh();
	}
}

void ScribusMainWindow::slotFileRevert()
{
	if ((doc->hasName) && (doc->isModified()) && (!doc->masterPageMode()))
	{
		int t = QMessageBox::warning(this, CommonStrings::trWarning, "<qt>" +
								 QObject::tr("The changes to your document have not been saved and you have requested to revert them. Do you wish to continue?") + "</qt>",
								 QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (t == QMessageBox::No)
			return;

		QString fn(doc->DocName);
		doc->setModified(false);
		if (doc==storyEditor->currentDocument())
			storyEditor->close();
		slotFileClose();
		qApp->processEvents();
		loadDoc(fn);
	}
}

void ScribusMainWindow::slotAutoSaved()
{
	if (ActWin == sender())
		updateActiveWindowCaption(doc->DocName);
}

bool ScribusMainWindow::slotFileSave()
{
	bool ret = false;
	if (doc->hasName)
	{
		//Scribus 1.3.x warning, remove at a later stage
		if (doc->is12doc && !warningVersion(this))
			return false;

		QString fn(doc->DocName), savedFileName;
		ret = DoFileSave(fn, &savedFileName);
		if (!ret && !savedFileName.isEmpty())
			QMessageBox::warning(this, CommonStrings::trWarning, tr("Your document was saved to a temporary file and could not be moved: \n%1").arg( QDir::toNativeSeparators(savedFileName) ), CommonStrings::tr_OK);
		else if (!ret)
			QMessageBox::warning(this, CommonStrings::trWarning, tr("Cannot write the file: \n%1").arg( QDir::toNativeSeparators(fn) ), CommonStrings::tr_OK);
	}
	else
		ret = slotFileSaveAs();
	return ret;
}

bool ScribusMainWindow::slotFileSaveAs()
{
	//Scribus 1.3.x warning, remove at a later stage
	if (doc->is12doc)
		if (!warningVersion(this))
			return false;
	//Turn off the warnings once the docs is saved.
	doc->is12doc=false;
	//
	bool ret = false;
	QString fna;
	PrefsContext* docContext = prefsManager->prefsFile->getContext("docdirs", false);
	QString wdir = ".";
	if (doc->hasName)
	{
		QFileInfo fi(doc->DocName);
		wdir = QDir::fromNativeSeparators( fi.path() );
		fna  = QDir::fromNativeSeparators( fi.path()+"/"+fi.baseName()+".sla" );
	}
	else
	{
		QString prefsDocDir=prefsManager->documentDir();
		if (!prefsDocDir.isEmpty())
			wdir = docContext->get("save_as", prefsDocDir);
		else
			wdir = docContext->get("save_as", ".");
		wdir = QDir::fromNativeSeparators( wdir );
		if (wdir.right(1) != "/")
			fna = wdir + "/";
		else
			fna = wdir;
		fna += doc->DocName + ".sla";
	}
	bool saveCompressed=prefsManager->appPrefs.docSetupPrefs.saveCompressed;
	if (saveCompressed)
		fna.append(".gz");

	QString fileSpec = tr("Documents (*.sla *.sla.gz);;All Files (*)");
	int optionFlags = fdCompressFile | fdHidePreviewCheckBox;
	QString fn = CFileDialog( wdir, tr("Save As"), fileSpec, fna, optionFlags, &saveCompressed);
	if (!fn.isEmpty())
	{
		docContext->set("save_as", fn.left(fn.lastIndexOf("/")));
		if ((fn.endsWith(".sla")) || (fn.endsWith(".sla.gz")))
			fna = fn;
		else
			fna = fn+".sla";
		if (overwrite(this, fna))
		{
			QString savedFileName;
			ret = DoFileSave(fna, &savedFileName);
			if (!ret && !savedFileName.isEmpty())
				QMessageBox::warning(this, CommonStrings::trWarning, tr("Your document was saved to a temporary file and could not be moved: \n%1").arg( QDir::toNativeSeparators(savedFileName) ), CommonStrings::tr_OK);
			else if (!ret)
				QMessageBox::warning(this, CommonStrings::trWarning, tr("Cannot write the file: \n%1").arg( QDir::toNativeSeparators(fn) ), CommonStrings::tr_OK);
			else
				doc->pdfOptions().fileName = ""; // #1482 reset the pdf file name
		}
	}
	mainWindowStatusLabel->setText( tr("Ready"));
	return ret;
}

bool ScribusMainWindow::DoFileSave(const QString& fileName, QString* savedFileName)
{
	ScCore->fileWatcher->forceScan();
	ScCore->fileWatcher->stop();
	doc->reorganiseFonts();
	mainWindowStatusLabel->setText( tr("Saving..."));
	mainWindowProgressBar->reset();
	bool ret=doc->save(fileName, savedFileName);
	qApp->processEvents();
	if (ret)
	{
		updateActiveWindowCaption(fileName);
		undoManager->renameStack(fileName);
// 		scrActions["fileSave"]->setEnabled(false);
		scrActions["fileRevert"]->setEnabled(false);
		updateRecent(fileName);
	}
	mainWindowStatusLabel->setText("");
	mainWindowProgressBar->reset();
	ScCore->fileWatcher->start();
	return ret;
}

bool ScribusMainWindow::slotFileClose()
{
	if (doc->symbolEditMode())
	{
		editSymbolEnd();
		return true;
	}
	ScribusWin* tw = ActWin;
//	ActWin->close();
	mdiArea->closeActiveSubWindow();
	if (tw == ActWin)
		return false;
	else
		return true;
	windowsMenuAboutToShow();
}

bool ScribusMainWindow::DoFileClose()
{
	view->Deselect(false);
	if (doc==storyEditor->currentDocument())
		storyEditor->close();
	actionManager->disconnectNewDocActions();
	actionManager->disconnectNewViewActions();
	disconnect(view, SIGNAL(signalGuideInformation(int, qreal)), alignDistributePalette, SLOT(setGuide(int, qreal)));
	/*CB currently unused
	if (doc->viewCount > 1)
	{
		--doc->viewCount;
		closeActiveWindowMasterPageEditor();
		slotSelect();
		disconnect(ScCore->fileWatcher, SIGNAL(fileChanged(QString )), doc, SLOT(updatePict(QString)));
		disconnect(ScCore->fileWatcher, SIGNAL(fileDeleted(QString )), doc, SLOT(removePict(QString)));
		view->close();
		delete view;
		view = NULL;
		doc = NULL;
		ActWin = NULL;
		return true;
	}*/
	undoManager->removeStack(doc->DocName);
	closeActiveWindowMasterPageEditor();
	slotSelect();
	doc->autoSaveTimer->stop();
	disconnect(doc->autoSaveTimer, SIGNAL(timeout()), doc->WinHan, SLOT(slotAutoSave()));
	disconnect(doc->WinHan, SIGNAL(AutoSaved()), this, SLOT(slotAutoSaved()));
	disconnect(ScCore->fileWatcher, SIGNAL(fileChanged(QString )), doc, SLOT(updatePict(QString)));
	disconnect(ScCore->fileWatcher, SIGNAL(fileDeleted(QString )), doc, SLOT(removePict(QString)));
	disconnect(ScCore->fileWatcher, SIGNAL(dirChanged(QString )), doc, SLOT(updatePictDir(QString )));
	if (ScCore->haveCMS())
		doc->CloseCMSProfiles();
	//<<Palettes
//	propertiesPalette->NewSel(-1);
	propertiesPalette->unsetDoc();
	symbolPalette->unsetDoc();
	pagePalette->setView(0);
	pagePalette->Rebuild();
	if (doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	bookmarkPalette->BView->clear();
	bookmarkPalette->BView->NrItems = 0;
	bookmarkPalette->BView->First = 1;
	bookmarkPalette->BView->Last = 0;
	outlinePalette->unsetDoc();
	alignDistributePalette->setDoc(NULL);
	//>>
	if ((mdiArea->subWindowList().isEmpty()) || (mdiArea->subWindowList().count() == 1))
	{
		PluginManager& pluginManager(PluginManager::instance());
		pluginManager.enableOnlyStartupPluginActions(this);
//		scrActions["fileDocSetup"]->setEnabled(false);
		scrActions["fileDocSetup150"]->setEnabled(false);
		scrActions["filePrint"]->setEnabled(false);
		scrActions["fileSave"]->setEnabled(false);
		scrActions["fileSaveAs"]->setEnabled(false);
		scrActions["fileRevert"]->setEnabled(false);
		scrActions["fileCollect"]->setEnabled(false);
		scrActions["fileClose"]->setEnabled(false);
		scrActions["PrintPreview"]->setEnabled(false);
		if (scrActions["SaveAsDocumentTemplate"])
			scrActions["SaveAsDocumentTemplate"]->setEnabled(false);
		scrMenuMgr->setMenuEnabled("FileImport", false);
		scrMenuMgr->setMenuEnabled("FileExport", false);
		scrActions["fileExportAsPDF"]->setEnabled(false);
		scrActions["fileExportText"]->setEnabled(false);
		scrActions["fileExportAsEPS"]->setEnabled(false);
		scrActions["fileImportText"]->setEnabled(false);
		scrActions["fileImportText2"]->setEnabled(false);
		scrActions["fileImportImage"]->setEnabled(false);
		scrActions["fileImportAppendText"]->setEnabled(false);
		//		scrMenuMgr->setMenuEnabled("Page", false);
		scrActions["pageInsert"]->setEnabled(false);
		scrActions["pageImport"]->setEnabled(false);
		scrActions["pageDelete"]->setEnabled(false);
		scrActions["pageImport"]->setEnabled(false);
		scrActions["pageMove"]->setEnabled(false);
		scrActions["pageCopy"]->setEnabled(false);
		scrActions["pageApplyMasterPage"]->setEnabled(false);
		scrActions["pageCopyToMasterPage"]->setEnabled(false);
		scrActions["pageManageGuides"]->setEnabled(false);
		scrActions["pageManageMargins"]->setEnabled(false);

		scrActions["editUndoAction"]->setEnabled(false);
		scrActions["editRedoAction"]->setEnabled(false);
		scrActions["editCut"]->setEnabled(false);
		scrActions["editCopy"]->setEnabled(false);
		scrActions["editPaste"]->setEnabled(false);
		scrMenuMgr->setMenuEnabled("EditPasteRecent", false);
		scrActions["editClearContents"]->setEnabled(false);
		scrActions["editSelectAll"]->setEnabled(false);
		scrActions["editSelectAllOnLayer"]->setEnabled(false);
		scrActions["editDeselectAll"]->setEnabled(false);
		scrActions["editReplaceColors"]->setEnabled(false);
//		scrActions["editPatterns"]->setEnabled(false);
//		scrActions["editGradients"]->setEnabled(false);
		scrActions["editStyles"]->setEnabled(false);
		scrActions["editSearchReplace"]->setEnabled(false);
		scrActions["editMasterPages"]->setEnabled(false);
		scrActions["editJavascripts"]->setEnabled(false);

		//scrActions["toolsPreflightVerifier"]->setEnabled(false);

		scrActions["extrasHyphenateText"]->setEnabled(false);
		scrActions["extrasDeHyphenateText"]->setEnabled(false);
//		scrMenuMgr->setMenuEnabled("View", false);
		scrActions["viewFitInWindow"]->setEnabled(false);
		scrActions["viewFitWidth"]->setEnabled(false);
		scrActions["viewFit50"]->setEnabled(false);
		scrActions["viewFit75"]->setEnabled(false);
		scrActions["viewFit100"]->setEnabled(false);
		scrActions["viewFit200"]->setEnabled(false);
		scrActions["viewFit400"]->setEnabled(false);
		//scrMenuMgr->setMenuEnabled("Windows", false);
		scrActions["viewSnapToGuides"]->setChecked(false);
		scrActions["viewSnapToGrid"]->setChecked(false);
		scrActions["viewShowRulers"]->setEnabled(false);

		scrMenuMgr->setMenuEnabled("Insert", false);
		scrActions["insertFrame"]->setEnabled(false);

		scrMenuMgr->setMenuEnabled("Extras", false);
		//		scrMenuMgr->setMenuEnabled("Item", false);
		scrActions["itemDuplicate"]->setEnabled(false);
		scrActions["itemMulDuplicate"]->setEnabled(false);
		scrActions["itemTransform"]->setEnabled(false);
		scrActions["itemDelete"]->setEnabled(false);
		scrActions["itemRaise"]->setEnabled(false);
		scrActions["itemLower"]->setEnabled(false);
		scrActions["itemRaiseToTop"]->setEnabled(false);
		scrActions["itemLowerToBottom"]->setEnabled(false);
//		scrActions["itemSendToScrapbook"]->setEnabled(false);
		scrMenuMgr->setMenuEnabled("itemSendToScrapbook", false);
		scrActions["itemSendToPattern"]->setEnabled(false);
		scrActions["itemAdjustFrameToImage"]->setEnabled(false);
		scrActions["itemAdjustImageToFrame"]->setEnabled(false);
		scrActions["itemExtendedImageProperties"]->setEnabled(false);
		scrActions["itemUpdateImage"]->setEnabled(false);
		scrActions["itemPreviewLow"]->setEnabled(false);
		scrActions["itemPreviewNormal"]->setEnabled(false);
		scrActions["itemPreviewFull"]->setEnabled(false);
		scrActions["itemAttributes"]->setEnabled(false);

		scrActions["toolsSelect"]->setEnabled(false);
		scrActions["toolsRotate"]->setEnabled(false);
		scrActions["toolsEditContents"]->setEnabled(false);
		scrActions["toolsEditWithStoryEditor"]->setEnabled(false);
		scrActions["toolsZoom"]->setEnabled(false);
		scrActions["toolsInsertTextFrame"]->setEnabled(false);
		scrActions["toolsInsertImageFrame"]->setEnabled(false);
		scrActions["toolsInsertTableFrame"]->setEnabled(false);
		scrActions["toolsInsertShape"]->setEnabled(false);
		scrActions["toolsInsertLine"]->setEnabled(false);
		scrActions["toolsInsertBezier"]->setEnabled(false);
		scrActions["toolsInsertFreehandLine"]->setEnabled(false);
		scrActions["toolsInsertCalligraphicLine"]->setEnabled(false);
		scrActions["toolsInsertPolygon"]->setEnabled(false);
		scrActions["toolsInsertArc"]->setEnabled(false);
		scrActions["toolsInsertSpiral"]->setEnabled(false);
		scrActions["toolsInsertRenderFrame"]->setEnabled(false);
		scrActions["toolsLinkTextFrame"]->setEnabled(false);
		scrActions["toolsUnlinkTextFrame"]->setEnabled(false);
		scrActions["toolsMeasurements"]->setEnabled(false);
		scrActions["toolsCopyProperties"]->setEnabled(false);
		scrActions["toolsEyeDropper"]->setEnabled(false);
		scrActions["toolsPDFPushButton"]->setEnabled(false);
		scrActions["toolsPDFTextField"]->setEnabled(false);
		scrActions["toolsPDFCheckBox"]->setEnabled(false);
		scrActions["toolsPDFComboBox"]->setEnabled(false);
		scrActions["toolsPDFListBox"]->setEnabled(false);
		scrActions["toolsPDFAnnotText"]->setEnabled(false);
		scrActions["toolsPDFAnnotLink"]->setEnabled(false);
#ifdef HAVE_OSG
		scrActions["toolsPDFAnnot3D"]->setEnabled(false);
#endif
		//CB dont need this until we have a doc...
		//propertiesPalette->Cpal->setColors(prefsManager->colorSet());
		mainWindowStatusLabel->setText( tr("Ready"));
		PrinterUsed = false;
	}
	// Give plugins a chance to react on closing the document
	PluginManager& pluginManager(PluginManager::instance());
	QStringList pluginNames(pluginManager.pluginNames(false));
	ScPlugin* plugin;
	QString pName;
	for (int i = 0; i < pluginNames.count(); ++i)
	{
		pName = pluginNames.at(i);
		plugin = pluginManager.getPlugin(pName, true);
		Q_ASSERT(plugin); // all the returned names should represent loaded plugins
		plugin->unsetDoc();
	}
	view->close();
// 	delete view;
	//CB Yes, we are setting it to NULL without deleting it. ActWin(ScribusWin) owns the view
	//due to it being the central widget and will delete it at the correct moment from its own pointer.
	view = NULL;
	doc->setLoading(true);
	guidePalette->setDoc(0);
	charPalette->setDoc(0);
	tocGenerator->setDoc(0);
	styleManager->setDoc(0);
	layerPalette->ClearInhalt();
	docCheckerPalette->buildErrorList(0);
	HaveDoc--;
	delete doc;
	doc = NULL;
	ActWin = NULL;
	if ( HaveDoc == 0 )
	{
		QString prefsDocDir( PrefsManager::instance()->documentDir() );
		if ( QDir().exists(prefsDocDir) )
			QDir::setCurrent( PrefsManager::instance()->documentDir() );
		else
			QDir::setCurrent( QDir::homePath() );
	}
	return true;
}

void ScribusMainWindow::slotFilePrint()
{
	if (doc->checkerProfiles()[doc->curCheckProfile()].autoCheck)
	{
		if (scanDocument())
		{
			if (doc->checkerProfiles()[doc->curCheckProfile()].ignoreErrors)
			{
				int t = QMessageBox::warning(this, CommonStrings::trWarning,
											"<qt>"+ tr("Scribus has detected some errors. Consider using the Preflight Verifier to correct them")+"</qt>",
											QMessageBox::Abort | QMessageBox::Ignore);
				if (t == QMessageBox::Abort)
					return;
			}
			else
			{
				connect(docCheckerPalette, SIGNAL(ignoreAllErrors()), this, SLOT(slotReallyPrint()));
				docCheckerPalette->setIgnoreEnabled(true);
				docCheckerPalette->checkMode = CheckDocument::checkPrint;
				docCheckerPalette->buildErrorList(doc);
				docCheckerPalette->show();
				scrActions["toolsPreflightVerifier"]->setChecked(true);
				return;
			}
		}
	}
	slotReallyPrint();
}

void ScribusMainWindow::slotReallyPrint()
{
	bool done;
	if (docCheckerPalette->isIgnoreEnabled())
	{
		docCheckerPalette->setIgnoreEnabled(false);
		docCheckerPalette->hide();
		docCheckerPalette->checkMode = CheckDocument::checkNULL;
		scrActions["toolsPreflightVerifier"]->setChecked(false);
		disconnect(docCheckerPalette, SIGNAL(ignoreAllErrors()), this, SLOT(slotReallyPrint()));
	}
	QString printError;
	PrintOptions options;
	mainWindowStatusLabel->setText( tr("Printing..."));
	if (doc->Print_Options.firstUse)
	{
		doc->Print_Options.printer = QString();
		if (!doc->DocName.startsWith( tr("Document")))
		{
			QFileInfo fi(doc->DocName);
			doc->Print_Options.filename = fi.path()+"/"+fi.baseName()+".ps";
		}
		else
		{
			QDir di = QDir();
			doc->Print_Options.filename = di.currentPath()+"/"+doc->DocName+".ps";
		}
	}
	doc->Print_Options.copies = 1;
	ColorList usedSpots;
	doc->getUsedColors(usedSpots, true);
	QStringList spots = usedSpots.keys();
	PrintDialog *printer = new PrintDialog(this, doc, doc->Print_Options, prefsManager->appPrefs.printerPrefs.GCRMode, spots);
	printer->setMinMax(1, doc->Pages->count(), doc->currentPage()->pageNr()+1);
	printDinUse = true;
	connect(printer, SIGNAL(doPreview()), this, SLOT(doPrintPreview()));
	if (printer->exec())
	{
		ReOrderText(doc, view);
		qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
		doc->Print_Options.pageNumbers.clear();
		if (printer->doPrintCurrentPage())
			doc->Print_Options.pageNumbers.push_back(doc->currentPage()->pageNr()+1);
		else
		{
			if (printer->doPrintAll())
				parsePagesString("*", &doc->Print_Options.pageNumbers, doc->DocPages.count());
			else
				parsePagesString(printer->getPageString(), &doc->Print_Options.pageNumbers, doc->DocPages.count());
		}
		PrinterUsed = true;
		done = doPrint(doc->Print_Options, printError);
		if (!done)
		{
			QString message = tr("Printing failed!");
			if (!printError.isEmpty())
				message += QString("\n%1").arg(printError);
			qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
			QMessageBox::warning(this, CommonStrings::trWarning, message, CommonStrings::tr_OK);
		}
		else
			doc->Print_Options.firstUse = false;
		getDefaultPrinter(PDef.Pname, PDef.Pname, PDef.Command);
		qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
	}
	printDinUse = false;
	disconnect(printer, SIGNAL(doPreview()), this, SLOT(doPrintPreview()));
	delete printer;
	mainWindowStatusLabel->setText( tr("Ready"));
}

bool ScribusMainWindow::doPrint(PrintOptions &options, QString& error)
{
	bool printDone = false;
	QString filename(options.filename);
	if (options.toFile)
	{
		qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
		if (!overwrite(this, filename))
		{
			return true;
		}
		qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
	}
	ScCore->fileWatcher->forceScan();
	ScCore->fileWatcher->stop();
	ScPrintEngine* prnEngine = NULL;
#if defined(_WIN32)
	SHORT shiftState = GetKeyState( VK_SHIFT );
	bool  forceGDI = ( shiftState & 0x8000 ) ? true : false;
	if (doc->Print_Options.toFile)
		prnEngine = dynamic_cast<ScPrintEngine*>(new ScPrintEngine_PS());
	else
	{
		ScPrintEngine_GDI* gdiEngine = new ScPrintEngine_GDI();
		gdiEngine->setForceGDI( forceGDI );
		prnEngine = dynamic_cast<ScPrintEngine*>(gdiEngine);
	}
#else
	prnEngine = dynamic_cast<ScPrintEngine*>(new ScPrintEngine_PS());
#endif
	if (prnEngine)
	{
		printDone = prnEngine->print(*doc, options);
		if (!printDone)
			error = prnEngine->errorMessage();
		delete prnEngine;
	}
	else
		error = tr( "Print engine initialization failed");
	ScCore->fileWatcher->start();
	return printDone;
}

void ScribusMainWindow::slotFileQuit()
{
	propertiesPalette->unsetDoc();
	symbolPalette->unsetDoc();
	ScCore->pluginManager->savePreferences();
	fileToolBar->connectPrefsSlot(false);
	editToolBar->connectPrefsSlot(false);
	modeToolBar->connectPrefsSlot(false);
	pdfToolBar->connectPrefsSlot(false);
	close();
}

void ScribusMainWindow::slotEditCut()
{
//	int a;
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	QString BufferI = "";
	uint docSelectionCount=doc->m_Selection->count();
	if ((HaveDoc) && (docSelectionCount != 0))
	{
		UndoTransaction* activeTransaction = NULL;
		PageItem *currItem;
		for (uint i = 0; i < docSelectionCount; ++i)
		{
			currItem=doc->m_Selection->itemAt(i);
			if ((currItem->asTextFrame() || currItem->asPathText()) && currItem==storyEditor->currentItem() && doc==storyEditor->currentDocument())
			{
					QMessageBox::critical(this, tr("Cannot Cut In-Use Item"), tr("The item %1 is currently being edited by Story Editor. The cut operation will be cancelled").arg(currItem->itemName()), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
					return;
			}
		}
		if (UndoManager::undoEnabled())
		{
			if (docSelectionCount > 1)
				activeTransaction = new UndoTransaction(undoManager->beginTransaction(Um::SelectionGroup, Um::IGroup, Um::Cut,"",Um::ICut));
			else
			{
				PageItem* item=doc->m_Selection->itemAt(0);
				activeTransaction = new UndoTransaction(undoManager->beginTransaction(item->getUName(), item->getUPixmap(), Um::Cut, "", Um::ICut));
			}
		}
		currItem = doc->m_Selection->itemAt(0);
		if (doc->appMode == modeEdit)
		{
			if ((currItem->itemText.length() == 0) || (!currItem->HasSel))
				return;
			StoryText itemText(doc);
			itemText.setDefaultStyle(currItem->itemText.defaultStyle());
			itemText.insert(0, currItem->itemText, true);

			std::ostringstream xmlString;
			SaxXML xmlStream(xmlString);
			xmlStream.beginDoc();
			itemText.saxx(xmlStream, "SCRIBUSTEXT");
			xmlStream.endDoc();
			std::string xml(xmlString.str());

			ScTextMimeData* mimeData = new ScTextMimeData();
			mimeData->setScribusText (QByteArray(xml.c_str(), xml.length()));
			mimeData->setText( itemText.text(0, itemText.length()) ) ;
			QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);

			dynamic_cast<PageItem_TextFrame*>(currItem)->deleteSelectedTextFromFrame();
			currItem->update();
		}
		else
		{
			if (((currItem->isSingleSel) && (currItem->isGroup())) || ((currItem->isSingleSel) && (currItem->isTableItem)))
				return;
			ScriXmlDoc ss;
			QString BufferS = ss.WriteElem(doc, doc->m_Selection);
			if ((prefsManager->appPrefs.scrapbookPrefs.doCopyToScrapbook) && (!internalCopy))
			{
				scrapbookPalette->ObjFromCopyAction(BufferS, currItem->itemName());
				rebuildRecentPasteMenu();
			}
			ScElemMimeData* mimeData = new ScElemMimeData();
			mimeData->setScribusElem(BufferS);
			mimeData->setText(BufferS);
			QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);
			for (int i=0; i < doc->m_Selection->count(); ++i)
			{
				PageItem* frame = doc->m_Selection->itemAt(i);
				if (frame->asTextFrame() && frame->prevInChain() == NULL)
					frame->clearContents();
			}
			doc->itemSelection_DeleteItem();
		}
		slotDocCh();
		scrActions["editPaste"]->setEnabled(true);
		scrMenuMgr->setMenuEnabled("EditPasteRecent", scrapbookPalette->tempBView->objectMap.count() != 0);
		if (activeTransaction)
		{
			activeTransaction->commit();
			delete activeTransaction;
			activeTransaction = NULL;
		}
	}
}

void ScribusMainWindow::slotEditCopy()
{
//	int a;
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	QString BufferI = "";
	if ((HaveDoc) && (doc->m_Selection->count() != 0))
	{
		PageItem *currItem = doc->m_Selection->itemAt(0);
		if ((doc->appMode == modeEdit) && (currItem->HasSel))
		{
			StoryText itemText(doc);
			itemText.setDefaultStyle(currItem->itemText.defaultStyle());
			itemText.insert(0, currItem->itemText, true);

			BufferI = itemText.text(0, itemText.length());

			std::ostringstream xmlString;
			SaxXML xmlStream(xmlString);
			xmlStream.beginDoc();
			itemText.saxx(xmlStream, "SCRIBUSTEXT");
			xmlStream.endDoc();
			std::string xml(xmlString.str());

			ScTextMimeData* mimeData = new ScTextMimeData();
			mimeData->setScribusText( QByteArray(xml.c_str(), xml.length()) );
			mimeData->setText( itemText.text(0, itemText.length()) );
			QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);
		}
		else
		{
			if (((currItem->isSingleSel) && (currItem->isGroup())) || ((currItem->isSingleSel) && (currItem->isTableItem)))
				return;
			ScriXmlDoc ss;
			QString BufferS = ss.WriteElem(doc, doc->m_Selection);
			if ((prefsManager->appPrefs.scrapbookPrefs.doCopyToScrapbook) && (!internalCopy))
			{
				scrapbookPalette->ObjFromCopyAction(BufferS, currItem->itemName());
				rebuildRecentPasteMenu();
			}
			ScElemMimeData* mimeData = new ScElemMimeData();
			mimeData->setScribusElem(BufferS);
			mimeData->setText(BufferS);
			QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);
		}
		scrActions["editPaste"]->setEnabled(true);
		scrMenuMgr->setMenuEnabled("EditPasteRecent", scrapbookPalette->tempBView->objectMap.count() != 0);
	}
}

void ScribusMainWindow::slotEditPaste()
{
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	if (HaveDoc)
	{
		UndoTransaction* activeTransaction = NULL;
		if (!ScMimeData::clipboardHasScribusData())
			return;
		if (UndoManager::undoEnabled())
			activeTransaction = new UndoTransaction(undoManager->beginTransaction(doc->currentPage()->getUName(), 0, Um::Paste, "", Um::IPaste));
		PageItem* selItem = doc->m_Selection->itemAt(0);
		bool isTextFrame  = (dynamic_cast<PageItem_TextFrame*>(selItem) != NULL);
		if (doc->appMode == modeEdit && isTextFrame)
		{
			PageItem_TextFrame *currItem = dynamic_cast<PageItem_TextFrame*>(doc->m_Selection->itemAt(0));
			assert(currItem != NULL);
			if (currItem->HasSel)
				currItem->deleteSelectedTextFromFrame();

			/*if (currItem->CPos < 0)
				currItem->CPos = 0;
			if (currItem->CPos > currItem->itemText.length())
				currItem->CPos = currItem->itemText.length();*/

			if (ScMimeData::clipboardHasScribusText())
			{
				Serializer dig(*doc); // TODO: do we really need a new serializer here?
				dig.store<ScribusDoc>("<scribusdoc>", doc);
				StoryText::desaxeRules("/", dig, "SCRIBUSTEXT");
				dig.addRule("/SCRIBUSTEXT", desaxe::Result<StoryText>());

				QByteArray xml = ScMimeData::clipboardScribusText();
				dig.parseMemory(xml, xml.length());

				StoryText* story = dig.result<StoryText>();

				currItem->itemText.insert(*story);

				delete story;
			}
			else if (ScMimeData::clipboardHasScribusElem() || ScMimeData::clipboardHasScribusFragment())
			{
				bool savedAlignGrid = doc->useRaster;
				bool savedAlignGuides = doc->SnapGuides;
				int ac = doc->Items->count();
				bool isGroup = false;
				double gx, gy, gh, gw;
				FPoint minSize = doc->minCanvasCoordinate;
				FPoint maxSize = doc->maxCanvasCoordinate;
				doc->useRaster = false;
				doc->SnapGuides = false;
				// HACK #6541 : undo does not handle text modification => do not record embedded item creation
				// if embedded item is deleted, undo system will not be aware of its deletion => crash - JG
				undoManager->setUndoEnabled(false);
				QString buffer  = ScMimeData::clipboardScribusElem();
				slotElemRead(buffer, 0, 0, false, true, doc, view);
				// update style lists:
				styleManager->setDoc(doc);
				propertiesPalette->unsetDoc();
				propertiesPalette->setDoc(doc);
				symbolPalette->unsetDoc();
				symbolPalette->setDoc(doc);

				doc->useRaster = savedAlignGrid;
				doc->SnapGuides = savedAlignGuides;
				//int tempList=doc->m_Selection->backupToTempList(0);
				Selection tempSelection(*doc->m_Selection);
				doc->m_Selection->clear();
				if (doc->Items->count() - ac > 1)
					isGroup = true;
				doc->m_Selection->delaySignalsOn();
				for (int as = ac; as < doc->Items->count(); ++as)
				{
					doc->m_Selection->addItem(doc->Items->at(as));
				}
				doc->m_Selection->delaySignalsOff();
				if (isGroup)
					doc->GroupCounter++;
				doc->m_Selection->setGroupRect();
				doc->m_Selection->getGroupRect(&gx, &gy, &gw, &gh);
				PageItem* currItem3 = doc->Items->at(ac);
				for (int as = ac; as < doc->Items->count(); ++as)
				{
					PageItem* currItem2 = doc->Items->at(as);
					currItem2->isEmbedded = true;
					currItem2->setIsAnnotation(false);
					currItem2->isBookmark = false;
					currItem2->gXpos = currItem2->xPos() - gx;
					currItem2->gYpos = currItem2->yPos() - gy;
					currItem2->gWidth = gw;
					currItem2->gHeight = gh;
					currItem2->ItemNr = doc->FrameItems.count();
					doc->FrameItems.append(currItem2);
				}
				int acc = doc->Items->count();
				for (int as = ac; as < acc; ++as)
				{
					doc->Items->takeAt(ac);
				}
				doc->m_Selection->clear();
				//doc->m_Selection->restoreFromTempList(0, tempList);
				*doc->m_Selection=tempSelection;
//				view->resizeContents(qRound((maxSize.x() - minSize.x()) * view->scale()), qRound((maxSize.y() - minSize.y()) * view->scale()));
//				view->scrollBy(qRound((doc->minCanvasCoordinate.x() - minSize.x()) * view->scale()), qRound((doc->minCanvasCoordinate.y() - minSize.y()) * view->scale()));
				doc->minCanvasCoordinate = minSize;
				doc->maxCanvasCoordinate = maxSize;
				if (outlinePalette->isVisible())
					outlinePalette->BuildTree();
				currItem->itemText.insertObject(currItem3);
				undoManager->setUndoEnabled(true);
			}
			else
			{
				// K.I.S.S.:
				QString text = QApplication::clipboard()->text(QClipboard::Clipboard);
				text = text.replace("\r\n", SpecialChars::PARSEP);
				text = text.replace('\n', SpecialChars::PARSEP);
				currItem->itemText.insertChars(text, true);
			}
			currItem->update();
		}
		else
		{
			if (ScMimeData::clipboardHasScribusElem() || ScMimeData::clipboardHasScribusFragment())
			{
				view->Deselect(true);
				uint ac = doc->Items->count();
				bool savedAlignGrid = doc->useRaster;
				bool savedAlignGuides = doc->SnapGuides;
				doc->useRaster = false;
				doc->SnapGuides = false;
				QString buffer  = ScMimeData::clipboardScribusElem();
				slotElemRead(buffer, doc->currentPage()->xOffset(), doc->currentPage()->yOffset(), false, true, doc, view);
				// update style lists:
				styleManager->setDoc(doc);
				propertiesPalette->unsetDoc();
				propertiesPalette->setDoc(doc);
				symbolPalette->unsetDoc();
				symbolPalette->setDoc(doc);

				doc->useRaster = savedAlignGrid;
				doc->SnapGuides = savedAlignGuides;
				doc->m_Selection->delaySignalsOn();
				for (int as = ac; as < doc->Items->count(); ++as)
				{
					PageItem* currItem = doc->Items->at(as);
					if (currItem->isBookmark)
						AddBookMark(currItem);
					doc->m_Selection->addItem(currItem);
				}
				doc->m_Selection->delaySignalsOff();
				int docSelectionCount=doc->m_Selection->count();
				if (docSelectionCount > 0)
				{
					doc->m_Selection->itemAt(0)->connectToGUI();
					doc->m_Selection->itemAt(0)->emitAllToGUI();
				}
				if (docSelectionCount > 1)
				{
					doc->m_Selection->setGroupRect();
					//double x, y, w, h; //CHECKME
					//doc->m_Selection->getGroupRect(&x, &y, &w, &h);
					//propertiesPalette->setXY(x, y);
					//propertiesPalette->setBH(w, h);
				}
				if (docSelectionCount > 0)
					HaveNewSel(doc->m_Selection->itemAt(0)->itemType());
			}
			view->DrawNew();
		}
		if (activeTransaction)
		{
			activeTransaction->commit();
			delete activeTransaction;
			activeTransaction = NULL;
		}
		slotDocCh(false);
	}
}

//CB-->Doc ?????
void ScribusMainWindow::SelectAllOnLayer()
{
	ColorList UsedC;
	doc->getUsedColors(UsedC);
	selectDialog *dia = new selectDialog(this, UsedC, doc->unitIndex());
	if (dia->exec())
	{
		PageItem *currItem;
		view->Deselect();
		uint docItemsCount = doc->Items->count();
		int docCurrentPage = doc->currentPageNumber();
		doc->m_Selection->delaySignalsOn();
		int range = dia->getSelectionRange();
		for (uint a = 0; a < docItemsCount; ++a)
		{
			currItem = doc->Items->at(a);
			if ((currItem->LayerID == doc->activeLayer()) && (!doc->layerLocked(currItem->LayerID)))
			{
				if ((range == 0) && (currItem->OwnPage != docCurrentPage))
					continue;
				if ((range == 2) && (currItem->OwnPage != -1))
					continue;
				if (dia->useAttributes())
				{
					bool useType = false;
					bool useFill = false;
					bool useLine = false;
					bool useLWidth = false;
					bool usePrint = false;
					bool useLocked = false;
					bool useResize = false;
					dia->getUsedAttributes(useType, useFill, useLine, useLWidth, usePrint, useLocked, useResize);
					int Type = 0;
					QString Fill = "";
					QString Line = "";
					double LWidth = 0.0;
					bool Print = false;
					bool Locked = false;
					bool Resize = false;
					dia->getUsedAttributesValues(Type, Fill, Line, LWidth, Print, Locked, Resize);
					LWidth = LWidth / doc->unitRatio();
					if ((useType) && (Type != currItem->realItemType()))
						continue;
					if ((useFill) && ((Fill != currItem->fillColor()) || (currItem->GrType != 0)))
						continue;
					if ((useLine) && (Line != currItem->lineColor()))
						continue;
					if ((useLWidth) && ((LWidth != currItem->lineWidth()) || (currItem->lineColor() == CommonStrings::None)))
						continue;
					if ((usePrint) && (Print != currItem->printEnabled()))
						continue;
					if ((useLocked) && (Locked != currItem->locked()))
						continue;
					if ((useResize) && (Resize != currItem->sizeLocked()))
						continue;
					doc->m_Selection->addItem(currItem);
				}
				else
					doc->m_Selection->addItem(currItem);
			}
		}
		doc->m_Selection->delaySignalsOff();
		int docSelectionCount = doc->m_Selection->count();
		if (docSelectionCount > 0)
		{
			doc->m_Selection->itemAt(0)->connectToGUI();
			doc->m_Selection->itemAt(0)->emitAllToGUI();
		}
		if (docSelectionCount > 1)
		{
			double x, y, w, h;
			doc->m_Selection->setGroupRect();
			doc->m_Selection->getGroupRect(&x, &y, &w, &h);
			//Now unuseful as PropertiesPalette_XYZ::setCurrentItem() handles multiple selection
			//propertiesPalette->setXY(x, y);
			//propertiesPalette->setBH(w, h);
		}
		if (docSelectionCount > 0)
		{
			currItem = doc->m_Selection->itemAt(0);
			HaveNewSel(currItem->itemType());
		}
		view->DrawNew();
	}
	delete dia;
}

void ScribusMainWindow::SelectAll(bool docWideSelect)
{
	if (doc->appMode == modeEdit)
	{
		PageItem *currItem = doc->m_Selection->itemAt(0);
		PageItem *nextItem = currItem;
		nextItem->itemText.selectAll();
		while (nextItem != 0)
		{
			if (nextItem->prevInChain() != 0)
				nextItem = nextItem->prevInChain();
			else
				break;
		}
		while (nextItem != 0)
		{
			nextItem->HasSel = true;
			nextItem = nextItem->nextInChain();
		}
		EnableTxEdit();
	}
	else
	{
		PageItem *currItem;
		view->Deselect();
		doc->m_Selection->delaySignalsOn();
		uint docItemsCount=doc->Items->count();
		int docCurrentPage=doc->currentPageNumber();
		for (uint a = 0; a < docItemsCount; ++a)
		{
			currItem = doc->Items->at(a);
			if ((currItem->LayerID == doc->activeLayer()) && (!doc->layerLocked(currItem->LayerID)))
			{
				if (docWideSelect)
					doc->m_Selection->addItem(currItem);
				else
				{
					if (currItem->OwnPage==docCurrentPage)
						doc->m_Selection->addItem(currItem);
				}
			}
		}
		doc->m_Selection->delaySignalsOff();
		int docSelectionCount=doc->m_Selection->count();
		if (docSelectionCount > 0)
		{
			doc->m_Selection->itemAt(0)->connectToGUI();
			doc->m_Selection->itemAt(0)->emitAllToGUI();
		}
		if (docSelectionCount > 1)
		{
			//double x, y, w, h; //CHECKME
			doc->m_Selection->setGroupRect();
			//doc->m_Selection->getGroupRect(&x, &y, &w, &h);
			//Now unuseful as PropertiesPalette_XYZ::setCurrentItem() handles multiple selection
			//propertiesPalette->setXY(x, y);
			//propertiesPalette->setBH(w, h);
		}
		if (docSelectionCount > 0)
		{
			currItem = doc->m_Selection->itemAt(0);
			HaveNewSel(currItem->itemType());
		}
	}
	view->DrawNew();
}

void ScribusMainWindow::deselectAll()
{
	if (doc->appMode == modeEdit)
	{
		if (doc->m_Selection->count() <= 0)
			return;
		PageItem *currItem = doc->m_Selection->itemAt(0);
		if (currItem->asTextFrame())
		{
			currItem->itemText.deselectAll();
			doc->regionsChanged()->update(currItem->getBoundingRect());
		}
		else
		{
			doc->view()->Deselect(true);
			doc->view()->requestMode(modeNormal);
		}
	}
	else if (view != NULL)
		view->Deselect(true);
}

void ScribusMainWindow::ClipChange()
{
	bool textFrameEditMode = false;
	bool hasScribusData = ScMimeData::clipboardHasScribusElem() || ScMimeData::clipboardHasScribusFragment();
	if (HaveDoc && doc->m_Selection->count() != 0)
	{
		PageItem *currItem = NULL;
		currItem = doc->m_Selection->itemAt(0);
		textFrameEditMode  = ((doc->appMode == modeEdit) && (currItem->asTextFrame()));
	}
	scrActions["editPaste"]->setEnabled(HaveDoc && (hasScribusData || textFrameEditMode));
}

void ScribusMainWindow::EnableTxEdit()
{
	scrActions["editCut"]->setEnabled(true);
	scrActions["editCopy"]->setEnabled(true);
	//scrActions["editClearContents"]->setEnabled(true);
}

void ScribusMainWindow::DisableTxEdit()
{
	scrActions["editCut"]->setEnabled(false);
	scrActions["editCopy"]->setEnabled(false);
	//scrActions["editClearContents"]->setEnabled(false);
}

void ScribusMainWindow::slotHelpAbout()
{
	About dia(this);
	dia.exec();
}

void ScribusMainWindow::slotHelpAboutPlugins()
{
	AboutPlugins dia(this);
	dia.exec();
}

void ScribusMainWindow::slotHelpAboutQt()
{
	QMessageBox::aboutQt(this, tr("About Qt"));
}

void ScribusMainWindow::slotHelpCheckUpdates()
{
	About dia(this, About::CheckUpdates);
	dia.exec();
}

void ScribusMainWindow::slotOnlineHelp(const QString & jumpToSection, const QString & jumpToFile)
{
	if (!helpBrowser)
	{
		helpBrowser = new HelpBrowser(0, tr("Scribus Manual"), ScCore->getGuiLanguage(), jumpToSection, jumpToFile);
		connect(helpBrowser, SIGNAL(closed()), this, SLOT(slotOnlineHelpClosed()));
	}
	else //just set the requested page
	{
		if (!jumpToSection.isNull() || !jumpToFile.isNull())
		{
			helpBrowser->jumpToHelpSection(jumpToSection, jumpToFile, true);
		}
	}
	slotRaiseOnlineHelp();
}

void ScribusMainWindow::slotRaiseOnlineHelp()
{
	if (helpBrowser)
		helpBrowser->show();
}

void ScribusMainWindow::slotOnlineHelpClosed()
{
	if (helpBrowser)
		helpBrowser->deleteLater();
}

void ScribusMainWindow::ToggleTips()
{
	prefsManager->appPrefs.displayPrefs.showToolTips = scrActions["helpTooltips"]->isChecked();
}

void ScribusMainWindow::ToggleMouseTips()
{
	prefsManager->appPrefs.displayPrefs.showMouseCoordinates = scrActions["showMouseCoordinates"]->isChecked();
}

void ScribusMainWindow::SaveText()
{
	LoadEnc = "";
	QString wdir = ".";
	QString prefsDocDir=prefsManager->documentDir();
	if (!prefsDocDir.isEmpty())
		wdir = prefsManager->prefsFile->getContext("dirs")->get("save_text", prefsDocDir);
	else
		wdir = prefsManager->prefsFile->getContext("dirs")->get("save_text", ".");
	QString fn = CFileDialog( wdir, tr("Save As"), tr("Text Files (*.txt);;All Files(*)"), "", fdShowCodecs|fdHidePreviewCheckBox);
	if (!fn.isEmpty())
	{
		prefsManager->prefsFile->getContext("dirs")->set("save_text", fn.left(fn.lastIndexOf("/")));
		const StoryText& story (doc->m_Selection->itemAt(0)->itemText);
		Serializer::writeWithEncoding(fn, LoadEnc, story.text(0, story.length()));
	}
}

void ScribusMainWindow::applyNewMaster(QString name)
{
	Apply_MasterPage(name, doc->currentPage()->pageNr(), false);
	view->reformPages();
	view->DrawNew();
	pagePalette->Rebuild();
}

void ScribusMainWindow::slotNewPageP(int wo, QString templ)
{
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	view->Deselect(true);
	int where = 1;
	if (wo == 0)
		where = 0;
	else if (wo == doc->Pages->count())
		where = 2;
	slotNewPage(wo, templ); //master page is applied now
	//applyNewMaster(templ);
	if (where == 2)
		doc->addPageToSection(wo, where, 1);
	else
		doc->addPageToSection(wo+1, where, 1);
	doc->changed();
	if (outlinePalette->isVisible())
		outlinePalette->BuildTree();
	pagePalette->rebuildPages();
	view->DrawNew();
}

/** Erzeugt eine neue Seite */
void ScribusMainWindow::slotNewPageM()
{
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	view->Deselect(true);
	InsPage *dia = new InsPage(this, doc, doc->currentPage()->pageNr(), doc->Pages->count());
	if (dia->exec())
	{
		QStringList base(dia->getMasterPages());
		double height=dia->heightSpinBox->value() / doc->unitRatio();
		double width=dia->widthSpinBox->value() / doc->unitRatio();
		int orientation=dia->orientationQComboBox->currentIndex();
		addNewPages(dia->getWherePage(), dia->getWhere(), dia->getCount(), height, width, orientation, 
			dia->prefsPageSizeName, dia->moveObjects->isChecked(), &base, dia->overrideMPSizingCheckBox->checkState()==Qt::Checked);
	}
	delete dia;
}

void ScribusMainWindow::addNewPages(int wo, int where, int numPages, double height, double width, int orient, QString siz, bool mov, QStringList* basedOn, bool overrideMasterPageSizing)
{
	UndoTransaction* activeTransaction = NULL;
	if (UndoManager::undoEnabled())
	{
		activeTransaction = new UndoTransaction(undoManager->beginTransaction(doc->getUName(), Um::IDocument, (numPages == 1) ? Um::AddPage : Um::AddPages, "", Um::ICreate));
		SimpleState *ss = new SimpleState(Um::AddPage, "", Um::ICreate);
		ss->set("ADD_PAGE", "add_page");
		ss->set("PAGE", wo);
		ss->set("WHERE", where);
		ss->set("COUNT", numPages);
		if (basedOn != NULL)
			ss->set("BASED", basedOn->join("|"));
		else
		{
			int setcol = doc->pageSets()[doc->pagePositioning()].Columns;
			if (setcol == 1)
				ss->set("BASED", CommonStrings::trMasterPageNormal);
			else if (setcol == 2)
				ss->set("BASED", CommonStrings::trMasterPageNormalLeft+"|"+ CommonStrings::trMasterPageNormalRight);
			else if ((setcol == 3) || (setcol == 4))
				ss->set("BASED", CommonStrings::trMasterPageNormalLeft+"|"+ CommonStrings::trMasterPageNormalMiddle+"|"+ CommonStrings::trMasterPageNormalRight);
		}
		ss->set("HEIGHT", height);
		ss->set("WIDTH", width);
		ss->set("ORIENT", orient);
		ss->set("SIZE", siz);
		ss->set("MOVED", mov);
		undoManager->action(this, ss);
	}
	// disable recording of undo actions related to new page creating
	// and object moving related to that
	undoManager->setUndoEnabled(false);

	QStringList base;
	if (basedOn != NULL)
	{
		base = *basedOn;
		// #10211 case when restoring page deletion, basedOn contains only masterpage name
		if (base.count() == 1)
		{
			int setcol = doc->pageSets()[doc->pagePositioning()].Columns;
			while (base.count() < setcol)
				base.append (base.at(0));
		}
	}
	if (base.empty())
	{
		int setcol = doc->pageSets()[doc->pagePositioning()].Columns;
		if (setcol == 1)
			base.append( CommonStrings::trMasterPageNormal);
		else if (setcol == 2)
		{
			base.append( CommonStrings::trMasterPageNormalLeft);
			base.append( CommonStrings::trMasterPageNormalRight);
		}
		else if (setcol == 3)
		{
			base.append( CommonStrings::trMasterPageNormalLeft);
			base.append( CommonStrings::trMasterPageNormalMiddle);
			base.append( CommonStrings::trMasterPageNormalRight);
		}
		else if (setcol == 4)
		{
			base.append( CommonStrings::trMasterPageNormalLeft);
			base.append( CommonStrings::trMasterPageNormalMiddle);
			base.append( CommonStrings::trMasterPageNormalMiddle);
			base.append( CommonStrings::trMasterPageNormalRight);
		}
	}
	int cc;
	int wot = wo;
	if (where==0)
		--wot;
	else if (where==2)
		wot=doc->Pages->count();
	qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
	view->updatesOn(false);
	ScPage* currentPage = doc->currentPage();
	for (cc = 0; cc < numPages; ++cc)
	{
		slotNewPage(wot, base[(wot+doc->pageSets()[doc->pagePositioning()].FirstPage) % doc->pageSets()[doc->pagePositioning()].Columns], mov); //Avoid the master page application with QString::null
//		slotNewPage(wot, QString::null, mov); //Avoid the master page application with QString::null
		//CB: #8212: added overrideMasterPageSizing, but keeping default to true for other calls for now, off for calls from InsPage
		if (overrideMasterPageSizing)
		{	
			doc->currentPage()->setInitialHeight(height);
			doc->currentPage()->setInitialWidth(width);
			doc->currentPage()->setOrientation(orient);
			doc->currentPage()->m_pageSize = siz;
		}
		//CB If we want to add this master page setting into the slotnewpage call, the pagenumber must be +1 I think
	//Apply_MasterPage(base[(doc->currentPage()->pageNr()+doc->pageSets[doc->currentPageLayout].FirstPage) % doc->pageSets[doc->currentPageLayout].Columns],
//						 doc->currentPage()->pageNr(), false); // this Apply_MasterPage avoids DreawNew and PagePalette->ReBuild, which is much faster for 100 pp :-)
		wot ++;
	}
	doc->setCurrentPage(currentPage);
	view->updatesOn(true);
	qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
	//Use wo, the dialog currently returns a page Index +1 due to old numbering scheme, function now does the -1 as required
	doc->changed();
	doc->addPageToSection(wo, where, numPages);
	pagePalette->rebuildPages();
	view->reformPages(mov);
	view->DrawNew();
	if (outlinePalette->isVisible())
		outlinePalette->BuildTree();

	undoManager->setUndoEnabled(true);

	if (activeTransaction)
	{
		activeTransaction->commit();
		delete activeTransaction;
		activeTransaction = NULL;
	}
}

void ScribusMainWindow::slotNewMasterPage(int w, const QString& name)
{
	if (doc->masterPageMode())
	{
		doc->addMasterPage(w, name);
		view->addPage(w);
	}
}

void ScribusMainWindow::slotNewPage(int w, const QString& masterPageName, bool mov)
{
	doc->addPage(w, masterPageName, true);
	view->addPage(w, mov);
}


void ScribusMainWindow::duplicateToMasterPage()
{
	if (!HaveDoc)
		return;
	view->Deselect(true);
	int pageLocationIndex=-1;
	int pageLocationCount=0;
	if (doc->pagePositioning() != singlePage)
	{
		QStringList locationEntries;
		QList<PageSet> pageSet(doc->pageSets());
		for(QStringList::Iterator pNames = pageSet[doc->pagePositioning()].pageNames.begin(); pNames != pageSet[doc->pagePositioning()].pageNames.end(); ++pNames )
		{
			locationEntries << CommonStrings::translatePageSetLocString(*pNames);
		}
		pageLocationIndex=doc->columnOfPage(doc->currentPageNumber());
		pageLocationCount=locationEntries.count();
	}

	CopyPageToMasterPageDialog copyDialog(doc->MasterNames.count(), doc->pageSets()[doc->pagePositioning()].pageNames, pageLocationIndex, this);
	if (copyDialog.exec())
	{
		bool copyFromMaster=false;
		QString masterPageName;
		int pageLocation=0;
		copyDialog.values(masterPageName, copyFromMaster, pageLocation);
		bool badMasterPageName = doc->MasterNames.contains(masterPageName);
		badMasterPageName |= (masterPageName == CommonStrings::masterPageNormal);
		badMasterPageName |= (masterPageName == CommonStrings::trMasterPageNormal);
		badMasterPageName |= (masterPageName == CommonStrings::trMasterPageNormalLeft);
		badMasterPageName |= (masterPageName == CommonStrings::trMasterPageNormalMiddle);
		badMasterPageName |= (masterPageName == CommonStrings::trMasterPageNormalRight);
		badMasterPageName |=  masterPageName.isEmpty();
		while (badMasterPageName)
		{
			if (!copyDialog.exec())
				return;
			copyDialog.values(masterPageName, copyFromMaster, pageLocation);
			badMasterPageName = doc->MasterNames.contains(masterPageName);
			badMasterPageName |= (masterPageName == CommonStrings::masterPageNormal);
			badMasterPageName |= (masterPageName == CommonStrings::trMasterPageNormal);
			badMasterPageName |= (masterPageName == CommonStrings::trMasterPageNormalLeft);
			badMasterPageName |= (masterPageName == CommonStrings::trMasterPageNormalMiddle);
			badMasterPageName |= (masterPageName == CommonStrings::trMasterPageNormalRight);
			badMasterPageName |=  masterPageName.isEmpty();
		}
		int currentPageNumber=doc->currentPage()->pageNr();
		bool ok=doc->copyPageToMasterPage(currentPageNumber, pageLocation, pageLocationCount, masterPageName, copyFromMaster);
		Q_ASSERT(ok); //TODO get a return value in case the copy was not possible
		pagePalette->Rebuild();
	}
}

void ScribusMainWindow::slotZoom(double zoomFactor)
{
	double finalZoomFactor;
	//Zoom to Fit
	if (zoomFactor == -100.0)
	{
		finalZoomFactor = (view->height()-70) / (doc->currentPage()->height()+30);
	}
	else if (zoomFactor == -200.0)
	{
		finalZoomFactor = (view->width()-50) / (doc->currentPage()->width()+30);
	}
	//Zoom to %
	else
		finalZoomFactor = zoomFactor*prefsManager->displayScale()/100.0;

	if (finalZoomFactor == view->scale())
		return;

	int x = qRound(qMax(view->contentsX() / view->scale(), 0.0));
	int y = qRound(qMax(view->contentsY() / view->scale(), 0.0));
	int w = qRound(qMin(view->visibleWidth() / view->scale(), doc->currentPage()->width()));
	int h = qRound(qMin(view->visibleHeight() / view->scale(), doc->currentPage()->height()));

	if (zoomFactor == -200.0)
		view->rememberOldZoomLocation(qRound(doc->currentPage()->xOffset() + doc->currentPage()->width() / 2.0), h / 2 + y);
	else if(zoomFactor == -100.0)
		view->rememberOldZoomLocation(w / 2 + x, qRound(doc->currentPage()->yOffset() + doc->currentPage()->height() / 2.0));
	else
		view->rememberOldZoomLocation(w / 2 + x, h / 2 + y);

	view->zoom(finalZoomFactor);
}

void ScribusMainWindow::ToggleStickyTools()
{
	prefsManager->appPrefs.uiPrefs.stickyTools = !prefsManager->appPrefs.uiPrefs.stickyTools;
	scrActions["stickyTools"]->setChecked(prefsManager->appPrefs.uiPrefs.stickyTools);
	if (HaveDoc && doc->appMode!=modeNormal && !prefsManager->appPrefs.uiPrefs.stickyTools)
		view->requestMode(modeNormal);
}

void ScribusMainWindow::ToggleAllPalettes()
{
	if (palettesStatus[0])
	{
		palettesStatus[0] = false;
		if (palettesStatus[1])
			propertiesPalette->show();
		if (palettesStatus[2])
			outlinePalette->show();
		if (palettesStatus[3])
			scrapbookPalette->show();
		if (palettesStatus[4])
			layerPalette->show();
		if (palettesStatus[5])
			pagePalette->show();
		if (palettesStatus[6])
			bookmarkPalette->show();
//		if (palettesStatus[7])
//			measurementPalette->show();
		if (palettesStatus[9])
			docCheckerPalette->show();
		setPagePalette(palettesStatus[5]);
		setUndoPalette(palettesStatus[8]);
	}
	else
	{
		palettesStatus[1] = propertiesPalette->isVisible();
		palettesStatus[2] = outlinePalette->isVisible();
		palettesStatus[3] = scrapbookPalette->isVisible();
		palettesStatus[4] = layerPalette->isVisible();
		palettesStatus[5] = pagePalette->isVisible();
		palettesStatus[6] = bookmarkPalette->isVisible();
//		palettesStatus[7] = measurementPalette->isVisible();
		palettesStatus[8] = undoPalette->isVisible();
		palettesStatus[9] = docCheckerPalette->isVisible();
		propertiesPalette->hide();
		outlinePalette->hide();
		scrapbookPalette->hide();
		bookmarkPalette->hide();
		pagePalette->hide();
		layerPalette->hide();
//		measurementPalette->hide();
		docCheckerPalette->hide();
		setPagePalette(false);
		setUndoPalette(false);
		palettesStatus[0] = true;
	}
}

void ScribusMainWindow::toggleCheckPal()
{
	palettesStatus[0] = false;
}

void ScribusMainWindow::setUndoPalette(bool visible)
{
	undoPalette->setPaletteShown(visible);
	scrActions["toolsActionHistory"]->setChecked(visible);
}
/*
void ScribusMainWindow::togglePropertiesPalette()
{
	palettesStatus[0] = false;
}

void ScribusMainWindow::toggleOutlinePalette()
{
	palettesStatus[0] = false;
}

void ScribusMainWindow::toggleScrapbookPalette()
{
	palettesStatus[0] = false;
}

void ScribusMainWindow::toggleLayerPalette()
{
	palettesStatus[0] = false;
}
*/
void ScribusMainWindow::setPagePalette(bool visible)
{
	if (!visible)
	{
		prefsManager->appPrefs.uiPrefs.SepalT = pagePalette->getThumb();
		prefsManager->appPrefs.uiPrefs.SepalN = pagePalette->getNamen();
	}
}

void ScribusMainWindow::togglePagePalette()
{
	setPagePalette(!pagePalette->isVisible());
	palettesStatus[0] = false;
}
/*
void ScribusMainWindow::toggleBookmarkPalette()
{
	palettesStatus[0] = false;
}
*/

void ScribusMainWindow::toggleUndoPalette()
{
	setUndoPalette(!undoPalette->isVisible());
	palettesStatus[0] = false;
}

void ScribusMainWindow::TogglePics()
{
	if (doc)
	{
		doc->guidesPrefs().showPic = !doc->guidesPrefs().showPic;
		QList<PageItem*> allItems;
		for (int a = 0; a < doc->Items->count(); ++a)
		{
			PageItem *currItem = doc->Items->at(a);
			if (currItem->isGroup())
				allItems = currItem->getItemList();
			else
				allItems.append(currItem);
			for (int ii = 0; ii < allItems.count(); ii++)
			{
				PageItem* item = allItems.at(ii);
				if (item->asImageFrame())
					item->setImageShown(doc->guidesPrefs().showPic);
			}
		}
		view->DrawNew();
	}
}

void ScribusMainWindow::ToggleAllGuides()
{
	if (!doc)
		return;
	keyrep=false;
	if (guidesStatus[0])
	{
		guidesStatus[0] = false;
		doc->guidesPrefs().marginsShown = guidesStatus[1];
		doc->guidesPrefs().framesShown = guidesStatus[2];
		doc->guidesPrefs().gridShown = guidesStatus[3];
		doc->guidesPrefs().guidesShown = guidesStatus[4];
		doc->guidesPrefs().baselineGridShown = guidesStatus[5];
		doc->guidesPrefs().linkShown = guidesStatus[6];
		doc->guidesPrefs().showControls = guidesStatus[7];
		doc->guidesPrefs().rulerMode = guidesStatus[8];
		doc->guidesPrefs().rulersShown = guidesStatus[9];
		doc->guidesPrefs().colBordersShown = guidesStatus[10];
		doc->guidesPrefs().layerMarkersShown = guidesStatus[11] ;
		doc->guidesPrefs().showBleed = guidesStatus[12] ;
		ToggleMarks();
		ToggleFrames();
		ToggleLayerMarkers();
		ToggleRaster();
		ToggleGuides();
		ToggleColumnBorders();
		ToggleBase();
		ToggleTextLinks();
		ToggleTextControls();
		ToggleRulerMode();
		ToggleRulers();
		ToggleBleeds();
	}
	else
	{
		guidesStatus[0] = true;
		guidesStatus[1] = !doc->guidesPrefs().marginsShown;
		guidesStatus[2] = !doc->guidesPrefs().framesShown;
		guidesStatus[3] = !doc->guidesPrefs().gridShown;
		guidesStatus[4] = !doc->guidesPrefs().guidesShown;
		guidesStatus[5] = !doc->guidesPrefs().baselineGridShown;
		guidesStatus[6] = !doc->guidesPrefs().linkShown;
		guidesStatus[7] = !doc->guidesPrefs().showControls;
		guidesStatus[8] = !doc->guidesPrefs().rulerMode;
		guidesStatus[9] = !doc->guidesPrefs().rulersShown;
		guidesStatus[10] = !doc->guidesPrefs().colBordersShown;
		guidesStatus[11] = !doc->guidesPrefs().layerMarkersShown;
		guidesStatus[12] = !doc->guidesPrefs().showBleed;
		doc->guidesPrefs().marginsShown = false;
		doc->guidesPrefs().framesShown = false;
		doc->guidesPrefs().gridShown = false;
		doc->guidesPrefs().guidesShown = false;
		doc->guidesPrefs().baselineGridShown = false;
		doc->guidesPrefs().linkShown = false;
		doc->guidesPrefs().showControls = false;
		doc->guidesPrefs().rulerMode = false;
		doc->guidesPrefs().rulersShown = false;
		doc->guidesPrefs().colBordersShown = false;
		doc->guidesPrefs().layerMarkersShown = false;
		doc->guidesPrefs().showBleed = false;
		view->setRulersShown(doc->guidesPrefs().rulersShown);
	}
	scrActions["viewShowMargins"]->setChecked(doc->guidesPrefs().marginsShown);
	scrActions["viewShowBleeds"]->setChecked(doc->guidesPrefs().showBleed);
	scrActions["viewShowFrames"]->setChecked(doc->guidesPrefs().framesShown);
	scrActions["viewShowLayerMarkers"]->setChecked(doc->guidesPrefs().layerMarkersShown);
	scrActions["viewShowGrid"]->setChecked(doc->guidesPrefs().gridShown);
	scrActions["viewShowGuides"]->setChecked(doc->guidesPrefs().guidesShown);
	scrActions["viewShowColumnBorders"]->setChecked(doc->guidesPrefs().colBordersShown);
	scrActions["viewShowBaseline"]->setChecked(doc->guidesPrefs().baselineGridShown);
	scrActions["viewShowTextChain"]->setChecked(doc->guidesPrefs().linkShown);
	scrActions["viewShowTextControls"]->setChecked(doc->guidesPrefs().showControls);
	scrActions["viewShowRulers"]->setChecked(doc->guidesPrefs().rulersShown);
	scrActions["viewRulerMode"]->setChecked(doc->guidesPrefs().rulerMode);
	view->DrawNew();
}

void ScribusMainWindow::ToggleMarks()
{
	if (doc)
	{
		guidesStatus[0] = false;
		doc->guidesPrefs().marginsShown = !doc->guidesPrefs().marginsShown;
		view->DrawNew();
	}
}

void ScribusMainWindow::ToggleBleeds()
{
	if (doc)
	{
		guidesStatus[0] = false;
		doc->guidesPrefs().showBleed = !doc->guidesPrefs().showBleed;
		view->DrawNew();
	}
}

void ScribusMainWindow::ToggleFrames()
{
	if (doc)
	{
		guidesStatus[0] = false;
		doc->guidesPrefs().framesShown = !doc->guidesPrefs().framesShown;
		view->DrawNew();
	}
}

void ScribusMainWindow::ToggleLayerMarkers()
{
	if (doc)
	{
		guidesStatus[0] = false;
		doc->guidesPrefs().layerMarkersShown = !doc->guidesPrefs().layerMarkersShown;
		view->DrawNew();
	}
}

void ScribusMainWindow::ToggleRaster()
{
	if (doc)
	{
		guidesStatus[0] = false;
		doc->guidesPrefs().gridShown = !doc->guidesPrefs().gridShown;
		view->DrawNew();
	}
}

void ScribusMainWindow::ToggleGuides()
{
	if (doc)
	{
		guidesStatus[0] = false;
		doc->guidesPrefs().guidesShown = !doc->guidesPrefs().guidesShown;
		view->DrawNew();
	}
}

void ScribusMainWindow::ToggleColumnBorders()
{
	if (doc)
	{
		guidesStatus[0] = false;
		doc->guidesPrefs().colBordersShown = !doc->guidesPrefs().colBordersShown;
		view->DrawNew();
	}
}

void ScribusMainWindow::ToggleBase()
{
	if (doc)
	{
		guidesStatus[0] = false;
		doc->guidesPrefs().baselineGridShown = !doc->guidesPrefs().baselineGridShown;
		view->DrawNew();
	}
}

void ScribusMainWindow::ToggleTextLinks()
{
	if (doc)
	{
		guidesStatus[0] = false;
		doc->guidesPrefs().linkShown = !doc->guidesPrefs().linkShown;
		view->DrawNew();
	}
}

void ScribusMainWindow::ToggleTextControls()
{
	if (doc)
	{
		guidesStatus[0] = false;
		doc->guidesPrefs().showControls = !doc->guidesPrefs().showControls;
		view->DrawNew();
	}
}

void ScribusMainWindow::ToggleRulers()
{
	if (doc)
	{
		guidesStatus[0] = false;
		doc->guidesPrefs().rulersShown = !doc->guidesPrefs().rulersShown;
		view->setRulersShown(doc->guidesPrefs().rulersShown);
	}
}

void ScribusMainWindow::ToggleRulerMode()
{
	if (doc)
	{
		guidesStatus[0] = false;
		doc->guidesPrefs().rulerMode = !doc->guidesPrefs().rulerMode;
		if (doc->guidesPrefs().rulerMode)
		{
			doc->rulerXoffset = 0;
			doc->rulerYoffset = 0;
		}
		else
		{
			doc->rulerXoffset += doc->currentPage()->xOffset();
			doc->rulerYoffset += doc->currentPage()->yOffset();
		}
		if (doc->m_Selection->count()==1)
		{
			PageItem* currItem=doc->m_Selection->itemAt(0);
			if (currItem!=NULL)
				currItem->emitAllToGUI();
		}
		//TODO emit from selection, handle group widths
		view->DrawNew();
	}
}

void ScribusMainWindow::ToggleURaster()
{
	if (doc)
	{
		doc->useRaster = !doc->useRaster;
		slotDocCh();
	}
}

void ScribusMainWindow::ToggleUGuides()
{
	if (doc)
	{
		doc->SnapGuides = !doc->SnapGuides;
		slotDocCh();
	}
}


void ScribusMainWindow::toggleNodeEdit()
{
	if (!doc)
		return;

	if (doc->appMode == modeEditClip)
	{
		view->requestMode(submodeEndNodeEdit);
	}
	else
	{
		view->requestMode(modeEditClip);
	}
}

void ScribusMainWindow::ToggleFrameEdit()
{
	if (!doc)
		return;
	if (doc->appMode == modeEditClip)
		NoFrameEdit();
	else
	{
		//CB Enable/Disable undo in frame edit mode
// 		undoManager->setUndoEnabled(false);
		scrActions["editUndoAction"]->setEnabled(false);
		scrActions["editRedoAction"]->setEnabled(false);
//done elsewhere now		slotSelect();
		nodePalette->setDoc(doc, view);
		nodePalette->MoveN();
		nodePalette->HaveNode(false, false);
		nodePalette->MoveNode->setChecked(true);
		nodePalette->show();
//		qDebug() << "nodepalette show:" << nodePalette->geometry();
		connect(view, SIGNAL(HavePoint(bool, bool)), nodePalette, SLOT(HaveNode(bool, bool)));
		doc->nodeEdit.reset();
//done elsewhere now		doc->appMode = modeEditClip;
		scrActions["toolsSelect"]->setEnabled(false);
		scrActions["toolsRotate"]->setEnabled(false);
		scrActions["toolsEditContents"]->setEnabled(false);
		scrActions["toolsEditWithStoryEditor"]->setEnabled(false);
		scrActions["toolsZoom"]->setEnabled(false);
		scrActions["toolsInsertTextFrame"]->setEnabled(false);
		scrActions["toolsInsertImageFrame"]->setEnabled(false);
		scrActions["toolsInsertTableFrame"]->setEnabled(false);
		scrActions["toolsInsertShape"]->setEnabled(false);
		scrActions["toolsInsertLine"]->setEnabled(false);
		scrActions["toolsInsertBezier"]->setEnabled(false);
		scrActions["toolsInsertFreehandLine"]->setEnabled(false);
		scrActions["toolsInsertCalligraphicLine"]->setEnabled(false);
		scrActions["toolsInsertPolygon"]->setEnabled(false);
		scrActions["toolsInsertArc"]->setEnabled(false);
		scrActions["toolsInsertSpiral"]->setEnabled(false);
		scrActions["toolsInsertRenderFrame"]->setEnabled(false);
		scrActions["toolsLinkTextFrame"]->setEnabled(false);
		scrActions["toolsUnlinkTextFrame"]->setEnabled(false);
		scrActions["toolsMeasurements"]->setEnabled(false);
		scrActions["toolsCopyProperties"]->setEnabled(false);
		scrActions["toolsEyeDropper"]->setEnabled(false);
		scrActions["toolsPDFPushButton"]->setEnabled(false);
		scrActions["toolsPDFTextField"]->setEnabled(false);
		scrActions["toolsPDFCheckBox"]->setEnabled(false);
		scrActions["toolsPDFComboBox"]->setEnabled(false);
		scrActions["toolsPDFListBox"]->setEnabled(false);
		scrActions["toolsPDFAnnotText"]->setEnabled(false);
		scrActions["toolsPDFAnnotLink"]->setEnabled(false);
#ifdef HAVE_OSG
		scrActions["toolsPDFAnnot3D"]->setEnabled(false);
#endif
		scrActions["itemDelete"]->setEnabled(false);
		layerPalette->setEnabled(false);
		outlinePalette->setEnabled(false);
		guidePalette->setEnabled(false);
		scrapbookPalette->setEnabled(false);
		pagePalette->setEnabled(false);
		bookmarkPalette->setEnabled(false);
		docCheckerPalette->setEnabled(false);
		symbolPalette->setEnabled(false);
		styleManager->setEnabled(false);
		alignDistributePalette->setEnabled(false);
		view->pageSelector->setEnabled(false);
		view->layerMenu->setEnabled(false);
		if (doc->m_Selection->count() != 0)
		{
			PageItem *currItem = doc->m_Selection->itemAt(0);
//			view->MarkClip(currItem, currItem->PoLine, true);
			nodePalette->EditCont->setEnabled(currItem->ContourLine.size() != 0);
			nodePalette->ResetCont->setEnabled(false);
			nodePalette->ResetContClip->setEnabled(false);
			nodePalette->PolyStatus(currItem->itemType(), currItem->PoLine.size());
			if ((currItem->asImageFrame()) && (currItem->imageClip.size() != 0))
			{
				nodePalette->ResetContClip->setSizePolicy(QSizePolicy(static_cast<QSizePolicy::Policy>(3), static_cast<QSizePolicy::Policy>(3)));
				nodePalette->ResetContClip->show();
				nodePalette->ResetShape2Clip->setSizePolicy(QSizePolicy(static_cast<QSizePolicy::Policy>(3), static_cast<QSizePolicy::Policy>(3)));
				nodePalette->ResetShape2Clip->show();
				nodePalette->layout()->activate();
				nodePalette->resize(QSize(170, 380).expandedTo(nodePalette->minimumSizeHint()));
			}
			else
			{
				nodePalette->ResetContClip->setSizePolicy(QSizePolicy(static_cast<QSizePolicy::Policy>(6), static_cast<QSizePolicy::Policy>(6)));
				nodePalette->ResetShape2Clip->setSizePolicy(QSizePolicy(static_cast<QSizePolicy::Policy>(6), static_cast<QSizePolicy::Policy>(6)));
				nodePalette->layout()->activate();
				nodePalette->ResetContClip->hide();
				nodePalette->ResetShape2Clip->hide();
				nodePalette->layout()->activate();
				nodePalette->resize(QSize(170, 380).expandedTo(nodePalette->minimumSizeHint()));
			}
		}
	}
	scrActions["itemShapeEdit"]->setChecked(doc->appMode == modeEditClip);
}

void ScribusMainWindow::NoFrameEdit()
{
	disconnect(view, SIGNAL(HavePoint(bool, bool)), nodePalette, SLOT(HaveNode(bool, bool)));
	actionManager->disconnectModeActions();
	nodePalette->setDoc(0,0);
	nodePalette->hide();
//	qDebug() << "nodepalette hide";
	scrActions["toolsSelect"]->setEnabled(true);
	scrActions["toolsSelect"]->setChecked(true);
	scrActions["toolsRotate"]->setEnabled(true);
	scrActions["toolsZoom"]->setEnabled(true);
	scrActions["toolsInsertTextFrame"]->setEnabled(true);
	scrActions["toolsInsertImageFrame"]->setEnabled(true);
	scrActions["toolsInsertTableFrame"]->setEnabled(true);
	scrActions["toolsInsertShape"]->setEnabled(true);
	scrActions["toolsInsertLine"]->setEnabled(true);
	scrActions["toolsInsertBezier"]->setEnabled(true);
	scrActions["toolsInsertFreehandLine"]->setEnabled(true);
	scrActions["toolsInsertCalligraphicLine"]->setEnabled(true);
	scrActions["toolsInsertPolygon"]->setEnabled(true);
	scrActions["toolsInsertArc"]->setEnabled(true);
	scrActions["toolsInsertSpiral"]->setEnabled(true);
	scrActions["toolsInsertRenderFrame"]->setEnabled(true);
	scrActions["toolsPDFPushButton"]->setEnabled(true);
	scrActions["toolsPDFTextField"]->setEnabled(true);
	scrActions["toolsPDFCheckBox"]->setEnabled(true);
	scrActions["toolsPDFComboBox"]->setEnabled(true);
	scrActions["toolsPDFListBox"]->setEnabled(true);
	scrActions["toolsPDFAnnotText"]->setEnabled(true);
	scrActions["toolsPDFAnnotLink"]->setEnabled(true);
#ifdef HAVE_OSG
	scrActions["toolsPDFAnnot3D"]->setEnabled(true);
#endif
	scrActions["toolsEditContents"]->setChecked(false);
	scrActions["toolsEditWithStoryEditor"]->setChecked(false);
	scrActions["toolsMeasurements"]->setEnabled(true);
	scrActions["toolsCopyProperties"]->setEnabled(true);
	scrActions["toolsEyeDropper"]->setEnabled(true);
	scrActions["toolsUnlinkTextFrame"]->setEnabled(true);
	scrActions["itemDelete"]->setEnabled(true);
	scrActions["itemShapeEdit"]->setChecked(false);
	layerPalette->setEnabled(true);
	outlinePalette->setEnabled(true);
	guidePalette->setEnabled(true);
	scrapbookPalette->setEnabled(true);
	pagePalette->setEnabled(true);
	bookmarkPalette->setEnabled(true);
	docCheckerPalette->setEnabled(true);
	styleManager->setEnabled(true);
	alignDistributePalette->setEnabled(true);
	symbolPalette->setEnabled(true);
	view->pageSelector->setEnabled(true);
	view->layerMenu->setEnabled(true);
// 	bool tmpClip = doc->EditClip; // for enabling undo if exiting shape edit mode
	if (HaveDoc)
	{
// done elsewhere now:		doc->appMode = modeNormal;
		doc->nodeEdit.reset();
		if (doc->m_Selection->count() != 0)
		{
			HaveNewSel(doc->m_Selection->itemAt(0)->itemType());
			doc->m_Selection->itemAt(0)->emitAllToGUI();
			view->DrawNew();
		}
		else
			HaveNewSel(-1);
	}
	actionManager->connectModeActions();
// 	if (tmpClip)
// 		undoManager->setUndoEnabled(true);
}

/** This is the safest method to return to modeNormal
*/
void ScribusMainWindow::slotSelect()
{
	if (doc)
		view->requestMode(modeNormal);
	else
		setAppMode(modeNormal);
}

void ScribusMainWindow::setAppModeByToggle(bool isOn, int newMode)
{
	keyrep=false;

	if (newMode==modeDrawLatex && !prefsManager->renderFrameConfigured())
	{
		QMessageBox::critical(this, "Render Frames Not Configured", "Your Render Frame configuration seems to be invalid. Please check the settings in the External Tools section of the Preferences dialog.");
		return;
	}

	if (doc && isOn)
		view->requestMode(newMode);
	else
		slotSelect();
}

void ScribusMainWindow::setAppMode(int mode)
{
	assert(mode < submodeFirstSubmode);

	//disconnect the tools actions so we dont fire them off
	actionManager->disconnectModeActions();
	//set the actions state based on incoming mode
	scrActions["toolsSelect"]->setChecked(mode==modeNormal);
	scrActions["toolsInsertTextFrame"]->setChecked(mode==modeDrawText);
	scrActions["toolsInsertImageFrame"]->setChecked(mode==modeDrawImage);
	scrActions["toolsInsertTableFrame"]->setChecked(mode==modeDrawTable);
	scrActions["toolsInsertShape"]->setChecked(mode==modeDrawShapes);
	scrActions["toolsInsertPolygon"]->setChecked(mode==modeDrawRegularPolygon);
	scrActions["toolsInsertArc"]->setChecked(mode==modeDrawArc);
	scrActions["toolsInsertSpiral"]->setChecked(mode==modeDrawSpiral);
	scrActions["toolsInsertLine"]->setChecked(mode==modeDrawLine);
	scrActions["toolsInsertBezier"]->setChecked(mode==modeDrawBezierLine);
	scrActions["toolsInsertFreehandLine"]->setChecked(mode==modeDrawFreehandLine);
	scrActions["toolsInsertCalligraphicLine"]->setChecked(mode == modeDrawCalligraphicLine);
	scrActions["toolsInsertRenderFrame"]->setChecked(mode==modeDrawLatex);
	scrActions["toolsRotate"]->setChecked(mode==modeRotation);
	scrActions["toolsZoom"]->setChecked(mode==modeMagnifier);
	scrActions["toolsEditContents"]->setChecked(mode==modeEdit);
	scrActions["toolsEditWithStoryEditor"]->setChecked(mode==modeStoryEditor);
	scrActions["toolsLinkTextFrame"]->setChecked(mode==modeLinkFrames);
	scrActions["toolsUnlinkTextFrame"]->setChecked(mode==modeUnlinkFrames);
	scrActions["toolsEyeDropper"]->setChecked(mode==modeEyeDropper);
	scrActions["toolsMeasurements"]->setChecked(mode==modeMeasurementTool);
	scrActions["toolsCopyProperties"]->setChecked(mode==modeCopyProperties);
	scrActions["toolsPDFPushButton"]->setChecked(mode==modeInsertPDFButton);
	scrActions["toolsPDFTextField"]->setChecked(mode==modeInsertPDFTextfield);
	scrActions["toolsPDFCheckBox"]->setChecked(mode==modeInsertPDFCheckbox);
	scrActions["toolsPDFComboBox"]->setChecked(mode==modeInsertPDFCombobox);
	scrActions["toolsPDFListBox"]->setChecked(mode==modeInsertPDFListbox);
	scrActions["toolsPDFAnnotText"]->setChecked(mode==modeInsertPDFTextAnnotation);
	scrActions["toolsPDFAnnotLink"]->setChecked(mode==modeInsertPDFLinkAnnotation);
#ifdef HAVE_OSG
	scrActions["toolsPDFAnnot3D"]->setChecked(mode==modeInsertPDF3DAnnotation);
#endif

	if (HaveDoc)
	{
		PageItem *currItem=0;
		if (doc->m_Selection->count() != 0)
			currItem = doc->m_Selection->itemAt(0);
		int oldMode = doc->appMode;
		if (oldMode == modeEditClip && mode != modeEditClip)
			NoFrameEdit();
		else if (oldMode != modeEditClip && mode == modeEditClip)
			ToggleFrameEdit();
		doc->appMode = mode;
//		if (oldMode == modeMeasurementTool)
//			disconnect(view, SIGNAL(MVals(double, double, double, double, double, double, int )), measurementPalette, SLOT(setValues(double, double, double, double, double, double, int )));
/*		if (mode != modeEdit && doc->CurTimer!=NULL)
		{
			disconnect(doc->CurTimer, SIGNAL(timeout()), view, SLOT(blinkCursor()));
			doc->CurTimer->stop();
			delete doc->CurTimer;
			doc->CurTimer = NULL;
		} */
		if (mode!=modeEdit && oldMode==modeEdit)
			actionManager->restoreActionShortcutsPostEditMode();
		else
		if (mode==modeEdit && oldMode!=modeEdit)
			actionManager->saveActionShortcutsPreEditMode();
		if (oldMode == modeEdit)
		{
			view->zoomSpinBox->setFocusPolicy(Qt::ClickFocus);
			view->pageSelector->setFocusPolicy(Qt::ClickFocus);
			scrActions["editClearContents"]->setEnabled(false);
			charPalette->setEnabled(false, 0);
//			view->slotDoCurs(false);
			if (currItem != 0)
			{
				currItem->update();
				//		scrMenuMgr->setMenuEnabled("Item", true);
				scrActions["itemDuplicate"]->setEnabled(true);
				scrActions["itemMulDuplicate"]->setEnabled(true);
				scrActions["itemTransform"]->setEnabled(true);
				scrActions["itemDelete"]->setEnabled(true);
				scrActions["itemRaise"]->setEnabled(true);
				scrActions["itemLower"]->setEnabled(true);
				scrActions["itemRaiseToTop"]->setEnabled(true);
				scrActions["itemLowerToBottom"]->setEnabled(true);
//				scrActions["itemSendToScrapbook"]->setEnabled(true);
				scrMenuMgr->setMenuEnabled("itemSendToScrapbook", true);
				scrActions["itemSendToPattern"]->setEnabled(true);
				scrActions["itemAdjustFrameToImage"]->setEnabled(true);
				scrActions["itemAdjustImageToFrame"]->setEnabled(true);
				scrActions["itemExtendedImageProperties"]->setEnabled(true);
				scrActions["itemUpdateImage"]->setEnabled(true);
				scrActions["itemPreviewLow"]->setEnabled(true);
				scrActions["itemPreviewNormal"]->setEnabled(true);
				scrActions["itemPreviewFull"]->setEnabled(true);
				scrActions["itemAttributes"]->setEnabled(true);
			}
			view->horizRuler->textMode(false);
			view->horizRuler->update();
		}
		if (mode == modeEdit)
		{
//			view->zoomSpinBox->setFocusPolicy(QWidget::NoFocus);
// 			view->pageSelector->setFocusPolicy(QWidget::NoFocus);
			if (currItem != 0)
			{
//				if ((currItem->itemType() == PageItem::Polygon) || (currItem->itemType() == PageItem::PolyLine) || (currItem->itemType() == PageItem::PathText))
//				{
//					doc->appMode = modeNormal;
//					view->requestMode(modeEditClip);
//					return;
//				}
				setTBvals(currItem);
				currItem->itemText.setCursorPosition(0);
			}
			scrActions["editPaste"]->setEnabled(false);
			charPalette->setEnabled(true, currItem);
			if (currItem!=NULL && currItem->asTextFrame())
				enableTextActions(&scrActions, true, currItem->currentCharStyle().font().scName());
			if (ScMimeData::clipboardHasScribusData())
			{
				bool textFrameEditMode = ((currItem != NULL) && (currItem->asTextFrame()));
				scrActions["editPaste"]->setEnabled( textFrameEditMode || (currItem == NULL) );
			}
//			view->slotDoCurs(true);
			//		scrMenuMgr->setMenuEnabled("Item", false);
			scrActions["itemDuplicate"]->setEnabled(false);
			scrActions["itemMulDuplicate"]->setEnabled(false);
			scrActions["itemTransform"]->setEnabled(false);
			scrActions["itemDelete"]->setEnabled(false);
			scrActions["itemRaise"]->setEnabled(false);
			scrActions["itemLower"]->setEnabled(false);
			scrActions["itemRaiseToTop"]->setEnabled(false);
			scrActions["itemLowerToBottom"]->setEnabled(false);
//			scrActions["itemSendToScrapbook"]->setEnabled(false);
			scrMenuMgr->setMenuEnabled("itemSendToScrapbook", false);
			scrActions["itemSendToPattern"]->setEnabled(false);
			scrActions["itemAdjustFrameToImage"]->setEnabled(false);
			scrActions["itemAdjustImageToFrame"]->setEnabled(false);
			scrActions["itemExtendedImageProperties"]->setEnabled(false);
			scrActions["itemUpdateImage"]->setEnabled(false);
			scrActions["itemPreviewLow"]->setEnabled(false);
			scrActions["itemPreviewNormal"]->setEnabled(false);
			scrActions["itemPreviewFull"]->setEnabled(false);
			scrActions["itemAttributes"]->setEnabled(false);
/*			doc->CurTimer = new QTimer(view);
			if (doc->CurTimer!=NULL)
			{
				connect(doc->CurTimer, SIGNAL(timeout()), view, SLOT(blinkCursor()));
				doc->CurTimer->start(500);
			} */
			if (currItem != 0)
			{
				scrActions["editCut"]->setEnabled(currItem->HasSel);
				scrActions["editCopy"]->setEnabled(currItem->HasSel);
				scrActions["editClearContents"]->setEnabled(currItem->HasSel);
				scrActions["editSearchReplace"]->setEnabled(true);

				// Why the hell do you want to update the item here? - pm
// 				currItem->update();
			}
		}
		int docSelectionCount=doc->m_Selection->count();
		if (mode == modeDrawBezierLine)
		{
			if ((docSelectionCount != 0) && (!prefsManager->appPrefs.uiPrefs.stickyTools))
				view->Deselect(true);
			view->FirstPoly = true;
		}
		if (mode == modeEditGradientVectors)
			propertiesPalette->setGradientEditMode(true);
//		if (mode == modeMeasurementTool)
//		{
//			measurementPalette->show();
//			connect(view, SIGNAL(MVals(double, double, double, double, double, double, int)), measurementPalette, SLOT(setValues(double, double, double, double, double, double, int )));
//		}
		switch (mode)
		{
			case modeDrawShapes:
			case modeDrawArc:
			case modeDrawSpiral:
				if (docSelectionCount!=0)
					view->Deselect(true);
				qApp->changeOverrideCursor(QCursor(loadIcon("DrawFrame.xpm")));
				break;
			case modeDrawImage:
				if (docSelectionCount!=0)
					view->Deselect(true);
				qApp->changeOverrideCursor(QCursor(loadIcon("DrawImageFrame.xpm")));
				break;
			case modeDrawLatex:
				if (docSelectionCount!=0)
					view->Deselect(true);
				qApp->changeOverrideCursor(QCursor(loadIcon("DrawLatexFrame.xpm")));
				break;
			case modeDrawText:
				if (docSelectionCount!=0)
					view->Deselect(true);
				qApp->changeOverrideCursor(QCursor(loadIcon("DrawTextFrame.xpm")));
				break;
			case modeDrawTable:
				if (docSelectionCount!=0)
					view->Deselect(true);
				qApp->changeOverrideCursor(QCursor(loadIcon("DrawTable.xpm")));
				break;
			case modeDrawRegularPolygon:
				if (docSelectionCount!=0)
					view->Deselect(true);
				qApp->changeOverrideCursor(QCursor(loadIcon("DrawPolylineFrame.xpm")));
				break;
			case modeMagnifier:
				if (docSelectionCount!=0)
					view->Deselect(true);
				view->Magnify = true;
				qApp->changeOverrideCursor(QCursor(loadIcon("LupeZ.xpm")));
				break;
			case modePanning:
				qApp->changeOverrideCursor(QCursor(loadIcon("HandC.xpm")));
				break;
			case modeDrawLine:
			case modeDrawBezierLine:
				qApp->changeOverrideCursor(QCursor(Qt::CrossCursor));
				break;
			case modeDrawCalligraphicLine:
			case modeDrawFreehandLine:
				qApp->changeOverrideCursor(QCursor(loadIcon("DrawFreeLine.png"), 0, 32));
				break;
			case modeEyeDropper:
				qApp->changeOverrideCursor(QCursor(loadIcon("colorpickercursor.png"), 0, 32));
				break;
			case modeInsertPDFButton:
			case modeInsertPDFTextfield:
			case modeInsertPDFCheckbox:
			case modeInsertPDFCombobox:
			case modeInsertPDFListbox:
			case modeInsertPDFTextAnnotation:
			case modeInsertPDFLinkAnnotation:
			case modeInsertPDF3DAnnotation:
				if (docSelectionCount!=0)
					view->Deselect(true);
				qApp->changeOverrideCursor(QCursor(Qt::CrossCursor));
				break;
			case modeMeasurementTool:
			case modeEditGradientVectors:
			case modeEditMeshGradient:
			case modeEditArc:
			case modeEditPolygon:
			case modeEditSpiral:
				qApp->changeOverrideCursor(QCursor(Qt::CrossCursor));
				break;
			default:
				qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
			break;
		}
		if (mode == modeDrawShapes)
		{
			doc->SubMode = modeToolBar->SubMode;
			doc->ShapeValues = modeToolBar->ShapeVals;
			doc->ValCount = modeToolBar->ValCount;
			emit UpdateRequest(reqCustomShapeUpdate);
		}
		else
			doc->SubMode = -1;
		if (mode == modeNormal)
		{
			propertiesPalette->setGradientEditMode(false);
		}
		if (mode == modeLinkFrames)
			doc->ElemToLink = doc->m_Selection->itemAt(0);
		if ((mode == modeLinkFrames) || (mode == modeUnlinkFrames) || (oldMode == modeLinkFrames) || (oldMode == modeUnlinkFrames))
			doc->regionsChanged()->update(QRect());

		if (mode == modeStoryEditor)
		{
			slotStoryEditor();
			slotSelect();
		}
		if (mode == modeCopyProperties)
		{
			if (doc->m_Selection->count() != 0)
			{
				doc->ElemToLink = doc->m_Selection->itemAt(0);
				view->Deselect(true);
				scrActions["toolsCopyProperties"]->setEnabled(true);
			}
		}
		if (mode != modeNormal && mode != modeStoryEditor)
			activateWindow();
		PluginManager& pluginManager(PluginManager::instance());
		pluginManager.enablePluginActionsForSelection(this);
/*
		QStringList pluginNames(pluginManager.pluginNames(false));
		ScPlugin* plugin;
		ScActionPlugin* ixplug;
		ScrAction* pluginAction = 0;
		QString pName;
		for (int i = 0; i < pluginNames.count(); ++i)
		{
			pName = pluginNames.at(i);
			plugin = pluginManager.getPlugin(pName, true);
			Q_ASSERT(plugin); // all the returned names should represent loaded plugins
			if (plugin->inherits("ScActionPlugin"))
			{
				ixplug = dynamic_cast<ScActionPlugin*>(plugin);
				Q_ASSERT(ixplug);
				ScActionPlugin::ActionInfo ai(ixplug->actionInfo());
				pluginAction = ScCore->primaryMainWindow()->scrActions[ai.name];
				if (pluginAction != 0)
				{
					if (doc->m_Selection->count() != 0)
						pluginAction->setEnabled(ixplug->handleSelection(doc, doc->m_Selection->itemAt(0)->itemType()));
					else
						pluginAction->setEnabled(ixplug->handleSelection(doc));
				}
			}
		}
*/
	}
	actionManager->connectModeActions();
}

void ScribusMainWindow::setMainWindowActive()
{
	activateWindow();
	if (!scriptIsRunning())
		raise();
}

void ScribusMainWindow::setItemTypeStyle(int id)
{
	int b = 0;
	if (id == 0)
	{
		scrActions["typeEffectNormal"]->setChecked(true);
		scrActions["typeEffectUnderline"]->setChecked(false);
		scrActions["typeEffectUnderlineWords"]->setChecked(false);
		scrActions["typeEffectStrikeThrough"]->setChecked(false);
		scrActions["typeEffectSmallCaps"]->setChecked(false);
		scrActions["typeEffectAllCaps"]->setChecked(false);
		scrActions["typeEffectSuperscript"]->setChecked(false);
		scrActions["typeEffectSubscript"]->setChecked(false);
		scrActions["typeEffectOutline"]->setChecked(false);
		scrActions["typeEffectShadow"]->setChecked(false);
	}
	else
	{
		scrActions["typeEffectNormal"]->setChecked(false);
		if (id == 4)
			scrActions["typeEffectSubscript"]->setChecked(false);
		if (id == 5)
			scrActions["typeEffectSuperscript"]->setChecked(false);
		if (id == 3)
			scrActions["typeEffectAllCaps"]->setChecked(false);
		if (id == 7)
			scrActions["typeEffectSmallCaps"]->setChecked(false);
		if (id == 8)
			scrActions["typeEffectUnderline"]->setChecked(false);
		if (id == 1)
			scrActions["typeEffectUnderlineWords"]->setChecked(false);
		if (scrActions["typeEffectUnderline"]->isChecked())
			b |= 8;
		if (scrActions["typeEffectUnderlineWords"]->isChecked())
			b |= 512;
		if (scrActions["typeEffectShadow"]->isChecked())
			b |= 256;
		if (scrActions["typeEffectStrikeThrough"]->isChecked())
			b |= 16;
		if (scrActions["typeEffectAllCaps"]->isChecked())
			b |= 32;
		if (scrActions["typeEffectSmallCaps"]->isChecked())
			b |= 64;
		if (scrActions["typeEffectSuperscript"]->isChecked())
			b |= 1;
		if (scrActions["typeEffectSubscript"]->isChecked())
			b |= 2;
		if (scrActions["typeEffectOutline"]->isChecked())
			b |= 4;
	}
	setItemEffects(b);
}

void ScribusMainWindow::setStyleEffects(int s)
{
	int c = s & 1919;
//	doc->currentStyle.charStyle().setFeatures(static_cast<StyleFlag>(c).featureList());
	scrActions["typeEffectNormal"]->setChecked(c==0);
	scrActions["typeEffectSuperscript"]->setChecked(c & 1);
	scrActions["typeEffectSubscript"]->setChecked(c & 2);
	scrActions["typeEffectOutline"]->setChecked(c & 4);
	scrActions["typeEffectUnderline"]->setChecked(c & 8);
	scrActions["typeEffectStrikeThrough"]->setChecked(c & 16);
	scrActions["typeEffectAllCaps"]->setChecked(c & 32);
	scrActions["typeEffectSmallCaps"]->setChecked(c & 64);
	scrActions["typeEffectShadow"]->setChecked(c & 256);
	scrActions["typeEffectUnderlineWords"]->setChecked(c & 512);
	emit TextEffects(s);
}

void ScribusMainWindow::setItemEffects(int h)
{
	if (doc->m_Selection->count() != 0)
	{
//		doc->currentStyle.charStyle().setFeatures(static_cast<StyleFlag>(h).featureList());
		setStyleEffects(h);
		doc->itemSelection_SetEffects(h);
	}
}

//CB-->Doc partly
void ScribusMainWindow::DeletePage2(int pg)
{
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	view->Deselect(true);
	if (doc->Pages->count() == 1)
		return;
	DeletePage(pg+1, pg+1);
}

void ScribusMainWindow::DeletePage()
{
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	view->Deselect(true);
	DelPages *dia = new DelPages(this, doc->currentPage()->pageNr()+1, doc->Pages->count());
	if (dia->exec())
		DeletePage(dia->getFromPage(), dia->getToPage());
	delete dia;
}

void ScribusMainWindow::DeletePage(int from, int to)
{
	UndoTransaction* activeTransaction = NULL;
	assert( from > 0 );
	assert( from <= to );
	assert( to <= static_cast<int>(doc->Pages->count()) );
	int oldPg = doc->currentPageNumber();
	guidePalette->setDoc(NULL);
	if (UndoManager::undoEnabled())
		activeTransaction = new UndoTransaction(undoManager->beginTransaction(doc->DocName, Um::IDocument,
																			  (from - to == 0) ? Um::DeletePage : Um::DeletePages, "",
																			  Um::IDelete));
	PageItem* ite;
	doc->m_Selection->clear();
	Selection tmpSelection(this, false);
	for (int a = to - 1; a >= from - 1; a--)
	{
		for (int d = 0; d < doc->Items->count(); ++d)
		{
			ite = doc->Items->at(d);
			if (ite->OwnPage == a)
			{
				ite->setLocked(false);
				ite->isSingleSel = false;
				if (ite->isBookmark)
					DelBookMark(ite);
				ite->isBookmark = false;
				tmpSelection.addItem(ite);
			}
		}
		ScPage *page = doc->Pages->at(a); // need to remove guides too to get their undo/redo actions working
		page->guides.clearHorizontals(GuideManagerCore::Standard);
		page->guides.clearHorizontals(GuideManagerCore::Auto);
		page->guides.clearVerticals(GuideManagerCore::Standard);
		page->guides.clearVerticals(GuideManagerCore::Auto);
	}
	if (tmpSelection.count() != 0)
		doc->itemSelection_DeleteItem(&tmpSelection);
	for (int a = to - 1; a >= from - 1; a--)
	{
		if (UndoManager::undoEnabled())
		{
			SimpleState *ss = new SimpleState(Um::DeletePage, "", Um::ICreate);
			ss->set("DELETE_PAGE", "delete_page");
			ss->set("PAGENR", a + 1);
			ss->set("PAGENAME",   doc->Pages->at(a)->pageName());
			ss->set("MASTERPAGE", doc->Pages->at(a)->MPageNam);
			// replace the deleted page in the undostack by a dummy object that will
			// replaced with the "undone" page if user choose to undo the action
			DummyUndoObject *duo = new DummyUndoObject();
			uint id = static_cast<uint>(duo->getUId());
			undoManager->replaceObject(doc->Pages->at(a)->getUId(), duo);
			ss->set("DUMMY_ID", id);
			undoManager->action(this, ss);
		}
		bool isMasterPage = !(doc->Pages->at(a)->pageName().isEmpty());
		if (doc->masterPageMode())
			doc->deleteMasterPage(a);
		else
			doc->deletePage(a);
		disconnect(view->pageSelector, SIGNAL(GotoPage(int)), view, SLOT(GotoPa(int)));
		view->pageSelector->setMaximum(doc->Pages->count());
		view->pageSelector->GotoPg(0);
		connect(view->pageSelector, SIGNAL(GotoPage(int)), view, SLOT(GotoPa(int)));
		if (!isMasterPage) // Master pages are not added to sections when created
			doc->removePageFromSection(a);
	}
	undoManager->setUndoEnabled(false); // ugly hack to disable object moving when undoing page deletion
	view->reformPages();
	undoManager->setUndoEnabled(true); // ugly hack continues
	view->GotoPage(qMin(doc->Pages->count()-1, oldPg));
	view->DrawNew();
	if (outlinePalette->isVisible())
		outlinePalette->BuildTree();
	doc->rebuildMasterNames();
	pagePalette->rebuildPages();
	pagePalette->rebuildMasters();
	if (activeTransaction)
	{
		activeTransaction->commit();
		delete activeTransaction;
		activeTransaction = NULL;
	}
}

void ScribusMainWindow::MovePage()
{
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	MovePages *dia = new MovePages(this, doc->currentPage()->pageNr()+1, doc->Pages->count(), true);
	if (dia->exec())
	{
		int from = dia->getFromPage();
		int to = dia->getToPage();
		int wie = dia->getWhere();
		int wo = dia->getWherePage();
		if (from != wo || (wie == 2 && to != signed(doc->Pages->count()) ) )
		{
			doc->movePage(from-1, to, wo-1, wie);
			view->reformPages();
			view->DrawNew();
			pagePalette->rebuildPages();
			if (outlinePalette->isVisible())
				outlinePalette->BuildTree();
		}
	}
	delete dia;
}

void ScribusMainWindow::CopyPage()
{
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	MovePages *dia = new MovePages(this, doc->currentPage()->pageNr()+1, doc->Pages->count(), false);
	if (dia->exec())
	{
		int pageNumberToCopy=dia->getFromPage()-1;
		int whereToInsert=dia->getWhere();
		int copyCount=dia->getCopyCount();
		int wo = dia->getWherePage();
		doc->copyPage(pageNumberToCopy, wo, whereToInsert, copyCount);
		view->Deselect(true);
		view->DrawNew();
		pagePalette->rebuildPages();
		slotDocCh();
	}
	delete dia;
}

void ScribusMainWindow::changePageMargins()
{
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	QString Nam = doc->currentPage()->MPageNam;
	MarginDialog *dia = new MarginDialog(this, doc);
	if (dia->exec())
	{
		int orientation = dia->getPageOrientation();
		double ph = dia->getPageHeight();
		double pw = dia->getPageWidth();
		QString sizeName = dia->getpPrefsPageSizeName();
		if (doc->masterPageMode())
		{
			int lp=0;
			if (doc->pagePositioning() != singlePage)
				lp = dia->pageOrder();
			doc->changePageMargins(dia->top(), dia->bottom(),
								   dia->left(), dia->right(),
								   ph, pw, ph, pw, orientation,
								   sizeName, dia->getMarginPreset(), dia->getMoveObjects(), doc->currentPage()->pageNr(), lp);
		}
		else
		{
			doc->changePageMargins(dia->top(), dia->bottom(),
								   dia->left(), dia->right(),
								   ph, pw, ph, pw, orientation,
								   sizeName, dia->getMarginPreset(), dia->getMoveObjects(), doc->currentPage()->pageNr());
			if (dia->masterPage() != Nam)
				Apply_MasterPage(dia->masterPage(), doc->currentPage()->pageNr());
		}
		//CB: Moved to changePageMargins for #2338
		//doc->currentPage()->marginPreset = dia->getMarginPreset();
		//view->reformPages(dia->getMoveObjects());
		//view->DrawNew();
	}
	delete dia;
}

void ScribusMainWindow::SetNewFont(const QString& nf)
{
	setMainWindowActive();
	QString nf2(nf);
	if (!doc->UsedFonts.contains(nf))
	{
		if (doc->AddFont(nf)) //, prefsManager->appPrefs.fontPrefs.AvailFonts[nf]->Font))
		{
		}
		else
		{//CB FIXME: to doc?
			if (doc->m_Selection->count() != 0)
			{
				PageItem *currItem = doc->m_Selection->itemAt(0);
				nf2 = currItem->currentCharStyle().font().scName();
			}
		}
	}
	doc->itemSelection_SetFont(nf2);
//	doc->currentStyle.charStyle().setFont((*doc->AllFonts)[nf2]);
	view->DrawNew();
// 	slotDocCh();
}

void ScribusMainWindow::setItemFSize(int id)
{
	int c = id;
	if (c != -1)
		doc->itemSelection_SetFontSize(c*10);
	else
	{
		bool ok = false;
		Query* dia = new Query(this, "New", 1, 0, tr("&Size:"), tr("Size"));
		if (dia->exec())
		{
			c = qRound(dia->getEditText().toDouble(&ok));
			if ((ok) && (c < 1025) && (c > 0))
				doc->itemSelection_SetFontSize(c*10);
		}
		delete dia;
	}
	propertiesPalette->textPal->displayFontSize(c*10);
// 	slotDocCh();
}

//CB-->Doc partly
void ScribusMainWindow::setItemShade(int id)
{
	int c = id;
	bool ok = false;
	if (doc->m_Selection->count() != 0)
	{
		PageItem *currItem = doc->m_Selection->itemAt(0);
		if (c != -1)
		{
			if ((currItem->itemType() == PageItem::TextFrame) || (currItem->itemType() == PageItem::PathText))
				doc->itemSelection_SetFillShade(c);
			else
				doc->itemSelection_SetItemBrushShade(c);
		}
		else
		{
			Query* dia = new Query(this, "New", 1, 0, tr("&Shade:"), tr("Shade"));
			if (dia->exec())
			{
				c = dia->getEditText().toInt(&ok);
				if (ok)
				{
					if ((currItem->itemType() == PageItem::TextFrame) || (currItem->itemType() == PageItem::PathText))
						doc->itemSelection_SetFillShade(c);
					else
						doc->itemSelection_SetItemBrushShade(c);
				}
			}
			delete dia;
		}
	}
// 	slotDocCh();
}

/* CB: unused in 135 without the colours menu
void ScribusMainWindow::setCSMenu()
{
	QString la = CommonStrings::tr_NoneColor;
	int lb = 100;
	PageItem *currItem;
	if (doc->m_Selection->count() != 0)
	{
		currItem = doc->m_Selection->itemAt(0);
		if ((currItem->asTextFrame()) || (currItem->asPathText()))
		{
			if ((doc->appMode == modeEdit) && (currItem->itemText.length() != 0))
			{
				la = currItem->itemText.charStyle(qMin(currItem->CPos, static_cast<int>(currItem->itemText.length()-1))).fillColor();
				lb = qRound(currItem->itemText.charStyle(qMin(currItem->CPos, static_cast<int>(currItem->itemText.length()-1))).fillShade());
			}
			else
			{
				la = currItem->itemText.defaultStyle().charStyle().fillColor();
				lb = qRound(currItem->itemText.defaultStyle().charStyle().fillShade());
			}
		}
		else
		{
			la = currItem->fillColor();
			lb = qRound(currItem->fillShade());
		}
	}
	if (la == CommonStrings::None)
		la = CommonStrings::tr_NoneColor;
	if (scrActions[QString("shade%1").arg(lb)])
		scrActions[QString("shade%1").arg(lb)]->setChecked(true);
}
*/

#if 0
//CB still called from SE
void ScribusMainWindow::saveStyles(StilFormate *dia)
{
	QList<uint> ers;
	QString nn;
// 	PageItem* ite = 0;
	bool ff;
	uint nr;
	ers.clear();
	/*FIXME:NLS
	ers.append(0);
	ers.append(1);
	ers.append(2);
	ers.append(3);
	ers.append(4);
	for (uint a=5; a<doc->docParagraphStyles.count(); ++a)
*/
	for (uint a=0; a<doc->paragraphStyles().count(); ++a)
	{
		ff = false;
		nn = doc->paragraphStyles()[a].name();
		for (uint b=0; b<dia->TempVorl.count(); ++b)
		{
			if (nn == dia->TempVorl[b].name())
			{
				nr = b;
				ff = true;
				break;
			}
		}
		if (ff)
			ers.append(nr);
		else
		{
			for (uint b=0; b<dia->TempVorl.count(); ++b)
			{
				if (doc->paragraphStyles()[a].equiv(dia->TempVorl[b]))
				{
					nr = b;
					ff = true;
					break;
				}
			}
			if (ff)
				ers.append(nr);
			else
			{
				if (dia->ReplaceList.count() != 0)
				{
					QString ne = dia->ReplaceList[nn];
					if (ne == tr("No Style"))
						ers.append(0);
					else
					{
						for (uint b=0; b<dia->TempVorl.count(); ++b)
						{
							if (ne == dia->TempVorl[b].name())
							{
								nr = b;
								ff = true;
								break;
							}
						}
						if (ff)
							ers.append(nr);
						else
							ers.append(0);
					}
				}
				else
					ers.append(0);
			}
		}
	}
// 	uint counter = 0;
/*
	for (uint lc = 0; lc < 3; ++lc)
	{
		switch (lc)
		{
			case 0:
				counter = doc->MasterItems.count();
				break;
			case 1:
				counter = doc->DocItems.count();
				break;
			case 2:
				counter = doc->FrameItems.count();
				break;
		}
		for (uint d=0; d< counter; ++d)
		{
			switch (lc)
			{
				case 0:
					ite = doc->MasterItems.at(d);
					break;
				case 1:
					ite = doc->DocItems.at(d);
					break;
				case 2:
					ite = doc->FrameItems.at(d);
					break;
			}
			if (ite->asTextFrame())
			{
				CharStyle lastStyle;
				int lastStyleStart = 0;
				int lastParaStyle = -1;
				for (int e=0; e<ite->itemText.length(); ++e)
				{
					const ParagraphStyle origStyle(ite->itemText.paragraphStyle(e));
					int cabori = findParagraphStyle(doc, origStyle);
					assert (cabori >= 0 && cabori < doc->docParagraphStyles.count() );
					int cabneu = ers[cabori];
					assert (cabneu >= 0 && cabneu < dia->TempVorl.count() );
					CharStyle newStyle;
					if (cabori > 4)
					{
						if (cabneu > 0)
						{
							if (ite->itemText.charStyle(e).font() == doc->docParagraphStyles[cabori].charStyle().font())
								newStyle.setFont(dia->TempVorl[cabneu].charStyle().font());
							if (ite->itemText.charStyle(e).fontSize() == doc->docParagraphStyles[cabori].charStyle().fontSize())
								newStyle.setFontSize(dia->TempVorl[cabneu].charStyle().fontSize());
							if ((ite->itemText.charStyle(e).effects() & 1919 ) == doc->docParagraphStyles[cabori].charStyle().effects())
							{
								StyleFlag fl = static_cast<StyleFlag>(ite->itemText.charStyle(e).effects() & ~1919);
								fl |= dia->TempVorl[cabneu].charStyle().effects();
								newStyle.setEffects(fl);
							}
							if (ite->itemText.charStyle(e).fillColor() == doc->docParagraphStyles[cabori].charStyle().fillColor())
								newStyle.setFillColor(dia->TempVorl[cabneu].charStyle().fillColor());
							if (ite->itemText.charStyle(e).fillShade() == doc->docParagraphStyles[cabori].charStyle().fillShade())
								newStyle.setFillShade(dia->TempVorl[cabneu].charStyle().fillShade());
							if (ite->itemText.charStyle(e).strokeColor() == doc->docParagraphStyles[cabori].charStyle().strokeColor())
								newStyle.setStrokeColor(dia->TempVorl[cabneu].charStyle().strokeColor());
							if (ite->itemText.charStyle(e).strokeShade() == doc->docParagraphStyles[cabori].charStyle().strokeShade())
								newStyle.setStrokeShade(dia->TempVorl[cabneu].charStyle().strokeShade());
							if (ite->itemText.charStyle(e).shadowXOffset() == doc->docParagraphStyles[cabori].charStyle().shadowXOffset())
								newStyle.setShadowXOffset(dia->TempVorl[cabneu].charStyle().shadowXOffset());
							if (ite->itemText.charStyle(e).shadowYOffset() == doc->docParagraphStyles[cabori].charStyle().shadowYOffset())
								newStyle.setShadowYOffset(dia->TempVorl[cabneu].charStyle().shadowYOffset());
							if (ite->itemText.charStyle(e).outlineWidth() == doc->docParagraphStyles[cabori].charStyle().outlineWidth())
								newStyle.setOutlineWidth(dia->TempVorl[cabneu].charStyle().outlineWidth());
							if (ite->itemText.charStyle(e).underlineOffset() == doc->docParagraphStyles[cabori].charStyle().underlineOffset())
								newStyle.setUnderlineOffset(dia->TempVorl[cabneu].charStyle().underlineOffset());
							if (ite->itemText.charStyle(e).underlineWidth() == doc->docParagraphStyles[cabori].charStyle().underlineWidth())
								newStyle.setUnderlineWidth(dia->TempVorl[cabneu].charStyle().underlineWidth());
							if (ite->itemText.charStyle(e).strikethruOffset() == doc->docParagraphStyles[cabori].charStyle().strikethruOffset())
								newStyle.setStrikethruOffset(dia->TempVorl[cabneu].charStyle().strikethruOffset());
							if (ite->itemText.charStyle(e).strikethruWidth() == doc->docParagraphStyles[cabori].charStyle().strikethruWidth())
								newStyle.setStrikethruWidth(dia->TempVorl[cabneu].charStyle().strikethruWidth());
							if (ite->itemText.charStyle(e).scaleH() == doc->docParagraphStyles[cabori].charStyle().scaleH())
								newStyle.setScaleH(dia->TempVorl[cabneu].charStyle().scaleH());
							if (ite->itemText.charStyle(e).scaleV() == doc->docParagraphStyles[cabori].charStyle().scaleV())
								newStyle.setScaleV(dia->TempVorl[cabneu].charStyle().scaleV());
							if (ite->itemText.charStyle(e).baselineOffset() == doc->docParagraphStyles[cabori].charStyle().baselineOffset())
								newStyle.setBaselineOffset(dia->TempVorl[cabneu].charStyle().baselineOffset());
							if (ite->itemText.charStyle(e).tracking() == doc->docParagraphStyles[cabori].charStyle().tracking())
								newStyle.setTracking(dia->TempVorl[cabneu].charStyle().tracking());
						}
						else
						{
							newStyle = ite->itemText.defaultStyle().charStyle();
								//.cstyle & static_cast<StyleFlag>(~1919);
							//newStyle.cstyle |= static_cast<StyleFlag>(ite->TxTStyle);
						}
						if (newStyle != lastStyle || lastParaStyle != cabneu) {
							ite->itemText.applyCharStyle(lastStyleStart, e-lastStyleStart, lastStyle);
							lastStyle = newStyle;
							lastStyleStart = e;
							lastParaStyle = cabneu;
						}
						if (ite->itemText.text(e) == SpecialChars::PARSEP && cabneu >= 0) {
							ite->itemText.applyStyle(e, dia->TempVorl[cabneu]);
						}
					}
					else if (lastParaStyle >= 0) {
						ite->itemText.applyCharStyle(lastStyleStart, e-lastStyleStart, lastStyle);
						lastStyle = newStyle;
						lastStyleStart = e;
						lastParaStyle = -1;
					}
				}
				if (ite->itemText.length() > 0) {
					ite->itemText.applyCharStyle(lastStyleStart, ite->itemText.length()-lastStyleStart, lastStyle);
					if (lastParaStyle >=0 )
						ite->itemText.applyStyle(ite->itemText.length()-1, dia->TempVorl[lastParaStyle]);
				}
			}
		}
	}
 */
	if (CurrStED != NULL)
	{
/*		if (CurrStED->Editor->StyledText.count() != 0)
		{
			for (uint pa = 0; pa < CurrStED->Editor->StyledText.count(); ++pa)
			{
				SEditor::ChList *chars;
				chars = CurrStED->Editor->StyledText.at(pa);
				(*CurrStED->Editor->ParagStyles.at(pa)) = ers[CurrStED->Editor->ParagStyles[pa]];

				int cabneu = 0;
				for (uint e = 0; e < chars->count(); ++e)
				{
					int cabori = chars->at(e)->cab;
					assert (cabore >= 0 && cabori < doc->docParagraphStyles.count());
					cabneu = ers[cabori];
					assert (cabneu >= 0 && cabneu < dia->TempVorl.count() );
					if (cabori > 4)
					{
						if (cabneu > 0)
						{
							if (chars->at(e)->charStyle.font().scName() == doc->docParagraphStyles[cabori].charStyle().font().scName())
								chars->at(e)->charStyle.setFont(dia->TempVorl[cabneu].charStyle().font());
							if (chars->at(e)->charStyle.fontSize() == doc->docParagraphStyles[cabori].charStyle().fontSize())
								chars->at(e)->charStyle.setFontSize(dia->TempVorl[cabneu].charStyle().fontSize());
							if ((chars->at(e)->charStyle.effects() & static_cast<StyleFlag>(1919) ) == doc->docParagraphStyles[cabori].charStyle().effects())
							{
								StyleFlag fl = chars->at(e)->charStyle.effects();
								fl&= static_cast<StyleFlag>(~1919);
								fl |= dia->TempVorl[cabneu].charStyle().effects();
								chars->at(e)->charStyle.setFeatures(fl.featureList());
							}
							if (chars->at(e)->charStyle.fillColor() == doc->docParagraphStyles[cabori].charStyle().fillColor())
								chars->at(e)->charStyle.setFillColor(dia->TempVorl[cabneu].charStyle().fillColor());
							if (chars->at(e)->charStyle.fillShade() == doc->docParagraphStyles[cabori].charStyle().fillShade())
								chars->at(e)->charStyle.setFillShade(dia->TempVorl[cabneu].charStyle().fillShade());
							if (chars->at(e)->charStyle.strokeColor() == doc->docParagraphStyles[cabori].charStyle().strokeColor())
								chars->at(e)->charStyle.setStrokeColor(dia->TempVorl[cabneu].charStyle().strokeColor());
							if (chars->at(e)->charStyle.strokeShade() == doc->docParagraphStyles[cabori].charStyle().strokeShade())
								chars->at(e)->charStyle.setStrokeShade(dia->TempVorl[cabneu].charStyle().strokeShade());
							if (chars->at(e)->charStyle.shadowXOffset() == doc->docParagraphStyles[cabori].charStyle().shadowXOffset())
								chars->at(e)->charStyle.setShadowXOffset(dia->TempVorl[cabneu].charStyle().shadowXOffset());
							if (chars->at(e)->charStyle.shadowYOffset() == doc->docParagraphStyles[cabori].charStyle().shadowYOffset())
								chars->at(e)->charStyle.setShadowYOffset(dia->TempVorl[cabneu].charStyle().shadowYOffset());
							if (chars->at(e)->charStyle.outlineWidth() == doc->docParagraphStyles[cabori].charStyle().outlineWidth())
								chars->at(e)->charStyle.setOutlineWidth(dia->TempVorl[cabneu].charStyle().outlineWidth());
							if (chars->at(e)->charStyle.underlineOffset() == doc->docParagraphStyles[cabori].charStyle().underlineOffset())
								chars->at(e)->charStyle.setUnderlineOffset(dia->TempVorl[cabneu].charStyle().underlineOffset());
							if (chars->at(e)->charStyle.underlineWidth() == doc->docParagraphStyles[cabori].charStyle().underlineWidth())
								chars->at(e)->charStyle.setUnderlineWidth(dia->TempVorl[cabneu].charStyle().underlineWidth());
							if (chars->at(e)->charStyle.strikethruOffset() == doc->docParagraphStyles[cabori].charStyle().strikethruOffset())
								chars->at(e)->charStyle.setStrikethruOffset(dia->TempVorl[cabneu].charStyle().strikethruOffset());
							if (chars->at(e)->charStyle.strikethruWidth() == doc->docParagraphStyles[cabori].charStyle().strikethruWidth())
								chars->at(e)->charStyle.setStrikethruWidth(dia->TempVorl[cabneu].charStyle().strikethruWidth());
							if (chars->at(e)->charStyle.scaleH() == doc->docParagraphStyles[cabori].charStyle().scaleH())
								chars->at(e)->charStyle.setScaleH(dia->TempVorl[cabneu].charStyle().scaleH());
							if (chars->at(e)->charStyle.scaleV() == doc->docParagraphStyles[cabori].charStyle().scaleV())
								chars->at(e)->charStyle.setScaleV(dia->TempVorl[cabneu].charStyle().scaleV());
							if (chars->at(e)->charStyle.baselineOffset() == doc->docParagraphStyles[cabori].charStyle().baselineOffset())
								chars->at(e)->charStyle.setBaselineOffset(dia->TempVorl[cabneu].charStyle().baselineOffset());
							if (chars->at(e)->charStyle.tracking() == doc->docParagraphStyles[cabori].charStyle().tracking())
								chars->at(e)->charStyle.setTracking(dia->TempVorl[cabneu].charStyle().tracking());
						}
						else
						{
							chars->at(e)->charStyle = ite->itemText.defaultStyle().charStyle();
						}
						chars->at(e)->cab = cabneu;
					}
				}

			}
			CurrStED->Editor->currentParaStyle = ers[CurrStED->Editor->currentParaStyle];
		}
	*/
	}
	doc->redefineStyles(dia->TempVorl);
	if (CurrStED != NULL)
	{
		if (CurrStED->Editor->StyledText.length() != 0)
			CurrStED->Editor->updateAll();
	}
	for (uint a=0; a<doc->paragraphStyles().count(); ++a)
	{
		if (!doc->paragraphStyles()[a].charStyle().font().isNone())
		{
			QString nf = doc->paragraphStyles()[a].charStyle().font().scName();
			if (!doc->UsedFonts.contains(nf))
			{
				if (doc->AddFont(nf)) //, prefsManager->appPrefs.fontPrefs.AvailFonts[nf]->Font))
				{
				}
//				else
//FIXME					doc->paragraphStyles()[a].charStyle().setFont((prefsManager->appPrefs.fontPrefs.AvailFonts[doc->toolSettings.textFont]));
			}
		}
	}

	emit UpdateRequest(reqColorsUpdate | reqTextStylesUpdate);
	view->DrawNew();
	slotDocCh();
}
#endif
//CB-->Doc
void ScribusMainWindow::setNewAlignment(int a)
{
	if (HaveDoc)
	{
//		doc->currentStyle.setAlignment(static_cast<ParagraphStyle::AlignmentType>(a));
		doc->itemSelection_SetAlignment(a);
		propertiesPalette->textPal->displayAlignment(a);
		PageItem *currItem = doc->m_Selection->itemAt(0);
		setTBvals(currItem);
	}
}

void ScribusMainWindow::setNewParStyle(const QString& name)
{
	if (HaveDoc)
	{
/*		if (name.isEmpty())
		{
			doc->itemSelection_SetNamedParagraphStyle(name);
			doc->itemSelection_EraseParagraphStyle();
		}
		else */
			doc->itemSelection_SetNamedParagraphStyle(name);
		PageItem *currItem = doc->m_Selection->itemAt(0);
		setTBvals(currItem);
	}
}

void ScribusMainWindow::setNewCharStyle(const QString& name)
{
	if (HaveDoc)
	{
/*		if (name.isEmpty())
		{
			doc->itemSelection_SetNamedCharStyle(name);
			doc->itemSelection_EraseCharStyle();
		}
		else */
			doc->itemSelection_SetNamedCharStyle(name);
		PageItem *currItem = doc->m_Selection->itemAt(0);
		setTBvals(currItem);
	}
}

void ScribusMainWindow::setAlignmentValue(int a)
{
//	doc->currentStyle = doc->docParagraphStyles[a];
//	doc->currentStyle.setAlignment(static_cast<ParagraphStyle::AlignmentType>(a<5 ? a : 0));
	propertiesPalette->textPal->displayAlignment(a);
	QString alignment[] = {"Left", "Center", "Right", "Block", "Forced"};
	for (int b=0; b<5; ++b)
	{
		QString actionName="align"+alignment[b];
		if (scrActions[actionName])
			scrActions[actionName]->setChecked(a==b);
	}
}

void ScribusMainWindow::updtGradMask()
{
	if (!HaveDoc)
		return;
	VGradient vg(propertiesPalette->getMaskGradient());
	doc->itemSelection_SetMaskGradient(vg);
}

void ScribusMainWindow::updtGradMaskGroup()
{
	if (!HaveDoc)
		return;
	VGradient vg(propertiesPalette->getMaskGradientGroup());
	doc->itemSelection_SetMaskGradient(vg);
}

//CB-->??
void ScribusMainWindow::MakeFrame(int f, int c, double *vals)
{
	PageItem *currItem = doc->m_Selection->itemAt(0);
	switch (f)
	{
	case 0:
		currItem->SetRectFrame();
		doc->setRedrawBounding(currItem);
		break;
	case 1:
		currItem->SetOvalFrame();
		doc->setRedrawBounding(currItem);
		break;
	default:
		currItem->SetFrameShape(c, vals);
		doc->setRedrawBounding(currItem);
		currItem->FrameType = f+2;
		break;
	}
	//propertiesPalette->setCurrentItem(currItem);
	currItem->update();
	slotDocCh();
}

void ScribusMainWindow::duplicateItem()
{
	slotSelect();
	bool savedAlignGrid = doc->useRaster;
	bool savedAlignGuides = doc->SnapGuides;
	internalCopy = true;
	doc->useRaster = false;
	doc->SnapGuides = false;
	slotEditCopy();
	view->Deselect(true);
	slotEditPaste();
	for (int b=0; b<doc->m_Selection->count(); ++b)
	{
		doc->m_Selection->itemAt(b)->setLocked(false);
		doc->MoveItem(doc->opToolPrefs().dispX, doc->opToolPrefs().dispY, doc->m_Selection->itemAt(b));
	}
	doc->useRaster = savedAlignGrid;
	doc->SnapGuides = savedAlignGuides;
	internalCopy = false;
	view->DrawNew();
}

void ScribusMainWindow::duplicateItemMulti()
{
	if (!HaveDoc)
		return;
	slotSelect();
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	internalCopy = true;
	MultipleDuplicate *dia = new MultipleDuplicate(doc->unitIndex(), this);
	if (dia->exec())
	{
		ItemMultipleDuplicateData mdData;
		dia->getMultiplyData(mdData);
		doc->itemSelection_MultipleDuplicate(mdData);
	}
	internalCopy = false;
	delete dia;
}

void ScribusMainWindow::selectItemsFromOutlines(PageItem* ite, bool single)
{
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	activateWindow();
	view->Deselect(true);
	if (!doc->symbolEditMode())
	{
		if ((ite->OwnPage != -1) && (ite->OwnPage != static_cast<int>(doc->currentPage()->pageNr())))
			view->GotoPage(ite->OwnPage);
	}
	doc->m_Selection->delaySignalsOn();
	view->SelectItem(ite, true, single);
	doc->m_Selection->delaySignalsOff();
	doc->m_Selection->connectItemToGUI();
	if (doc->m_Selection->count() != 0)
	{
		PageItem *currItem = doc->m_Selection->itemAt(0);
	 	double rotation=currItem->rotation();
		if ( rotation != 0.0 )
		{
			double MPI180=1.0/(180.0*M_PI);
			double y1 = sin(rotation*MPI180) * currItem->width();
			double x1 = cos(rotation*MPI180) * currItem->width();
			double y2 = sin((rotation+90.0)*MPI180) * currItem->height();
			double x2 = cos((rotation+90.0)*MPI180) * currItem->height();
			double mx = currItem->xPos() + ((x1 + x2)/2.0);
			double my = currItem->yPos() + ((y1 + y2)/2.0);
			view->SetCCPo(mx, my);
		}
		else
		{
			view->SetCCPo(currItem->xPos() + currItem->width() / 2.0, currItem->yPos() + currItem->height() / 2.0);
		}
	}
}

void ScribusMainWindow::selectItemsFromOutlines(int Page, int Item, bool single)
{
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	activateWindow();
	view->Deselect(true);
	if ((Page != -1) && (Page != static_cast<int>(doc->currentPage()->pageNr())))
		view->GotoPage(Page);
	doc->m_Selection->delaySignalsOn();
	view->SelectItemNr(Item, true, single);
	doc->m_Selection->delaySignalsOff();
	doc->m_Selection->connectItemToGUI();
	if (doc->m_Selection->count() != 0)
	{
		PageItem *currItem = doc->m_Selection->itemAt(0);
	 // jjsa 23-05-2004 added for centering of rotated objects
	 	double rotation=currItem->rotation();
		if ( rotation != 0.0 )
		{
			double MPI180=1.0/(180.0*M_PI);
			double y1 = sin(rotation*MPI180) * currItem->width();
			double x1 = cos(rotation*MPI180) * currItem->width();
			double y2 = sin((rotation+90.0)*MPI180) * currItem->height();
			double x2 = cos((rotation+90.0)*MPI180) * currItem->height();
			double mx = currItem->xPos() + ((x1 + x2)/2.0);
			double my = currItem->yPos() + ((y1 + y2)/2.0);
//			double viewScale=view->scale();
//			if ((qRound((currItem->xPos() + qMax(x1, x2)) * viewScale) > view->contentsWidth()) ||
//				(qRound((currItem->yPos() + qMax(y1, y2)) * viewScale) > view->contentsHeight()))
//				view->resizeContents(qMax(qRound((currItem->xPos() + qMax(x1, x2)) * viewScale),
//									view->contentsWidth()),
//									qMax(qRound((currItem->yPos() + qMax(y1, y2)) * viewScale), view->contentsHeight()));
			view->SetCCPo(mx, my);
		}
		else
		{
//			double viewScale=view->scale();
//			if ((qRound((currItem->xPos() + currItem->width()) * viewScale) > view->contentsWidth()) ||
//				(qRound((currItem->yPos() + currItem->height()) * viewScale) > view->contentsHeight())
//				)
//				view->resizeContents(qMax(qRound((currItem->xPos() + currItem->width()) * viewScale), view->contentsWidth()),
//									 qMax(qRound((currItem->yPos() + currItem->height()) * viewScale), view->contentsHeight()));
			view->SetCCPo(currItem->xPos() + currItem->width() / 2.0, currItem->yPos() + currItem->height() / 2.0);
		}
	}
}

void ScribusMainWindow::selectPagesFromOutlines(int Page)
{
	if (HaveDoc && doc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);
	activateWindow();
	view->Deselect(true);
	if (Page < 0)
		return;
	view->GotoPage(Page);
}
/*
void ScribusMainWindow::prefsOrg(Preferences *dia)
{
	//reset the appMode so we restore our tools shortcuts
	QString oldUILanguage = prefsManager->uiLanguage();
	QString oldUIStyle    = prefsManager->guiStyle();
	QString oldMonitorProfile = ScCore->monitorProfile.productDescription();
	bool    oldShowPageShadow = prefsManager->showPageShadow();
	int     oldUIFontSize     = prefsManager->guiFontSize();
	double  oldDisplayScale   = prefsManager->displayScale();
	int     oldImageQuality   = prefsManager->applicationPrefs()->itemToolPrefs.imageLowResType;

	dia->updatePreferences();
	DocDir = prefsManager->documentDir();
//		scrapbookPalette->rebuildView();
//		scrapbookPalette->AdjustMenu();
	QString newUILanguage = prefsManager->uiLanguage();
	if (oldUILanguage != newUILanguage || ScQApp->currGUILanguage()!=newUILanguage)
		ScQApp->changeGUILanguage(newUILanguage);
	QString newUIStyle = prefsManager->guiStyle();
	if (oldUIStyle != newUIStyle)
	{
		if (newUIStyle.isEmpty())
			qApp->setStyle(prefsManager->guiSystemStyle());
		else
			qApp->setStyle(QStyleFactory::create(newUIStyle));
		// Plain wrong, a style may set a palette different from the standard palette
		// Eg : Windows XP and Windows Vista styles
		// qApp->setPalette(qApp->style()->standardPalette());
	}
	int newUIFontSize = prefsManager->guiFontSize();
	if (oldUIFontSize != newUIFontSize)
	{
		QFont apf = qApp->font();
		apf.setPointSize(prefsManager->appPrefs.uiPrefs.applicationFontSize);
		qApp->setFont(apf);
	}
	propertiesPalette->textPal->Fonts->RebuildList(0);
	ScCore->getCMSProfiles(false);
	ScCore->recheckGS();
	prefsManager->applyLoadedShortCuts();

	QString newMonitorProfile = prefsManager->appPrefs.colorPrefs.DCMSset.DefaultMonitorProfile;
	if (oldMonitorProfile != newMonitorProfile)
	{
		bool success = false;
		if (ScCore->MonitorProfiles.contains(newMonitorProfile))
		{
			QString profilePath = ScCore->MonitorProfiles[newMonitorProfile];
			ScColorProfile newProfile = ScCore->defaultEngine.openProfileFromFile(profilePath);
			if (!newProfile.isNull())
			{
				ScCore->monitorProfile = newProfile;
				success = true;
			}
		}
		if (!success)
		{
			prefsManager->appPrefs.colorPrefs.DCMSset.DefaultMonitorProfile = oldMonitorProfile;
			QString message = tr("An error occurred while opening monitor profile.\nFormer monitor profile will be used." );
			if (ScCore->usingGUI())
				QMessageBox::warning(this, CommonStrings::trWarning, message, QMessageBox::Ok, 0, 0);
			else
				qWarning( "%s", message.toLocal8Bit().data() );
		}
	}

	int newImageQuality = prefsManager->appPrefs.itemToolPrefs.imageLowResType;
	if (oldImageQuality != newImageQuality)
		view->previewQualitySwitcher->setCurrentIndex(newImageQuality);

	if (prefsManager->appPrefs.uiPrefs.useTabs)
		mdiArea->setViewMode(QMdiArea::TabbedView);
	else
		mdiArea->setViewMode(QMdiArea::SubWindowView);
	QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
	bool shadowChanged = oldShowPageShadow != prefsManager->showPageShadow();
	if (!windows.isEmpty())
	{
		int windowCount=static_cast<int>(windows.count());
		for ( int i = 0; i < windowCount; ++i )
		{
			QWidget* w = windows.at(i)->widget();
			ScribusWin* scw = (ScribusWin*) w;
			if (oldDisplayScale != prefsManager->displayScale())
			{
				int x = qRound(qMax(scw->view()->contentsX() / scw->view()->scale(), 0.0));
				int y = qRound(qMax(scw->view()->contentsY() / scw->view()->scale(), 0.0));
				int w = qRound(qMin(scw->view()->visibleWidth() / scw->view()->scale(), scw->doc()->currentPage()->width()));
				int h = qRound(qMin(scw->view()->visibleHeight() / scw->view()->scale(), scw->doc()->currentPage()->height()));
				scw->view()->rememberOldZoomLocation(w / 2 + x,h / 2 + y);
				scw->view()->zoom((scw->view()->scale() / oldDisplayScale) * prefsManager->displayScale());
			}
			if (shadowChanged)
				scw->view()->DrawNew();
		}
	}

	prefsManager->SavePrefs();
}

void ScribusMainWindow::slotPrefsOrg()
{
	slotSelect();

	Preferences dia(this);
	if (dia.exec()==QDialog::Accepted)
		prefsOrg(&dia);
}
*/
void ScribusMainWindow::slotPrefs150Org()
{
	QString oldMonitorProfile(ScCore->monitorProfile.productDescription());
	slotSelect();
	struct ApplicationPrefs oldPrefs(prefsManager->appPrefs);
	PreferencesDialog prefsDialog(this, oldPrefs);
	int prefsResult=prefsDialog.exec();
	if (prefsResult==QDialog::Accepted)
	{
		struct ApplicationPrefs newPrefs(prefsDialog.prefs());
		prefsManager->setNewPrefs(newPrefs);
		prefsManager->applyLoadedShortCuts();

		//TODO: and the other dirs?
		DocDir = prefsManager->documentDir();
		ScQApp->neverSplash(!prefsManager->appPrefs.uiPrefs.showSplashOnStartup);

		QString newUILanguage = prefsManager->uiLanguage();
		if (oldPrefs.uiPrefs.language != newUILanguage || ScQApp->currGUILanguage()!=newUILanguage)
			ScQApp->changeGUILanguage(newUILanguage);
		QString newUIStyle = prefsManager->guiStyle();
		if (oldPrefs.uiPrefs.style != newUIStyle)
		{
			if (newUIStyle.isEmpty())
				qApp->setStyle(prefsManager->guiSystemStyle());
			else
				qApp->setStyle(QStyleFactory::create(newUIStyle));
		}
		int newUIFontSize = prefsManager->guiFontSize();
		if (oldPrefs.uiPrefs.applicationFontSize != newUIFontSize)
		{
			QFont apf = qApp->font();
			apf.setPointSize(newUIFontSize);
			qApp->setFont(apf);
		}
		emit UpdateRequest(reqDefFontListUpdate);
		if (prefsManager->appPrefs.uiPrefs.useTabs)
			mdiArea->setViewMode(QMdiArea::TabbedView);
		else
			mdiArea->setViewMode(QMdiArea::SubWindowView);
		bool shadowChanged = oldPrefs.displayPrefs.showPageShadow != prefsManager->showPageShadow();
		QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
		if (!windows.isEmpty())
		{
			int windowCount=static_cast<int>(windows.count());
			for ( int i = 0; i < windowCount; ++i )
			{
				QWidget* w = windows.at(i)->widget();
				ScribusWin* scw = (ScribusWin*) w;
				ScribusView* scw_v = scw->view();
				if (oldPrefs.displayPrefs.displayScale != prefsManager->displayScale())
				{
					int x = qRound(qMax(scw_v->contentsX() / scw_v->scale(), 0.0));
					int y = qRound(qMax(scw_v->contentsY() / scw_v->scale(), 0.0));
					int w = qRound(qMin(scw_v->visibleWidth() / scw_v->scale(), scw->doc()->currentPage()->width()));
					int h = qRound(qMin(scw_v->visibleHeight() / scw_v->scale(), scw->doc()->currentPage()->height()));
					scw_v->rememberOldZoomLocation(w / 2 + x,h / 2 + y);
					scw_v->zoom((scw_v->scale() / oldPrefs.displayPrefs.displayScale) * prefsManager->displayScale());
				}
				if (shadowChanged)
					scw->view()->DrawNew();
			}
		}

		QString newMonitorProfile = newPrefs.colorPrefs.DCMSset.DefaultMonitorProfile;
		if (oldMonitorProfile != newMonitorProfile)
		{
			bool success = false;
			if (ScCore->MonitorProfiles.contains(newMonitorProfile))
			{
				QString profilePath = ScCore->MonitorProfiles[newMonitorProfile];
				ScColorProfile newProfile = ScCore->defaultEngine.openProfileFromFile(profilePath);
				if (!newProfile.isNull())
				{
					ScCore->monitorProfile = newProfile;
					success = true;
				}
			}
			if (!success)
			{
				newPrefs.colorPrefs.DCMSset.DefaultMonitorProfile = oldMonitorProfile;
				prefsManager->setNewPrefs(newPrefs);
				QString message = tr("An error occurred while opening monitor profile.\nFormer monitor profile will be used." );
				if (ScCore->usingGUI())
					QMessageBox::warning(this, CommonStrings::trWarning, message, QMessageBox::Ok, 0, 0);
				else
					qWarning( "%s", message.toLocal8Bit().data() );
			}
		}
		ScImageCacheManager & icm = ScImageCacheManager::instance();
		icm.setEnabled(newPrefs.imageCachePrefs.cacheEnabled);
		icm.setMaxCacheSizeMiB(newPrefs.imageCachePrefs.maxCacheSizeMiB);
		icm.setMaxCacheEntries(newPrefs.imageCachePrefs.maxCacheEntries);
		icm.setCompressionLevel(newPrefs.imageCachePrefs.compressionLevel);

		prefsManager->SavePrefs();
	}
}

/*
void ScribusMainWindow::docSetup(ReformDoc* dia)
{
	slotChangeUnit(dia->getSelectedUnit(), false);
	dia->updateDocumentSettings();
	if (dia->imageResolutionChanged())
	{
		setStatusBarInfoText( tr("Updating Images"));
		mainWindowProgressBar->reset();
		qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
		qApp->processEvents();
		doc->recalcPicturesRes(true);
		qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
		setStatusBarInfoText("");
		mainWindowProgressBar->reset();
		view->previewQualitySwitcher->blockSignals(true);
		view->previewQualitySwitcher->setCurrentIndex(doc->itemToolPrefs().imageLowResType);
		view->previewQualitySwitcher->blockSignals(false);
	}
	propertiesPalette->textPal->Fonts->RebuildList(doc);
	scrActions["viewShowMargins"]->setChecked(doc->guidesPrefs().marginsShown);
	scrActions["viewShowBleeds"]->setChecked(doc->guidesPrefs().showBleed);
	scrActions["viewShowFrames"]->setChecked(doc->guidesPrefs().framesShown);
	scrActions["viewShowLayerMarkers"]->setChecked(doc->guidesPrefs().layerMarkersShown);
	scrActions["viewShowGrid"]->setChecked(doc->guidesPrefs().gridShown);
	scrActions["viewShowGuides"]->setChecked(doc->guidesPrefs().guidesShown);
	scrActions["viewShowColumnBorders"]->setChecked(doc->guidesPrefs().colBordersShown);
	scrActions["viewShowBaseline"]->setChecked(doc->guidesPrefs().baselineGridShown);
	scrActions["viewShowImages"]->setChecked(doc->guidesPrefs().showPic);
	scrActions["viewShowTextChain"]->setChecked(doc->guidesPrefs().linkShown);
	scrActions["viewShowTextControls"]->setChecked(doc->guidesPrefs().showControls);
	scrActions["viewShowRulers"]->setChecked(doc->guidesPrefs().rulersShown);
	scrActions["viewRulerMode"]->setChecked(doc->guidesPrefs().rulerMode);
	scrActions["extrasGenerateTableOfContents"]->setEnabled(doc->hasTOCSetup());
	view->cmsToolbarButton->setChecked(doc->HasCMS);
	//doc emits changed() via this
	doc->setMasterPageMode(true);
	view->reformPages();
	doc->setMasterPageMode(false);
//	doc->setLoading(true);
//	uint pageCount=doc->DocPages.count();
//	for (uint c=0; c<pageCount; ++c)
//		Apply_MasterPage(doc->DocPages.at(c)->MPageNam, c, false);
//	doc->setLoading(false);
	view->reformPages();
	view->GotoPage(doc->currentPage()->pageNr());
	view->DrawNew();
	propertiesPalette->imagePal->showCMSOptions();
	pagePalette->rebuildPages();
}

bool ScribusMainWindow::slotDocSetup()
{
	bool ret = false;
	ReformDoc* dia = new ReformDoc(this, doc);
	Q_CHECK_PTR(dia);
	if (dia->exec())
	{
		docSetup(dia);
		ret = true;
	}
	delete dia;
	return ret;
}
*/
void ScribusMainWindow::slotDocSetup150()
{
	if (!doc)
		return;
	struct ApplicationPrefs oldDocPrefs(doc->prefsData());
	PreferencesDialog prefsDialog(this, oldDocPrefs, doc);
	int prefsResult=prefsDialog.exec();
	if (prefsResult==QDialog::Accepted)
	{
		struct ApplicationPrefs newDocPrefs(prefsDialog.prefs());
		bool resizePages, resizeMasterPages, resizePageMargins, resizeMasterPageMargins;
		prefsDialog.getResizeDocumentPages(resizePages, resizeMasterPages, resizePageMargins, resizeMasterPageMargins);
		doc->setNewPrefs(newDocPrefs, oldDocPrefs, resizePages, resizeMasterPages, resizePageMargins, resizeMasterPageMargins);


		slotChangeUnit(doc->unitIndex(), false);
		//dia->updateDocumentSettings();
		if (oldDocPrefs.itemToolPrefs.imageLowResType!=newDocPrefs.itemToolPrefs.imageLowResType)
		{
			setStatusBarInfoText( tr("Updating Images"));
			mainWindowProgressBar->reset();
			qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
			qApp->processEvents();
			doc->recalcPicturesRes(true);
			qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
			setStatusBarInfoText("");
			mainWindowProgressBar->reset();
			view->previewQualitySwitcher->blockSignals(true);
			view->previewQualitySwitcher->setCurrentIndex(doc->itemToolPrefs().imageLowResType);
			view->previewQualitySwitcher->blockSignals(false);
		}
		emit UpdateRequest(reqDocFontListUpdate);
		scrActions["viewShowMargins"]->setChecked(doc->guidesPrefs().marginsShown);
		scrActions["viewShowBleeds"]->setChecked(doc->guidesPrefs().showBleed);
		scrActions["viewShowFrames"]->setChecked(doc->guidesPrefs().framesShown);
		scrActions["viewShowLayerMarkers"]->setChecked(doc->guidesPrefs().layerMarkersShown);
		scrActions["viewShowGrid"]->setChecked(doc->guidesPrefs().gridShown);
		scrActions["viewShowGuides"]->setChecked(doc->guidesPrefs().guidesShown);
		scrActions["viewShowColumnBorders"]->setChecked(doc->guidesPrefs().colBordersShown);
		scrActions["viewShowBaseline"]->setChecked(doc->guidesPrefs().baselineGridShown);
		scrActions["viewShowImages"]->setChecked(doc->guidesPrefs().showPic);
		scrActions["viewShowTextChain"]->setChecked(doc->guidesPrefs().linkShown);
		scrActions["viewShowTextControls"]->setChecked(doc->guidesPrefs().showControls);
		scrActions["viewShowRulers"]->setChecked(doc->guidesPrefs().rulersShown);
		scrActions["viewRulerMode"]->setChecked(doc->guidesPrefs().rulerMode);
		scrActions["extrasGenerateTableOfContents"]->setEnabled(doc->hasTOCSetup());
		view->cmsToolbarButton->setChecked(doc->HasCMS);
		//doc emits changed() via this
		doc->setMasterPageMode(true);
		view->reformPages();
		doc->setMasterPageMode(false);
		view->reformPages();
		view->GotoPage(doc->currentPage()->pageNr());
		view->DrawNew();
		pagePalette->rebuildPages();
		emit UpdateRequest(reqCmsOptionsUpdate);
	}
}
int ScribusMainWindow::ShowSubs()
{
	bool showGSHelp=false;
	if (!ScCore->haveGS())
	{
		QMessageBox mb(this);
		QString msg = tr("Ghostscript is not installed on your system, or Scribus is not configured with the path to the software.");
		QString msg2("<qt>");
#ifndef _WIN32
		msg2 += tr("Until this is remedied, you cannot import EPS images or use Print Preview. ")+"\n";
#else
		msg2 += tr("Until this is remedied, you cannot import EPS images or use PostScript Print Preview. ")+"\n";
#endif
		//msg2 += tr("Please read our <a href=\"http://wiki.scribus.net/index.php/Ghostscript\">help and installation instructions</a>.") + "</qt>";
		msg2 += tr("Click the Help button to read Scribus-related Ghostscript help and installation instructions.") + "</qt>";
		QMessageBox msgBox;
		msgBox.addButton(QMessageBox::Ok);
		msgBox.addButton(QMessageBox::Help);
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setWindowTitle( tr("Ghostscript is missing") );
		msgBox.setText(msg);
		msgBox.setInformativeText(msg2);
		int i=msgBox.exec();
		if (i==QMessageBox::Help)
			showGSHelp=true;
	}

	propertiesPalette->startup();
	outlinePalette->startup();
	scrapbookPalette->startup();
	bookmarkPalette->startup();
	pagePalette->startup();
	layerPalette->startup();
//	measurementPalette->startup();
	docCheckerPalette->startup();
	alignDistributePalette->startup();
	undoPalette->startup();
	guidePalette->startup();
	charPalette->startup();
	styleManager->startup();
	symbolPalette->startup();

	// init the toolbars
	fileToolBar->initVisibility();
	editToolBar->initVisibility();
	modeToolBar->initVisibility();
	pdfToolBar->initVisibility();

	activateWindow();
	if (!scriptIsRunning())
	{
		raise();
		if (showGSHelp)
		{
			slotOnlineHelp("", "toolbox5.html");
			return QMessageBox::Help;
		}
	}
	return 0;
}

void ScribusMainWindow::doPrintPreview()
{
	if (docCheckerPalette->isIgnoreEnabled())
	{
		docCheckerPalette->hide();
		docCheckerPalette->checkMode = CheckDocument::checkNULL;
		docCheckerPalette->setIgnoreEnabled(false);
		scrActions["toolsPreflightVerifier"]->setChecked(false);
		disconnect(docCheckerPalette, SIGNAL(ignoreAllErrors()), this, SLOT(doPrintPreview()));
	}
	if (HaveDoc)
	{
		PrefsContext* prefs = PrefsManager::instance()->prefsFile->getContext("print_options");
		QString currentPrinter    = prefs->get("CurrentPrn");
		PrintEngine currentEngine = (PrintEngine) prefs->get("CurrentPrnEngine", "3").toInt();
		if ( PPreview::usePostscriptPreview(currentPrinter, currentEngine) && ( !ScCore->haveGS() ) )
		{
			QString mess = tr("Ghostscript is missing : PostScript Print Preview is not available")+"\n\n";
			QMessageBox::warning(this, CommonStrings::trWarning, mess, 1, 0, 0);
			return;
		}
		PPreview *dia = new PPreview(this, view, doc, currentPrinter, currentEngine);
		previewDinUse = true;
		connect(dia, SIGNAL(doPrint()), this, SLOT(slotReallyPrint()));
		dia->exec();
		PrefsManager *prefsManager=PrefsManager::instance();
		prefsManager->appPrefs.printPreviewPrefs.PrPr_Mode = dia->EnableCMYK->isChecked();
		prefsManager->appPrefs.printPreviewPrefs.PrPr_AntiAliasing = dia->AntiAlias->isChecked();
		prefsManager->appPrefs.printPreviewPrefs.PrPr_Transparency = dia->AliasTr->isChecked();
		if ( !ScCore->haveTIFFSep() || !dia->postscriptPreview )
		{
			prefsManager->appPrefs.printPreviewPrefs.PrPr_C = dia->EnableCMYK_C->isChecked();
			prefsManager->appPrefs.printPreviewPrefs.PrPr_M = dia->EnableCMYK_M->isChecked();
			prefsManager->appPrefs.printPreviewPrefs.PrPr_Y = dia->EnableCMYK_Y->isChecked();
			prefsManager->appPrefs.printPreviewPrefs.PrPr_K = dia->EnableCMYK_K->isChecked();
		}
		else
		{
			prefsManager->appPrefs.printPreviewPrefs.PrPr_C = dia->flagsVisible["Cyan"]->isChecked();
			prefsManager->appPrefs.printPreviewPrefs.PrPr_M = dia->flagsVisible["Magenta"]->isChecked();
			prefsManager->appPrefs.printPreviewPrefs.PrPr_Y = dia->flagsVisible["Yellow"]->isChecked();
			prefsManager->appPrefs.printPreviewPrefs.PrPr_K = dia->flagsVisible["Black"]->isChecked();
			prefsManager->appPrefs.printPreviewPrefs.PrPr_InkCoverage = dia->EnableInkCover->isChecked();
			prefsManager->appPrefs.printPreviewPrefs.PrPr_InkThreshold = dia->CoverThresholdValue->value();
		}
//		prefsManager->appPrefs.Gcr_Mode = dia->EnableGCR->isChecked();
		disconnect(dia, SIGNAL(doPrint()), this, SLOT(slotReallyPrint()));
		previewDinUse = false;
		delete dia;
		QFile::remove(prefsManager->preferencesLocation()+"/tmp.ps");
		QFile::remove(prefsManager->preferencesLocation()+"/sc.png");
		QDir d(prefsManager->preferencesLocation()+"/", "sc.*", QDir::Name, QDir::Files | QDir::NoSymLinks);
		if ((d.exists()) && (d.count() != 0))
		{
			for (uint dc = 0; dc < d.count(); dc++)
			{
				QFile::remove(prefsManager->preferencesLocation() +"/" + d[dc]);
			}
		}
	}
}

void ScribusMainWindow::printPreview()
{
	if (doc->checkerProfiles()[doc->curCheckProfile()].autoCheck)
	{
		if (scanDocument())
		{
			if (doc->checkerProfiles()[doc->curCheckProfile()].ignoreErrors)
			{
				int t = QMessageBox::warning(this, CommonStrings::trWarning,
											"<qt>"+ tr("Scribus has detected some errors. Consider using the Preflight Verifier to correct them")+"</qt>",
											QMessageBox::Abort | QMessageBox::Ignore);
				if (t == QMessageBox::Abort)
					return;
			}
			else
			{
				connect(docCheckerPalette, SIGNAL(ignoreAllErrors()), this, SLOT(doPrintPreview()));
				docCheckerPalette->setIgnoreEnabled(true);
				docCheckerPalette->checkMode = CheckDocument::checkPrintPreview;
				docCheckerPalette->buildErrorList(doc);
				docCheckerPalette->show();
				scrActions["toolsPreflightVerifier"]->setChecked(true);
				return;
			}
		}
	}
	doPrintPreview();
}

bool ScribusMainWindow::DoSaveAsEps(QString fn, QString& error)
{
	QStringList spots;
	bool return_value = true;
	ReOrderText(doc, view);
	qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
	QMap<QString, QMap<uint, FPointArray> > ReallyUsed;
	ReallyUsed.clear();
	doc->getUsedFonts(ReallyUsed);
	ColorList usedColors;
	doc->getUsedColors(usedColors);
	ScCore->fileWatcher->forceScan();
	ScCore->fileWatcher->stop();
	PrintOptions options;
	options.pageNumbers.push_back(doc->currentPage()->pageNr()+1);
	options.outputSeparations = false;
	options.separationName = tr("All");
	options.allSeparations = spots;
	options.useColor = true;
	options.mirrorH = false;
	options.mirrorV = false;
	options.useICC = true;
	options.doGCR = prefsManager->appPrefs.printerPrefs.GCRMode;
	options.setDevParam = false;
	options.doClip = true;
	options.cropMarks = false;
	options.bleedMarks = false;
	options.registrationMarks = false;
	options.colorMarks = false;
	options.markOffset = 0.0;
	options.bleeds.Top = 0.0;
	options.bleeds.Left = 0.0;
	options.bleeds.Right = 0.0;
	options.bleeds.Bottom = 0.0;
	PSLib *dd = new PSLib(options, false, prefsManager->appPrefs.fontPrefs.AvailFonts, ReallyUsed, usedColors, false, true);
	if (dd != NULL)
	{
		if (dd->PS_set_file(fn))
		{
			int psRet = dd->CreatePS(doc, options);
			if (psRet == 1)
			{
				error = dd->errorMessage();
				return_value = false;
			}
		}
		else
			return_value = false;
		delete dd;
		qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
	}
	ScCore->fileWatcher->start();
	return return_value;
}

void ScribusMainWindow::SaveAsEps()
{
	if (doc->checkerProfiles()[doc->curCheckProfile()].autoCheck)
	{
		if (scanDocument())
		{
			if (doc->checkerProfiles()[doc->curCheckProfile()].ignoreErrors)
			{
				int t = QMessageBox::warning(this, CommonStrings::trWarning,
											tr("Scribus detected some errors.\nConsider using the Preflight Verifier  to correct them."),
											QMessageBox::Abort | QMessageBox::Ignore);
				if (t == QMessageBox::Abort)
					return;
			}
			else
			{
				connect(docCheckerPalette, SIGNAL(ignoreAllErrors()), this, SLOT(reallySaveAsEps()));
				docCheckerPalette->setIgnoreEnabled(true);
				docCheckerPalette->checkMode = CheckDocument::checkEPS;
				docCheckerPalette->buildErrorList(doc);
				docCheckerPalette->show();
				scrActions["toolsPreflightVerifier"]->setChecked(true);
				return;
			}
		}
	}
	reallySaveAsEps();
}

void ScribusMainWindow::reallySaveAsEps()
{
	QString fna;
	if (docCheckerPalette->isIgnoreEnabled())
	{
		docCheckerPalette->hide();
		docCheckerPalette->checkMode = CheckDocument::checkNULL;
		docCheckerPalette->setIgnoreEnabled(false);
		scrActions["toolsPreflightVerifier"]->setChecked(false);
		disconnect(docCheckerPalette, SIGNAL(ignoreAllErrors()), this, SLOT(reallySaveAsEps()));
	}
	if (!doc->DocName.startsWith( tr("Document")))
	{
		QFileInfo fi(doc->DocName);
		if (doc->m_Selection->count() != 0)
			fna = fi.path() + "/" + fi.completeBaseName() + "_selection.eps";
		else
			fna = fi.path() + "/" + getFileNameByPage(doc, doc->currentPage()->pageNr(), "eps");
	}
	else
	{
		QDir di = QDir();
		if (doc->m_Selection->count() != 0)
			fna = di.currentPath() + "/" + doc->DocName + "_selection.eps";
		else
			fna = di.currentPath() + "/" + getFileNameByPage(doc, doc->currentPage()->pageNr(), "eps");
	}
	fna = QDir::toNativeSeparators(fna);
	QString wdir = ".";
	QString prefsDocDir=prefsManager->documentDir();
	if (!prefsDocDir.isEmpty())
		wdir = prefsManager->prefsFile->getContext("dirs")->get("eps", prefsDocDir);
	else
		wdir = prefsManager->prefsFile->getContext("dirs")->get("eps", ".");
	QString fn = CFileDialog( wdir, tr("Save As"), tr("%1;;All Files (*)").arg(formatsManager->extensionsForFormat(FormatsManager::EPS)), fna, fdHidePreviewCheckBox | fdNone);
	if (!fn.isEmpty())
	{
		prefsManager->prefsFile->getContext("dirs")->set("eps", fn.left(fn.lastIndexOf("/")));
		if (overwrite(this, fn))
		{
			QString epsError;
			if (!DoSaveAsEps(fn, epsError))
			{
				QString message = tr("Cannot write the file: \n%1").arg(fn);
				if (!epsError.isEmpty())
					message += QString("\n%1").arg(epsError);
				QMessageBox::warning(this, CommonStrings::trWarning, message, CommonStrings::tr_OK);
			}
		}
	}
}

bool ScribusMainWindow::getPDFDriver(const QString & fn, const QString & nam, int Components,
									 const std::vector<int> & pageNs, const QMap<int,QPixmap> & thumbs,
									 QString& error, bool* cancelled)
{
	ScCore->fileWatcher->forceScan();
	ScCore->fileWatcher->stop();
	PDFlib pdflib(*doc);
	bool ret = pdflib.doExport(fn, nam, Components, pageNs, thumbs);
	if (!ret)
		error = pdflib.errorMessage();
	if (cancelled)
		*cancelled = pdflib.exportAborted();
	ScCore->fileWatcher->start();
	return ret;
}

void ScribusMainWindow::SaveAsPDF()
{
	if (doc->checkerProfiles()[doc->curCheckProfile()].autoCheck)
	{
		if (scanDocument())
		{
			if (doc->checkerProfiles()[doc->curCheckProfile()].ignoreErrors)
			{
				int t = QMessageBox::warning(this, CommonStrings::trWarning,
											tr("Detected some errors.\nConsider using the Preflight Verifier to correct them"),
											QMessageBox::Abort | QMessageBox::Ignore);
				if (t == QMessageBox::Abort)
					return;
			}
			else
			{
				connect(docCheckerPalette, SIGNAL(ignoreAllErrors()), this, SLOT(doSaveAsPDF()));
				docCheckerPalette->setIgnoreEnabled(true);
				docCheckerPalette->checkMode = CheckDocument::checkPDF;
				docCheckerPalette->buildErrorList(doc);
				docCheckerPalette->show();
				scrActions["toolsPreflightVerifier"]->setChecked(true);
				return;
			}
		}
	}
	doSaveAsPDF();
}

void ScribusMainWindow::doSaveAsPDF()
{
	if (docCheckerPalette->isIgnoreEnabled())
	{
		docCheckerPalette->hide();
		docCheckerPalette->checkMode = CheckDocument::checkNULL;
		docCheckerPalette->setIgnoreEnabled(false);
		scrActions["toolsPreflightVerifier"]->setChecked(false);
		disconnect(docCheckerPalette, SIGNAL(ignoreAllErrors()), this, SLOT(doSaveAsPDF()));
	}
/*	if (bookmarkPalette->BView->childCount() == 0)
		doc->PDF_Options.Bookmarks = false; */
// reenabling the following line fixes Bug #7630, not sure why this line was commented out.
// 	doc->reorganiseFonts();
	QMap<QString, int> ReallyUsed = doc->reorganiseFonts(); //doc->UsedFonts;
	if (doc->pdfOptions().EmbedList.count() != 0)
	{
		QList<QString> tmpEm;
		QList<QString>::Iterator itef;
		for (itef = doc->pdfOptions().EmbedList.begin(); itef != doc->pdfOptions().EmbedList.end(); ++itef)
		{
			if (ReallyUsed.contains((*itef)))
				tmpEm.append((*itef));
		}
		doc->pdfOptions().EmbedList = tmpEm;
	}
	if (doc->pdfOptions().SubsetList.count() != 0)
	{
		QList<QString> tmpEm;
		QList<QString>::Iterator itef;
		for (itef = doc->pdfOptions().SubsetList.begin(); itef != doc->pdfOptions().SubsetList.end(); ++itef)
		{
			if (ReallyUsed.contains((*itef)))
				tmpEm.append((*itef));
		}
		doc->pdfOptions().SubsetList = tmpEm;
	}
	MarginStruct optBleeds(doc->pdfOptions().bleeds);
	PDFExportDialog dia(this, doc->DocName, ReallyUsed, view, doc->pdfOptions(), doc->pdfOptions().PresentVals, ScCore->PDFXProfiles, prefsManager->appPrefs.fontPrefs.AvailFonts, doc->unitRatio(), ScCore->PrinterProfiles);
	if (dia.exec())
	{
		qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
		dia.updateDocOptions();
		doc->pdfOptions().firstUse = false;
		ReOrderText(doc, view);
		QString pageString(dia.getPagesString());
		std::vector<int> pageNs;
		uint pageNumbersSize;
		QMap<int,QPixmap> thumbs;
		int components=dia.colorSpaceComponents();
		QString nam(dia.cmsDescriptor());
		QString fileName = doc->pdfOptions().fileName;
		QString errorMsg;
		parsePagesString(pageString, &pageNs, doc->DocPages.count());
		if (doc->pdfOptions().useDocBleeds)
			doc->pdfOptions().bleeds = *doc->bleeds();

		if (doc->pdfOptions().doMultiFile)
		{
			bool cancelled = false;
			QFileInfo fi(fileName);
			QString ext = fi.suffix();
			QString path = fi.path();
			QString name = fi.completeBaseName();
			uint aa = 0;
			while (aa < pageNs.size() && !cancelled)
			{
				thumbs.clear();
				std::vector<int> pageNs2;
				pageNs2.clear();
				pageNs2.push_back(pageNs[aa]);
				pageNumbersSize = pageNs2.size();
				QPixmap pm(10,10);
				if (doc->pdfOptions().Thumbnails)
					pm=QPixmap::fromImage(view->PageToPixmap(pageNs[aa]-1, 100));
				thumbs.insert(1, pm);
				QString realName = QDir::toNativeSeparators(path+"/"+name+ tr("-Page%1").arg(pageNs[aa], 3, 10, QChar('0'))+"."+ext);
				if (!getPDFDriver(realName, nam, components, pageNs2, thumbs, errorMsg, &cancelled))
				{
					qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
					QString message = tr("Cannot write the file: \n%1").arg(doc->pdfOptions().fileName);
					if (!errorMsg.isEmpty())
						message = QString("%1\n%2").arg(message).arg(errorMsg);
					QMessageBox::warning(this, CommonStrings::trWarning, message, CommonStrings::tr_OK);
					return;
				}
				aa++;
			}
		}
		else
		{
			pageNumbersSize = pageNs.size();
			for (uint ap = 0; ap < pageNumbersSize; ++ap)
			{
				QPixmap pm(10,10);
				if (doc->pdfOptions().Thumbnails)
					pm=QPixmap::fromImage(view->PageToPixmap(pageNs[ap]-1, 100));
				thumbs.insert(pageNs[ap], pm);
			}
			if (!getPDFDriver(fileName, nam, components, pageNs, thumbs, errorMsg))
			{
				qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
				QString message = tr("Cannot write the file: \n%1").arg(doc->pdfOptions().fileName);
				if (!errorMsg.isEmpty())
					message = QString("%1\n%2").arg(message).arg(errorMsg);
				QMessageBox::warning(this, CommonStrings::trWarning, message, CommonStrings::tr_OK);
			}
		}
		if (doc->pdfOptions().useDocBleeds)
			doc->pdfOptions().bleeds = optBleeds;
		qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
		if (errorMsg.isEmpty() && doc->pdfOptions().openAfterExport && !doc->pdfOptions().doMultiFile)
		{
			QString pdfViewer(PrefsManager::instance()->appPrefs.extToolPrefs.pdfViewerExecutable);
			QFileInfo fi(pdfViewer);
			if (pdfViewer.isEmpty())
			{
				pdfViewer = QFileDialog::getOpenFileName(this, tr("Locate your PDF viewer"), QString::null, QString::null);
				if (!QFileInfo(pdfViewer).exists())
					pdfViewer="";
				PrefsManager::instance()->appPrefs.extToolPrefs.pdfViewerExecutable=pdfViewer;
			}
			if (!pdfViewer.isEmpty())
			{
				QStringList args;
				args << QDir::toNativeSeparators(doc->pdfOptions().fileName);
				System(pdfViewer, args);
			}
		}
	}
}

//CB-->Doc, stop _storing_ bookmarks in the palette
void ScribusMainWindow::AddBookMark(PageItem *ite)
{
	bookmarkPalette->BView->AddPageItem(ite);
}

//CB-->Doc, stop _storing_ bookmarks in the palette
void ScribusMainWindow::DelBookMark(PageItem *ite)
{
	bookmarkPalette->BView->DeleteItem(ite);
}

//CB-->Doc, stop _storing_ bookmarks in the palette
void ScribusMainWindow::BookMarkTxT(PageItem *ite)
{
	bookmarkPalette->BView->ChangeText(ite);
}

//CB-->Doc, stop _storing_ bookmarks in the palette
void ScribusMainWindow::RestoreBookMarks()
{
	QList<ScribusDoc::BookMa>::Iterator it2 = doc->BookMarks.begin();
	bookmarkPalette->BView->clear();
	bookmarkPalette->BView->NrItems = 0;
	bookmarkPalette->BView->First = 1;
	bookmarkPalette->BView->Last = 0;
	if (doc->BookMarks.count() == 0)
		return;
	BookMItem* ip;
	BookMItem* ip2 = NULL;
	BookMItem* ip3 = NULL;
	BookMItem *ite = new BookMItem(bookmarkPalette->BView, &(*it2));
	bookmarkPalette->BView->NrItems++;
	++it2;
	for( ; it2 != doc->BookMarks.end(); ++it2 )
	{
		if ((*it2).Parent == 0)
		{
			ite = new BookMItem(bookmarkPalette->BView, ite, &(*it2));
			bookmarkPalette->BView->NrItems++;
		}
		else
		{
			QTreeWidgetItemIterator it3(bookmarkPalette->BView);
			while (*it3)
			{
				ip = (BookMItem*)(*it3);
				if ((*it2).Parent == ip->ItemNr)
				{
					ip2 = ip;
					break;
				}
				++it3;
			}
			if ((*it2).Prev == 0)
			{
				(void) new BookMItem(ip2, &(*it2));
				bookmarkPalette->BView->NrItems++;
			}
			else
			{
				QTreeWidgetItemIterator it4(bookmarkPalette->BView);
				while (*it4)
				{
					ip = (BookMItem*)(*it4);
					if ((*it2).Prev == ip->ItemNr)
					{
						ip3 = ip;
						break;
					}
					++it4;
				}
				(void) new BookMItem(ip2, ip3, &(*it2));
				bookmarkPalette->BView->NrItems++;
			}
		}
	}
	bookmarkPalette->BView->Last = bookmarkPalette->BView->NrItems;
	bookmarkPalette->BView->rebuildTree();
}

//CB-->Doc, stop _storing_ bookmarks in the palette
void ScribusMainWindow::StoreBookmarks()
{
	doc->BookMarks.clear();
	BookMItem* ip;
	QTreeWidgetItemIterator it(bookmarkPalette->BView);
	struct ScribusDoc::BookMa Boma;
	while (*it)
	{
		ip = (BookMItem*)(*it);
		Boma.Title = ip->Title;
		Boma.Text = ip->text(0);
		Boma.Aktion = ip->Action;
		Boma.ItemNr = ip->ItemNr;
		Boma.PageObject = ip->PageObject;
//		Boma.Seite = ip->Seite;
// 		Boma.Element = ip->Element;
		Boma.Parent = ip->Pare;
		Boma.First = ip->First;
		Boma.Prev = ip->Prev;
		Boma.Next = ip->Next;
		Boma.Last = ip->Last;
		doc->BookMarks.append(Boma);
		++it;
	}
	doc->NrItems = bookmarkPalette->BView->NrItems;
	doc->First = bookmarkPalette->BView->First;
	doc->Last = bookmarkPalette->BView->Last;
}

void ScribusMainWindow::slotElemRead(QString xml, double x, double y, bool art, bool loca, ScribusDoc* docc, ScribusView* vie)
{
	if (doc == docc && docc->appMode == modeEditClip)
		view->requestMode(submodeEndNodeEdit);

	ScriXmlDoc ss;
	if(ss.ReadElem(xml, prefsManager->appPrefs.fontPrefs.AvailFonts, docc, x, y, art, loca, prefsManager->appPrefs.fontPrefs.GFontSub))
	{
		vie->DrawNew();
		if (doc == docc)
		{
			emit UpdateRequest(reqColorsUpdate | reqTextStylesUpdate | reqLineStylesUpdate);
			slotDocCh();
		}
	}
}

void ScribusMainWindow::slotChangeUnit(int unitIndex, bool draw)
{
	// Hack
	QByteArray stylesheet;
	if (loadRawText(ScPaths::getApplicationDataDir() + "/stylesheet.css", stylesheet))
	{
		qApp->setStyleSheet(QString(stylesheet));
	}

	doc->setUnitIndex(unitIndex);
	setCurrentComboItem(view->unitSwitcher, unitGetStrFromIndex(doc->unitIndex()));
	propertiesPalette->unitChange();
	nodePalette->unitChange();
	alignDistributePalette->unitChange();
	guidePalette->setupPage();
	styleManager->unitChange();
	if (draw)
		view->DrawNew();
}

void ScribusMainWindow::ManageJava()
{
	JavaDocs *dia = new JavaDocs(this, doc, view);
	connect(dia, SIGNAL(docChanged(bool)), this, SLOT(slotDocCh(bool )));
	dia->exec();
	disconnect(dia, SIGNAL(docChanged(bool)), this, SLOT(slotDocCh(bool )));
	delete dia;
}

void ScribusMainWindow::editSelectedSymbolStart()
{
	if (doc->m_Selection->count() > 0)
		editSymbolStart(doc->m_Selection->itemAt(0)->pattern());
}

void ScribusMainWindow::editSymbolStart(QString temp)
{
	if (HaveDoc)
	{
		view->Deselect(true);
		storedPageNum = doc->currentPageNumber();
		storedViewXCoor = view->contentsX();
		storedViewYCoor = view->contentsY();
		storedViewScale = view->scale();
		view->showSymbolPage(temp);
		scrActions["pageInsert"]->setEnabled(false);
		scrActions["pageImport"]->setEnabled(false);
		scrActions["pageDelete"]->setEnabled(false);
		scrActions["pageCopy"]->setEnabled(false);
		scrActions["pageMove"]->setEnabled(false);
		scrActions["pageApplyMasterPage"]->setEnabled(false);
		scrActions["pageCopyToMasterPage"]->setEnabled(false);
		scrActions["editMasterPages"]->setEnabled(false);
		scrActions["fileNew"]->setEnabled(false);
		scrActions["fileNewFromTemplate"]->setEnabled(false);
		scrActions["fileOpen"]->setEnabled(false);
		scrActions["fileSave"]->setEnabled(false);
		scrActions["fileClose"]->setToolTip( tr("Click here to leave symbol edit mode."));
		scrMenuMgr->setMenuEnabled("FileOpenRecent", false);
		scrActions["fileRevert"]->setEnabled(false);
		scrActions["fileDocSetup150"]->setEnabled(false);
		scrActions["filePrint"]->setEnabled(false);
		scrActions["fileCollect"]->setEnabled(false);
		scrActions["fileSaveAs"]->setEnabled(false);
		scrMenuMgr->setMenuEnabled("FileExport", false);
		scrActions["fileExportAsEPS"]->setEnabled(false);
		scrActions["fileExportAsPDF"]->setEnabled(false);
		scrActions["PrintPreview"]->setEnabled(false);
		scrActions["toolsPDFPushButton"]->setEnabled(false);
		scrActions["toolsPDFTextField"]->setEnabled(false);
		scrActions["toolsPDFCheckBox"]->setEnabled(false);
		scrActions["toolsPDFComboBox"]->setEnabled(false);
		scrActions["toolsPDFListBox"]->setEnabled(false);
		scrActions["toolsPDFAnnotText"]->setEnabled(false);
#ifdef HAVE_OSG
		scrActions["toolsPDFAnnot3D"]->setEnabled(false);
#endif
		pagePalette->enablePalette(false);
		layerPalette->setEnabled(false);
		patternsDependingOnThis.clear();
		QStringList mainPatterns = doc->docPatterns.keys();
		for (int a = 0; a < mainPatterns.count(); a++)
		{
			if (mainPatterns[a] != temp)
			{
				QStringList subPatterns;
				subPatterns = doc->getUsedPatternsHelper(mainPatterns[a], subPatterns);
				if (subPatterns.contains(temp))
					patternsDependingOnThis.prepend(mainPatterns[a]);
			}
		}
		patternsDependingOnThis.prepend(temp);
		symbolPalette->editingStart(patternsDependingOnThis);
		propertiesPalette->Cpal->hideEditedPatterns(patternsDependingOnThis);
		propertiesPalette->Tpal->hideEditedPatterns(patternsDependingOnThis);
		if (outlinePalette->isVisible())
			outlinePalette->BuildTree(false);
		updateActiveWindowCaption( tr("Editing Symbol: %1").arg(temp));
	}
}

void ScribusMainWindow::editSymbolEnd()
{
	view->hideSymbolPage();
	slotSelect();
	scrActions["editMasterPages"]->setEnabled(true);
	scrActions["fileNew"]->setEnabled(true);
	scrActions["fileNewFromTemplate"]->setEnabled(true);
	scrActions["fileOpen"]->setEnabled(true);
	scrActions["fileClose"]->setEnabled(true);
	scrActions["fileClose"]->setToolTip( tr("Close"));
	scrActions["fileSave"]->setEnabled(true);
	scrMenuMgr->setMenuEnabled("FileOpenRecent", true);
	scrActions["fileRevert"]->setEnabled(true);
	scrActions["fileDocSetup150"]->setEnabled(true);
	scrActions["filePrint"]->setEnabled(true);
	scrActions["fileCollect"]->setEnabled(true);
	scrActions["fileSaveAs"]->setEnabled(true);
	scrMenuMgr->setMenuEnabled("FileExport", true);
	scrActions["fileExportAsEPS"]->setEnabled(true);
	scrActions["fileExportAsPDF"]->setEnabled(true);
	if ( ScCore->haveGS() || ScCore->isWinGUI() )
		scrActions["PrintPreview"]->setEnabled(true);
	scrActions["pageInsert"]->setEnabled(true);
	scrActions["pageCopy"]->setEnabled(true);
	scrActions["pageImport"]->setEnabled(true);
	scrActions["pageApplyMasterPage"]->setEnabled(true);
	scrActions["pageCopyToMasterPage"]->setEnabled(true);
	bool setter = doc->DocPages.count() > 1 ? true : false;
	scrActions["pageDelete"]->setEnabled(setter);
	scrActions["pageMove"]->setEnabled(setter);
	scrActions["toolsPDFPushButton"]->setEnabled(true);
	scrActions["toolsPDFTextField"]->setEnabled(true);
	scrActions["toolsPDFCheckBox"]->setEnabled(true);
	scrActions["toolsPDFComboBox"]->setEnabled(true);
	scrActions["toolsPDFListBox"]->setEnabled(true);
	scrActions["toolsPDFAnnotText"]->setEnabled(true);
#ifdef HAVE_OSG
	scrActions["toolsPDFAnnot3D"]->setEnabled(true);
#endif
	pagePalette->enablePalette(true);
	pagePalette->rebuildMasters();
	view->setScale(storedViewScale);
	doc->setCurrentPage(doc->DocPages.at(storedPageNum));
	view->setContentsPos(static_cast<int>(storedViewXCoor * storedViewScale), static_cast<int>(storedViewYCoor * storedViewScale));
	view->DrawNew();
	pagePalette->Rebuild();
	propertiesPalette->updateColorList();
	symbolPalette->editingFinished();
	layerPalette->setEnabled(true);
	if (outlinePalette->isVisible())
		outlinePalette->BuildTree(false);
	updateActiveWindowCaption(doc->DocName);
}

void ScribusMainWindow::manageMasterPages(QString temp)
{
	if (HaveDoc)
	{
		view->Deselect(true);
		if (doc->masterPageMode())
		{
			ActWin->masterPagesPalette()->updateMasterPageList(temp);
			ActWin->masterPagesPalette()->selectMasterPage(temp);
		}
		else
		{
			storedPageNum = doc->currentPageNumber();
			storedViewXCoor = view->contentsX();
			storedViewYCoor = view->contentsY();
			storedViewScale = view->scale();
			MasterPagesPalette *dia = new MasterPagesPalette(this, doc, view, temp);
			//connect(dia, SIGNAL(createNew(int)), this, SLOT(slotNewMasterPage(int)));
			connect(dia, SIGNAL(removePage(int )), this, SLOT(DeletePage2(int )));
			//connect(dia, SIGNAL(loadPage(QString, int, bool)), this, SLOT(loadPage(QString, int, bool)));
			connect(dia, SIGNAL(finished()), this, SLOT(manageMasterPagesEnd()));
			scrActions["pageInsert"]->setEnabled(false);
			scrActions["pageImport"]->setEnabled(false);
			scrActions["pageDelete"]->setEnabled(false);
			scrActions["pageCopy"]->setEnabled(false);
			scrActions["pageMove"]->setEnabled(false);
			scrActions["pageApplyMasterPage"]->setEnabled(false);
			scrActions["pageCopyToMasterPage"]->setEnabled(false);
			scrActions["editMasterPages"]->setEnabled(false);
			scrActions["fileNew"]->setEnabled(false);
			scrActions["fileNewFromTemplate"]->setEnabled(false);
			scrActions["fileOpen"]->setEnabled(false);
			scrActions["fileClose"]->setEnabled(false);
			scrMenuMgr->setMenuEnabled("FileOpenRecent", false);
			scrActions["fileRevert"]->setEnabled(false);
			scrActions["fileDocSetup150"]->setEnabled(false);
			scrActions["filePrint"]->setEnabled(false);
			scrActions["PrintPreview"]->setEnabled(false);
			scrActions["toolsPDFPushButton"]->setEnabled(false);
			scrActions["toolsPDFTextField"]->setEnabled(false);
			scrActions["toolsPDFCheckBox"]->setEnabled(false);
			scrActions["toolsPDFComboBox"]->setEnabled(false);
			scrActions["toolsPDFListBox"]->setEnabled(false);
			scrActions["toolsPDFAnnotText"]->setEnabled(false);
#ifdef HAVE_OSG
			scrActions["toolsPDFAnnot3D"]->setEnabled(false);
#endif
			pagePalette->enablePalette(false);
			dia->show();
			ActWin->setMasterPagesPalette(dia);
		}
	}
}

void ScribusMainWindow::manageMasterPagesEnd()
{
	view->setScale(storedViewScale);
	view->hideMasterPage();
	slotSelect();
	scrActions["editMasterPages"]->setEnabled(true);
	scrActions["fileNew"]->setEnabled(true);
	scrActions["fileNewFromTemplate"]->setEnabled(true);
	scrActions["fileOpen"]->setEnabled(true);
	scrActions["fileClose"]->setEnabled(true);
	scrActions["fileSave"]->setEnabled(true);
	scrMenuMgr->setMenuEnabled("FileOpenRecent", true);
	scrActions["fileRevert"]->setEnabled(true);
//	scrActions["fileDocSetup"]->setEnabled(true);
	scrActions["fileDocSetup150"]->setEnabled(true);
	scrActions["filePrint"]->setEnabled(true);
	if ( ScCore->haveGS() || ScCore->isWinGUI() )
		scrActions["PrintPreview"]->setEnabled(true);
	scrActions["pageInsert"]->setEnabled(true);
	scrActions["pageCopy"]->setEnabled(true);
	scrActions["pageImport"]->setEnabled(true);
	scrActions["pageApplyMasterPage"]->setEnabled(true);
	scrActions["pageCopyToMasterPage"]->setEnabled(true);
	bool setter = doc->DocPages.count() > 1 ? true : false;
	scrActions["pageDelete"]->setEnabled(setter);
	scrActions["pageMove"]->setEnabled(setter);
	scrActions["toolsPDFPushButton"]->setEnabled(true);
	scrActions["toolsPDFTextField"]->setEnabled(true);
	scrActions["toolsPDFCheckBox"]->setEnabled(true);
	scrActions["toolsPDFComboBox"]->setEnabled(true);
	scrActions["toolsPDFListBox"]->setEnabled(true);
	scrActions["toolsPDFAnnotText"]->setEnabled(true);
#ifdef HAVE_OSG
	scrActions["toolsPDFAnnot3D"]->setEnabled(true);
#endif
	uint pageCount=doc->DocPages.count();
	for (uint c=0; c<pageCount; ++c)
		Apply_MasterPage(doc->DocPages.at(c)->MPageNam, c, false);
//	doc->setMasterPageMode(false);
	pagePalette->enablePalette(true);
	pagePalette->rebuildMasters();
	ActWin->setMasterPagesPalette(NULL);
	doc->setCurrentPage(doc->DocPages.at(storedPageNum));
	view->reformPages(false);
	view->setContentsPos(static_cast<int>(storedViewXCoor * storedViewScale), static_cast<int>(storedViewYCoor * storedViewScale));
	view->DrawNew();
	pagePalette->Rebuild();
//	if (outlinePalette->isVisible())
//		outlinePalette->BuildTree();
//	slotDocCh();
}

void ScribusMainWindow::ApplyMasterPage()
{
	Q_ASSERT(!doc->masterPageMode());
	ApplyMasterPageDialog *dia = new ApplyMasterPageDialog(this);
	dia->setup(doc, doc->currentPage()->MPageNam);
	if (dia->exec())
	{
		QString masterPageName = dia->getMasterPageName();
		int pageSelection = dia->getPageSelection(); //0=current, 1=even, 2=odd, 3=all
		if (pageSelection==0) //current page only
			Apply_MasterPage(masterPageName, doc->currentPage()->pageNr(), false);
		else
		{
			int startPage, endPage;
			if (dia->usingRange())
			{
				startPage=dia->getFromPage()-1; //Pages start from 0, not 1
				endPage=dia->getToPage();
			}
			else
			{
				startPage = pageSelection==1 ? 1 : 0; //if even, startPage is 1 (real page 2)
				endPage=doc->DocPages.count();
			}
			for (int pageNum = startPage; pageNum < endPage; ++pageNum)// +=pageStep)
			{
				//Increment by 1 and not 2 even for even/odd application as user
				//can select to eg apply to even pages with a single odd page selected
				if (pageSelection==1 && pageNum%2!=0) //Even, %2!=0 as 1st page is numbered 0
					Apply_MasterPage(masterPageName, pageNum, false);
				else
				if (pageSelection==2 && pageNum%2==0) //Odd, %2==0 as 1st page is numbered 0
					Apply_MasterPage(masterPageName, pageNum, false);
				else
				if (pageSelection==3) //All
					Apply_MasterPage(masterPageName, pageNum, false);
			}
		}
	}
	view->reformPages();
	view->DrawNew();
	//CB done by doc::reformpages
	//slotDocCh();
	pagePalette->Rebuild();
	// #9476 : call setupPage with false arg to setup only guidePalette GUI
	// Otherwise setupPage() will apply guides to current page, doesn't need that, 
	// Apply_MasterPage() has already done it
	guidePalette->setupPage(false);
	delete dia;
}

void ScribusMainWindow::Apply_MasterPage(QString pageName, int pageNumber, bool reb)
{
	if (!HaveDoc)
		return;
	doc->applyMasterPage(pageName, pageNumber);
	if (reb)
	{
		view->DrawNew();
//		pagePalette->Rebuild();
	}
}

//CB-->Doc
void ScribusMainWindow::GroupObj(bool showLockDia)
{
	if (HaveDoc)
	{
		Selection* itemSelection = doc->m_Selection;
		if (itemSelection->count() < 2)
			return;
		bool lockObject = false;
		bool modifyLock = false;
		uint selectedItemCount=itemSelection->count();
		if (showLockDia)
		{
			uint lockedCount=0;
			for (uint a=0; a<selectedItemCount; ++a)
			{
				if (itemSelection->itemAt(a)->locked())
					++lockedCount;
			}
			if (lockedCount!=0 && lockedCount!=selectedItemCount)
			{
				QMessageBox msgBox;
				QPushButton *abortButton = msgBox.addButton(QMessageBox::Cancel);
				QPushButton *lockButton = msgBox.addButton(tr("&Lock All"), QMessageBox::AcceptRole);
				QPushButton *unlockButton = msgBox.addButton(tr("&Unlock All"), QMessageBox::AcceptRole);
				msgBox.setIcon(QMessageBox::Warning);
				msgBox.setWindowTitle(CommonStrings::trWarning);
				msgBox.setText( tr("Some objects are locked."));
				msgBox.exec();
				if (msgBox.clickedButton() == abortButton)
					return;
				else if (msgBox.clickedButton() == lockButton)
					lockObject = true;
				modifyLock = true;
				unlockButton = NULL;	// just to silence the compiler
			}
		}
		doc->itemSelection_GroupObjects(modifyLock, lockObject);
	}
}

//CB-->Doc
void ScribusMainWindow::UnGroupObj()
{
	if (HaveDoc)
		doc->itemSelection_UnGroupObjects();
}

void ScribusMainWindow::restore(UndoState* state, bool isUndo)
{
	SimpleState *ss = dynamic_cast<SimpleState*>(state);
	if (ss)
	{
		if (ss->contains("ADD_PAGE"))
			restoreAddPage(ss, isUndo);
		else if (ss->contains("DELETE_PAGE"))
			restoreDeletePage(ss, isUndo);
	}
}

void ScribusMainWindow::restoreDeletePage(SimpleState *state, bool isUndo)
{
	int where, wo;
	int pagenr = state->getUInt("PAGENR");
	QStringList tmpl;
	tmpl << state->get("MASTERPAGE");
	QString pageName = state->get("PAGENAME");
	bool oldPageMode = doc->masterPageMode();
	if (!pageName.isEmpty() && !oldPageMode) // We try do undo a master page deletion in standard mode
		doc->setMasterPageMode(true);
	if (pagenr == 1)
	{
		where = 0;
		wo = 1;
	}
	else if (pagenr > doc->Pages->count())
	{
		where = 2;
		wo = doc->Pages->count();
	}
	else
	{
		where = 1;
		wo = pagenr - 1;
	}
	if (isUndo)
	{
		if (doc->masterPageMode())
		{
			slotNewMasterPage(wo, pageName);
		}
		else
		{
			addNewPages(wo, where, 1, doc->pageHeight(), doc->pageWidth(), doc->pageOrientation(), doc->pageSize(), true, &tmpl);
		}
		UndoObject *tmp =
			undoManager->replaceObject(state->getUInt("DUMMY_ID"), doc->Pages->at(pagenr - 1));
		delete tmp;
	}
	else
	{
		DummyUndoObject *duo = new DummyUndoObject();
		uint id = static_cast<uint>(duo->getUId());
		undoManager->replaceObject(doc->Pages->at(pagenr - 1)->getUId(), duo);
		state->set("DUMMY_ID", id);
		DeletePage(pagenr, pagenr);
	}
	if (!pageName.isEmpty() && !oldPageMode)
	{
		doc->setMasterPageMode(oldPageMode);
		doc->rebuildMasterNames();
		pagePalette->rebuildMasters();
	}
	if (doc->masterPageMode() && !pageName.isEmpty())
		ActWin->masterPagesPalette()->updateMasterPageList();
	pagePalette->rebuildPages();
}

void ScribusMainWindow::restoreAddPage(SimpleState *state, bool isUndo)
{
	int wo    = state->getInt("PAGE");
	int where = state->getInt("WHERE");
	int count = state->getInt("COUNT");
	QStringList based = state->get("BASED").split("|", QString::SkipEmptyParts);
	double height = state->getDouble("HEIGHT");
	double width = state->getDouble("WIDTH");
	int orient = state->getInt("ORIENT");
	QString siz = state->get("SIZE");
	bool mov = static_cast<bool>(state->getInt("MOVED"));

	int delFrom = 0;
	int delTo = 0;
	switch (where)
	{
		case 0:
			delTo = wo + count - 1;
			delFrom = delTo - count + 1;
			break;
		case 1:
			delFrom = wo + 1;
			delTo = wo + count;
			break;
		case 2:
			delTo = doc->Pages->count();
			delFrom = doc->Pages->count() - count + 1;
			if (!isUndo)
			{
				delFrom += count;
				delTo   += count;
			}
			break;
	}
	if (isUndo)
	{
		for (int i = delFrom - 1; i < delTo; ++i)
		{
			DummyUndoObject *duo = new DummyUndoObject();
			ulong did = duo->getUId();
			undoManager->replaceObject(doc->Pages->at(i)->getUId(), duo);
			state->set(QString("Page%1").arg(i), static_cast<uint>(did));
		}
		if (HaveDoc && doc->appMode == modeEditClip)
			view->requestMode(submodeEndNodeEdit);
		view->Deselect(true);
		DeletePage(delFrom, delTo);
	}
	else
	{
		if (doc->masterPageMode())
		{
			assert (count == 1);
			slotNewMasterPage(wo, based[0]);
		}
		else
		{
			addNewPages(wo, where, count, height, width, orient, siz, mov, &based);
		}
		for (int i = delFrom - 1; i < delTo; ++i)
		{
			UndoObject *tmp = undoManager->replaceObject(
					state->getUInt(QString("Page%1").arg(i)), doc->Pages->at(i));
			delete tmp;
		}
	}
}

void ScribusMainWindow::restoreGrouping(SimpleState *state, bool isUndo)
{
	int itemCount = state->getInt("itemcount");
	view->Deselect();
	for (int i = 0; i < itemCount; ++i)
	{
		int itemNr = doc->getItemNrfromUniqueID(state->getUInt(QString("item%1").arg(i)));
		if (doc->Items->at(itemNr)->uniqueNr == state->getUInt(QString("item%1").arg(i)))
			view->SelectItemNr(itemNr);
	}
	if (isUndo)
		UnGroupObj();
	else
		GroupObj(false);
}

void ScribusMainWindow::restoreUngrouping(SimpleState *state, bool isUndo)
{
	int itemCount = state->getInt("itemcount");
	view->Deselect();
	for (int i = 0; i < itemCount; ++i)
	{
		int itemNr = doc->getItemNrfromUniqueID(state->getUInt(QString("item%1").arg(i)));
		if (doc->Items->at(itemNr)->uniqueNr == state->getUInt(QString("item%1").arg(i)))
		{
			if (isUndo)
				doc->Items->at(itemNr)->isTableItem = static_cast<bool>(state->getInt(QString("tableitem%1").arg(i)));
			view->SelectItemNr(itemNr);
		}
	}
	if (isUndo)
		GroupObj(false);
	else
		UnGroupObj();
}

void ScribusMainWindow::StatusPic()
{
	if (HaveDoc)
	{
		PicStatus *dia = new PicStatus(this, doc);
		connect(dia, SIGNAL(selectPage(int)), this, SLOT(selectPagesFromOutlines(int)));
		connect(dia, SIGNAL(selectMasterPage(QString)), this, SLOT(manageMasterPages(QString)));
		connect(dia, SIGNAL(selectElementByItem(PageItem *, bool)), this, SLOT(selectItemsFromOutlines(PageItem *, bool)));
//		connect(dia, SIGNAL(refreshItem(PageItem*)), view, SLOT(RefreshItem(PageItem*)));
		dia->exec();
		delete dia;
	}
}

QString ScribusMainWindow::CFileDialog(QString wDir, QString caption, QString filter, QString defNa,
                                int optionFlags, bool *docom, bool *doFont, bool *doProfiles)
{
	QString retval("");
	// changed from "this" to qApp->activeWindow() to be sure it will be opened
	// with the current active window as parent. E.g. it won't hide StoryEditor etc. -- PV
	CustomFDialog *dia = new CustomFDialog(qApp->activeWindow(), wDir, caption, filter, optionFlags);
	if (!defNa.isEmpty())
	{
		QFileInfo f(defNa);
		dia->setExtension(f.completeSuffix());
		dia->setZipExtension(f.completeSuffix() + ".gz");
		dia->setSelection(defNa);
		if (docom != NULL)
			dia->SaveZip->setChecked(*docom);
	}
	if (optionFlags & fdDirectoriesOnly)
	{
		if (docom != NULL)
			dia->SaveZip->setChecked(*docom);
		if (doFont != NULL)
			dia->WithFonts->setChecked(*doFont);
		if (doProfiles != NULL)
			dia->WithProfiles->setChecked(*doProfiles);
	}
	if (dia->exec() == QDialog::Accepted)
	{
		LoadEnc = "";
		if (!(optionFlags & fdDirectoriesOnly))
		{
			LoadEnc = (optionFlags & fdShowCodecs) ? dia->TxCodeM->currentText() : QString("");
			if (optionFlags & fdCompressFile)
			{
				if (dia->SaveZip->isChecked())
					dia->handleCompress();
			}
		}
		else
		{
			if (docom != NULL)
				*docom = dia->SaveZip->isChecked();
			if (doFont != NULL)
				*doFont = dia->WithFonts->isChecked();
			if (doProfiles != NULL)
				*doProfiles = dia->WithProfiles->isChecked();
		}
		this->repaint();
		retval = dia->selectedFile();
		qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
	}
	delete dia;
	return retval;
}



void ScribusMainWindow::recalcColors(QProgressBar *dia)
{
	if (HaveDoc)
	{
		doc->recalculateColors();
		propertiesPalette->updateColorList();
	}
}

void ScribusMainWindow::ModifyAnnot()
{
//	Q_ASSERT(!doc->masterPageMode());
	if (doc->m_Selection->count() != 0)
	{
		PageItem *currItem = doc->m_Selection->itemAt(0);
		if ((currItem->annotation().Type() == 0) || (currItem->annotation().Type() == 1) || (currItem->annotation().Type() > 9))
		{
			int AnType = currItem->annotation().Type();
			int AnActType = currItem->annotation().ActionType();
			QString AnAction = currItem->annotation().Action();
			QString An_Extern = currItem->annotation().Extern();
			Annota *dia = new Annota(this, currItem, doc->DocPages.count(), static_cast<int>(doc->pageWidth()), static_cast<int>(doc->pageHeight()), view);
			if (dia->exec())
				slotDocCh();
			else
			{
				currItem->annotation().setType(AnType);
				currItem->annotation().setActionType(AnActType);
				currItem->annotation().setAction(AnAction);
				currItem->annotation().setExtern(An_Extern);
			}
			delete dia;
		}
		else
		{
			ScAnnot *dia = new ScAnnot(this, currItem, doc->DocPages.count(), static_cast<int>(doc->pageWidth()), static_cast<int>(doc->pageHeight()), doc->PageColors, view);
			if (dia->exec())
				slotDocCh();
			delete dia;
		}
		currItem->update();
	}
}

void ScribusMainWindow::SetShortCut()
{
	for (QMap<QString,Keys>::Iterator it = prefsManager->appPrefs.keyShortcutPrefs.KeyActions.begin(); it != prefsManager->appPrefs.keyShortcutPrefs.KeyActions.end(); ++it )
	{
		if (!it.value().actionName.isEmpty())
			if (scrActions[it.value().actionName])
				scrActions[it.value().actionName]->setShortcut(it.value().keySequence);
	}
}

void ScribusMainWindow::PutScrap(int scID)
{
	ScriXmlDoc ss;
	QString objectString = ss.WriteElem(doc, doc->m_Selection);
	QDomDocument docu("scridoc");
	docu.setContent(objectString);
	QDomElement elem = docu.documentElement();
	QDomNode DOC = elem.firstChild();
	bool first = true;
	DOC = elem.firstChild();
	while(!DOC.isNull())
	{
		QDomElement pg = DOC.toElement();
		if(pg.tagName() == "ITEM")
		{
			if (first)
				pg.setAttribute("ANNAME", doc->m_Selection->itemAt(0)->itemName());
			first = false;
		}
		DOC = DOC.nextSibling();
	}
	objectString = docu.toString();
	scrapbookPalette->ObjFromMainMenu(objectString, scID);
}

void ScribusMainWindow::changeLayer(int )
{
	if (doc->appMode == modeEdit)
		slotSelect();
	else if (doc->appMode == modeEditClip)
		NoFrameEdit();
	view->Deselect(true);
	rebuildLayersList();
	view->updateLayerMenu();
	view->setLayerMenuText(doc->activeLayerName());
	view->DrawNew();
	bool setter = !doc->layerLocked( doc->activeLayer() );
	scrActions["editPaste"]->setEnabled((ScMimeData::clipboardHasScribusData() || (scrapbookPalette->tempBView->objectMap.count() > 0)) && (setter));
	scrMenuMgr->setMenuEnabled("EditPasteRecent", ((scrapbookPalette->tempBView->objectMap.count() > 0) && (setter)));
	scrActions["editSelectAll"]->setEnabled(setter);
	scrActions["editSelectAllOnLayer"]->setEnabled(setter);
	scrActions["editDeselectAll"]->setEnabled(false);
	scrMenuMgr->setMenuEnabled("Insert", setter);
	scrActions["insertFrame"]->setEnabled(setter);
	scrActions["toolsSelect"]->setEnabled(setter);
	scrActions["toolsInsertTextFrame"]->setEnabled(setter);
	scrActions["toolsInsertImageFrame"]->setEnabled(setter);
	scrActions["toolsInsertTableFrame"]->setEnabled(setter);
	scrActions["toolsInsertShape"]->setEnabled(setter);
	scrActions["toolsInsertLine"]->setEnabled(setter);
	scrActions["toolsInsertBezier"]->setEnabled(setter);
	scrActions["toolsInsertFreehandLine"]->setEnabled(setter);
	scrActions["toolsInsertCalligraphicLine"]->setEnabled(setter);
	scrActions["toolsInsertPolygon"]->setEnabled(setter);
	scrActions["toolsInsertArc"]->setEnabled(setter);
	scrActions["toolsInsertSpiral"]->setEnabled(setter);
	scrActions["toolsInsertRenderFrame"]->setEnabled(setter);
	if (doc->masterPageMode())
	{
		scrActions["toolsPDFPushButton"]->setEnabled(false);
		scrActions["toolsPDFTextField"]->setEnabled(false);
		scrActions["toolsPDFCheckBox"]->setEnabled(false);
		scrActions["toolsPDFComboBox"]->setEnabled(false);
		scrActions["toolsPDFListBox"]->setEnabled(false);
		scrActions["toolsPDFAnnotText"]->setEnabled(false);
#ifdef HAVE_OSG
		scrActions["toolsPDFAnnot3D"]->setEnabled(false);
#endif
	}
	else
	{
		scrActions["toolsPDFPushButton"]->setEnabled(setter);
		scrActions["toolsPDFTextField"]->setEnabled(setter);
		scrActions["toolsPDFCheckBox"]->setEnabled(setter);
		scrActions["toolsPDFComboBox"]->setEnabled(setter);
		scrActions["toolsPDFListBox"]->setEnabled(setter);
		scrActions["toolsPDFAnnotText"]->setEnabled(setter);
#ifdef HAVE_OSG
		scrActions["toolsPDFAnnot3D"]->setEnabled(setter);
#endif
	}
	scrActions["toolsPDFAnnotLink"]->setEnabled(setter);
	scrMenuMgr->setMenuEnabled("ItemLayer", doc->layerCount() > 1);
}

void ScribusMainWindow::showLayer()
{
	view->DrawNew();
}

void ScribusMainWindow::UnDoAction()
{
	undoManager->undo(1);
}

void ScribusMainWindow::RedoAction()
{
	undoManager->redo(1);
}

void ScribusMainWindow::initHyphenator()
{
	InstLang.clear();
	//Build our list of hyphenation dictionaries we have in the install dir
	//Grab the language abbreviation from it, get the full language text
	//Insert the name as key and a new string list into the map
	QString hyphDirName = QDir::toNativeSeparators(ScPaths::instance().dictDir());
	QDir hyphDir(hyphDirName, "*.dic", QDir::Name, QDir::Files | QDir::NoSymLinks);
	if ((hyphDir.exists()) && (hyphDir.count() != 0))
	{
// 		LanguageManager langmgr;
// 		langmgr.init(false);
		QString languageOfHyphFile;
		for (uint dc = 0; dc < hyphDir.count(); ++dc)
		{
			QFileInfo fi(hyphDir[dc]);
			QString fileLangAbbrev=fi.baseName().section('_', 1);
			languageOfHyphFile = LanguageManager::instance()->getLangFromAbbrev(fileLangAbbrev, false);
			InstLang.insert(languageOfHyphFile, QStringList());
		}
	}

	//For each qm file existing, load the file and find the translations of the names
	QString pfad = ScPaths::instance().translationDir();
	QDir d2(pfad, "*.*", QDir::Name, QDir::Files | QDir::NoSymLinks);
	if ((d2.exists()) && (d2.count() != 0))
	{
		for (uint dc = 0; dc < d2.count(); dc++)
		{
			QFileInfo fi(pfad + d2[dc]);
			QString ext = fi.suffix().toLower();
			if (ext == "qm")
			{
    			QTranslator *trans = new QTranslator(0);
				trans->load(pfad + d2[dc]);

				QString translatedLang;
				for (QMap<QString, QStringList>::Iterator it=InstLang.begin(); it!=InstLang.end(); ++it)
				{
					translatedLang="";
					translatedLang = trans->translate("QObject", it.key().toLocal8Bit().data(), "");
					if (!translatedLang.isEmpty())
						it.value().append(translatedLang);
				}
				delete trans;
			}
		}
	}
	//For each hyphenation file, grab the strings and the hyphenation data.
	QString lang = QString(QLocale::system().name()).left(2);
	LangTransl.clear();
	prefsManager->appPrefs.hyphPrefs.Language = "English";
	if ((hyphDir.exists()) && (hyphDir.count() != 0))
	{
		LanguageManager *langmgr(LanguageManager::instance());
// 		langmgr.init(false);
		QString datein = "";
		for (uint dc = 0; dc < hyphDir.count(); ++dc)
		{
			QFileInfo fi(hyphDir[dc]);
			QString fileLangAbbrev=fi.baseName().section('_', 1);
			datein = langmgr->getLangFromAbbrev(fileLangAbbrev);
			QString tDatein = datein;
			datein = GetLang(datein);
			LangTransl.insert(datein, tDatein);
			langmgr->addHyphLang(fileLangAbbrev, hyphDir[dc]);
// 			Sprachen.insert(datein, hyphDir[dc]);
			if (fileLangAbbrev == lang)
				prefsManager->appPrefs.hyphPrefs.Language = datein;
		}
		if (datein.isEmpty())
			prefsManager->appPrefs.hyphPrefs.Language = "English";
	}
//	propertiesPalette->fillLangCombo(LangTransl);
}

QString ScribusMainWindow::GetLang(QString inLang)
{
	QMap<QString, QStringList>::Iterator itlend=InstLang.end();
 	for (QMap<QString, QStringList>::Iterator itl = InstLang.begin(); itl != itlend; ++itl)
	{
		if (itl.value().contains(inLang))
			return itl.key();
	}
	return inLang;
}

void ScribusMainWindow::ImageEffects()
{
	if (HaveDoc)
	{
		if (doc->m_Selection->count() != 0)
		{
			PageItem *currItem = doc->m_Selection->itemAt(0);
			EffectsDialog* dia = new EffectsDialog(this, currItem, doc);
			if (dia->exec())
				doc->itemSelection_ApplyImageEffects(dia->effectsList);
			delete dia;
		}
	}
}

QString ScribusMainWindow::fileCollect(bool compress, bool withFonts, const bool withProfiles, const QString& )
{
	CollectForOutput_UI c(this, doc, QString::null, withFonts, withProfiles, compress);
	QString newFileName;
	QString errorMsg=c.collect(newFileName);
	return newFileName;
}

void ScribusMainWindow::docCheckToggle(bool visible)
{
	if (!visible)
	{
		if (docCheckerPalette->checkMode == CheckDocument::checkPDF)
			disconnect(docCheckerPalette, SIGNAL(ignoreAllErrors()), this, SLOT(doSaveAsPDF()));
		if (docCheckerPalette->checkMode == CheckDocument::checkEPS)
			disconnect(docCheckerPalette, SIGNAL(ignoreAllErrors()), this, SLOT(reallySaveAsEps()));
		if (docCheckerPalette->checkMode == CheckDocument::checkPrint)
			disconnect(docCheckerPalette, SIGNAL(ignoreAllErrors()), this, SLOT(slotReallyPrint()));
		if (docCheckerPalette->checkMode == CheckDocument::checkPrintPreview)
			disconnect(docCheckerPalette, SIGNAL(ignoreAllErrors()), this, SLOT(doPrintPreview()));
		docCheckerPalette->setIgnoreEnabled(false);
		docCheckerPalette->checkMode = CheckDocument::checkNULL;
	}
	else
	{
		if (HaveDoc)
		{
			scanDocument();
			docCheckerPalette->buildErrorList(doc);
		}
	}
}

bool ScribusMainWindow::scanDocument()
{
	return DocumentChecker().checkDocument(doc);
}

void ScribusMainWindow::slotStoryEditor()
{
	if (doc->m_Selection->count() != 0)
	{
		PageItem *currItem = doc->m_Selection->itemAt(0);
		PageItem *currItemSE=storyEditor->currentItem();
		ScribusDoc *currDocSE=storyEditor->currentDocument();
		storyEditor->activFromApp = true;
		//CB shouldnt these be after the if?
		//Why are we resetting the doc and item in this case. My original code didnt do this.
		storyEditor->setCurrentDocumentAndItem(doc, currItem);
		if (currItem==currItemSE && doc==currDocSE)
		{
			storyEditor->show();
			storyEditor->raise();
			return;
		}
		CurrStED = storyEditor;
		connect(storyEditor, SIGNAL(DocChanged()), this, SLOT(slotDocCh()));
		storyEditor->show();
		storyEditor->raise();
	}
}

void ScribusMainWindow::emergencySave()
{
	emergencyActivated=true;
	std::cout << "Calling Emergency Save" << std::endl;
	QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
	if (!windows.isEmpty())
	{
		uint windowCount=windows.count();
		for (uint i=0; i<windowCount ; ++i)
		{
			ActWin = (ScribusWin*)windows.at(i)->widget();
			doc = ActWin->doc();
			view = ActWin->view();
			doc->setModified(false);
			if (doc->hasName)
			{
				std::cout << "Saving: " << doc->DocName.toStdString() << ".emergency" << std::endl;
				doc->autoSaveTimer->stop();
				QString emName = doc->DocName+".emergency";
				if (doc->DocName.right(2) == "gz")
					emName += ".gz";
				FileLoader fl(emName);
				fl.SaveFile(emName, doc, 0);
			}
			view->close();
			uint numPages=doc->Pages->count();
			for (uint a=0; a<numPages; ++a)
				delete doc->Pages->at(a);
			delete doc;
			ActWin->close();
		}
	}
}

void ScribusMainWindow::EditTabs()
{
	if (HaveDoc)
	{
		if (doc->m_Selection->count() != 0)
		{
			PageItem *currItem = doc->m_Selection->itemAt(0);
			TabManager *dia = new TabManager(this, doc->unitIndex(), currItem->itemText.defaultStyle().tabValues(), currItem->width());
			if (dia->exec())
			{
				ParagraphStyle newTabs(currItem->itemText.defaultStyle());
				newTabs.setTabValues(dia->tmpTab);
				currItem->itemText.setDefaultStyle(newTabs);
				currItem->update();
				slotDocCh();
			}
			delete dia;
		}
	}
}

void ScribusMainWindow::SearchText()
{
	PageItem *currItem = doc->m_Selection->itemAt(0);
	view->requestMode(modeEdit);
	currItem->itemText.setCursorPosition(0);
	SearchReplace* dia = new SearchReplace(this, doc, currItem);
	connect(dia, SIGNAL(NewFont(const QString&)), this, SLOT(SetNewFont(const QString&)));
	connect(dia, SIGNAL(NewAbs(int)), this, SLOT(setAlignmentValue(int)));
	dia->exec();
	disconnect(dia, SIGNAL(NewFont(const QString&)), this, SLOT(SetNewFont(const QString&)));
	disconnect(dia, SIGNAL(NewAbs(int)), this, SLOT(setAlignmentValue(int)));
	delete dia;
	slotSelect();
}

void ScribusMainWindow::imageEditorExited(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
	if ( ExternalApp != 0 )
	{
		delete ExternalApp;
		ExternalApp = 0;
	}
}

/* call gimp and wait upon completion */
void ScribusMainWindow::callImageEditor()
{
	if (doc->m_Selection->count() != 0)
	{
		//NOTE to reviewers: I added my code to this function,
		// - as it performs a similar function,
		// - when the frame is a latex frame it makes only sense
		//   to run a latex editor
		// - IMHO ScribusMainWindow has way to many slots already
		// - my code here is short and without sideeffects
		PageItem *currItem = doc->m_Selection->itemAt(0);
		if (currItem->asLatexFrame())
		{
			currItem->asLatexFrame()->runEditor();
			return; //Don't process the functions for imageframes!
		}
#ifdef HAVE_OSG
		if (currItem->asOSGFrame())
		{
			OSGEditorDialog *dia = new OSGEditorDialog(this, currItem->asOSGFrame());
			dia->exec();
			return;
		}
#endif
		QString imageEditorExecutable=prefsManager->imageEditorExecutable();
		if (ExternalApp != 0)
		{
			QString ieExe = QDir::toNativeSeparators(imageEditorExecutable);
			QMessageBox::information(this, tr("Information"), "<qt>" + tr("The program %1 is already running!").arg(ieExe) + "</qt>", 1, 0, 0);
			return;
		}
		if (currItem->PictureIsAvailable)
		{
			int index;
			QString imEditor;
			ExternalApp = new QProcess(NULL);
			QStringList cmd;
		#if defined(_WIN32)
			index = imageEditorExecutable.indexOf( ".exe" );
			if ( index >= 0 )
				imEditor = imageEditorExecutable.left( index + 4 );
			imEditor.replace( "\\", "/" );
			if ( imEditor.length() < imageEditorExecutable.length() )
			{
				int diffLength = imageEditorExecutable.length() - imEditor.length();
				QString cmdStr = imageEditorExecutable.right( diffLength );
				QStringList cmd1 = cmdStr.split( " ", QString::SkipEmptyParts);
				cmd += cmd1;
			}
		#else
			cmd = imageEditorExecutable.split(" ", QString::SkipEmptyParts);
			if ( cmd.count() > 0 )
				imEditor = cmd[0];
			cmd.clear();
		#endif
			index = imEditor.lastIndexOf( "/" );
			if (index > -1 )
			{
				QString imEditorDir = imEditor.left( index + 1 );
				ExternalApp->setWorkingDirectory( imEditorDir );
			}
			cmd.append(QDir::toNativeSeparators(currItem->Pfile));
			ExternalApp->start(imEditor, cmd);
			if (!ExternalApp->waitForStarted())
			{
				delete ExternalApp;
				ExternalApp = 0;
				QMessageBox::critical(this, CommonStrings::trWarning, "<qt>" + tr("The program %1 is missing!").arg(imageEditorExecutable) + "</qt>", 1, 0, 0);
				return;
			}
			connect(ExternalApp, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(imageEditorExited(int, QProcess::ExitStatus)));
		}
	}
}

void ScribusMainWindow::slotCharSelect()
{
	charPalette->show();
}

void ScribusMainWindow::setUndoMode(bool isObjectSpecific)
{
	objectSpecificUndo = isObjectSpecific;

	if (!objectSpecificUndo && HaveDoc)
		undoManager->showObject(Um::GLOBAL_UNDO_MODE);
	else if (HaveDoc)
	{
		uint docSelectionCount=doc->m_Selection->count();
		if (docSelectionCount == 1)
			undoManager->showObject(doc->m_Selection->itemAt(0)->getUId());
		else if (docSelectionCount == 0)
			undoManager->showObject(doc->currentPage()->getUId());
		else
			undoManager->showObject(Um::NO_UNDO_STACK);
	}
}

bool ScribusMainWindow::isObjectSpecificUndo()
{
	return objectSpecificUndo;
}

void ScribusMainWindow::slotTest()
{
}

void ScribusMainWindow::slotTest2()
{
}

void ScribusMainWindow::getImageInfo()
{
	if ((HaveDoc) && (doc->m_Selection->count() == 1))
	{
		PageItem *pageItem = doc->m_Selection->itemAt(0);
		if (pageItem != NULL)
		{
			if (pageItem->itemType() == PageItem::ImageFrame)
			{
				ImageInfoDialog *dia = new ImageInfoDialog(this, &pageItem->pixm.imgInfo);
				dia->exec();
				delete dia;
			}
		}
	}
}

void ScribusMainWindow::objectAttributes()
{
	if ((HaveDoc) && (doc->m_Selection->count() == 1))
	{
		PageItem *pageItem = doc->m_Selection->itemAt(0);
		if (pageItem!=NULL)
		{
			PageItemAttributes *pageItemAttrs = new PageItemAttributes( this );
			pageItemAttrs->setup(pageItem->getObjectAttributes(), &doc->itemAttributes());
			//CB TODO Probably want this non modal in the future
			if (pageItemAttrs->exec()==QDialog::Accepted)
				pageItem->setObjectAttributes(pageItemAttrs->getNewAttributes());
			delete pageItemAttrs;
		}
	}
}

void ScribusMainWindow::generateTableOfContents()
{
	if (HaveDoc)
		tocGenerator->generateDefault();
}

void ScribusMainWindow::insertSampleText()
{
	LoremManager m(doc, this);
	if (prefsManager->appPrefs.miscPrefs.useStandardLI)
	{
		m.insertLoremIpsum("la.xml", prefsManager->appPrefs.miscPrefs.paragraphsLI);
		return;
	}

	if (m.exec())
		m.insertLoremIpsum(m.getName(), m.paragraphCount(), m.randomize());
}

void ScribusMainWindow::languageChange()
{
	if (ScCore->initialized())
	{
		//Update colours in case someone has a translated None colour in their preference settings
		//before changing the tr_NoneColor to the new value. See #9267, #5529
		prefsManager->languageChange();
		CommonStrings::languageChange();
		//Update actions
		if (actionManager!=NULL)
		{
			actionManager->languageChange();
			ScCore->pluginManager->languageChange();
			initKeyboardShortcuts();
		}
		//Update menu texts
		if (scrMenuMgr!=NULL && !scrMenuMgr->empty())
			scrMenuMgr->languageChange();
		if (undoManager!=NULL)
			undoManager->languageChange();

		mainWindowXPosLabel->setText( tr("X-Pos:"));
		mainWindowYPosLabel->setText( tr("Y-Pos:"));
		mainWindowXPosDataLabel->setText("         ");
		mainWindowYPosDataLabel->setText("         ");
		mainWindowStatusLabel->setText( tr("Ready"));
	}
}

void ScribusMainWindow::setDefaultPrinter(const QString& name, const QString& file, const QString& command)
{
	PDef.Pname = name;
	PDef.Dname = file;
	PDef.Command = command;
}

void ScribusMainWindow::getDefaultPrinter(QString& name, QString& file, QString& command)
{
	name=PDef.Pname;
	file=PDef.Dname;
	command=PDef.Command;
}

void ScribusMainWindow::closeActiveWindowMasterPageEditor()
{
	if (!HaveDoc)
		return;
	if(doc->masterPageMode())
	{
		ActWin->masterPagesPalette()->close();
		qApp->processEvents();
	}
}

void ScribusMainWindow::updateActiveWindowCaption(const QString &newCaption)
{
	if (!HaveDoc)
		return;
	ActWin->setWindowTitle(QDir::toNativeSeparators(newCaption));
}

void ScribusMainWindow::dragEnterEvent ( QDragEnterEvent* e)
{
	bool accepted = false;
	if ( e->mimeData()->hasFormat("text/uri-list"))
	{
		QString fileUrl;
		QList<QUrl> fileUrls = e->mimeData()->urls();
		for( int i = 0; i < fileUrls.count(); ++i )
		{
			fileUrl = fileUrls[i].toLocalFile().toLower();
			if (fileUrl.endsWith(".sla") || fileUrl.endsWith(".sla.gz") || fileUrl.endsWith(".shape") || fileUrl.endsWith(".sce"))
			{
				accepted = true;
				break;
			}
			else
			{
				QUrl url( fileUrls[i] );
				FileLoader *fileLoader = new FileLoader(url.path());
				int testResult = fileLoader->TestFile();
				delete fileLoader;
				if ((testResult != -1) && (testResult >= FORMATID_ODGIMPORT))
				{
					accepted = true;
					break;
				}
			}
		}
	}
	else if (e->mimeData()->hasText())
	{
		QString text = e->mimeData()->text();
		if ((text.startsWith("<SCRIBUSELEM")) || (text.startsWith("SCRIBUSELEMUTF8")))
			accepted = true;
	}
	if (accepted)
		e->accept();
}

void ScribusMainWindow::dropEvent ( QDropEvent * e)
{
	bool accepted = false;
	if ( e->mimeData()->hasFormat("text/uri-list"))
	{
		QString fileUrl;
		QList<QUrl> fileUrls = e->mimeData()->urls();
		for( int i = 0; i < fileUrls.count(); ++i )
		{
			fileUrl = fileUrls[i].toLocalFile().toLower();
			if ( fileUrl.endsWith(".sla") || fileUrl.endsWith(".sla.gz") )
			{
				QUrl url( fileUrls[i] );
				QFileInfo fi(url.path());
				if ( fi.exists() )
				{
					accepted = true;
					loadDoc( fi.absoluteFilePath() );
				}
			}
			else if (fileUrl.endsWith(".sce"))
			{
				QUrl url( fileUrls[i] );
				QFileInfo fi(url.path());
				if ( fi.exists() )
				{
					accepted = true;
					QString data;
					QByteArray cf;
					loadRawText(url.toLocalFile(), cf);
					data = QString::fromUtf8(cf.data());
					double gx, gy, gw, gh;
					ScriXmlDoc *ss = new ScriXmlDoc();
					if(ss->ReadElemHeader(data, false, &gx, &gy, &gw, &gh))
					{
						doFileNew(gw, gh, 0, 0, 0, 0, 0, 0, false, false, 0, false, 0, 1, "Custom", true);
						HaveNewDoc();
						doc->reformPages(true);
						slotElemRead(data, doc->currentPage()->xOffset(), doc->currentPage()->yOffset(), false, false, doc, view);
						slotDocCh(false);
						doc->regionsChanged()->update(QRectF());
						delete ss;
					}
				}
			}
			else
			{
				QUrl url( fileUrls[i] );
				FileLoader *fileLoader = new FileLoader(url.path());
				int testResult = fileLoader->TestFile();
				delete fileLoader;
				if ((testResult != -1) && (testResult >= FORMATID_ODGIMPORT))
				{
					QFileInfo fi(url.path());
					if ( fi.exists() )
					{
						accepted = true;
						loadDoc( fi.absoluteFilePath() );
					}
				}
			}
		}
	}
	else
	{
		if (e->mimeData()->hasText())
		{
			QString text = e->mimeData()->text();
			if ((text.startsWith("<SCRIBUSELEM")) || (text.startsWith("SCRIBUSELEMUTF8")))
			{
				double gx, gy, gw, gh;
				ScriXmlDoc *ss = new ScriXmlDoc();
				if(ss->ReadElemHeader(text, false, &gx, &gy, &gw, &gh))
				{
					doFileNew(gw, gh, 0, 0, 0, 0, 0, 0, false, false, 0, false, 0, 1, "Custom", true);
					HaveNewDoc();
					doc->reformPages(true);
					slotElemRead(text, doc->currentPage()->xOffset(), doc->currentPage()->yOffset(), false, false, doc, view);
					slotDocCh(false);
					doc->regionsChanged()->update(QRectF());
					delete ss;
				}
			}
		}
	}
	if (accepted)
		e->accept();
}

void ScribusMainWindow::slotEditCopyContents()
{
	PageItem *currItem=NULL;
	contentsBuffer.contentsFileName="";
	if (HaveDoc && (currItem=doc->m_Selection->itemAt(0))!=NULL)
	{
		if (currItem->itemType()==PageItem::ImageFrame)
		{
			PageItem_ImageFrame* imageItem=currItem->asImageFrame();
			if (imageItem->PictureIsAvailable)
			{
				contentsBuffer.sourceType = PageItem::ImageFrame;
				contentsBuffer.contentsFileName = imageItem->Pfile;
				contentsBuffer.LocalScX = imageItem->imageXScale();
				contentsBuffer.LocalScY = imageItem->imageYScale();
				contentsBuffer.LocalX   = imageItem->imageXOffset();
				contentsBuffer.LocalY   = imageItem->imageYOffset();
				contentsBuffer.LocalRot = imageItem->imageRotation();
				contentsBuffer.ItemX   = imageItem->xPos();
				contentsBuffer.ItemY   = imageItem->yPos();
				contentsBuffer.effects = imageItem->effectsInUse;
				contentsBuffer.inputProfile = imageItem->IProfile;
				contentsBuffer.useEmbedded  = imageItem->UseEmbedded;
				contentsBuffer.renderingIntent = imageItem->IRender;
			}
		}
	}
}

void ScribusMainWindow::slotEditPasteContents(int absolute)
{
	PageItem *currItem=NULL;
	if (HaveDoc && !contentsBuffer.contentsFileName.isEmpty() && (currItem=doc->m_Selection->itemAt(0))!=NULL)
	{
		if (contentsBuffer.sourceType==PageItem::ImageFrame && currItem->itemType()==PageItem::ImageFrame)
		{
			PageItem_ImageFrame* imageItem=currItem->asImageFrame();
			int t=QMessageBox::Yes;
			if (imageItem->PictureIsAvailable)
				t = QMessageBox::warning(this, CommonStrings::trWarning,
										tr("Do you really want to replace your existing image?"),
										QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (t == QMessageBox::Yes)
			{
				imageItem->EmProfile = "";
				imageItem->pixm.imgInfo.isRequest = false;
				imageItem->IProfile = doc->cmsSettings().DefaultImageRGBProfile;
				imageItem->IRender  = doc->cmsSettings().DefaultIntentImages;
				imageItem->effectsInUse = contentsBuffer.effects;
				qApp->changeOverrideCursor( QCursor(Qt::WaitCursor) );
				qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
				doc->loadPict(contentsBuffer.contentsFileName, imageItem);
				imageItem->setImageXYScale(contentsBuffer.LocalScX, contentsBuffer.LocalScY);
				if (absolute==0)
					imageItem->setImageXYOffset(contentsBuffer.LocalX, contentsBuffer.LocalY);
				else
					imageItem->setImageXYOffset(
					((contentsBuffer.ItemX-imageItem->xPos()) / contentsBuffer.LocalScX)+contentsBuffer.LocalX,
					((contentsBuffer.ItemY-imageItem->yPos()) / contentsBuffer.LocalScY)+contentsBuffer.LocalY);
				imageItem->setImageRotation(contentsBuffer.LocalRot);
				imageItem->IProfile=contentsBuffer.inputProfile;
				imageItem->UseEmbedded=contentsBuffer.useEmbedded;
				imageItem->IRender=contentsBuffer.renderingIntent;
				qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
				view->DrawNew();
				propertiesPalette->updateColorList();
				emit UpdateRequest(reqCmsOptionsUpdate);
				currItem->emitAllToGUI();
			}
		}
	}
}

void ScribusMainWindow::slotInsertFrame()
{
	if (HaveDoc)
	{
		if (doc->m_Selection->count() != 0)
			view->Deselect(false);
		InsertAFrame dia(this, doc);
		if (dia.exec())
		{
			InsertAFrameData iafData;
			dia.getNewFrameProperties(iafData);
			doc->itemAddUserFrame(iafData);
		}
	}
}

void ScribusMainWindow::slotItemTransform()
{
	if (HaveDoc)
	{
		if (doc->m_Selection->count() == 0)
			return;
		TransformDialog td(this, doc);
		if (td.exec())
		{
			qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
			int count=td.getCount();
			QTransform matrix(td.getTransformMatrix());
			int basepoint=td.getBasepoint();
			doc->itemSelection_Transform(count, matrix, basepoint);
			qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
		}
	}
}

void ScribusMainWindow::PutToPatterns()
{
	int z;
	uint docSelectionCount = doc->m_Selection->count();
	QString patternName = "Pattern_"+doc->m_Selection->itemAt(0)->itemName();
	patternName = patternName.trimmed().simplified().replace(" ", "_");
	Query dia(this, "tt", 1, 0, tr("&Name:"), tr("New Entry"));
	dia.setEditText(patternName, true);
	dia.setTestList(doc->docPatterns.keys());
	dia.setCheckMode(true);
	if (dia.exec())
		patternName = dia.getEditText();
	else
		return;
	bool wasUndo = undoManager->undoEnabled();
	undoManager->setUndoEnabled(false);
	PageItem* currItem;
	Selection itemSelection(this, false);
	itemSelection.copy(*doc->m_Selection, false);
	view->Deselect(true);
	if (docSelectionCount > 1)
		currItem = doc->groupObjectsSelection(&itemSelection);
	else
		currItem = itemSelection.itemAt(0);
	ScPattern pat = ScPattern();
	pat.setDoc(doc);
	double minx =  std::numeric_limits<double>::max();
	double miny =  std::numeric_limits<double>::max();
	double maxx = -std::numeric_limits<double>::max();
	double maxy = -std::numeric_limits<double>::max();
	double x1, x2, y1, y2;
	currItem->getVisualBoundingRect(&x1, &y1, &x2, &y2);
	minx = qMin(minx, x1);
	miny = qMin(miny, y1);
	maxx = qMax(maxx, x2);
	maxy = qMax(maxy, y2);
	pat.pattern = currItem->DrawObj_toImage(qMax(maxx - minx, maxy - miny));
	pat.width = currItem->gWidth;
	pat.height = currItem->gHeight;
	pat.items.append(currItem);
	if (doc->docPatterns.contains(patternName))
		doc->docPatterns.remove(patternName);
	doc->addPattern(patternName, pat);
	double x = currItem->xPos();
	double y = currItem->yPos();
	double w = currItem->width();
	double h = currItem->height();
	int d = doc->Items->indexOf(currItem);
	z = doc->itemAdd(PageItem::Symbol, PageItem::Rectangle, x, y, w, h, 0, CommonStrings::None, CommonStrings::None, true);
	PageItem* groupItem = doc->Items->takeAt(z);
	groupItem->setPattern(patternName);
	doc->Items->replace(d, groupItem);
	propertiesPalette->updateColorList();
	symbolPalette->updateSymbolList();
	if (outlinePalette->isVisible())
		outlinePalette->BuildTree();
	doc->m_Selection->delaySignalsOff();
	view->DrawNew();
	undoManager->setUndoEnabled(wasUndo);
}

void ScribusMainWindow::managePaints()
{
	ColorList edc;
	QMap<QString, VGradient> *Gradients;
	QMap<QString, ScPattern> *docPatterns;
	ScribusDoc* tmpDoc;
	if (HaveDoc)
	{
		Gradients = &doc->docGradients;
		edc = doc->PageColors;
		docPatterns = &doc->docPatterns;
		tmpDoc = doc;
	}
	else
	{
		Gradients = &prefsManager->appPrefs.defaultGradients;
		edc = prefsManager->colorSet();
		docPatterns = &prefsManager->appPrefs.defaultPatterns;
		tmpDoc = m_doc;
		doc = m_doc;
	}
	undoManager->setUndoEnabled(false);
	PaintManagerDialog *dia = new PaintManagerDialog(this, Gradients, edc, prefsManager->colorSetName(), docPatterns, tmpDoc, this);
	if (dia->exec())
	{
		if (HaveDoc)
		{
			QColor tmpc;
			slotDocCh();
			doc->PageColors = dia->m_colorList;
			if (dia->replaceColorMap.isEmpty())
			{
				// invalidate all charstyles, as replaceNamedResources() wont do it if all maps are empty
				const StyleSet<CharStyle> dummy;
				doc->redefineCharStyles(dummy, false);
			}
			else
			{
				ResourceCollection colorrsc;
				colorrsc.mapColors(dia->replaceColorMap);
				// Update tools colors
				PrefsManager::replaceToolColors(doc->itemToolPrefs(), colorrsc.colors());
				// Update objects and styles colors
				doc->replaceNamedResources(colorrsc);
				// Temporary code until LineStyle is effectively used
				doc->replaceLineStyleColors(dia->replaceColorMap);
			}
			doc->recalculateColors();
			doc->recalcPicturesRes();
			doc->setGradients(dia->dialogGradients);
			if (!dia->replaceMap.isEmpty())
			{
				ResourceCollection gradrsc;
				gradrsc.mapPatterns(dia->replaceMap);
				doc->replaceNamedResources(gradrsc);
			}
			doc->setPatterns(dia->dialogPatterns);
			if (!dia->replaceMapPatterns.isEmpty())
			{
				ResourceCollection colorrsc;
				colorrsc.mapPatterns(dia->replaceMapPatterns);
				doc->replaceNamedResources(colorrsc);
			}
			symbolPalette->updateSymbolList();
			updateColorLists();
			if (doc->m_Selection->count() != 0)
				doc->m_Selection->itemAt(0)->emitAllToGUI();
			view->DrawNew();
		}
		else
		{
			// Update tools colors if needed
			prefsManager->replaceToolColors(dia->replaceColorMap);
			prefsManager->setColorSet(dia->m_colorList);
			propertiesPalette->Cpal->setColors(prefsManager->colorSet());
			prefsManager->appPrefs.defaultGradients = dia->dialogGradients;
			prefsManager->appPrefs.defaultPatterns = dia->dialogPatterns;
			QString Cpfad = QDir::toNativeSeparators(ScPaths::getApplicationDataDir())+"DefaultColors.xml";
			const FileFormat *fmt = LoadSavePlugin::getFormatById(FORMATID_SLA150EXPORT);
			if (fmt)
			{
				ScribusDoc *s_doc = new ScribusDoc();
				s_doc->setup(0, 1, 1, 1, 1, "Custom", "Custom");
				s_doc->setPage(100, 100, 0, 0, 0, 0, 0, 0, false, false);
				s_doc->addPage(0);
				s_doc->setGUI(false, this, 0);
				s_doc->PageColors = dia->m_colorList;
				s_doc->setGradients(dia->dialogGradients);
				s_doc->setPatterns(dia->dialogPatterns);
				fmt->setupTargets(s_doc, 0, this, mainWindowProgressBar, &(PrefsManager::instance()->appPrefs.fontPrefs.AvailFonts));
				fmt->savePalette(Cpfad);
				delete s_doc;
			}
			prefsManager->setColorSetName(dia->getColorSetName());
			doc = NULL;
		}
	}
	delete dia;
	undoManager->setUndoEnabled(true);
}

void ScribusMainWindow::slotReplaceColors()
{
	if (HaveDoc)
	{
		ColorList UsedC;
		doc->getUsedColors(UsedC);
		replaceColorsDialog *dia2 = new replaceColorsDialog(this, doc->PageColors, UsedC);
		if (dia2->exec())
		{
			ResourceCollection colorrsc;
			colorrsc.mapColors(dia2->replaceMap);
			PrefsManager::replaceToolColors(doc->itemToolPrefs(), colorrsc.colors());
			doc->replaceNamedResources(colorrsc);
			doc->replaceLineStyleColors(dia2->replaceMap);
			doc->recalculateColors();
			doc->recalcPicturesRes();
			requestUpdate(reqColorsUpdate | reqLineStylesUpdate);
			styleManager->updateColorList();
			if (doc->m_Selection->count() != 0)
				doc->m_Selection->itemAt(0)->emitAllToGUI();
			view->DrawNew();
		}
		delete dia2;
	}
}

void ScribusMainWindow::enableTextActions(QMap<QString, QPointer<ScrAction> > *actionMap, bool enabled, const QString& fontName)
{
	scrActions["insertGlyph"]->setEnabled(enabled);
	actionManager->enableUnicodeActions(actionMap, enabled, fontName);
	scrMenuMgr->setMenuEnabled("InsertChar", enabled);
	scrMenuMgr->setMenuEnabled("InsertQuote", enabled);
	scrMenuMgr->setMenuEnabled("InsertSpace", enabled);
	scrMenuMgr->setMenuEnabled("InsertLigature", enabled);
}
