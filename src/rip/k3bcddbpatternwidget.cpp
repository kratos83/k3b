/* 
 *
 * $Id$
 * Copyright (C) 2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2004 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include "k3bcddbpatternwidget.h"

#include <kconfig.h>
#include <klocale.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <kurllabel.h>

#include <qregexp.h>
#include <qvalidator.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qlayout.h>


K3bCddbPatternWidget::K3bCddbPatternWidget( QWidget* parent, const char* name )
  : base_K3bCddbPatternWidget( parent, name )
{
  // fix the layout
  ((QGridLayout*)layout())->setRowStretch( 4, 1 );

  // setup validators
  // there can never be one of the following characters in both dir and filename:
  // * ? "
  // additional the filename can never contain a slash /
  // and the dir should never start with a slash since it should always be a relative path

  QRegExpValidator* dirValidator = new QRegExpValidator( QRegExp( "[^/][^?\\*\\\"]*" ), this );
  m_comboFilenamePattern->setValidator( dirValidator );
  m_comboPlaylistPattern->setValidator( dirValidator );
  m_editBlankReplace->setValidator( dirValidator );

  connect( m_comboFilenamePattern, SIGNAL(textChanged(const QString&)),
	   this, SIGNAL(changed()) );
  connect( m_comboPlaylistPattern, SIGNAL(textChanged(const QString&)),
	   this, SIGNAL(changed()) );
  connect( m_editBlankReplace, SIGNAL(textChanged(const QString&)),
	   this, SIGNAL(changed()) );
  connect( m_checkBlankReplace, SIGNAL(toggled(bool)),
	   this, SIGNAL(changed()) );
  connect( m_specialStringsLabel, SIGNAL(leftClickedURL()), 
	   this, SLOT(slotSeeSpecialStrings()) );
}


K3bCddbPatternWidget::~K3bCddbPatternWidget()
{
}


QString K3bCddbPatternWidget::filenamePattern() const
{
  return m_comboFilenamePattern->currentText();
}


QString K3bCddbPatternWidget::playlistPattern() const
{
  return m_comboPlaylistPattern->currentText();
}


QString K3bCddbPatternWidget::blankReplaceString() const
{
  return m_editBlankReplace->text();
}


bool K3bCddbPatternWidget::replaceBlanks() const
{
  return m_checkBlankReplace->isChecked();
}


void K3bCddbPatternWidget::loadConfig( KConfig* c )
{
  m_comboPlaylistPattern->setEditText( c->readEntry( "playlist pattern", "%r - %m.m3u" ) );
  m_comboFilenamePattern->setEditText( c->readEntry( "filename pattern", "%r - %m/%a - %t" ) );
  m_checkBlankReplace->setChecked( c->readBoolEntry( "replace blanks", false ) );
  m_editBlankReplace->setText( c->readEntry( "blank replace string", "_" ) );
}


void K3bCddbPatternWidget::saveConfig( KConfig* c )
{
  c->writeEntry( "playlist pattern", m_comboPlaylistPattern->currentText() );
  c->writeEntry( "filename pattern", m_comboFilenamePattern->currentText() );
  c->writeEntry( "replace blanks", m_checkBlankReplace->isChecked() );
  c->writeEntry( "blank replace string", m_editBlankReplace->text() );
}


void K3bCddbPatternWidget::loadDefaults()
{
  m_comboPlaylistPattern->setEditText( "%r - %m.m3u" );
  m_comboFilenamePattern->setEditText( "%r - %m/%a - %t" );
  m_checkBlankReplace->setChecked( false );
  m_editBlankReplace->setText( "_" );
}


void K3bCddbPatternWidget::slotSeeSpecialStrings()
{
  QWhatsThis::display( i18n( "<p><b>Pattern special strings:</b>"
			     "<ul>\n"
			     "<li>%a - artist of the track\n"
			     "<li>%t - title of the track\n"
			     "<li>%n - track number\n"
			     "<li>%y - year of the CD\n"
			     "<li>%e - extended information about the track\n"
			     "<li>%g - genre of the CD\n"
			     "<li>%r - album artist (differs from %a only on soundtracks or compilations)\n"
			     "<li>%m - album title\n"
			     "<li>%x - extended information about the CD\n"
			     "<li>%d - current date\n"
			     "</ul>" ) );
}

#include "k3bcddbpatternwidget.moc"

