/*
 For general Scribus (>=1.3.2) copyright and licensing information please refer
 to the COPYING file provided with the program. Following this notice may exist
 a copyright and/or license notice that predates the release of Scribus 1.3.2
 for which a new license (GPL+exception) is in place.
 */
/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/
/***************************************************************************
                 canvasmode_editspiral.h  -  description
                             -------------------
    begin                : Wed Jan 27 2011
    copyright            : (C) 2011 by Franz Schmid
    email                : Franz.Schmid@altmuehlnet.de
 ***************************************************************************/

#ifndef CANVAS_MODE_EDITSPIRAL_H
#define CANVAS_MODE_EDITSPIRAL_H

#include <QObject>
#include <QTime>

#include "canvasmode.h"
#include "fpointarray.h"
#include "ui/spiralvectordialog.h"

class PageItem;
class ScribusMainWindow;
class ScribusView;


// This class encapsulate the old code for mouse interaction from scribusview.cpp

class CanvasMode_EditSpiral :  public CanvasMode
{	
	Q_OBJECT
public:
	explicit CanvasMode_EditSpiral(ScribusView* view);
	virtual ~CanvasMode_EditSpiral() {}

	virtual void enterEvent(QEvent *);
	virtual void leaveEvent(QEvent *);
	
	virtual void activate(bool);
	virtual void deactivate(bool);
	virtual void mouseDoubleClickEvent(QMouseEvent *m);
	virtual void mouseReleaseEvent(QMouseEvent *m);
	virtual void mouseMoveEvent(QMouseEvent *m);
	virtual void mousePressEvent(QMouseEvent *m);
	virtual void drawControls(QPainter* p);
	void drawControlsSpiral(QPainter* pp, PageItem* currItem);

private:

	typedef enum 
	{
		noPointDefined,
		useControlStart,
		useControlEnd,
	} eSpiralPoint;

	inline bool GetItem(PageItem** pi);
	QPointF getSegment(double angle);
	double computeRealAngle(double angle, bool fromDia);

	double Mxp, Myp;
	ScribusMainWindow* m_ScMW;
	QPointF startPoint;
	QPointF endPoint;
	double startAngle;
	double endAngle;
	eSpiralPoint m_arcPoint;
	SpiralVectorDialog* VectorDialog;

	int    m_blockUpdateFromItem;
	void   blockUpdateFromItem(bool block) { if (block) ++m_blockUpdateFromItem; else --m_blockUpdateFromItem; }
	bool   updateFromItemBlocked() { return (m_blockUpdateFromItem > 0); }

public slots:
	void updateFromItem();

private slots:
	void endEditing(bool active);
	void endEditing();
	void applyValues(double start, double end, double factor);
};


#endif
