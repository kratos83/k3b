/***************************************************************************
 *   Copyright (C) 2002 by Sebastian Trueg                                 *
 *   trueg@k3b.org                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _K3B_MOVIX_OPTIONSWIDGET_H_
#define _K3B_MOVIX_OPTIONSWIDGET_H_

#include "base_k3bmovixoptionswidget.h"

class K3bMovixDoc;

class K3bMovixOptionsWidget : public base_K3bMovixOptionsWidget
{
  Q_OBJECT

 public:
  K3bMovixOptionsWidget( K3bMovixDoc* doc, QWidget* parent = 0, const char* name = 0 );
  ~K3bMovixOptionsWidget();

 private:
  K3bMovixDoc* m_doc;
};


#endif
