/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include "commonstrings.h"

#include "importai.h"
#include "importaiplugin.h"

#include "prefscontext.h"
#include "prefsfile.h"
#include "prefsmanager.h"
#include "scconfig.h"
#include "scpage.h"
#include "scraction.h"
#include "scribuscore.h"
#include "undomanager.h"
#include "util_formats.h"

#include "ui/customfdialog.h"
#include "ui/scmwmenumanager.h"

#include <QApplication>
#include <QMessageBox>

int importai_getPluginAPIVersion()
{
	return PLUGIN_API_VERSION;
}

ScPlugin* importai_getPlugin()
{
	ImportAIPlugin* plug = new ImportAIPlugin();
	Q_CHECK_PTR(plug);
	return plug;
}

void importai_freePlugin(ScPlugin* plugin)
{
	ImportAIPlugin* plug = dynamic_cast<ImportAIPlugin*>(plugin);
	Q_ASSERT(plug);
	delete plug;
}

ImportAIPlugin::ImportAIPlugin() : LoadSavePlugin(),
	importAction(new ScrAction(ScrAction::DLL, QPixmap(), QPixmap(), "", QKeySequence(), this))
{
	// Set action info in languageChange, so we only have to do it in one
	// place. This includes registering file format support.
	languageChange();
}
/*
void ImportAIPlugin::addToMainWindowMenu(ScribusMainWindow *mw)
{
	importAction->setEnabled(true);
	connect( importAction, SIGNAL(triggered()), SLOT(import()) );
	mw->scrMenuMgr->addMenuItem(importAction, "FileImport");
}
*/
void ImportAIPlugin::languageChange()
{
	importAction->setText( tr("Import AI..."));
	// (Re)register file format support
	unregisterAll();
	registerFormats();
}

ImportAIPlugin::~ImportAIPlugin()
{
	unregisterAll();
};

const QString ImportAIPlugin::fullTrName() const
{
	return QObject::tr("Adobe Illustrator Importer");
}


const ScActionPlugin::AboutData* ImportAIPlugin::getAboutData() const
{
	AboutData* about = new AboutData;
	about->authors = "Franz Schmid <franz@scribus.info>";
	about->shortDescription = tr("Imports Illustrator Files");
	about->description = tr("Imports most Illustrator files into the current document,\nconverting their vector data into Scribus objects.");
	about->license = "GPL";
	Q_CHECK_PTR(about);
	return about;
}

void ImportAIPlugin::deleteAboutData(const AboutData* about) const
{
	Q_ASSERT(about);
	delete about;
}

void ImportAIPlugin::registerFormats()
{
	FileFormat fmt(this);
	fmt.trName = FormatsManager::instance()->nameOfFormat(FormatsManager::AI); // Human readable name
	fmt.formatId = FORMATID_AIIMPORT;
	fmt.filter = FormatsManager::instance()->extensionsForFormat(FormatsManager::AI); // QFileDialog filter
	fmt.nameMatch = QRegExp("\\."+FormatsManager::instance()->extensionListForFormat(FormatsManager::AI, 1)+"$", Qt::CaseInsensitive);
	fmt.fileExtensions = QStringList() << "ai";
	fmt.load = true;
	fmt.save = false;
	fmt.thumb = true;
	fmt.colorReading = true;
	fmt.mimeTypes = FormatsManager::instance()->mimetypeOfFormat(FormatsManager::AI); // MIME types
	fmt.priority = 64; // Priority
	registerFormat(fmt);
}

bool ImportAIPlugin::fileSupported(QIODevice* /* file */, const QString & fileName) const
{
	// TODO: check for %!PS-Adobe
	return true;
}

bool ImportAIPlugin::loadFile(const QString & fileName, const FileFormat &, int flags, int /*index*/)
{
	// There's only one format to handle, so we just call import(...)
	return import(fileName, flags);
}

bool ImportAIPlugin::import(QString fileName, int flags)
{
	if (!checkFlags(flags))
		return false;
	if( fileName.isEmpty() )
	{
		flags |= lfInteractive;
		PrefsContext* prefs = PrefsManager::instance()->prefsFile->getPluginContext("importai");
		QString wdir = prefs->get("wdir", ".");
		CustomFDialog diaf(ScCore->primaryMainWindow(), wdir, QObject::tr("Open"), FormatsManager::instance()->fileDialogFormatList(FormatsManager::AI));
		if (diaf.exec())
		{
			fileName = diaf.selectedFile();
			prefs->set("wdir", fileName.left(fileName.lastIndexOf("/")));
		}
		else
			return true;
	}
	m_Doc=ScCore->primaryMainWindow()->doc;
	UndoTransaction* activeTransaction = NULL;
	bool emptyDoc = (m_Doc == NULL);
	bool hasCurrentPage = (m_Doc && m_Doc->currentPage());
	TransactionSettings trSettings;
	trSettings.targetName   = hasCurrentPage ? m_Doc->currentPage()->getUName() : "";
	trSettings.targetPixmap = Um::IImageFrame;
	trSettings.actionName   = Um::ImportAI;
	trSettings.description  = fileName;
	trSettings.actionPixmap = Um::IAI;
	if (emptyDoc || !(flags & lfInteractive) || !(flags & lfScripted))
		UndoManager::instance()->setUndoEnabled(false);
	if (UndoManager::undoEnabled())
		activeTransaction = new UndoTransaction(UndoManager::instance()->beginTransaction(trSettings));
#ifdef HAVE_POPPLER
	if (!(flags & LoadSavePlugin::lfLoadAsPattern))
	{
/* Check if the file is an old style AI or one of the newer PDF wrapped ones */
		QFile fT(fileName);
		if (fT.open(QIODevice::ReadOnly))
		{
			QByteArray tempBuf(9, ' ');
			fT.read(tempBuf.data(), 8);
			fT.close();
			if (tempBuf.startsWith("%PDF"))
			{
				qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
				QMessageBox msgBox(ScCore->primaryMainWindow());
				msgBox.setText( tr("This file contains 2 versions of the data."));
				msgBox.setInformativeText( tr("Choose which one should be imported"));
				msgBox.setIcon(QMessageBox::Question);
				QPushButton *pdfButton = msgBox.addButton( tr("Use the pdf part"), QMessageBox::ActionRole);
				msgBox.addButton( tr("Use the ai part"), QMessageBox::ActionRole);
				msgBox.setDefaultButton(pdfButton);
				msgBox.exec();
				if ((QPushButton *)msgBox.clickedButton() == pdfButton)
				{
					//Import PDF
					const FileFormat *fmt = LoadSavePlugin::getFormatById(FORMATID_PDFIMPORT);
					if (!fmt)
					{
						QMessageBox::warning(ScCore->primaryMainWindow(), CommonStrings::trWarning, tr("The PDF Import plugin could not be found"), 1, 0, 0);
						return false;
					}
					qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
					bool success = fmt->loadFile(fileName, flags);
					if (activeTransaction)
					{
						activeTransaction->commit();
						delete activeTransaction;
						activeTransaction = NULL;
					}
					if (emptyDoc || !(flags & lfInteractive) || !(flags & lfScripted))
						UndoManager::instance()->setUndoEnabled(true);
					if (!success)
						QMessageBox::warning(ScCore->primaryMainWindow(), CommonStrings::trWarning, tr("The file could not be imported"), 1, 0, 0);
					return success;
				}
				qApp->changeOverrideCursor(QCursor(Qt::WaitCursor));
			}
		}
	}
#endif
	AIPlug *dia = new AIPlug(m_Doc, flags);
	Q_CHECK_PTR(dia);
	bool success = dia->import(fileName, trSettings, flags);
	if (activeTransaction)
	{
		activeTransaction->commit();
		delete activeTransaction;
		activeTransaction = NULL;
	}
	if (emptyDoc || !(flags & lfInteractive) || !(flags & lfScripted))
		UndoManager::instance()->setUndoEnabled(true);
	if (!success)
		QMessageBox::warning(ScCore->primaryMainWindow(), CommonStrings::trWarning, tr("The file could not be imported"), 1, 0, 0);
	delete dia;
	return success;
}

QImage ImportAIPlugin::readThumbnail(const QString& fileName)
{
	bool wasUndo = false;
	if( fileName.isEmpty() )
		return QImage();
	if (UndoManager::undoEnabled())
	{
		UndoManager::instance()->setUndoEnabled(false);
		wasUndo = true;
	}
	m_Doc = NULL;
	AIPlug *dia = new AIPlug(m_Doc, lfCreateThumbnail);
	Q_CHECK_PTR(dia);
	QImage ret = dia->readThumbnail(fileName);
	if (wasUndo)
		UndoManager::instance()->setUndoEnabled(true);
	delete dia;
	return ret;
}

bool ImportAIPlugin::readColors(const QString& fileName, ColorList &colors)
{
	bool wasUndo = false;
	if( fileName.isEmpty() )
		return false;
	if (UndoManager::undoEnabled())
	{
		UndoManager::instance()->setUndoEnabled(false);
		wasUndo = true;
	}
	m_Doc = NULL;
	AIPlug *dia = new AIPlug(m_Doc, lfCreateThumbnail);
	Q_CHECK_PTR(dia);
	bool ret = dia->readColors(fileName, colors);
	if (wasUndo)
		UndoManager::instance()->setUndoEnabled(true);
	delete dia;
	return ret;
}
