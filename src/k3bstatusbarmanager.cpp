#include "k3bstatusbarmanager.h"
#include "k3b.h"
#include "k3bbusywidget.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kio/global.h>
#include <kstatusbar.h>
#include <kaboutdata.h>
#include <kdiskfreesp.h>

#include <qlabel.h>
#include <qhbox.h>
#include <qfile.h>


K3bStatusBarManager::K3bStatusBarManager( K3bMainWindow* parent )
  : QObject(parent),
    m_mainWindow(parent)
{
  // setup free temp space box
  QHBox* boxFreeTemp = new QHBox( m_mainWindow->statusBar() );
  m_pixFreeTemp = new QLabel( boxFreeTemp );
  (void)new QLabel( i18n("Temp:"), boxFreeTemp );
  m_pixFreeTemp->setPixmap( SmallIcon("folder_green") );
  m_labelFreeTemp = new QLabel( boxFreeTemp );

  // busy widget
  m_busyWidget = new K3bBusyWidget( m_mainWindow->statusBar() );

  // setup info area
  m_labelInfoMessage = new QLabel( " ", m_mainWindow->statusBar() );


  // setup the statusbar
  m_mainWindow->statusBar()->addWidget( m_labelInfoMessage, 1, true ); // for showing some info
  m_mainWindow->statusBar()->addWidget( boxFreeTemp, 0, true );
  m_mainWindow->statusBar()->addWidget( m_busyWidget, 0, true );
  m_mainWindow->statusBar()->insertFixedItem( QString("K3b %1").arg(kapp->aboutData()->version()), 0, true );

  connect( m_mainWindow, SIGNAL(configChanged(KConfig*)), this, SLOT(update()) );

  update();
}


K3bStatusBarManager::~K3bStatusBarManager()
{
}


void K3bStatusBarManager::update()
{
  kapp->config()->setGroup( "General Options" );
  QString tempdir = kapp->config()->readEntry( "Temp Dir", locateLocal( "appdata", "temp/" ) );

  QString mountPoint = KIO::findPathMountPoint( tempdir );
  if( QFile::exists( mountPoint ) )
    connect( KDiskFreeSp::findUsageInfo( mountPoint ), 
	     SIGNAL(foundMountPoint(const QString&, unsigned long, unsigned long, unsigned long)),
	     this, SLOT(slotFreeTempSpace(const QString&, unsigned long, unsigned long, unsigned long)) );
  else {
    m_labelFreeTemp->setText("No info");
  }
}


void K3bStatusBarManager::slotFreeTempSpace(const QString&, 
					    unsigned long kbSize, 
					    unsigned long, 
					    unsigned long kbAvail)
{
  m_labelFreeTemp->setText( KIO::convertSizeFromKB(kbAvail) + "/" + KIO::convertSizeFromKB(kbSize)  );

  // if we have less than 640 MB that is not good
  if( kbAvail < 655360 )
    m_pixFreeTemp->setPixmap( SmallIcon("folder_red") );
  else
    m_pixFreeTemp->setPixmap( SmallIcon("folder_green") );
}


void K3bStatusBarManager::showBusyInfo( const QString& str )
{
  m_labelInfoMessage->setText( str );
  m_busyWidget->showBusy( true );
}


void K3bStatusBarManager::endBusy()
{
  m_labelInfoMessage->setText( " " );
  m_busyWidget->showBusy( false );
}

#include "k3bstatusbarmanager.moc"
