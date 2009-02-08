#include <windows.h>
#include <mmreg.h>
#include <msacm.h>
#include <new>
#include "msacmdrv.h"

#include "dbglog.h"
#include "decoder.h"
#include "win32\winspk.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global defines
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define VERSION_ACM     MAKE_ACM_VERSION(3, 51, 0)
#define VERSION_DRIVER  MAKE_ACM_VERSION(0, 2, 0)

#define WAVE_FORMAT_AVI_AC3 0x2000
#define WAVE_FORMAT_AVI_DTS 0x2001

#define BUFFER_SIZE 4096

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Override memory allocation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void *operator new(size_t size)
{
  void *ptr = LocalAlloc(LPTR, size);

#if _MSC_VER >= 1400
  // Looks like Visual C++ CRT relies on std::bad_alloc exception,
  // because it crashes without it.
  //
  // To reproduce the crach we need:
  // * Visual Studio 2008
  // * Multithreaded static release CRT (debug CRT works well)
  // * Compile and install the ACM
  // * Run Windows media player and choose Help->About->Technical support information

  if (ptr == 0) throw std::bad_alloc();
#endif

  return ptr;
}
extern void  operator delete(void *block)  { LocalFree(block); }

extern void *malloc(size_t size) { return LocalAlloc(LPTR, size); }
extern void free(void *block)    { LocalFree(block); }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formats
//
// We should publish a list of supported formats. But there're many different
// conversions between formats supported and the WAVEFORMATEX structure.
//
// So we should take in account input format conversion sequence:
// (it is true only if application does format enumeration)
// 1) we publish a list of supported input formats
// 2) application enumerates it and compare with the format given to it
// 3) if formats match application can use our driver
//
// Therefore we must publish a list of input formats in the way as it used in
// real media files. Usually for AC3/DTS formats WAVEFORMATEXTENSIBLE structure
// is not used and so we may not publish it. Following fields may change:
//
// * Number of channels
//     This field may be unspecified so we should publish zero number of
//     channels too.
//
// * Sample rate
//     Sample rate must be specified because otherwise we cannot determine 
//     output format. So we should publish only correct sample rates.
//
// * Bitrate
//     This field may be unspecified so we should publish zero bitrate too.
//
// * Block algin
//     Use only 1-byte block align.
//
// * Bits per sample
//     Use only 0 bits per sample.
//
// It is another sequence for output format:
// (it is true only if application does format enumeration)
// 1) we publish a list of supported input and output formats
// 2) application chooses input format (see above)
// 3) application enumerates output format and chooses one that is supported
//    by both application and driver.
//
// Bit per sample can vary for PCM formats so we need to enumerate it too.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// FormatTag
// Structure that holds format tag info, including format enumeration info.
///////////////////////////////////////////////////////////////////////////////

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

    dbglog("FormatTag::format_details(): tag: 0x%04X channles: %i sample rate: %iHz bits/sample: %i bytes/sec: %i", wfx->wFormatTag, wfx->nChannels, wfx->nSamplesPerSec, wfx->wBitsPerSample, wfx->nAvgBytesPerSec);
    return true;
  }

  size_t nformats() const
  {
    return (size_t) nsample_rates * nchannels * nbitrates * nbps;
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Description of supported formats
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const int ac3_sample_rate[] = 
{ 
  48000, 44100, 32000, 
  24000, 22050, 16000, 
  12000, 11025,  8000 
};
static const int dts_sample_rate[] =
{
   8000, 16000, 32000,
  11025, 22050, 44100,
  12000, 24000, 48000, 96000, 192000
};
// PCM should contain all frequencies from other formats
static const int pcm_sample_rate[] =
{
   8000, 16000, 32000,
  11025, 22050, 44100,
  12000, 24000, 48000, 96000, 192000
};

static const int ac3_channels[] =
{ 
  0,            // support undefined mode (0 channels)
  MODE_1_0,     // 1 channel
  MODE_2_0,     // 2 channels
  MODE_2_1,     // 3 channels
  MODE_2_2,     // 4 channels
  MODE_3_2,     // 5 channels
  MODE_3_2_LFE, // 6 channels
};

static const int dts_channels[] =
{ 
  0,            // support undefined mode (0 channels)
  MODE_1_0,     // 1 channel
  MODE_2_0,     // 2 channels
  MODE_2_1,     // 3 channels
  MODE_2_2,     // 4 channels
  MODE_3_2,     // 5 channels
  MODE_3_2_LFE, // 6 channels
};

static const int ac3_bitrate[] =
{
       0, // support undefined bitrate
   32000,  40000,  48000,  56000,  64000,  80000,  96000, 112000, 128000, 
  160000, 192000, 224000, 256000, 320000, 384000, 448000, 512000, 576000, 640000 
};

static const int dts_bitrate[] =
{
        0, // support undefined bitrate
    32000,   56000,   64000,   96000,  112000,
   128000,  192000,  224000,  256000,  320000,
   384000,  448000,  512000,  576000,  640000,
   768000,  896000, 1024000, 1152000, 1280000,
  1344000, 1408000, 1411200, 1472000, 1536000,
  1920000, 2048000, 3072000, 3840000
};

static const int pcm_channels[] =
{
  MODE_STEREO,  // 1 channel
  MODE_MONO,    // 2 channel
};

static const int pcm_bps[] =
{
  16
};

const FormatTag tags[] =
{
  { 
    0, L"AC3", WAVE_FORMAT_AVI_AC3, 
    ac3_sample_rate, array_size(ac3_sample_rate), 
    ac3_channels, array_size(ac3_channels), 
    ac3_bitrate, array_size(ac3_bitrate), 
    0, 1 
  },
  { 
    1, L"DTS", WAVE_FORMAT_AVI_DTS, 
    dts_sample_rate, array_size(dts_sample_rate), 
    dts_channels, array_size(dts_channels), 
    dts_bitrate, array_size(dts_bitrate), 
    0, 1 
  },
  { 
    2, 0, WAVE_FORMAT_PCM, 
    pcm_sample_rate, array_size(pcm_sample_rate), 
    pcm_channels, array_size(pcm_channels), 
    0, 1, 
    pcm_bps, array_size(pcm_bps) 
  },
};

const int ntags = array_size(tags);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ACM
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ACM
{
protected:
  HMODULE hmodule;
  LRESULT DriverProcedure(const HDRVR hdrvr, const UINT msg, LONG lParam1, LONG lParam2);

  inline DWORD about(HWND parent);
  inline DWORD config(HWND parent);

  // ACM additional messages
  inline DWORD on_driver_details(const HDRVR hdrvr, LPACMDRIVERDETAILS driver_details);
  inline DWORD on_formattag_details(LPACMFORMATTAGDETAILS formattag_details, const LPARAM flags);
  inline DWORD on_format_details(LPACMFORMATDETAILS format_details, const LPARAM flags);
  inline DWORD on_format_suggest(LPACMDRVFORMATSUGGEST format_suggest);

  // ACM stream messages
  inline DWORD on_stream_open(LPACMDRVSTREAMINSTANCE stream_instance);
  inline DWORD on_stream_close(LPACMDRVSTREAMINSTANCE stream_instance);

  inline DWORD on_stream_size(LPACMDRVSTREAMINSTANCE stream_instance, LPACMDRVSTREAMSIZE stream_size);
  inline DWORD on_stream_prepare(LPACMDRVSTREAMINSTANCE stream_instance, LPACMSTREAMHEADER stream_header);
  inline DWORD on_stream_unprepare(LPACMDRVSTREAMINSTANCE stream_instance, LPACMSTREAMHEADER stream_header);
  inline DWORD on_stream_convert(LPACMDRVSTREAMINSTANCE stream_instance, LPACMDRVSTREAMHEADER stream_header);

  friend LRESULT WINAPI DriverProc(DWORD dwDriverId, HDRVR hdrvr, UINT msg, LONG lParam1, LONG lParam2);

public:
  ACM(HMODULE hmodule);
  ~ACM();
};


ACM::ACM(HMODULE _hmodule)
{
  hmodule = _hmodule;
}
ACM::~ACM()
{
}



DWORD 
ACM::about(HWND parent)
{
  MessageBox(parent, "Copyright (c) 2007 by Alexander Vigovsky", "About", MB_OK);
  return DRVCNF_OK;
}

DWORD 
ACM::config(HWND parent)
{
  MessageBox(parent, "Sorry, configuration is not implemented", "Configuration", MB_OK);
  return DRVCNF_OK;
}

DWORD 
ACM::on_driver_details(const HDRVR hdrvr, LPACMDRIVERDETAILS driver_details)
{
  driver_details->hicon       = 0;
  driver_details->fccType     = ACMDRIVERDETAILS_FCCTYPE_AUDIOCODEC;
  driver_details->fccComp     = ACMDRIVERDETAILS_FCCCOMP_UNDEFINED;
  driver_details->wMid        = 0;
  driver_details->wPid        = 0;
  driver_details->vdwACM      = VERSION_ACM;
  driver_details->vdwDriver   = VERSION_DRIVER;
  driver_details->fdwSupport  = ACMDRIVERDETAILS_SUPPORTF_CODEC;
  driver_details->cFormatTags = ntags;
  driver_details->cFilterTags = 0;

  lstrcpyW(driver_details->szShortName, L"AC3Filter ACM codec");
  lstrcpyW(driver_details->szLongName,  L"AC3Filter ACM codec");
  lstrcpyW(driver_details->szCopyright, L"2007 Alexander Vigovsky");
  lstrcpyW(driver_details->szLicensing, L"GPL ver 2");
  lstrcpyW(driver_details->szFeatures,  L"Only stereo decoding");

  return MMSYSERR_NOERROR;
}

DWORD 
ACM::on_formattag_details(LPACMFORMATTAGDETAILS formattag_details, const LPARAM flags)
{
  if (formattag_details->cbStruct < sizeof(ACMFORMATTAGDETAILS))
  {
    dbglog("ACM::on_formattag_details() error: formattag_details->cbStruct < sizeof(formattag_details)");
    return MMSYSERR_INVALPARAM;
  }

  /////////////////////////////////////////////////////////
  // Find format tag index
  // Check parameters

  int itag = -1;
  switch (flags & ACM_FORMATTAGDETAILSF_QUERYMASK)
  {
    case ACM_FORMATTAGDETAILSF_INDEX:
      // formattag_details->dwFormatTagIndex is given
      dbglog("ACM::on_formattag_details(): Details for format tag index = %i", formattag_details->dwFormatTagIndex);

      if (formattag_details->dwFormatTagIndex >= ntags)
      {
        dbglog("ACM::on_formattag_details() warning: unsupported format tag index #%i", formattag_details->dwFormatTagIndex);
        return ACMERR_NOTPOSSIBLE;
      }

      itag = formattag_details->dwFormatTagIndex;

      break; // case ACM_FORMATTAGDETAILSF_INDEX:

    case ACM_FORMATTAGDETAILSF_FORMATTAG:
      // formattag_details->dwFormatTag is given
      dbglog("ACM::on_formattag_details(): Details for format tag = 0x%04X", formattag_details->dwFormatTag);

      for (itag = 0; itag < ntags; itag++)
        if (tags[itag].format_tag == formattag_details->dwFormatTag)
          break;

      if (itag >= ntags)
      {
        dbglog("ACM::on_formattag_details() warning: unsupported format tag 0x%04X", formattag_details->dwFormatTag);
        return ACMERR_NOTPOSSIBLE;
      }

      break; // case ACM_FORMATTAGDETAILSF_FORMATTAG:

    case ACM_FORMATTAGDETAILSF_LARGESTSIZE:
      itag = 0;
      break; // case ACM_FORMATTAGDETAILSF_FORMATTAG:

    default:
      dbglog("ACM::on_formattag_details() error: unknown flag: 0x%08X", flags);
      return ACMERR_NOTPOSSIBLE;

  } // switch (flags & ACM_FORMATTAGDETAILSF_QUERYMASK)

  /////////////////////////////////////////////////////////
  // Fill format tag details

  tags[itag].formattag_details(formattag_details);
  return MMSYSERR_NOERROR;
}

DWORD 
ACM::on_format_details(LPACMFORMATDETAILS format_details, const LPARAM flags)
{
  if (format_details->cbStruct < sizeof(ACMFORMATDETAILS))
  {
    dbglog("ACM::on_format_details() error: format_details->cbStruct < sizeof(ACMFORMATDETAILS)");
    return MMSYSERR_INVALPARAM;
  }

  if (format_details->cbwfx < sizeof(WAVEFORMATEX))
  {
    dbglog("ACM::on_format_details() error: format_detils->cbwfx < sizeof(WAVEOFORMATEX)");
    return ACMERR_NOTPOSSIBLE;
  }

  /////////////////////////////////////////////////////////
  // Find format tag index
  // Check parameters

  int itag = -1;
  switch (flags & ACM_FORMATDETAILSF_QUERYMASK)
  {
    ///////////////////////////////////////////////////////
    // dwFormatTag and dwFormatTagIndex is given. 
    // Fill following:
    // fdwSupport, pwfx, szFormat (optional)

    case ACM_FORMATDETAILSF_INDEX:
      dbglog("ACM::on_format_details() details for format tag 0x%04X index %i", format_details->dwFormatTag, format_details->dwFormatIndex);

      for (itag = 0; itag < ntags; itag++)
        if (tags[itag].format_tag == format_details->dwFormatTag)
          break;

      if (itag >= ntags)
      {
        dbglog("ACM::on_format_details() warning: unsupported format tag 0x%04X", format_details->dwFormatTag);
        return ACMERR_NOTPOSSIBLE;
      }

      if (format_details->dwFormatIndex > tags[itag].nformats())
      {
        dbglog("ACM::on_format_details() error: unknown index %i for format tag %i", format_details->dwFormatIndex, format_details->dwFormatTag);
        return ACMERR_NOTPOSSIBLE;
      }

      tags[itag].format_details(format_details);
      return MMSYSERR_NOERROR;

    case ACM_FORMATDETAILSF_FORMAT:
      format_details->fdwSupport = ACMDRIVERDETAILS_SUPPORTF_CODEC;
      // todo: add description string format_details->szFormat 
      // todo: format check?
      return MMSYSERR_NOERROR;

    default:
      dbglog("ACM::on_formattag_details() error: unknown flag: 0x%08X", flags);
      return ACMERR_NOTPOSSIBLE;
  }

  // note: we should never be here
}

DWORD 
ACM::on_format_suggest(LPACMDRVFORMATSUGGEST format_suggest)
{
  DWORD suggest = (ACM_FORMATSUGGESTF_TYPEMASK & format_suggest->fdwSuggest);
  WAVEFORMATEX *src = format_suggest->pwfxSrc;
  WAVEFORMATEX *dst = format_suggest->pwfxDst;
 
  dbglog("Suggest %s%s%s%s (0x%08X)",
    (suggest & ACM_FORMATSUGGESTF_NCHANNELS) ? "channels, ":"",
    (suggest & ACM_FORMATSUGGESTF_NSAMPLESPERSEC) ? "samples/sec, ":"",
    (suggest & ACM_FORMATSUGGESTF_WBITSPERSAMPLE) ? "bits/sample, ":"",
    (suggest & ACM_FORMATSUGGESTF_WFORMATTAG) ? "format, ":"",
    suggest);
 
  dbglog("Suggest for source format = 0x%04X, channels = %d, Samples/s = %d, AvgB/s = %d, BlockAlign = %d, b/sample = %d",
    src->wFormatTag,
    src->nChannels,
    src->nSamplesPerSec,
    src->nAvgBytesPerSec,
    src->nBlockAlign,
    src->wBitsPerSample);

  dbglog("Suggest for destination format = 0x%04X, channels = %d, Samples/s = %d, AvgB/s = %d, BlockAlign = %d, b/sample = %d",
    dst->wFormatTag,
    dst->nChannels,
    dst->nSamplesPerSec,
    dst->nAvgBytesPerSec,
    dst->nBlockAlign,
    dst->wBitsPerSample);
  
  switch (src->wFormatTag)
  {
    ///////////////////////////////////////////////////////
    // Suggest decompression format

    case WAVE_FORMAT_AVI_AC3:
    case WAVE_FORMAT_AVI_DTS:
      if (suggest & ACM_FORMATSUGGESTF_WFORMATTAG)
      {
        if (dst->wFormatTag != WAVE_FORMAT_PCM)
          return ACMERR_NOTPOSSIBLE;
      }
      else
        dst->wFormatTag = WAVE_FORMAT_PCM;

      if (suggest & ACM_FORMATSUGGESTF_NCHANNELS)
      {
        if (dst->nChannels != 1 && dst->nChannels != 2)
          return ACMERR_NOTPOSSIBLE;
      }
      else
        dst->nChannels = src->nChannels > 1? 2: 1;

      if (suggest & ACM_FORMATSUGGESTF_NSAMPLESPERSEC)
      {
        if (src->nSamplesPerSec != dst->nSamplesPerSec)
          return ACMERR_NOTPOSSIBLE;
      }
      else
        dst->nSamplesPerSec = src->nSamplesPerSec;

      if (suggest & ACM_FORMATSUGGESTF_WBITSPERSAMPLE)
      {
        if (dst->wBitsPerSample != 16)
          return ACMERR_NOTPOSSIBLE;
      }
      else
        dst->wBitsPerSample = 16;

      dst->nBlockAlign = dst->nChannels * dst->wBitsPerSample / 8;
      dst->nAvgBytesPerSec = dst->nSamplesPerSec * dst->nBlockAlign;
      dst->cbSize = 0;
      break;

    ///////////////////////////////////////////////////////
    // Suggest compression format

    case WAVE_FORMAT_PCM:
      return ACMERR_NOTPOSSIBLE;

    default:
      return ACMERR_NOTPOSSIBLE;
  }
 

  dbglog("Suggested destination format = 0x%04X, channels = %d, Samples/s = %d, AvgB/s = %d, BlockAlign = %d, b/sample = %d",
    dst->wFormatTag,
    dst->nChannels,
    dst->nSamplesPerSec,
    dst->nAvgBytesPerSec,
    dst->nBlockAlign,
    dst->wBitsPerSample);

  return MMSYSERR_NOERROR;
}


DWORD 
ACM::on_stream_open(LPACMDRVSTREAMINSTANCE stream_instance)
{
  //  the most important condition to check before doing anything else
  //  is that this ACM driver can actually perform the conversion we are
  //  being opened for. this check should fail as quickly as possible
  //  if the conversion is not possible by this driver.
  //
  //  it is VERY important to fail quickly so the ACM can attempt to
  //  find a driver that is suitable for the conversion. also note that
  //  the ACM may call this driver several times with slightly different
  //  format specifications before giving up.

  WAVEFORMATEX *src = stream_instance->pwfxSrc;
  WAVEFORMATEX *dst = stream_instance->pwfxDst;
  Speakers in_spk;
  Speakers out_spk;

  if (src->wFormatTag != WAVE_FORMAT_AVI_AC3 &&
      src->wFormatTag != WAVE_FORMAT_AVI_DTS)
    return ACMERR_NOTPOSSIBLE;

  if (dst->wFormatTag != WAVE_FORMAT_PCM)
    return ACMERR_NOTPOSSIBLE;

  if (stream_instance->fdwOpen & ACM_STREAMOPENF_ASYNC)
  {
    dbglog("ACM::on_stream_open() error: async mode is not supported");
    return ACMERR_NOTPOSSIBLE;
  }

  if (!wfx2spk(src, in_spk))
  {
    dbglog("ACM::on_stream_open() error: wrong input format");
    return ACMERR_NOTPOSSIBLE;
  }

  if (!wfx2spk(dst, out_spk))
  {
    dbglog("ACM::on_stream_open() error: wrong output format");
    return ACMERR_NOTPOSSIBLE;
  }

  if ((stream_instance->fdwOpen & ACM_STREAMOPENF_QUERY) == 0)
  {
    StreamDecoder *dec = new StreamDecoder;
    if (dec->open(in_spk, out_spk))
      stream_instance->dwInstance = (DWORD)dec;  
    else
    {
      dbglog("ACM::on_stream_open() error: cannot open stream");
      return ACMERR_NOTPOSSIBLE;
    }
  }

  return MMSYSERR_NOERROR;
}

DWORD 
ACM::on_stream_close(LPACMDRVSTREAMINSTANCE stream_instance)
{
  StreamDecoder *dec = (StreamDecoder *)stream_instance->dwInstance;
  delete dec;
  stream_instance->dwInstance = 0;
  return MMSYSERR_NOERROR;
}

DWORD 
ACM::on_stream_size(LPACMDRVSTREAMINSTANCE stream_instance, LPACMDRVSTREAMSIZE stream_size)
{
  // Most common AC3 format is 5.1 48kHz 448kbps
  // It is about 1:10 compression ratio, so we can 
  // use this ration between input and output buffers
  // to use buffers in optimal way. But limit minimal 
  // buffer size to 2048 bytes

  switch (stream_size->fdwSize & ACM_STREAMSIZEF_QUERYMASK)
  {
    case ACM_STREAMSIZEF_DESTINATION:
    {
      stream_size->cbSrcLength = stream_size->cbDstLength / 10 + 2048;
      return MMSYSERR_NOERROR;
    }

    case ACM_STREAMSIZEF_SOURCE:
    {
      stream_size->cbDstLength = stream_size->cbSrcLength * 10 + 2048;
      return MMSYSERR_NOERROR;
    }

    default:
      return MMSYSERR_INVALFLAG;
  }
}

DWORD 
ACM::on_stream_prepare(LPACMDRVSTREAMINSTANCE stream_instance, LPACMSTREAMHEADER stream_header)
{
  dbglog("ACM::on_stream_prepare(): Src = %d (0x%04X) / %d\tDst = %d (0x%04X) / %d",
    stream_header->cbSrcLength,
    stream_header->pbSrc,
    stream_header->cbSrcLengthUsed,
    stream_header->cbDstLength,
    stream_header->pbDst,
    stream_header->cbDstLengthUsed);

  return MMSYSERR_NOERROR;
}

DWORD 
ACM::on_stream_unprepare(LPACMDRVSTREAMINSTANCE stream_instance, LPACMSTREAMHEADER stream_header)
{
  dbglog("ACM::on_stream_unprepare(): Src = %d / %d\tDst = %d / %d",
    stream_header->cbSrcLength,
    stream_header->cbSrcLengthUsed,
    stream_header->cbDstLength,
    stream_header->cbDstLengthUsed);

  // todo: flush
  return MMSYSERR_NOERROR;  
}

DWORD 
ACM::on_stream_convert(LPACMDRVSTREAMINSTANCE stream_instance, LPACMDRVSTREAMHEADER stream_header)
{
  StreamDecoder *dec = (StreamDecoder *)stream_instance->dwInstance;
  stream_header->cbSrcLengthUsed = 0;
  stream_header->cbDstLengthUsed = 0;

  size_t gone_src = 0;
  size_t gone_dst = 0;
  while (stream_header->cbSrcLength > stream_header->cbSrcLengthUsed && stream_header->cbDstLength > stream_header->cbDstLengthUsed)
    if (dec->decode(
      (uint8_t*)stream_header->pbSrc + stream_header->cbSrcLengthUsed, stream_header->cbSrcLength - stream_header->cbSrcLengthUsed, 
      (uint8_t*)stream_header->pbDst + stream_header->cbDstLengthUsed, stream_header->cbDstLength - stream_header->cbDstLengthUsed, 
      &gone_src, &gone_dst))
    {
      stream_header->cbSrcLengthUsed += (DWORD) gone_src;
      stream_header->cbDstLengthUsed += (DWORD) gone_dst;
      gone_src = 0;
      gone_dst = 0;
    }
    else
    {
      // What to do in case of errors?
      stream_header->cbSrcLengthUsed += (DWORD) gone_src;
      stream_header->cbDstLengthUsed += (DWORD) gone_dst;
      gone_src = 0;
      gone_dst = 0;
    }

  return MMSYSERR_NOERROR;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ACM::DriverProc
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT
ACM::DriverProcedure(const HDRVR hdrvr, const UINT msg, LPARAM lParam1, LPARAM lParam2)
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
      if (lParam2)
        dbglog("DRV_OPEN (ID 0x%08X), pDesc = 0x%08X", dwDriverId, lParam2);
      else
        dbglog("DRV_OPEN (ID 0x%08X), pDesc = NULL", dwDriverId);

      if (lParam2) 
      {
        LPACMDRVOPENDESC desc = (LPACMDRVOPENDESC)lParam2;
        
        if (desc->fccType != ACMDRIVERDETAILS_FCCTYPE_AUDIOCODEC) 
        {
          dbglog("error: wrong desc->fccType (0x%04X)", desc->fccType);
          return FALSE;
        }
      } 
      
      ACM* acm = new ACM(GetDriverModuleHandle(hdrvr));
      dbglog("Instance open (0x%08X)", acm);
      return (LPARAM)acm;
    }
    
    ///////////////////////////////////////////////////////
    // DRV_CLOSE
    // Close the driver instance

    case DRV_CLOSE: 
    {
      dbglog("DRV_CLOSE");
      
      ACM *acm = (ACM *)dwDriverId;
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
      break;
    }
    
    ///////////////////////////////////////////////////////
    // Other driver messages
    // Pass it to the driver instance

    default:
    {
      ACM *acm = (ACM *)dwDriverId;

      if (acm)
        return acm->DriverProcedure(hdrvr, msg, lParam1, lParam2);

      dbglog("warning: driver was not open, message (0x%08X), lParam1 = 0x%08X, lParam2 = 0x%08X", msg, lParam1, lParam2);
      return DefDriverProc (dwDriverId, hdrvr, msg, lParam1, lParam2);
    }
  }
}
