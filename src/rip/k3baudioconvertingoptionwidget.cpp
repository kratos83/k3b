/* 
 *
 * $Id$
 * Copyright (C) 2004 Sebastian Trueg <trueg@k3b.org>
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

#include "k3baudioconvertingoptionwidget.h"

#include <k3bpluginmanager.h>
#include <k3baudioencoder.h>
#include <k3bcore.h>
#include <k3bglobals.h>

#include <kcombobox.h>
#include <kurlrequester.h>
#include <kio/global.h>
#include <kconfig.h>
#include <klocale.h>
#include <kiconloader.h>

#include <qintdict.h>
#include <qmap.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qcheckbox.h>



class K3bAudioConvertingOptionWidget::Private
{
public:
  QIntDict<K3bAudioEncoderFactory> factoryMap;
  QMap<int, QString> extensionMap;

  QTimer freeSpaceUpdateTimer;
};


K3bAudioConvertingOptionWidget::K3bAudioConvertingOptionWidget( QWidget* parent, const char* name )
  : base_K3bAudioRippingOptionWidget( parent, name )
{
  d = new Private();

  connect( m_editBaseDir, SIGNAL(textChanged(const QString&)),
	   this, SLOT(slotUpdateFreeTempSpace()) );
  connect( m_comboFileType, SIGNAL(activated(int)), 
	   this, SLOT(slotEncoderChanged()) );
  connect( &d->freeSpaceUpdateTimer, SIGNAL(timeout()),
	   this, SLOT(slotUpdateFreeTempSpace()) );
  connect( m_checkCreatePlaylist, SIGNAL(toggled(bool)), this, SIGNAL(changed()) );
  connect( m_checkSingleFile, SIGNAL(toggled(bool)), this, SIGNAL(changed()) );
  connect( m_checkWriteCueFile, SIGNAL(toggled(bool)), this, SIGNAL(changed()) );
  connect( m_comboFileType, SIGNAL(activated(int)), this, SIGNAL(changed()) );
  connect( m_editBaseDir, SIGNAL(textChanged(const QString&)), this, SIGNAL(changed()) );
  connect( m_buttonConfigurePlugin, SIGNAL(clicked()), this, SLOT(slotConfigurePlugin()) );

  m_editBaseDir->setMode( KFile::Directory );
  m_buttonConfigurePlugin->setIconSet( SmallIconSet( "gear" ) );

  d->factoryMap.clear();
  d->extensionMap.clear();
  m_comboFileType->clear();
  m_comboFileType->insertItem( i18n("Wave") );
  d->extensionMap[0] = "wav";

  // check the available encoding plugins
  QPtrList<K3bPluginFactory> fl = k3bpluginmanager->factories( "AudioEncoder" );
  for( QPtrListIterator<K3bPluginFactory> it( fl ); it.current(); ++it ) {
    K3bAudioEncoderFactory* f = (K3bAudioEncoderFactory*)it.current();
    QStringList exL = f->extensions();

    for( QStringList::const_iterator exIt = exL.begin();
	 exIt != exL.end(); ++exIt ) {
      d->extensionMap.insert( m_comboFileType->count(), *exIt );
      d->factoryMap.insert( m_comboFileType->count(), f );
      m_comboFileType->insertItem( f->fileTypeComment(*exIt) );
    }
  }

  // refresh every 2 seconds
  d->freeSpaceUpdateTimer.start(2000);
  slotUpdateFreeTempSpace();
}


K3bAudioConvertingOptionWidget::~K3bAudioConvertingOptionWidget()
{
  delete d;
}


QString K3bAudioConvertingOptionWidget::baseDir() const
{
  return m_editBaseDir->url();
}


void K3bAudioConvertingOptionWidget::setBaseDir( const QString& path )
{
  m_editBaseDir->setURL( path );
}


void K3bAudioConvertingOptionWidget::slotConfigurePlugin()
{
  // 0 for wave
  K3bAudioEncoderFactory* factory = d->factoryMap[m_comboFileType->currentItem()];
  if( factory )
    k3bpluginmanager->execPluginDialog( factory, this );
}


void K3bAudioConvertingOptionWidget::slotUpdateFreeTempSpace()
{
  QString path = m_editBaseDir->url();

  if( !QFile::exists( path ) )
    path.truncate( path.findRev('/') );

  unsigned long size, avail;
  if( K3b::kbFreeOnFs( path, size, avail ) )
    m_labelFreeSpace->setText( KIO::convertSizeFromKB(avail) );
  else
    m_labelFreeSpace->setText("-");
}


void K3bAudioConvertingOptionWidget::slotEncoderChanged()
{
  // 0 for wave
  m_buttonConfigurePlugin->setEnabled( d->factoryMap[m_comboFileType->currentItem()] != 0 );
}


K3bAudioEncoderFactory* K3bAudioConvertingOptionWidget::encoderFactory() const
{
  return d->factoryMap[m_comboFileType->currentItem()];  // 0 for wave
}


QString K3bAudioConvertingOptionWidget::extension() const
{
  return d->extensionMap[m_comboFileType->currentItem()];
}


void K3bAudioConvertingOptionWidget::loadDefaults()
{
  m_editBaseDir->setURL( QDir::homeDirPath() );
  m_checkSingleFile->setChecked( false );
  m_checkWriteCueFile->setChecked( false );
  m_comboFileType->setCurrentItem(0); // Wave
  m_checkCreatePlaylist->setChecked(false);
  m_checkPlaylistRelative->setChecked(false);

  slotEncoderChanged();
}


void K3bAudioConvertingOptionWidget::loadConfig( KConfig* c )
{
  m_editBaseDir->setURL( c->readPathEntry( "last ripping directory", QDir::homeDirPath() ) );

  m_checkSingleFile->setChecked( c->readBoolEntry( "single_file", false ) );
  m_checkWriteCueFile->setChecked( c->readBoolEntry( "write_cue_file", false ) );

  m_checkCreatePlaylist->setChecked( c->readBoolEntry( "create_playlist", false ) );
  m_checkPlaylistRelative->setChecked( c->readBoolEntry( "relative_path_in_playlist", false ) );

  QString filetype = c->readEntry( "filetype", "wav" );
  if( filetype == "wav" )
    m_comboFileType->setCurrentItem(0);
  else {
    for( QMap<int, QString>::iterator it = d->extensionMap.begin();
	 it != d->extensionMap.end(); ++it ) {
      if( it.data() == filetype ) {
	m_comboFileType->setCurrentItem( it.key() );
	break;
      }
    }
  }

  slotEncoderChanged();
}


void K3bAudioConvertingOptionWidget::saveConfig( KConfig* c )
{
  c->writePathEntry( "last ripping directory", m_editBaseDir->url() );

  c->writeEntry( "single_file", m_checkSingleFile->isChecked() );
  c->writeEntry( "write_cue_file", m_checkWriteCueFile->isChecked() );

  c->writeEntry( "create_playlist", m_checkCreatePlaylist->isChecked() );
  c->writeEntry( "relative_path_in_playlist", m_checkPlaylistRelative->isChecked() );

  if( d->extensionMap.contains(m_comboFileType->currentItem()) )
    c->writeEntry( "filetype", d->extensionMap[m_comboFileType->currentItem()] );
  else
    c->writeEntry( "filetype", "wav" );
}

#include "k3baudioconvertingoptionwidget.moc"
