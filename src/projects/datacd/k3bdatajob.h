/* 
 *
 * $Id$
 * Copyright (C) 2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */


#ifndef K3BDATAJOB_H
#define K3BDATAJOB_H

#include <k3bjob.h>

#include <qfile.h>

class K3bDataDoc;
class QString;
class QDataStream;
class K3bAbstractWriter;
class K3bIsoImager;
class KTempFile;
class K3bMsInfoFetcher;


/**
  *@author Sebastian Trueg
  */

class K3bDataJob : public K3bBurnJob
{
  Q_OBJECT
	
 public:
  K3bDataJob( K3bDataDoc*, QObject* parent = 0 );
  virtual ~K3bDataJob();
	
  K3bDoc* doc() const;
  K3bCdDevice::CdDevice* writer() const;

  virtual QString jobDescription() const;
  virtual QString jobDetails() const;
		
 public slots:
  void cancel();
  void start();

  /**
   * Used to specify a non-default writer.
   * If this does notget called K3bDataJob determines
   * the writer itself.
   */
  void setWriterJob( K3bAbstractWriter* );
  void setImager( K3bIsoImager* );

 protected slots:
  void slotReceivedIsoImagerData( const char* data, int len );
  void slotIsoImagerFinished( bool success );
  void slotDataWritten();
  void slotIsoImagerPercent(int);
  void slotSizeCalculationFinished( int, int );
  void slotWriterJobPercent( int p );
  void slotWriterNextTrack( int t, int tt );
  void slotWriterJobFinished( bool success );
  void slotVerificationProgress( int );
  void slotVerificationFinished( bool );
  void slotMsInfoFetched(bool);
  void writeImage();
  void cancelAll();

  /**
   * Just a little helper method that makes subclassing easier.
   * Basically used for DVD writing.
   */
  virtual void waitForDisk();
		
 protected:
  virtual bool prepareWriterJob();
  virtual void prepareImager();

  K3bAbstractWriter* m_writerJob;
  K3bIsoImager* m_isoImager;
  K3bMsInfoFetcher* m_msInfoFetcher;

 private:
  bool startWriting();
  void determineWritingMode();

  class Private;
  Private* d;
};

#endif
