#include "guids.h"
#include "ac3filter.h"
#include "ac3filter_dlg.h"

// setup data

const AMOVIESETUP_MEDIATYPE sudInPinTypes[] =
{
// MPEG2 formats
{&MEDIATYPE_MPEG2_PES,          &MEDIASUBTYPE_DOLBY_AC3         },
{&MEDIATYPE_MPEG2_PES,          &MEDIASUBTYPE_MPEG1AudioPayload },
{&MEDIATYPE_MPEG2_PES,          &MEDIASUBTYPE_MPEG2_AUDIO       },
{&MEDIATYPE_MPEG2_PES,          &MEDIASUBTYPE_DVD_LPCM_AUDIO    },
{&MEDIATYPE_MPEG2_PES,          &MEDIASUBTYPE_DTS               },
// DVD formats
{&MEDIATYPE_DVD_ENCRYPTED_PACK, &MEDIASUBTYPE_DOLBY_AC3         },
{&MEDIATYPE_DVD_ENCRYPTED_PACK, &MEDIASUBTYPE_MPEG1AudioPayload },
{&MEDIATYPE_DVD_ENCRYPTED_PACK, &MEDIASUBTYPE_MPEG2_AUDIO       },
{&MEDIATYPE_DVD_ENCRYPTED_PACK, &MEDIASUBTYPE_DVD_LPCM_AUDIO    },
{&MEDIATYPE_DVD_ENCRYPTED_PACK, &MEDIASUBTYPE_DTS               },
// PCM formats
{&MEDIATYPE_Audio,              &MEDIASUBTYPE_PCM               },
{&MEDIATYPE_Audio,              &MEDIASUBTYPE_IEEE_FLOAT        },
{&MEDIATYPE_Audio,              &MEDIASUBTYPE_DVD_LPCM_AUDIO    },
// Compressed formats
{&MEDIATYPE_Audio,              &MEDIASUBTYPE_AVI_AC3           },
{&MEDIATYPE_Audio,              &MEDIASUBTYPE_AVI_DTS           },
{&MEDIATYPE_Audio,              &MEDIASUBTYPE_AVI_AAC           },
{&MEDIATYPE_Audio,              &MEDIASUBTYPE_AVI_MP3           },
{&MEDIATYPE_Audio,              &MEDIASUBTYPE_DOLBY_AC3         },
{&MEDIATYPE_Audio,              &MEDIASUBTYPE_DTS               },
{&MEDIATYPE_Audio,              &MEDIASUBTYPE_MPEG1AudioPayload },
{&MEDIATYPE_Audio,              &MEDIASUBTYPE_MPEG2_AUDIO       },
{&MEDIATYPE_Audio,              &MEDIASUBTYPE_FLAC_FRAME        },
// SPDIF formats
{&MEDIATYPE_Audio,              &MEDIASUBTYPE_DOLBY_AC3_SPDIF   },
};

const AMOVIESETUP_MEDIATYPE sudOutPinTypes[] =
{
{&MEDIATYPE_Audio, &MEDIASUBTYPE_PCM             },
{&MEDIATYPE_Audio, &MEDIASUBTYPE_DOLBY_AC3_SPDIF },
};

const AMOVIESETUP_PIN psudPins[] =                 
{
  {
    L"Input",           // String pin name
    FALSE,              // Is it rendered
    FALSE,              // Is it an output
    FALSE,              // Allowed none
    FALSE,              // Allowed many
    &CLSID_NULL,        // Connects to filter
    L"Output",          // Connects to pin
    sizeof(sudInPinTypes) / sizeof(AMOVIESETUP_MEDIATYPE), // Number of types
    sudInPinTypes       // The pin details
  },     
  { 
    L"Output",          // String pin name
    FALSE,              // Is it rendered
    TRUE,               // Is it an output
    FALSE,              // Allowed none
    FALSE,              // Allowed many
    &CLSID_NULL,        // Connects to filter
    L"Input",           // Connects to pin
    sizeof(sudOutPinTypes) / sizeof(AMOVIESETUP_MEDIATYPE), // Number of types
    sudOutPinTypes      // The pin details
  }
};


const AMOVIESETUP_FILTER sudAC3Filter =
{
    &CLSID_AC3Filter,       // Filter CLSID
    L"AC3Filter",           // Filter name
    0x40000000,             // Its merit      MERIT_PREFERRED
    2,                      // Number of pins
    psudPins                // Pin details
};


CFactoryTemplate g_Templates[] = {
    { L"AC3Filter Main page"
    , &CLSID_AC3Filter_main
    , AC3FilterDlg::CreateMain },
    { L"AC3Filter Mixer page"
    , &CLSID_AC3Filter_mixer
    , AC3FilterDlg::CreateMixer },
    { L"AC3Filter Gains page"
    , &CLSID_AC3Filter_gains
    , AC3FilterDlg::CreateGains },
    { L"AC3Filter Equalizer page"
    , &CLSID_AC3Filter_eq
    , AC3FilterDlg::CreateEq },
    { L"AC3Filter SPDIF page"
    , &CLSID_AC3Filter_spdif
    , AC3FilterDlg::CreateSPDIF },
    { L"AC3Filter System page"
    , &CLSID_AC3Filter_sys
    , AC3FilterDlg::CreateSystem },
    { L"AC3Filter About page"
    , &CLSID_AC3Filter_about
    , AC3FilterDlg::CreateAbout },
    { L"AC3Filter"
    , &CLSID_AC3Filter
    , AC3Filter::CreateInstance
    , NULL
    , &sudAC3Filter }
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


//
// Handle registration of this filter
//

HINSTANCE ac3filter_instance = 0;

STDAPI DllRegisterServer()
{
  return AMovieDllRegisterServer2( TRUE );
}

STDAPI DllUnregisterServer()
{
  return AMovieDllRegisterServer2( FALSE );
}

///////////////////////////////////////////////////////////////////////////////
// Truncate the last element of the path with the trailing slash
// "c:\dir\dir2\filename.ext" turns into
// "c:\dir\dir2"
// "c:\dir"
// etc..

size_t cut_last_name(char *name, size_t size)
{
  size_t pos = 0;
  while (pos < size && name[pos])
    pos++;

  // skip the trailing slash if it is present
  if (name[pos] == 0) pos--;
  if (name[pos] == '\\') pos--;

  while (pos > 0 && name[pos] != '\\')
    pos--;

  name[pos] = 0;
  return pos;
}

///////////////////////////////////////////////////////////////////////////////
// DllMain

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
{
  if (reason == DLL_PROCESS_ATTACH)
  {
    ac3filter_instance = hinst;

    // Init NLS
    // Search NLS DLL at the filter's folder
    size_t path_size = MAX_PATH;
    char path[MAX_PATH];

    path_size = GetModuleFileName(hinst, path, (DWORD)path_size);
    if (path_size)
      if (cut_last_name(path, path_size))
        init_nls(path);
  }
  return DllEntryPoint(hinst, reason, reserved);
}
