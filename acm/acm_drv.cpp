#include "acm_drv.h"
#include "dbglog.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory allocation
//
// A driver instance may be closed AFTER the driver module was unloaded from the process'es memory. I.e. init/uninit
// sequence looks like:
//
// * DLLMain(DLL_PROCESS_ATTACH)
//   On this step CRT initializes and can be used afterwards.
// * DriverProc(DRV_LOAD)
// * DriverProc(DRV_OPEN)
//   Here we allocate the driver instance
// * DLLMain(DLL_PROCESS_DETACH)
//   On this step CRT shuts down and destroys memory heap.
// * DriverProc(DRV_CLOSE)
//   Here we must deallocate instance data
// * DriverProc(DRV_FREE)
//
// Therefore we cannot rely on CRT memory allocation in this case...
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void *
ACMDrv::operator new(size_t size)
{
  void *ptr = LocalAlloc(LPTR, size);
  return ptr;
}

void 
ACMDrv::operator delete(void *ptr, size_t size)
{
  LocalFree(ptr);
}

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
  
  // Sent to determine if the driver can be configured.
  // Zero indicates configuration NOT supported
  case DRV_QUERYCONFIGURE:
    dbglog("DRV_QUERYCONFIGURE");
    return query_configure();
    
  // Sent to display the configuration
  // dialog box for the driver.
  // Can return DRVCNF_OK, DRVCNF_CANCEL, DRVCNF_RESTART
  case DRV_CONFIGURE:
    dbglog("DRV_CONFIGURE");
    return configure((HWND)lParam1, (LPDRVCONFIGINFO)lParam2);
    
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
    return driver_details(hdrvr, (LPACMDRIVERDETAILS)lParam1);
    
  // acmFormatTagDetails
  case ACMDM_FORMATTAG_DETAILS: 
    dbglog("ACMDM_FORMATTAG_DETAILS");
    return formattag_details((LPACMFORMATTAGDETAILS)lParam1, lParam2);
    
  // acmFormatDetails
  case ACMDM_FORMAT_DETAILS: 
    dbglog("ACMDM_FORMAT_DETAILS");
    return format_details((LPACMFORMATDETAILS) lParam1, lParam2);
    
  // acmFormatSuggest
  // Sent to determine if the driver can be configured.
  case ACMDM_FORMAT_SUGGEST: 
    dbglog("ACMDM_FORMAT_SUGGEST");
    return format_suggest((LPACMDRVFORMATSUGGEST)lParam1);
    
  /////////////////////////////////////////////////////////
  // ACM stream messages
  /////////////////////////////////////////////////////////
    
  // Sent to determine if the driver can be configured.
  case ACMDM_STREAM_OPEN:
    dbglog("ACMDM_STREAM_OPEN");
    return stream_open((LPACMDRVSTREAMINSTANCE)lParam1);
    
  // returns a recommended size for a source 
  // or destination buffer on an ACM stream
  case ACMDM_STREAM_SIZE:
    dbglog("ACMDM_STREAM_SIZE");
    return stream_size((LPACMDRVSTREAMINSTANCE)lParam1, (LPACMDRVSTREAMSIZE)lParam2);
    
  // prepares an ACMSTREAMHEADER structure for
  // an ACM stream conversion
  case ACMDM_STREAM_PREPARE:
    dbglog("ACMDM_STREAM_PREPARE");
    return stream_prepare((LPACMDRVSTREAMINSTANCE)lParam1, (LPACMSTREAMHEADER) lParam2);
    
  // cleans up the preparation performed by
  // the ACMDM_STREAM_PREPARE message for an ACM stream
  case ACMDM_STREAM_UNPREPARE:
    dbglog("ACMDM_STREAM_UNPREPARE");
    return stream_unprepare((LPACMDRVSTREAMINSTANCE)lParam1, (LPACMSTREAMHEADER) lParam2);
    
  // perform a conversion on the specified conversion stream
  case ACMDM_STREAM_CONVERT:
    dbglog("ACMDM_STREAM_CONVERT");
    return stream_convert((LPACMDRVSTREAMINSTANCE)lParam1, (LPACMDRVSTREAMHEADER) lParam2);
    
  // closes an ACM conversion stream
  case ACMDM_STREAM_CLOSE:
    dbglog("ACMDM_STREAM_CLOSE");
    return stream_close((LPACMDRVSTREAMINSTANCE)lParam1);
    break;
    
  /////////////////////////////////////////////////////////
  // ACM other messages
  /////////////////////////////////////////////////////////
   
  default:
    if (msg >= ACMDM_USER)
    {
      dbglog("ACM::DriverProc unknown user message (0x%08X), lParam1 = 0x%08X, lParam2 = 0x%08X", msg, lParam1, lParam2);
      return MMSYSERR_NOTSUPPORTED;
    }
    return DefDriverProc ((DWORD_PTR)this, hdrvr, msg, lParam1, lParam2);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DriverProc
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT WINAPI
DriverProc(DWORD_PTR dwDriverId, HDRVR hdrvr, UINT msg, LPARAM lParam1, LPARAM lParam2)
{
  ACMDrv *acm = (ACMDrv *)dwDriverId;

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
    
    ///////////////////////////////////////////////////////
    // DRV_OPEN
    // Open the driver instance
    
    case DRV_OPEN: 
    {
      LPACMDRVOPENDESC desc = (LPACMDRVOPENDESC)lParam2;
      dbglog("DRV_OPEN (ID 0x%08X), pDesc = 0x%08X", dwDriverId, desc);

      if (desc) if (desc->fccType != ACMDRIVERDETAILS_FCCTYPE_AUDIOCODEC) 
      {
        dbglog("error: wrong desc->fccType (0x%04X)", desc->fccType);
        return FALSE;
      }

      acm = make_acm(hdrvr, desc);
      if (acm == 0 && desc)
        desc->dwError = MMSYSERR_NOMEM;

      if (acm == 0)
        dbglog("error: make_acm() returns zero");
      else
        dbglog("Instance open (0x%08X)", acm);

      return (LRESULT)acm;
    }
    
    ///////////////////////////////////////////////////////
    // DRV_CLOSE
    // Close the driver instance

    case DRV_CLOSE: 
    {
      dbglog("DRV_CLOSE");
      if (acm)
      {
        dbglog("Instance close (0x%08X)", acm);
        delete acm;
      }
      else
        dbglog("error: cannot close null instance");

      return TRUE;
    }
    
    ///////////////////////////////////////////////////////
    // Other driver messages
    // Pass it to the driver instance

    default:
    {
      if (acm)
        return acm->DriverProcedure(hdrvr, msg, lParam1, lParam2);
      dbglog("warning: driver was not open, message (0x%08X), lParam1 = 0x%08X, lParam2 = 0x%08X", msg, lParam1, lParam2);
      return DefDriverProc (dwDriverId, hdrvr, msg, lParam1, lParam2);
    }
  }
}


BOOL WINAPI DllMain(
  HINSTANCE hinstDLL,
  DWORD fdwReason,
  LPVOID lpvReserved
)
{
  switch (fdwReason)
  {
  case DLL_PROCESS_ATTACH:
    dbglog("DLL_PROCESS_ATTACH");
    break;

  case DLL_PROCESS_DETACH:
    dbglog("DLL_PROCESS_DETACH");
    break;
  }
  return TRUE;
}
