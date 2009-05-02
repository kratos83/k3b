/*
 *
 * Copyright (C) 2009 Michal Malek <michalm@jabster.pl>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2009 Sebastian Trueg <trueg@k3b.org>
 * Copyright (C) 2009 Michal Malek <michalm@jabster.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#ifndef _K3BSETUPPROGRAMITEM_H_
#define _K3BSETUPPROGRAMITEM_H_

#include <QDBusArgument>
#include <QMetaType>
#include <QString>

namespace K3b {
namespace Setup {

class ProgramItem
{
public:
    ProgramItem();
    ProgramItem( const QString& path, bool needSuid );
    
    QString m_path;
    bool m_needSuid;
};

} // namespace Setup
} // namespace K3b

Q_DECLARE_METATYPE( K3b::Setup::ProgramItem )

QDBusArgument& operator<<( QDBusArgument& argument, const K3b::Setup::ProgramItem& item );
const QDBusArgument& operator>>( const QDBusArgument& argument, K3b::Setup::ProgramItem& item );

#endif