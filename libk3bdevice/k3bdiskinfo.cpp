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

#include "k3bdiskinfo.h"
#include "k3bdeviceglobals.h"

#include <k3bmsf.h>

#include <klocale.h>
#include <kdebug.h>
#include <kio/global.h>

#include <qstringlist.h>


K3bCdDevice::DiskInfo::DiskInfo()
  : m_mediaType(MEDIA_UNKNOWN),
    m_currentProfile(MEDIA_UNKNOWN),
    m_diskState(STATE_UNKNOWN),
    m_lastSessionState(STATE_UNKNOWN),
    m_bgFormatState(0),
    m_numSessions(0),
    m_numTracks(0),
    m_rewritable(false)
{
}


K3bCdDevice::DiskInfo::~DiskInfo()
{
}


int K3bCdDevice::DiskInfo::diskState() const
{
  return m_diskState;
}


int K3bCdDevice::DiskInfo::lastSessionState() const
{
  return m_lastSessionState;
}


int K3bCdDevice::DiskInfo::bgFormatState() const
{
  return m_bgFormatState;
}


bool K3bCdDevice::DiskInfo::empty() const
{
  return diskState() == STATE_EMPTY;
}


bool K3bCdDevice::DiskInfo::rewritable() const
{
  return m_rewritable;
}


bool K3bCdDevice::DiskInfo::appendable() const
{
  return diskState() == STATE_INCOMPLETE;
}


int K3bCdDevice::DiskInfo::mediaType() const
{
  return m_mediaType;
}


bool K3bCdDevice::DiskInfo::isDvdMedia() const
{
  return K3bCdDevice::isDvdMedia( mediaType() );
}


int K3bCdDevice::DiskInfo::numSessions() const
{
  if( empty() )
    return 0;
  else
    return m_numSessions;
}


int K3bCdDevice::DiskInfo::numTracks() const
{
  if( empty() )
    return 0;
  else
    return m_numTracks;
}


int K3bCdDevice::DiskInfo::numLayers() const
{
  if( isDvdMedia() )
    return m_numLayers;
  else
    return 1;
}


K3bMsf K3bCdDevice::DiskInfo::remainingSize() const
{
  if( empty() )
    return capacity();
  else if( appendable() )
    return capacity() - m_usedCapacity;

  //
  // There is no way to properly determine the used size on an overwrite media
  // without having a look at the filesystem (or is there?)
  //
  else if( mediaType() & (MEDIA_DVD_PLUS_RW|MEDIA_DVD_RW_OVWR) )
    return capacity();
  else
    return 0;
}


K3bMsf K3bCdDevice::DiskInfo::capacity() const
{
  return (m_capacity == 0 ? size() : m_capacity);
}


K3bMsf K3bCdDevice::DiskInfo::size() const
{
  if( empty() )
    return 0;
  else
    return m_usedCapacity;
}


K3b::Msf K3bCdDevice::DiskInfo::firstLayerSize() const
{
  if( numLayers() > 1 )
    return m_firstLayerSize;
  else
    return size();
}


void K3bCdDevice::DiskInfo::debug() const
{
  kdDebug() << "DiskInfo:" << endl
	    << "Mediatype:       " << K3bCdDevice::mediaTypeString( mediaType() ) << endl
	    << "Current Profile: " << K3bCdDevice::mediaTypeString( currentProfile() ) << endl
	    << "Disk state:      " << ( diskState() == K3bCdDevice::STATE_EMPTY ? 
					"empty" :
					( diskState() == K3bCdDevice::STATE_INCOMPLETE ?
					  "incomplete" :
					  ( diskState() == K3bCdDevice::STATE_COMPLETE ?
					    "complete" : 
					    ( diskState() == K3bCdDevice::STATE_NO_MEDIA ?
					      "no media" : 
					      "unknown" ) ) ) ) << endl
	    << "Empty:           " << empty() << endl
	    << "Rewritable:      " << rewritable() << endl
	    << "Appendable:      " << appendable() << endl
	    << "Sessions:        " << numSessions() << endl
	    << "Tracks:          " << numTracks() << endl
	    << "Layers:          " << numLayers() << endl
	    << "Capacity:        " << capacity().toString() 
	    << " (LBA " << QString::number(capacity().lba())
	    << ") (" << QString::number(capacity().mode1Bytes()) << " Bytes) (" 
	    << KIO::convertSize(capacity().mode1Bytes()) << ")" << endl

	    << "Remaining size:  " << remainingSize().toString() 
	    << " (LBA " << QString::number(remainingSize().lba())
	    << ") (" << QString::number(remainingSize().mode1Bytes()) << " Bytes) (" 
	    << KIO::convertSize(remainingSize().mode1Bytes()) << ")" << endl

	    << "Used Size:       " << size().toString()  
	    << " (LBA " << QString::number(size().lba())
	    << ") (" << QString::number(size().mode1Bytes()) << " Bytes) (" 
	    << KIO::convertSize(size().mode1Bytes()) << ")" << endl;

  if( mediaType() == K3bCdDevice::MEDIA_DVD_PLUS_RW )
    kdDebug() << "Bg Format:       " << ( bgFormatState() == BG_FORMAT_NONE ? 
					  "none" :
					  ( bgFormatState() == BG_FORMAT_INCOMPLETE ?
					    "incomplete" :
					    ( bgFormatState() == BG_FORMAT_IN_PROGRESS ?
					      "in progress" :
					      "complete" ) ) ) << endl;
}


// kdbgstream& K3bCdDevice::operator<<( kdbgstream& s, const K3bCdDevice::DiskInfo& ngInf )
// {
//    s << "DiskInfo:" << endl
//      << "Mediatype:       " << K3bCdDevice::mediaTypeString( ngInf.mediaType() ) << endl
//      << "Current Profile: " << K3bCdDevice::mediaTypeString( ngInf.currentProfile() ) << endl
//      << "Disk state:      " << ( ngInf.diskState() == K3bCdDevice::STATE_EMPTY ? 
// 				 "empty" :
// 				 ( ngInf.diskState() == K3bCdDevice::STATE_INCOMPLETE ?
// 				   "incomplete" :
// 				   ( ngInf.diskState() == K3bCdDevice::STATE_COMPLETE ?
// 				     "complete" : 
// 				     ( ngInf.diskState() == K3bCdDevice::STATE_NO_MEDIA ?
// 				       "no media" : 
// 				       "unknown" ) ) ) ) << endl
//      << "Empty:           " << ngInf.empty() << endl
//      << "Rewritable:      " << ngInf.rewritable() << endl
//      << "Appendable:      " << ngInf.appendable() << endl
//      << "Sessions:        " << ngInf.numSessions() << endl
//      << "Tracks:          " << ngInf.numTracks() << endl
//      << "Size:            " << ngInf.capacity().toString() << endl
//      << "Remaining size:  " << ngInf.remainingSize().toString() << endl;
   
//    return s;
// }
