#include "acm_drv.h"
#include "dbglog.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ACMDrv::DriverProc
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT
ACMDrv::DriverProcedure(const HDRVR hdrvr, const UINT msg, LPARAM lParam1, LPARAM lParam2)
{
  switch (msg) 
  {
  ///////////////////////////////////////////////////////
  // Driver instance messages
  ///////////////////////////////////////////////////////

  // Sent when the driver is installed.
  // Can return DRVCNF_OK, DRVCNF_CANCEL, DRV_RESTART
  case DRV_INSTALL:
    dbglog("DRV_INSTALL");
    return DRVCNF_OK;
    
  // Sent when the driver is removed.
  // Return value ignored
  case DRV_REMOVE:
    dbglog("DRV_REMOVE");
    return 1;
    
  // Sent to determine if the driver can be configured.
  // Zero indicates configuration NOT supported
  case DRV_QUERYCONFIGURE:
    dbglog("DRV_QUERYCONFIGURE");
    return 0;
    
  // Sent to display the configuration
  // dialog box for the driver.
  // Can return DRVCNF_OK, DRVCNF_CANCEL, DRVCNF_RESTART
  case DRV_CONFIGURE:
    dbglog("DRV_CONFIGURE");
    return DRVCNF_OK; // config((HWND)lParam1);
    
  /////////////////////////////////////////////////////////
  // ACM additional messages
  /////////////////////////////////////////////////////////
    
  case ACMDM_DRIVER_ABOUT:
    dbglog("ACMDM_DRIVER_ABOUT");
    return DRVCNF_OK; // about((HWND)lParam1);
 
  // acmDriverDetails
  // Fill-in general informations about the driver/codec
  case ACMDM_DRIVER_DETAILS: 
    dbglog("ACMDM_DRIVER_DETAILS");
    return on_driver_details(hdrvr, (LPACMDRIVERDETAILS)lParam1);
    
  // acmFormatTagDetails
  case ACMDM_FORMATTAG_DETAILS: 
    dbglog("ACMDM_FORMATTAG_DETAILS");
    return on_formattag_details((LPACMFORMATTAGDETAILS)lParam1, lParam2);
    
  // acmFormatDetails
  case ACMDM_FORMAT_DETAILS: 
    dbglog("ACMDM_FORMAT_DETAILS");
    return on_format_details((LPACMFORMATDETAILS) lParam1, lParam2);
    
  // acmFormatSuggest
  // Sent to determine if the driver can be configured.
  case ACMDM_FORMAT_SUGGEST: 
    dbglog("ACMDM_FORMAT_SUGGEST");
    return on_format_suggest((LPACMDRVFORMATSUGGEST)lParam1);
    
  /////////////////////////////////////////////////////////
  // ACM stream messages
  /////////////////////////////////////////////////////////
    
  // Sent to determine if the driver can be configured.
  case ACMDM_STREAM_OPEN:
    dbglog("ACMDM_STREAM_OPEN");
    return on_stream_open((LPACMDRVSTREAMINSTANCE)lParam1);
    
  // returns a recommended size for a source 
  // or destination buffer on an ACM stream
  case ACMDM_STREAM_SIZE:
    dbglog("ACMDM_STREAM_SIZE");
    return on_stream_size((LPACMDRVSTREAMINSTANCE)lParam1, (LPACMDRVSTREAMSIZE)lParam2);
    
  // prepares an ACMSTREAMHEADER structure for
  // an ACM stream conversion
  case ACMDM_STREAM_PREPARE:
    dbglog("ACMDM_STREAM_PREPARE");
    return on_stream_prepare((LPACMDRVSTREAMINSTANCE)lParam1, (LPACMSTREAMHEADER) lParam2);
    
  // cleans up the preparation performed by
  // the ACMDM_STREAM_PREPARE message for an ACM stream
  case ACMDM_STREAM_UNPREPARE:
    dbglog("ACMDM_STREAM_UNPREPARE");
    return on_stream_unprepare((LPACMDRVSTREAMINSTANCE)lParam1, (LPACMSTREAMHEADER) lParam2);
    
  // perform a conversion on the specified conversion stream
  case ACMDM_STREAM_CONVERT:
    dbglog("ACMDM_STREAM_CONVERT");
    return on_stream_convert((LPACMDRVSTREAMINSTANCE)lParam1, (LPACMDRVSTREAMHEADER) lParam2);
    
  // closes an ACM conversion stream
  case ACMDM_STREAM_CLOSE:
    dbglog("ACMDM_STREAM_CLOSE");
    return on_stream_close((LPACMDRVSTREAMINSTANCE)lParam1);
    break;
    
  /////////////////////////////////////////////////////////
  // ACM other messages
  /////////////////////////////////////////////////////////
   
  default:
    // Process any other messages.
    dbglog("ACM::DriverProc unknown message (0x%08X), lParam1 = 0x%08X, lParam2 = 0x%08X", msg, lParam1, lParam2);
    return DefDriverProc ((DWORD_PTR)this, hdrvr, msg, lParam1, lParam2);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DriverProc
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


LRESULT WINAPI
DriverProc(DWORD_PTR dwDriverId, HDRVR hdrvr, UINT msg, LPARAM lParam1, LPARAM lParam2)
{
  switch (msg)
  {
    ///////////////////////////////////////////////////////
    // Driver init messages
    // The following messages are not related to a
    // particular open instance of the driver. For these
    // messages, the dwDriverId will always be zero.

    case DRV_LOAD:
      dbglog("DRV_LOAD");
      return TRUE;
    
    case DRV_FREE:
      dbglog("DRV_FREE");
      return TRUE;
    
    case DRV_ENABLE:
      dbglog("DRV_ENABLE");
      return TRUE;
    
    case DRV_DISABLE:
      dbglog("DRV_DISABLE");
      return TRUE;
    
    ///////////////////////////////////////////////////////
    // DRV_OPEN
    // Open the driver instance
    
    case DRV_OPEN: 
    {
      dbglog("DRV_OPEN (ID 0x%08X), pDesc = 0x%08X", dwDriverId, lParam2);
      LPACMDRVOPENDESC desc = (LPACMDRVOPENDESC)lParam2;     

      if (desc)
        if (desc->fccType != ACMDRIVERDETAILS_FCCTYPE_AUDIOCODEC) 
        {
          dbglog("error: wrong desc->fccType (0x%04X)", desc->fccType);
          return FALSE;
        }

      ACMDrv* acm = make_acm(hdrvr);
      dbglog("Instance open (0x%08X)", acm);
      return (LRESULT)acm;
    }
    
    ///////////////////////////////////////////////////////
    // DRV_CLOSE
    // Close the driver instance

    case DRV_CLOSE: 
    {
      dbglog("DRV_CLOSE");
      ACMDrv *acm = (ACMDrv *)dwDriverId;
      if (acm)
      {
        delete acm;
        dbglog("Instance close (0x%08X)", acm);
        return TRUE;
      }
      else
      {
        dbglog("error: cannot close instance NULL");
        return FALSE;
      }
    }
    
    ///////////////////////////////////////////////////////
    // Other driver messages
    // Pass it to the driver instance

    default:
    {
      ACMDrv *acm = (ACMDrv *)dwDriverId;
      if (acm)
        return acm->DriverProcedure(hdrvr, msg, lParam1, lParam2);
      dbglog("warning: driver was not open, message (0x%08X), lParam1 = 0x%08X, lParam2 = 0x%08X", msg, lParam1, lParam2);
      return DefDriverProc (dwDriverId, hdrvr, msg, lParam1, lParam2);
    }
  }
}
