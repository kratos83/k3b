#include "k3bdiskinfodetector.h"

#include "../device/k3bdevice.h"
#include "../device/k3btoc.h"
#include "../rip/k3btcwrapper.h"
#include "../k3b.h"
#include "../tools/k3bexternalbinmanager.h"

#include <kdebug.h>
#include <kprocess.h>

#include <qtimer.h>
#include <qfile.h>


#include <sys/ioctl.h>		// ioctls
#include <unistd.h>		// lseek, read. etc
#include <fcntl.h>		// O_RDONLY etc.
#include <linux/cdrom.h>	// old ioctls for cdrom
#include <stdlib.h>


K3bDiskInfoDetector::K3bDiskInfoDetector( QObject* parent )
  : QObject( parent )
{
  m_tcWrapper = 0;
}


K3bDiskInfoDetector::~K3bDiskInfoDetector()
{
}


void K3bDiskInfoDetector::detect( K3bDevice* device )
{
  if( !device ) {
    kdDebug() << "(K3bDiskInfoDetector) detect should really not be called with NULL!" << endl;
    return;
  }

  m_device = device;

  // reset
  m_info = K3bDiskInfo();
  m_info.device = m_device;

  QTimer::singleShot(0,this,SLOT(fetchTocInfo()));
}


void K3bDiskInfoDetector::cancel()
{
}

void K3bDiskInfoDetector::finish(bool success)
{
  m_info.valid=success;
  ::close(m_cdfd);
  emit diskInfoReady(m_info);

}


void K3bDiskInfoDetector::fetchDiskInfo()
{
  struct cdrom_generic_command cmd;
  unsigned char inf[32];

  ::memset(&cmd,0,sizeof (struct cdrom_generic_command));
  ::memset(inf,0,32);
  cmd.cmd[0] = GPCMD_READ_DISC_INFO;
  cmd.cmd[8] = 32;
  cmd.buffer = inf;
  cmd.buflen = 32;
  cmd.data_direction = CGC_DATA_READ;
 
  if ( ::ioctl(m_cdfd,CDROM_SEND_PACKET,&cmd) == 0 ) {
    m_info.appendable = ( (inf[2] & 0x03) < 2 );        // disc state incomplete
    m_info.empty = ( (inf[2] & 0x03) == 0 );
    m_info.cdrw =( ((inf[2] >> 4 ) & 0x01) == 1 );      // erasable
    if ( inf[21] != 0xFF && inf[22] != 0xFF && inf[23] != 0xFF ) {
      m_info.size = inf[21]*4500 + inf[22]*75 +inf[23] - 150;
      m_info.sizeString = QString("%1:%2:%3").arg(inf[21]).arg(inf[22]).arg(inf[23]);
    }
    if ( inf[17] != 0xFF && inf[18] != 0xFF && inf[19] != 0xFF ) { // start of last leadin - 4650
      m_info.remaining = m_info.size - inf[17]*4500 - inf[18]*75 - inf[19] - 4650;
      m_info.remainingString = QString("%1:%2:%3").arg(inf[17]).arg(inf[18]).arg(inf[19]);
    }
  } 
}

void K3bDiskInfoDetector::fetchTocInfo()
{
  struct cdrom_tochdr tochdr;
  struct cdrom_tocentry tocentry;
  int status;
  if ( (m_cdfd = ::open(m_device->ioctlDevice().latin1(),O_RDONLY | O_NONBLOCK)) == -1 ) {
    kdDebug() << "(K3bDiskInfoDetector) could not open device !" << endl;
    m_info.valid=false;
    emit diskInfoReady(m_info);
    return;
  }
  
  if ( (status = ::ioctl(m_cdfd,CDROM_DISC_STATUS)) != 0 )
    switch (status) {
      case CDS_AUDIO:  m_info.tocType = K3bDiskInfo::AUDIO;
                       break;
      case CDS_DATA_1:
      case CDS_DATA_2: m_info.tocType = K3bDiskInfo::DATA;
                       break;
      case CDS_XA_2_1: 
      case CDS_XA_2_2: 
      case CDS_MIXED:  m_info.tocType = K3bDiskInfo::MIXED;
                       break;
      case CDS_NO_DISC: m_info.noDisk = true;
                        finish(true);
                        return;  

  }

  struct cdrom_generic_command cmd;
  unsigned char dat[4];

  ::memset(&cmd,0,sizeof (struct cdrom_generic_command));
  ::memset(dat,0,4);
  cmd.cmd[0] = GPCMD_READ_TOC_PMA_ATIP; 
  cmd.cmd[2] = 1;
  cmd.cmd[8] = 4;
  cmd.buffer = dat;
  cmd.buflen = 4;
  cmd.data_direction = CGC_DATA_READ;
  if ( ::ioctl(m_cdfd,CDROM_SEND_PACKET,&cmd) == 0 )
     m_info.sessions = dat[3];

  if ( ::ioctl(m_cdfd,CDROMREADTOCHDR,&tochdr) != 0 )
  {
     kdDebug() << "(K3bDiskInfoDetector) could not get toc header !" << endl;
     finish(false);
     return;
  }
  K3bTrack lastTrack;
  for (int i = tochdr.cdth_trk0; i <= tochdr.cdth_trk1 + 1; i++) {
    ::memset(&tocentry,0,sizeof (struct cdrom_tocentry));
    tocentry.cdte_track = (i<=tochdr.cdth_trk1) ? i : CDROM_LEADOUT;
    tocentry.cdte_format = CDROM_LBA;
    ::ioctl(m_cdfd,CDROMREADTOCENTRY,&tocentry);
    int startSec = tocentry.cdte_addr.lba;
    int control  = tocentry.cdte_ctrl & 0x0f;
    int mode     = tocentry.cdte_datamode;
    if( !lastTrack.isEmpty() ) {
		   m_info.toc.append( K3bTrack( lastTrack.firstSector(), startSec-1, lastTrack.type(), lastTrack.mode() ) );
	  }
    int trackType = 0;
    int trackMode = K3bTrack::UNKNOWN;
	  if( control & 0x04 ) {
	  	trackType = K3bTrack::DATA;
		  if( mode == 1 )
		    trackMode = K3bTrack::MODE1;
		  else if( mode == 2 )
		    trackMode = K3bTrack::MODE2;
	  } else
		  trackType = K3bTrack::AUDIO;

	  lastTrack = K3bTrack( startSec, startSec, trackType, trackMode );

  }
  if (m_info.device->burner())
    fetchDiskInfo();

  if (m_info.tocType != K3bDiskInfo::AUDIO)
    fetchIsoInfo();
  else
    calculateDiscId();

  testForDvd();
}

void K3bDiskInfoDetector::fetchIsoInfo()
{
  char buf[17*2048];
  ::lseek( m_cdfd, 0, SEEK_SET );

  if( ::read( m_cdfd, buf, 17*2048 ) == 17*2048 ) {
    m_info.isoId = QString::fromLocal8Bit( &buf[16*2048+1], 5 ).stripWhiteSpace();
    m_info.isoSystemId = QString::fromLocal8Bit( &buf[16*2048+8], 32 ).stripWhiteSpace();
    m_info.isoVolumeId = QString::fromLocal8Bit( &buf[16*2048+40], 32 ).stripWhiteSpace();
    m_info.isoVolumeSetId = QString::fromLocal8Bit( &buf[16*2048+190], 128 ).stripWhiteSpace();
    m_info.isoPublisherId = QString::fromLocal8Bit( &buf[16*2048+318], 128 ).stripWhiteSpace();
    m_info.isoPreparerId = QString::fromLocal8Bit( &buf[16*2048+446], 128 ).stripWhiteSpace();
    m_info.isoApplicationId = QString::fromLocal8Bit( &buf[16*2048+574], 128 ).stripWhiteSpace();
  }
}


void K3bDiskInfoDetector::testForDvd()
{

  if( m_info.tocType == K3bDiskInfo::DATA && K3bTcWrapper::supportDvd() ) {
    // check if it is a dvd we can display

    if( !m_tcWrapper ) {
      kdDebug() << "(K3bDiskInfoDetector) testForDvd" << endl;
      m_tcWrapper = new K3bTcWrapper( this );
      connect( m_tcWrapper, SIGNAL(successfulDvdCheck(bool)), this, SLOT(slotIsDvd(bool)) );
    }

    m_tcWrapper->isDvdInsert( m_device );

  } else {
    finish(true);
  }
}


void K3bDiskInfoDetector::slotIsDvd( bool dvd )
{
  if( dvd ) {
    m_info.empty = false;
    m_info.noDisk = false;
    m_info.tocType = K3bDiskInfo::DVD;
  }
  finish(true);
}


void K3bDiskInfoDetector::calculateDiscId()
{
  // calculate cddb-id
  unsigned int id = 0;
  for( K3bToc::iterator it = m_info.toc.begin(); it != m_info.toc.end(); ++it ) {
    unsigned int n = (*it).firstSector() + 150;
    n /= 75;
    while( n > 0 ) {
      id += n % 10;
      n /= 10;
    }
  }
  unsigned int l = m_info.toc.lastSector() - m_info.toc.firstSector();
  l /= 75;
  id = ( ( id % 0xff ) << 24 ) | ( l << 8 ) | m_info.toc.count();
  m_info.toc.setDiscId( id );

  kdDebug() << "(K3bDiskInfoDetector) calculated disk id: " << id << endl;
}

#include "k3bdiskinfodetector.moc"
