/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          page.cpp  -  description
                             -------------------
    begin                : Sat Apr 7 2001
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

#include <QDebug>
#include <QPixmap>

#include "scpage.h"
#include "scribus.h"
#include "selection.h"
#include "undomanager.h"
#include "undostate.h"
#include "util_icon.h"

#include "ui/guidemanager.h"
#include "ui/nodeeditpalette.h"

ScPage::ScPage(const double x, const double y, const double b, const double h) :
	UndoObject(QObject::tr("Page")),
	SingleObservable<ScPage>(NULL),
	Margins(40,40,40,40),
	initialMargins(40,40,40,40),
	LeftPg(0),
	MPageNam(""),
	FromMaster(),
	undoManager(UndoManager::instance()),
	m_xOffset(x),
	m_yOffset(y),
	m_width(b),
	m_height(h),
	m_initialWidth(b),
	m_initialHeight(h),
	m_PageName(""),
	m_Doc(NULL)
{
	guides.setPage(this);
	marginPreset = 0;
}

ScPage::~ScPage()
{
	// Clean up any modified template items (unused)
//	for (PageItem *currItem = FromMaster.first(); currItem; currItem = FromMaster.next())
//	{
//		if (currItem->ChangedMasterItem)
//		{
//			FromMaster.remove(currItem);
//			delete currItem;
//		}
//	}
	FromMaster.clear();
}

void ScPage::setDocument(ScribusDoc *doc)
{
	m_Doc=doc;
	setMassObservable(doc? doc->pagesChanged() : NULL);
}

void ScPage::setPageNr(int pageNr)
{
	m_pageNr = pageNr;
	if (m_PageName.isEmpty())
		setUName(QString(QObject::tr("Page") + " %1").arg(m_Doc->FirstPnum + m_pageNr));
	else
		setUName(m_PageName);
}

void ScPage::setPageName(const QString& newName)
{
	m_PageName = newName;
	if (!newName.isEmpty())
		setUName(QObject::tr("Master Page ") + newName);
}

void ScPage::restore(UndoState* state, bool isUndo)
{
	SimpleState* ss = dynamic_cast<SimpleState*>(state);
	if (ss)
	{
//		int stateCode = ss->transactionCode;
		if (ss->contains("ADD_V"))
		{
			double position = ss->getDouble("ADD_V");
			if (isUndo)
				guides.deleteVertical(position, GuideManagerCore::Standard);//removeXGuide(position);
			else
				guides.addVertical(position, GuideManagerCore::Standard);//addXGuide(position);
			m_Doc->scMW()->guidePalette->setupGui();
		}
		else if (ss->contains("ADD_H"))
		{
			double position = ss->getDouble("ADD_H");
			if (isUndo)
				guides.deleteHorizontal(position, GuideManagerCore::Standard);//removeYGuide(position);
			else
				guides.addHorizontal(position, GuideManagerCore::Standard);//addYGuide(position);
			m_Doc->scMW()->guidePalette->setupGui();
		}
		else if (ss->contains("REMOVE_V"))
		{
			double position = ss->getDouble("REMOVE_V");
			if (isUndo)
				guides.addVertical(position, GuideManagerCore::Standard);//addXGuide(position);
			else
				guides.deleteVertical(position, GuideManagerCore::Standard);//removeXGuide(position);
			m_Doc->scMW()->guidePalette->setupGui();
		}
		else if (ss->contains("REMOVE_H"))
		{
			double position = ss->getDouble("REMOVE_H");
			if (isUndo)
				guides.addHorizontal(position, GuideManagerCore::Standard);//addYGuide(position);
			else
				guides.deleteHorizontal(position, GuideManagerCore::Standard);//removeYGuide(position);
			m_Doc->scMW()->guidePalette->setupGui();
		}
		else if (ss->contains("MOVE_H_FROM"))
		{
			double from = ss->getDouble("MOVE_H_FROM");
			double to   = ss->getDouble("MOVE_H_TO");
			if (isUndo)
			{
				guides.deleteHorizontal(to, GuideManagerCore::Standard);//removeYGuide(position);
				guides.addHorizontal(from, GuideManagerCore::Standard);//addYGuide(position);
			}
			else
			{
				guides.deleteHorizontal(from, GuideManagerCore::Standard);//removeYGuide(position);
				guides.addHorizontal(to, GuideManagerCore::Standard);//addYGuide(position);
			}
			m_Doc->scMW()->guidePalette->setupGui();
		}
		else if (ss->contains("MOVE_V_FROM"))
		{
			double from = ss->getDouble("MOVE_V_FROM");
			double to   = ss->getDouble("MOVE_V_TO");
			if (isUndo)
			{
				guides.deleteVertical(to, GuideManagerCore::Standard);//removeXGuide(position);
				guides.addVertical(from, GuideManagerCore::Standard);//removeXGuide(position);
			}
			else
			{
				guides.deleteVertical(from, GuideManagerCore::Standard);//removeXGuide(position);
				guides.addVertical(to, GuideManagerCore::Standard);//removeXGuide(position);
			}
			m_Doc->scMW()->guidePalette->setupGui();
		}
		// automatic guides
		else if (ss->contains("REMOVE_HA_GAP"))
		{
			if (isUndo)
			{
				guides.setHorizontalAutoCount(ss->getInt("REMOVE_HA_COUNT"));
				guides.setHorizontalAutoGap(ss->getDouble("REMOVE_HA_GAP"));
				guides.setHorizontalAutoRefer(ss->getInt("REMOVE_HA_REFER"));
			}
			else
			{
				guides.setHorizontalAutoCount(0);
				guides.setHorizontalAutoGap(0.0);
				guides.setHorizontalAutoRefer(0);
			}
			m_Doc->scMW()->guidePalette->setupGui();
		}
		else if (ss->contains("REMOVE_VA_GAP"))
		{
			if (isUndo)
			{
				guides.setVerticalAutoCount(ss->getInt("REMOVE_VA_COUNT"));
				guides.setVerticalAutoGap(ss->getDouble("REMOVE_VA_GAP"));
				guides.setVerticalAutoRefer(ss->getInt("REMOVE_VA_REFER"));
			}
			else
			{
				guides.setVerticalAutoCount(0);
				guides.setVerticalAutoGap(0.0);
				guides.setVerticalAutoRefer(0);
			}
			m_Doc->scMW()->guidePalette->setupGui();
		}
		else if (ss->contains("CREATE_ITEM"))
			restorePageItemCreation(dynamic_cast<ScItemState<PageItem*>*>(ss), isUndo);
		else if (ss->contains("DELETE_ITEM"))
			restorePageItemDeletion(dynamic_cast<ScItemState< QList<PageItem*> >*>(ss), isUndo);
		else if (ss->contains("CONVERT_ITEM"))
			restorePageItemConversion(dynamic_cast<ScItemState<std::pair<PageItem*, PageItem*> >*>(ss), isUndo);
	}
}

void ScPage::restorePageItemCreation(ScItemState<PageItem*> *state, bool isUndo)
{
	if (!state)
		return;
	int stateCode = state->transactionCode;
	PageItem *ite = state->getItem();
	bool oldMPMode=m_Doc->masterPageMode();
	if ((stateCode == 0) || (stateCode == 1))
	{
		m_Doc->setMasterPageMode(!ite->OnMasterPage.isEmpty());
		if (m_Doc->appMode == modeEditClip) // switch off from edit shape
			m_Doc->scMW()->nodePalette->EndEdit();
	}
	m_Doc->m_Selection->delaySignalsOn();
	if (isUndo)
	{
		if (m_Doc->m_Selection->findItem(ite)!=-1)
		{
			if (m_Doc->appMode == modeEdit)
				m_Doc->view()->requestMode(modeNormal);
			m_Doc->m_Selection->removeItem(ite);
		}
		if ((stateCode == 0) || (stateCode == 1))
			m_Doc->view()->Deselect(true);
		Selection tmpSelection(m_Doc, false);
		tmpSelection.addItem(ite);
		m_Doc->itemSelection_DeleteItem(&tmpSelection);
	}
	else
	{
		if ((stateCode == 0) || (stateCode == 1))
			m_Doc->view()->Deselect(true);
		m_Doc->Items->append(ite);
		ite->ItemNr = m_Doc->Items->count()-1;
		if ((stateCode == 0) || (stateCode == 2))
			update();
	}
	m_Doc->setMasterPageMode(oldMPMode);
	m_Doc->m_Selection->delaySignalsOff();
}

void ScPage::restorePageItemDeletion(ScItemState< QList<PageItem*> > *state, bool isUndo)
{
	if (!state)
		return;
	QList<PageItem*> itemList = state->getItem();
	if (itemList.count() <= 0) 
		return;
	m_Doc->view()->Deselect(true);
	bool oldMPMode=m_Doc->masterPageMode();
	m_Doc->setMasterPageMode(!itemList.at(0)->OnMasterPage.isEmpty());
	if (m_Doc->appMode == modeEditClip) // switch off from edit shape
		m_Doc->scMW()->nodePalette->EndEdit();
	m_Doc->m_Selection->delaySignalsOn();
	if (isUndo)
	{
		//CB #3373 reinsert at old position and renumber items
		for (int i = 0; i < itemList.count(); ++i)
		{
			PageItem* ite = itemList.at(i);
			m_Doc->Items->insert(ite->ItemNr, ite);
		}
		m_Doc->renumberItemsInListOrder();
 		update();
	}
	else
	{
		Selection tmpSelection(m_Doc, false);
		for (int i = 0; i < itemList.count(); ++i)
		{
			PageItem* ite = itemList.at(i);
			if (m_Doc->m_Selection->findItem(ite)!=-1)
			{
				if (m_Doc->appMode == modeEdit)
					m_Doc->view()->requestMode(modeNormal);
				m_Doc->m_Selection->removeItem(ite);
			}
			tmpSelection.addItem(ite);
		}
		m_Doc->itemSelection_DeleteItem(&tmpSelection);
	}
	m_Doc->m_Selection->delaySignalsOff();
	m_Doc->setMasterPageMode(oldMPMode);
}

void ScPage::restorePageItemConversion(ScItemState<std::pair<PageItem*, PageItem*> >*state, bool isUndo)
{
	if (!state)
		return;

	PageItem *oldItem=state->getItem().first;
	PageItem *newItem=state->getItem().second;
	bool oldMPMode=m_Doc->masterPageMode();
	m_Doc->setMasterPageMode(!oldItem->OnMasterPage.isEmpty());
	if (isUndo)
	{
		m_Doc->Items->replace(newItem->ItemNr, oldItem);
		oldItem->updatePolyClip();
		m_Doc->AdjustItemSize(oldItem);
	}
	else
	{
		m_Doc->Items->replace(oldItem->ItemNr, newItem);
	}
	m_Doc->setMasterPageMode(oldMPMode);
}

void ScPage::setXOffset(const double newCanvasXOffset)
{
	m_xOffset = newCanvasXOffset;
}

void ScPage::setYOffset(const double newCanvasYOffset)
{
	m_yOffset = newCanvasYOffset;
}

void ScPage::setWidth(const double newWidth)
{
	m_width = newWidth;
}

void ScPage::setHeight(const double newHeight)
{
	m_height = newHeight;
}

void ScPage::setInitialWidth(const double newInitialWidth)
{
	m_initialWidth = newInitialWidth;
}

void ScPage::setInitialHeight(const double newInitialHeight)
{
	m_initialHeight = newInitialHeight;
}

void ScPage::setOrientation(int ori)
{
	m_orientation = ori;
}

void ScPage::setPageSectionNumber(const QString& newPageSectionNumber)
{
	m_pageSectionNumber=newPageSectionNumber;
}

void ScPage::copySizingProperties(ScPage* sourcePage, const MarginStruct& pageMargins)
{
	if (sourcePage==NULL)
		return;
	m_pageSize = sourcePage->m_pageSize;
	m_orientation = sourcePage->m_orientation;
	m_width=sourcePage->m_width;
	m_height=sourcePage->m_height;
	m_initialWidth = sourcePage->m_initialWidth;
	m_initialHeight = sourcePage->m_initialHeight;

	Margins.Top    = pageMargins.Top;
	Margins.Bottom = pageMargins.Bottom;
	Margins.Left   = pageMargins.Left;//todo fix for layouts
	Margins.Right  = pageMargins.Right;
	// #8859 do not get initialMargins from pageMargins otherwise
	// margins may be inverted when applying master pages
	initialMargins.Top    = sourcePage->initialMargins.Top;
	initialMargins.Bottom = sourcePage->initialMargins.Bottom;
	initialMargins.Left   = sourcePage->initialMargins.Left;
	initialMargins.Right  = sourcePage->initialMargins.Right;
}
