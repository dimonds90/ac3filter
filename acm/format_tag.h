/*
  FormatTag structure holds all info about a format tag, including all formats
  supported. Can fill format tag details structure and enumerate formats.
*/

#ifndef FORMAT_TAG_H
#define FROMAT_TAG_H

#include <windows.h>
#include <mmreg.h>
#include <msacm.h>
#include "msacmdrv.h"
#include "win32\winspk.h"
#include "log.h"

struct FormatTag
{
  int     index;          // format tag index
  LPCWSTR name;           // format name string (may be null)
  DWORD   format_tag;     // format tag

  const int *sample_rate; // allowed sample rates
  int nsample_rates;      // number of sample rates

  const int *channels;    // allowed channel configs
  int nchannels;          // number of channel configs

  const int *bitrate;     // allowed bitrates   (null for PCM formats)
  int nbitrates;          // number of bitrates (1 for PCM formats)

  const int *bps;         // allowed bit per sample   (null for non-PCM formats)
  int nbps;               // number of bit per sample (1 for non-PCM formats)

  void formattag_details(LPACMFORMATTAGDETAILS details) const
  {
    details->dwFormatTagIndex  = index;
    details->dwFormatTag       = format_tag;
    details->cbFormatSize      = sizeof(WAVEFORMATEX);
    details->fdwSupport        = ACMDRIVERDETAILS_SUPPORTF_CODEC;
    details->cStandardFormats  = (DWORD)nformats();
    details->szFormatTag[0]    = 0;
    if (name)
      lstrcpyW(details->szFormatTag, name);
  }

  bool format_details(LPACMFORMATDETAILS details) const
  {
    int i = details->dwFormatIndex;

    int ibps         = i % nbps;
    int ibitrate     = i / nbps % nbitrates;
    int isample_rate = i / nbps / nbitrates % nsample_rates;
    int ichannels    = i / nbps / nbitrates / nsample_rates;

    details->fdwSupport  = ACMDRIVERDETAILS_SUPPORTF_CODEC;
    details->szFormat[0] = 0; // let windows to fill this

    WAVEFORMATEX *wfx = details->pwfx;
    wfx->wFormatTag      = (WORD) format_tag;
    wfx->nChannels       = mask_nch(channels[ichannels]);
    wfx->nSamplesPerSec  = sample_rate[isample_rate];
    wfx->wBitsPerSample  = bps? bps[ibps]: 0;
    wfx->nAvgBytesPerSec = bitrate? bitrate[ibitrate] / 8: wfx->wBitsPerSample * wfx->nSamplesPerSec / 8;
    wfx->cbSize          = 0;

    valib_log(log_event, "FormatTag", "format_details(): tag: 0x%04X channles: %i sample rate: %iHz bits/sample: %i bytes/sec: %i", wfx->wFormatTag, wfx->nChannels, wfx->nSamplesPerSec, wfx->wBitsPerSample, wfx->nAvgBytesPerSec);
    return true;
  }

  size_t nformats() const
  {
    return (size_t) nsample_rates * nchannels * nbitrates * nbps;
  }
};

#endif
