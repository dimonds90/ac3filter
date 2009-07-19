/*
  ACM Driver skeleton class
  You have to define make_acm() function to make the driver instance.
*/

#ifndef ACM_DRV_H
#define ACM_DRV_H

#include <windows.h>
#include <mmreg.h>
#include <msacm.h>
#include "msacmdrv.h"

class ACMDrv
{
private:
  LRESULT DriverProcedure(const HDRVR hdrvr, const UINT msg, LPARAM lParam1, LPARAM lParam2);
  friend LRESULT WINAPI DriverProc(DWORD_PTR dwDriverId, HDRVR hdrvr, UINT msg, LPARAM lParam1, LPARAM lParam2);

protected:
  virtual LRESULT about(HWND parent) = 0;
  virtual BOOL    query_configure() = 0;
  virtual LRESULT configure(HWND parent, LPDRVCONFIGINFO config_info) = 0;

  // ACM additional messages
  virtual LRESULT driver_details(const HDRVR hdrvr, LPACMDRIVERDETAILS driver_details) = 0;
  virtual LRESULT formattag_details(LPACMFORMATTAGDETAILS formattag_details, const LPARAM flags) = 0;
  virtual LRESULT format_details(LPACMFORMATDETAILS format_details, const LPARAM flags) = 0;
  virtual LRESULT format_suggest(LPACMDRVFORMATSUGGEST format_suggest) = 0;

  // ACM stream messages
  virtual LRESULT stream_open(LPACMDRVSTREAMINSTANCE stream_instance) = 0;
  virtual LRESULT stream_close(LPACMDRVSTREAMINSTANCE stream_instance) = 0;

  virtual LRESULT stream_size(LPACMDRVSTREAMINSTANCE stream_instance, LPACMDRVSTREAMSIZE stream_size) = 0;
  virtual LRESULT stream_prepare(LPACMDRVSTREAMINSTANCE stream_instance, LPACMSTREAMHEADER stream_header) = 0;
  virtual LRESULT stream_unprepare(LPACMDRVSTREAMINSTANCE stream_instance, LPACMSTREAMHEADER stream_header) = 0;
  virtual LRESULT stream_convert(LPACMDRVSTREAMINSTANCE stream_instance, LPACMDRVSTREAMHEADER stream_header) = 0;

public:
  ACMDrv() {};
  virtual ~ACMDrv() {};

  void *operator new(size_t size);
  void operator delete(void *ptr);
};

extern ACMDrv *make_acm(HDRVR hdrvr, LPACMDRVOPENDESC pado);

#endif
