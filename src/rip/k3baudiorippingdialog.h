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


#ifndef _K3B_AUDIO_RIPPING_DIALOG_H_
#define _K3B_AUDIO_RIPPING_DIALOG_H_

#include <k3binteractiondialog.h>

#include <qstringlist.h>

#include <k3bcddbquery.h>

namespace K3bCdDevice {
  class CdDevice;
  class Toc;
}


class KListView;
class QCheckBox;
class QSpinBox;
class QComboBox;
class K3bCddbPatternWidget;
class QToolButton;
class base_K3bAudioRippingOptionWidget;


/**
  *@author Sebastian Trueg
  */
class K3bAudioRippingDialog : public K3bInteractionDialog
{
  Q_OBJECT

 public: 
  K3bAudioRippingDialog( const K3bCdDevice::Toc&, 
			 K3bCdDevice::CdDevice*,
			 const K3bCddbResultEntry&, 
			 const QValueList<int>&, 
			 QWidget *parent = 0, const char *name = 0 );
  ~K3bAudioRippingDialog();

  void setStaticDir( const QString& path );

 public slots:  
  void refresh();
  void init();

 private:
  K3bCdDevice::Toc m_toc;
  K3bCdDevice::CdDevice* m_device;
  K3bCddbResultEntry m_cddbEntry;
  QValueList<int> m_trackNumbers;

  KListView*    m_viewTracks;

  QComboBox* m_comboParanoiaMode;
  QSpinBox* m_spinRetries;
  QCheckBox* m_checkNeverSkip;
  QCheckBox* m_checkUseIndex0;

  K3bCddbPatternWidget* m_patternWidget;

  base_K3bAudioRippingOptionWidget* m_optionWidget;

  void setupGui();
  void setupContextHelp();

  class Private;
  Private* d;
  
 private slots:
  void slotStartClicked();

  void slotLoadK3bDefaults();
  void slotLoadUserDefaults();
  void slotSaveUserDefaults();

  void slotConfigurePlugin();
  void slotToggleAll();
  void slotSeeSpecialStrings();

  void slotUpdateFreeTempSpace();
};

#endif
