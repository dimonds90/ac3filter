#include <windows.h>
#include <mmreg.h>
#include <msacm.h>
#include "msacmdrv.h"

#include "acm_drv.h"
#include "format_tag.h"

#include "log.h"
#include "decoder.h"
#include "win32\winspk.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global defines
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const std::string module("AC3FilterACM");

#define VERSION_ACM     MAKE_ACM_VERSION(3, 51, 0)
#define VERSION_DRIVER  MAKE_ACM_VERSION(0, 2, 0)

#define WAVE_FORMAT_AVI_AC3 0x2000
#define WAVE_FORMAT_AVI_DTS 0x2001

#define BUFFER_SIZE 4096

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
    0, L"PCM", WAVE_FORMAT_PCM, 
    pcm_sample_rate, array_size(pcm_sample_rate), 
    pcm_channels, array_size(pcm_channels), 
    0, 1, 
    pcm_bps, array_size(pcm_bps) 
  },
  { 
    1, L"AC3", WAVE_FORMAT_AVI_AC3, 
    ac3_sample_rate, array_size(ac3_sample_rate), 
    ac3_channels, array_size(ac3_channels), 
    ac3_bitrate, array_size(ac3_bitrate), 
    0, 1 
  },
  { 
    2, L"DTS", WAVE_FORMAT_AVI_DTS, 
    dts_sample_rate, array_size(dts_sample_rate), 
    dts_channels, array_size(dts_channels), 
    dts_bitrate, array_size(dts_bitrate), 
    0, 1 
  },
};

const int ntags = array_size(tags);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ACM
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AC3FilterACM : public ACMDrv
{
protected:
  virtual LRESULT about(HWND parent);
  virtual BOOL    query_configure();
  virtual LRESULT configure(HWND parent, LPDRVCONFIGINFO config_info);

  // ACM additional messages
  virtual LRESULT driver_details(const HDRVR hdrvr, LPACMDRIVERDETAILS driver_details);
  virtual LRESULT formattag_details(LPACMFORMATTAGDETAILS formattag_details, const LPARAM flags);
  virtual LRESULT format_details(LPACMFORMATDETAILS format_details, const LPARAM flags);
  virtual LRESULT format_suggest(LPACMDRVFORMATSUGGEST format_suggest);

  // ACM stream messages
  virtual LRESULT stream_open(LPACMDRVSTREAMINSTANCE stream_instance);
  virtual LRESULT stream_close(LPACMDRVSTREAMINSTANCE stream_instance);

  virtual LRESULT stream_size(LPACMDRVSTREAMINSTANCE stream_instance, LPACMDRVSTREAMSIZE stream_size);
  virtual LRESULT stream_prepare(LPACMDRVSTREAMINSTANCE stream_instance, LPACMSTREAMHEADER stream_header);
  virtual LRESULT stream_unprepare(LPACMDRVSTREAMINSTANCE stream_instance, LPACMSTREAMHEADER stream_header);
  virtual LRESULT stream_convert(LPACMDRVSTREAMINSTANCE stream_instance, LPACMDRVSTREAMHEADER stream_header);

public:
  AC3FilterACM() {};
  virtual ~AC3FilterACM() {};
};

ACMDrv *make_acm(HDRVR hdrvr, LPACMDRVOPENDESC pado)
{
  return new AC3FilterACM();
}

///////////////////////////////////////////////////////////////////////////////
// Config and about
///////////////////////////////////////////////////////////////////////////////

LRESULT
AC3FilterACM::about(HWND parent)
{
  MessageBox(parent, "Copyright (c) 2007-2013 by Alexander Vigovsky", "About", MB_OK);
  return DRVCNF_OK;
}

BOOL
AC3FilterACM::query_configure()
{
  return FALSE;
}

LRESULT
AC3FilterACM::configure(HWND parent, LPDRVCONFIGINFO config_info)
{
  MessageBox(parent, "Sorry, configuration is not implemented", "Configuration", MB_OK);
  return DRVCNF_OK;
}

///////////////////////////////////////////////////////////////////////////////
// Details and enumerations
///////////////////////////////////////////////////////////////////////////////

LRESULT 
AC3FilterACM::driver_details(const HDRVR hdrvr, LPACMDRIVERDETAILS driver_details)
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
  lstrcpyW(driver_details->szLongName,  L"Decode AC3 and DTS audio");
  lstrcpyW(driver_details->szCopyright, L"2007-2013 Alexander Vigovsky");
  lstrcpyW(driver_details->szLicensing, L"GPL");
  lstrcpyW(driver_details->szFeatures,  L"Stereo decoding only");

  return MMSYSERR_NOERROR;
}

LRESULT 
AC3FilterACM::formattag_details(LPACMFORMATTAGDETAILS formattag_details, const LPARAM flags)
{
  if (formattag_details->cbStruct < sizeof(ACMFORMATTAGDETAILS))
  {
    valib_log(log_error, module, "formattag_details(): formattag_details->cbStruct < sizeof(ACMFORMATTAGDETAILS)");
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
      valib_log(log_event, module, "formattag_details(): Details for format tag index = %i", formattag_details->dwFormatTagIndex);

      if (formattag_details->dwFormatTagIndex >= ntags)
      {
        valib_log(log_warning, module, "formattag_details(): unsupported format tag index #%i", formattag_details->dwFormatTagIndex);
        return ACMERR_NOTPOSSIBLE;
      }

      itag = formattag_details->dwFormatTagIndex;

      break; // case ACM_FORMATTAGDETAILSF_INDEX:

    case ACM_FORMATTAGDETAILSF_FORMATTAG:
      // formattag_details->dwFormatTag is given
      valib_log(log_event, module, "formattag_details(): Details for format tag = 0x%04X", formattag_details->dwFormatTag);

      for (itag = 0; itag < ntags; itag++)
        if (tags[itag].format_tag == formattag_details->dwFormatTag)
          break;

      if (itag >= ntags)
      {
        valib_log(log_warning, module, "formattag_details(): unsupported format tag 0x%04X", formattag_details->dwFormatTag);
        return ACMERR_NOTPOSSIBLE;
      }

      break; // case ACM_FORMATTAGDETAILSF_FORMATTAG:

    case ACM_FORMATTAGDETAILSF_LARGESTSIZE:
      itag = 0;
      break; // case ACM_FORMATTAGDETAILSF_FORMATTAG:

    default:
      valib_log(log_error, module, "formattag_details(): unknown flag: 0x%08X", flags);
      return ACMERR_NOTPOSSIBLE;

  } // switch (flags & ACM_FORMATTAGDETAILSF_QUERYMASK)

  /////////////////////////////////////////////////////////
  // Fill format tag details

  tags[itag].formattag_details(formattag_details);
  return MMSYSERR_NOERROR;
}

LRESULT 
AC3FilterACM::format_details(LPACMFORMATDETAILS format_details, const LPARAM flags)
{
  if (format_details->cbStruct < sizeof(ACMFORMATDETAILS))
  {
    valib_log(log_error, module, "format_details(): format_details->cbStruct < sizeof(ACMFORMATDETAILS)");
    return MMSYSERR_INVALPARAM;
  }

  if (format_details->cbwfx < sizeof(WAVEFORMATEX))
  {
    valib_log(log_error, module, "format_details(): format_detils->cbwfx < sizeof(WAVEOFORMATEX)");
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
      valib_log(log_event, module, "format_details(): details for format tag 0x%04X index %i", format_details->dwFormatTag, format_details->dwFormatIndex);

      for (itag = 0; itag < ntags; itag++)
        if (tags[itag].format_tag == format_details->dwFormatTag)
          break;

      if (itag >= ntags)
      {
        valib_log(log_warning, module, "format_details(): unsupported format tag 0x%04X", format_details->dwFormatTag);
        return ACMERR_NOTPOSSIBLE;
      }

      if (format_details->dwFormatIndex > tags[itag].nformats())
      {
        valib_log(log_error, module, "format_details(): unknown index %i for format tag %i", format_details->dwFormatIndex, format_details->dwFormatTag);
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
      valib_log(log_error, module, "formattag_details(): unknown flag: 0x%08X", flags);
      return ACMERR_NOTPOSSIBLE;
  }

  // note: we should never be here
}

LRESULT 
AC3FilterACM::format_suggest(LPACMDRVFORMATSUGGEST format_suggest)
{
  DWORD suggest = (ACM_FORMATSUGGESTF_TYPEMASK & format_suggest->fdwSuggest);
  WAVEFORMATEX *src = format_suggest->pwfxSrc;
  WAVEFORMATEX *dst = format_suggest->pwfxDst;

  valib_log(log_event, module, "Suggest %s%s%s%s (0x%08X)",
    (suggest & ACM_FORMATSUGGESTF_NCHANNELS) ? "channels, ":"",
    (suggest & ACM_FORMATSUGGESTF_NSAMPLESPERSEC) ? "samples/sec, ":"",
    (suggest & ACM_FORMATSUGGESTF_WBITSPERSAMPLE) ? "bits/sample, ":"",
    (suggest & ACM_FORMATSUGGESTF_WFORMATTAG) ? "format, ":"",
    suggest);

  valib_log(log_event, module, "Suggest for source format = 0x%04X, channels = %d, Samples/s = %d, AvgB/s = %d, BlockAlign = %d, b/sample = %d",
    src->wFormatTag,
    src->nChannels,
    src->nSamplesPerSec,
    src->nAvgBytesPerSec,
    src->nBlockAlign,
    src->wBitsPerSample);

  valib_log(log_event, module, "Suggest for destination format = 0x%04X, channels = %d, Samples/s = %d, AvgB/s = %d, BlockAlign = %d, b/sample = %d",
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
 

  valib_log(log_event, module, "Suggested destination format = 0x%04X, channels = %d, Samples/s = %d, AvgB/s = %d, BlockAlign = %d, b/sample = %d",
    dst->wFormatTag,
    dst->nChannels,
    dst->nSamplesPerSec,
    dst->nAvgBytesPerSec,
    dst->nBlockAlign,
    dst->wBitsPerSample);

  return MMSYSERR_NOERROR;
}

///////////////////////////////////////////////////////////////////////////////
// Stream functions
///////////////////////////////////////////////////////////////////////////////

LRESULT 
AC3FilterACM::stream_open(LPACMDRVSTREAMINSTANCE stream_instance)
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
    valib_log(log_error, module, "stream_open(): async mode is not supported");
    return ACMERR_NOTPOSSIBLE;
  }

  in_spk = wf2spk(src, sizeof(WAVEFORMATEX) + src->cbSize);
  if (in_spk.is_unknown())
  {
    valib_log(log_error, module, "stream_open(): wrong input format");
    return ACMERR_NOTPOSSIBLE;
  }

  out_spk = wf2spk(dst, sizeof(WAVEFORMATEX) + dst->cbSize);
  if (out_spk.is_unknown())
  {
    valib_log(log_error, module, "stream_open(): wrong output format");
    return ACMERR_NOTPOSSIBLE;
  }

  if ((stream_instance->fdwOpen & ACM_STREAMOPENF_QUERY) == 0)
  {
    StreamDecoder *dec = new StreamDecoder;
    if (dec->open(in_spk, out_spk))
      stream_instance->dwInstance = (LRESULT)dec;  
    else
    {
      valib_log(log_error, module, "stream_open(): cannot open stream");
      return ACMERR_NOTPOSSIBLE;
    }
  }

  return MMSYSERR_NOERROR;
}

LRESULT 
AC3FilterACM::stream_close(LPACMDRVSTREAMINSTANCE stream_instance)
{
  StreamDecoder *dec = (StreamDecoder *)stream_instance->dwInstance;
  delete dec;
  stream_instance->dwInstance = 0;
  return MMSYSERR_NOERROR;
}

LRESULT 
AC3FilterACM::stream_size(LPACMDRVSTREAMINSTANCE stream_instance, LPACMDRVSTREAMSIZE stream_size)
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

LRESULT 
AC3FilterACM::stream_prepare(LPACMDRVSTREAMINSTANCE stream_instance, LPACMSTREAMHEADER stream_header)
{
  valib_log(log_event, module, "stream_prepare(): Src = %d (0x%04X) / %d\tDst = %d (0x%04X) / %d",
    stream_header->cbSrcLength,
    stream_header->pbSrc,
    stream_header->cbSrcLengthUsed,
    stream_header->cbDstLength,
    stream_header->pbDst,
    stream_header->cbDstLengthUsed);

  return MMSYSERR_NOERROR;
}

LRESULT 
AC3FilterACM::stream_unprepare(LPACMDRVSTREAMINSTANCE stream_instance, LPACMSTREAMHEADER stream_header)
{
  valib_log(log_event, module, "stream_unprepare(): Src = %d / %d\tDst = %d / %d",
    stream_header->cbSrcLength,
    stream_header->cbSrcLengthUsed,
    stream_header->cbDstLength,
    stream_header->cbDstLengthUsed);

  // todo: flush
  return MMSYSERR_NOERROR;  
}

LRESULT 
AC3FilterACM::stream_convert(LPACMDRVSTREAMINSTANCE stream_instance, LPACMDRVSTREAMHEADER stream_header)
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
