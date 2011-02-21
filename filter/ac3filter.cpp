#include "guids.h"
#include "ac3filter.h"
#include "ac3filter_intl.h"
#include "logging.h"
#include "decss\DeCSSInputPin.h"

struct EOpenSink : public ValibException {};

///////////////////////////////////////////////////////////////////////////////
// Define number of buffers and max buffer size sent to downstream.
// So these numbers define total buffer length. For buffer with 2048 samples:
// 2048 [samples] / 48000 [samples/sec] * 30 [buffers] = 1.28 [sec]
//
// In SPDIF mode (48kHz AC3):
// 1536 [samples] / 48000 [samples/sec] * 30 [buffers] = 960 [ms]

#define DSHOW_BUFFERS 30
#define BUF_SAMPLES 2048
#define MAX_BUFFER_SIZE (BUF_SAMPLES * NCHANNELS * 4)


// uncomment this to log timing information into DirectShow log
//#define LOG_TIMING

// uncomment this to register the graph at running objects table
//#ifdef _DEBUG
//#define REGISTER_FILTERGRAPH
//#endif


CUnknown * WINAPI 
AC3Filter::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter::CreateInstance"));
  AC3Filter *pobj = new AC3Filter("AC3Filter", punk, phr);
  if (!pobj) *phr = E_OUTOFMEMORY;
  return pobj;
}

AC3Filter::AC3Filter(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr) :
  CTransformFilter(tszName, punk, CLSID_AC3Filter), 
  dec((IAC3Filter*)this, BUF_SAMPLES), tray_ctl((IAC3Filter*)this)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x, %s)::AC3Filter", this, tszName));

  if (!(m_pInput = new CDeCSSInputPin(this, phr))) 
  {
    *phr = E_OUTOFMEMORY;
    return;
  }

  if (!(sink = new DShowSink(this, phr))) 
  {
    delete m_pInput; 
    m_pInput = 0; 
    *phr = E_OUTOFMEMORY;
    return;
  }
  else
    m_pOutput = sink;

  tray = false;
  reinit = 0;
  spdif_no_pcm = false;

  // Read filter options (processing options are read by COMDecoder)
  RegistryKey reg(REG_KEY);
  reg.get_bool("tray", tray);
  reg.get_int32("reinit", reinit);
  reg.get_bool ("spdif_no_pcm", spdif_no_pcm);

  // Read current language
  if (is_nls_available())
  {
    char lang[LANG_LEN];
    memset(lang, 0, LANG_LEN);
    reg.get_text("Language", lang, LANG_LEN);
    if (lang_index(lang) != -1)
    {
      char lang_dir[MAX_PATH];
      reg.get_text("Lang_Dir", lang_dir, array_size(lang_dir));
      DWORD attr = GetFileAttributes(lang_dir);
      if (attr != -1 && (attr & FILE_ATTRIBUTE_DIRECTORY))
        set_lang(lang, "ac3filter", lang_dir);
    }
  }

  // init decoder
  dec.set_sink(sink);
  dec.load_params(0, AC3FILTER_ALL);
  *phr = S_OK;
}

AC3Filter::~AC3Filter()
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::~AC3Filter", this));
}

STDMETHODIMP 
AC3Filter::JoinFilterGraph(IFilterGraph *pGraph, LPCWSTR pName)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::JoinFilterGraph(%x)", this, pGraph));

  // Register graph at running objects table
  #ifdef REGISTER_FILTERGRAPH
    rot.register_graph(pGraph);
  #endif

  /////////////////////////////////////////////////////////
  // We should not show tray icon when filter is included
  // into the graph. This may happen during graph building
  // and then filter may be removed (not used). Because of
  // this flicking tray icon(s) may appear.
  //
  // Instead we should shoy tray icon when filter has
  // actually connected both input and output pins.
  //
  // But when filter is removed from the graph we must
  // ensure that tray icon is removed and config dialog is
  // destroyed. Config dialog holds reference to the filter
  // and therefore filter cannot be destructed normally
  // while config dialog lives. If filter is removed from
  // the graph it means that we must prepare it to
  // destruct.

  if (!pGraph)
    tray_ctl.hide();

  return CTransformFilter::JoinFilterGraph(pGraph, pName);
}



void 
AC3Filter::reset()
{
  dec.reset();
  cpu.reset();
}

bool        
AC3Filter::open(const CMediaType &_mt)
{
  Speakers spk_tmp;
  return mt2spk(_mt, spk_tmp) && open(spk_tmp);
}

bool        
AC3Filter::open(Speakers _in_spk)
{
  if (!dec.can_open(_in_spk))
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_input(%s): format refused", this, _in_spk.print().c_str() ));
    return false;
  }

  if (!dec.open(_in_spk))
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_input(%s): failed", this, _in_spk.print().c_str() ));
    return false;
  }

  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_input(%s): succeeded", this, _in_spk.print().c_str() ));
  return true;
}

void
AC3Filter::process(const Chunk *chunk)
{
  // Here we want to measure processor time used by filter only
  // so we cannot use just dec->process_to(_chunk, sink)
  // (time used by downstream filters will be also counted in this case)
  // and we have to write full processing cycle.
  // It's also useful because of extended error reporting.

#ifdef LOG_TIMING
  log_input_chunk(chunk);
#endif

  try {
    cpu.start();
    Chunk in(*chunk), out;
    while (dec.process(in, out))
    {
      if (dec.new_stream())
      {
        Speakers spk = dec.get_output();
        DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::process(): new stream (%s)", this, spk.print().c_str() ));

        if (!sink->open(spk))
          THROW(EOpenSink() << errinfo_spk(spk));
      }
#ifdef LOG_TIMING
      log_output_chunk(chunk);
#endif
      cpu.stop();
      sink->process(out);
      cpu.start();
    }
  }
  catch (ValibException &e)
  {
    DbgLog((LOG_ERROR, 3, "AC3Filter(%x)::process(): exception:\n%s", this, boost::diagnostic_information(e).c_str() ));
    reset();
  }
}

void
AC3Filter::flush()
{
  try {
    Chunk out;
    cpu.start();
    while (dec.flush(out))
    {
      if (dec.new_stream())
      {
        Speakers spk = dec.get_output();
        DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::flush(): new stream (%s)", this, spk.print().c_str() ));

        if (!sink->open(spk))
          THROW(EOpenSink() << errinfo_spk(spk));
      }
#ifdef LOG_TIMING
      log_output_chunk(chunk);
#endif
      cpu.stop();
      sink->process(out);
      cpu.start();
    }
  }
  catch (ValibException &e)
  {
    DbgLog((LOG_ERROR, 3, "AC3Filter(%x)::flush(): exception:\n%s", this, boost::diagnostic_information(e).c_str() ));
    reset();
  }
}



STDMETHODIMP 
AC3Filter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
  CheckPointer(ppv, E_POINTER);

  if (riid == IID_IAC3Filter)
    return GetInterface((IAC3Filter *) this, ppv);

  if (riid == IID_IDecoder)
    return GetInterface((IDecoder *) &dec, ppv);

  if (riid == IID_IAudioProcessor)
    return GetInterface((IAudioProcessor *) &dec, ppv);

  if (riid == IID_ISpecifyPropertyPages)
    return GetInterface((ISpecifyPropertyPages *) this, ppv);

  return CTransformFilter::NonDelegatingQueryInterface(riid, ppv);
}


///////////////////////////////////////////////////////////////////////////////
////////////////////////////// DATA FLOW //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

HRESULT
AC3Filter::Receive(IMediaSample *in)
{
  CAutoLock lock(&m_csReceive);

  uint8_t *buf;
  int buf_size;

  Chunk chunk;

  /////////////////////////////////////////////////////////
  // Dynamic input format change

  CMediaType *mt;
  if (in->GetMediaType((_AMMediaType**)&mt) == S_OK)
  {
    if (*mt->FormatType() != FORMAT_WaveFormatEx)
    {
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::Receive(): Input format change to non-audio format", this));
      return E_FAIL;
    }

    Speakers in_spk;
    if (!mt2spk(*mt, in_spk))
    {
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::Receive(): Input format change to unsupported format", this));
      return E_FAIL;
    }

    if (dec.get_input() != in_spk)
    {
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::Receive(): Input format change", this));

      flush();
      reset();
      sink->send_discontinuity();

      if (!open(in_spk))
        return VFW_E_INVALIDMEDIATYPE;
    }
  }

  /////////////////////////////////////////////////////////
  // Discontinuity
  // After receiving discontinuity we must:
  // * Flush the buffered data (send some samples)
  // * Drop unfinished buffers
  // * Mark next sample with discontinuity flag

  if (in->IsDiscontinuity() == S_OK)
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::Receive(): Discontinuity", this));

    flush();
    reset();
    sink->send_discontinuity();
  }

  /////////////////////////////////////////////////////////
  // Data

  in->GetPointer((BYTE**)&buf);
  buf_size = in->GetActualDataLength();

  // Do not process preroll data
  if (in->IsPreroll() == S_OK)
    buf_size = 0;

  /////////////////////////////////////////////////////////
  // Fill chunk

  chunk.set_rawdata(buf, buf_size);

  /////////////////////////////////////////////////////////
  // Timing

  REFERENCE_TIME begin, end;
  switch (in->GetTime(&begin, &end))
  {
    case S_OK:
    case VFW_S_NO_STOP_TIME:
      chunk.set_sync(true, vtime_t(begin) / 10000000);
      break;
  }

  /////////////////////////////////////////////////////////
  // Process

  sink->reset_hresult();
  process(&chunk);
  return FAILED(sink->get_hresult())? sink->get_hresult(): S_OK;
}

HRESULT 
AC3Filter::StartStreaming()
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::StartStreaming()", this));

  // Reset before starting a new stream
  CAutoLock lock(&m_csReceive);
  reset();

  // It's right time to show the tray icon
  // Application may construct several graphs,
  // but we're interested only in working graphs...
  if (tray)
    tray_ctl.show();

  return CTransformFilter::StartStreaming();
}

HRESULT 
AC3Filter::StopStreaming()
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::StopStreaming()", this));
  return CTransformFilter::StopStreaming();
}

HRESULT 
AC3Filter::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::NewSegment(%ims, %ims)", this, int(tStart/10000), int(tStop/10000)));

  // We have to reset because we may need to 
  // drop incomplete frame in the decoder

  CAutoLock lock(&m_csReceive);
  reset();

  return CTransformFilter::NewSegment(tStart, tStop, dRate);
}

HRESULT 
AC3Filter::EndOfStream()
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::EndOfStream()", this));

  // Syncronize with streaming thread 
  // (wait for all data to process)

  CAutoLock lock(&m_csReceive);

  // Force flushing of internal buffers of 
  // processing chain.

  flush();
  reset();

  // Send end-of-stream downstream to indicate that we have no
  // more samples to send.
  return CTransformFilter::EndOfStream();
}

HRESULT 
AC3Filter::BeginFlush()
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::BeginFlush()", this));

  // Serialize with state changes
  CAutoLock filter_lock(&m_csFilter);

  // Send BeginFlush() downstream to release all holding samples
  HRESULT hr = CTransformFilter::BeginFlush();
  if FAILED(hr) return hr;

  // Now we can be sure that Receive() at streaming thread is 
  // unblocked waiting for GetBuffer so we can now serialize 
  // with streaming thread

  CAutoLock streaming_lock(&m_csReceive);

  // Now we can be sure that Receive() (or other call at streaming thread) 
  // is finished and all data is processed.

  reset();

  // All internal processing buffers are now dropped. So we can 
  // now correctly start processing from new position.

  return S_OK;
}
HRESULT 
AC3Filter::EndFlush()
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::EndFlush()", this));

  // Syncronize with streaming thread 
  // (wait for all data to process)
  CAutoLock lock(&m_csReceive);

  return CTransformFilter::EndFlush();
}

STDMETHODIMP 
AC3Filter::Stop()
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::Stop()", this));
  return CTransformFilter::Stop();
}
STDMETHODIMP 
AC3Filter::Pause()
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::Pause()", this));
  return CTransformFilter::Pause();
}
STDMETHODIMP 
AC3Filter::Run(REFERENCE_TIME tStart)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::Run(%ims)", this, int(tStart/10000)));
  HRESULT hr = CTransformFilter::Run(tStart);
  if FAILED(hr)
    return hr;

  if (reinit)
  {
    // Quick hack to overcome 2 'play/pause' problems:
    //
    // Some sound cards mess channel mapping after pause: channels are
    // shifted around: left to center, center ro right, etc.
    // 
    // Some sound cards (I have found it on AD1985) have a bug with pausing
    // of SPDIF playback: after pause or seeking SPDIF transmission disappears
    // at all. The reason is a bug in sound card driver: when Pause() is
    // called on DirectSound's SPDIF playback buffer sound card switches to
    // PCM mode and does not switch back to SPDIF when playback is resumed.
    // The only way to continue playback is to reopen SPDIF output. 
    //
    // To force the renderer to reopen audio output we send a portion of 
    // standard stereo 16bit PCM data (all sound cards can handle it good) so
    // forcing the renderer to close current audio output and open stereo PCM
    // playback. After this we may continue normal operation but should reset
    // DVDGraph's processing chain to force DVDGraph to re-check possibility
    // of SPDIF output.
    //
    // Also discontiniuity flag should be sent with next normal output sample
    // to force the renderer to sync time correctly because excessive PCM
    // output and format switching may produce desynchronization.
    //
    // This method is a 'quick hack' because it breaks normal DirectShow
    // data flow and produces glitches on seeking and pause.

    CAutoLock lock(&m_csReceive);

    Rawdata buf(reinit * 4);
    buf.zero();

    BeginFlush();
    EndFlush();

    Chunk chunk;
    chunk.set_rawdata(buf, buf.size());
    Speakers spk = sink->get_input();

    sink->open(Speakers(FORMAT_PCM16, MODE_STEREO, spk.sample_rate));
    sink->process(chunk);
    sink->open(spk);

    BeginFlush();
    EndFlush();
  }

  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
////////////////////////////// PIN CONNECTIION ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


HRESULT 
AC3Filter::GetMediaType(int i, CMediaType *_mt)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::GetMediaType #%i", this, i));

  if (m_pInput->IsConnected() == FALSE)
    return E_UNEXPECTED;

  if (i < 0) 
    return E_INVALIDARG;

  /////////////////////////////////////////////////////////////////////////////
  // Depending on current settings output formats may be:
  //
  // i | simple    | ext        | spdif/simple | spdif/ext    
  // --|-----------|------------|--------------|--------------
  // 0 | mt_pcm_wf | mt_pcm_wfx | mt_spdif_wf  | mt_spdif_wf  
  // 1 |           | mt_pcm_wf  | mt_pcm_wf    | mt_pcm_wfx   
  // 2 |           |            |              | mt_pcm_wf    
  //
  // where i - format number
  //
  //       simple - user set mono/stereo PCM16 format
  //       ext - user set set to any other format (extended formats)
  //       spdif - 'use spdif if possible' flag set
  //
  //       mt_spdif_wf  - spdif media type using WAVEFORMATEX
  //       mt_pcm_wfx   - PCM media type using WAVEFORMATEXTENSIBLE
  //       mt_pcm_wf    - PCM media type using WAVEFORMATEX
  //
  // If downstream filter does not support some format, it can use another.
  // So automatic switch to lower format is used. But it is possible that
  // some intermediate filter that does not support spdif format is inserted
  // into the chain after AC3Filter. In this case spdif cannot be enabled.
  // To avoid this 'Do not publish PCM format in SPDIF mode' is used.
  //
  // Multichannel mt_pcm_wf formats are nessesary for some old sound cards 
  // that do not understand WAVEFORMATEXTENSIBLE format 
  // (Vortex-based cards for example).

  CMediaType mt;
  Speakers spk;
  Speakers in_spk;

  /////////////////////////////////////////////////////////
  // Publish SPDIF format

  bool use_spdif;
  dec.get_use_spdif(&use_spdif);
  dec.get_in_spk(&in_spk);

  if (use_spdif)
  {
    // dummy spdif formats
    spk = Speakers(FORMAT_SPDIF, 0, in_spk.sample_rate);
    if (!i--) return spk2mt(spk, *_mt, false)? NOERROR: E_FAIL;
    if (!i--) return spk2mt(spk, *_mt, true)? NOERROR: E_FAIL;
  }

  /////////////////////////////////////////////////////////
  // Publish user (PCM) formats
  //
  // Do not publish PCM formats when both 'use_spdif' and
  // 'spdif_no_pcm' options are enabled. This solves the
  // problem when postprocessing filters that do not
  // support SPDIF are used.
  //
  // Input format may be partially specified. For encoded
  // formats channel mask is not known, thereofre we should
  // publish stereo format in this case.
  
  if (!use_spdif || !spdif_no_pcm)
  {
    dec.get_user_spk(&spk);
    if (!spk.mask)
      spk.mask = in_spk.mask? in_spk.mask: MODE_STEREO;
    if (!spk.sample_rate)
      spk.sample_rate = in_spk.sample_rate;

    if ((spk.mask != MODE_MONO && spk.mask != MODE_STEREO) || spk.format != FORMAT_PCM16)
    {
      // mt_pcm_wfx
      if (!i--) return spk2mt(spk, *_mt, true)? NOERROR: E_FAIL;
    }

    // mt_pcm_wf
    if (!i--) return spk2mt(spk, *_mt, false)? NOERROR: E_FAIL;
  }

  return VFW_S_NO_MORE_ITEMS;
}


HRESULT 
AC3Filter::CheckInputType(const CMediaType *mt)
{
  if (m_pInput->IsConnected() == TRUE)
  {
    // If input is already connected agree with current media type
    CMediaType out_mt;
    m_pInput->ConnectionMediaType(&out_mt);
    if (*mt == out_mt)
    {
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckInputType: No change...", this));
      return S_OK;
    }
  }

  Speakers spk_tmp;

  if (!mt2spk(*mt, spk_tmp))
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckInputType(): cannot determine format", this));
    return VFW_E_TYPE_NOT_ACCEPTED;
  }

  if (!dec.can_open(spk_tmp))
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckInputType(%s): format refused by decoder", this, spk_tmp.print().c_str() ));
    return VFW_E_TYPE_NOT_ACCEPTED;
  }

  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckInputType(): Ok...", this));
  return S_OK;
} 

HRESULT 
AC3Filter::CheckOutputType(const CMediaType *mt)
{
  if (m_pOutput->IsConnected() == TRUE)
  {
    // If output is already connected agree with current media type
    CMediaType out_mt;
    m_pOutput->ConnectionMediaType(&out_mt);
    if (*mt == out_mt)
    {
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckOutputType: No change...", this));
      return S_OK;
    }
  }

  // Agree with our proposed media types
  int i = 0;
  CMediaType out_mt;
  while (GetMediaType(i++, &out_mt) == S_OK)
    if (*mt == out_mt)
    {
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckOutputType: Ok...", this));
      return S_OK;
    }

  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckOutputType(): Not our type", this));
  return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT 
AC3Filter::CheckTransform(const CMediaType *mt_in, const CMediaType *mt_out)
{
  DbgLog((LOG_TRACE, 3, "> AC3Filter(%x)::CheckTransform", this));

  if FAILED(CheckInputType(mt_in))
  {
    DbgLog((LOG_TRACE, 3, "< AC3Filter(%x)::CheckTransform(): Input type rejected", this));
    return VFW_E_TYPE_NOT_ACCEPTED;
  }

  if FAILED(CheckOutputType(mt_out))
  {
    DbgLog((LOG_TRACE, 3, "< AC3Filter(%x)::CheckTransform(): Output type rejected", this));
    return VFW_E_TYPE_NOT_ACCEPTED;
  }

  DbgLog((LOG_TRACE, 3, "< AC3Filter(%x)::CheckTransform: Ok...", this));
  return S_OK;
}

bool 
AC3Filter::CheckConnectPin(IPin *pin)
{
  PIN_DIRECTION dir;
  PIN_INFO      pin_info;
  IBaseFilter  *some_filter;
  IUnknown     *some_interface;
  IEnumPins    *enum_pins;
  IPin         *some_pin;
  IPin         *connected_pin;

  if (!pin)
    return true;

  if FAILED(pin->QueryPinInfo(&pin_info)) 
    return true;

  if (!(some_filter = pin_info.pFilter)) 
    return true;

  if SUCCEEDED(pin_info.pFilter->QueryInterface(IID_IMatrixMixer, (void**) &some_interface))
  {
    some_interface->Release();
    some_filter->Release();
    return false; 
  }
  
  if SUCCEEDED(pin_info.pFilter->QueryInterface(IID_IAC3Filter, (void**) &some_interface))
  {
    some_interface->Release();
    some_filter->Release();
    return false; 
  }
  
  bool result = true;

  some_filter->EnumPins(&enum_pins);
  while (result && enum_pins->Next(1, &some_pin, 0) == S_OK)
  {
    some_pin->QueryDirection(&dir);
    if (dir == PINDIR_INPUT)
      if (some_pin->ConnectedTo(&connected_pin) == S_OK)
      {
        result = CheckConnectPin(connected_pin);
        connected_pin->Release();
      }
    some_pin->Release();
  }
  enum_pins->Release();
  some_filter->Release();

  return result;
}

HRESULT 
AC3Filter::CheckConnect(PIN_DIRECTION dir, IPin *pin)
{
  if (dir == PINDIR_INPUT && !CheckConnectPin(pin))
    return E_FAIL;
  else
    return CTransformFilter::CheckConnect(dir, pin);
}

HRESULT 
AC3Filter::SetMediaType(PIN_DIRECTION direction, const CMediaType *mt)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::SetMediaType(%s)", this, direction == PINDIR_INPUT? "input": "output"));

  if (direction == PINDIR_INPUT)
  {
    if FAILED(CheckInputType(mt))
      return E_FAIL;

    if (!open(*mt))
      return E_FAIL;
  }

  if (direction == PINDIR_OUTPUT)
  {
    if FAILED(CheckOutputType(mt))
      return E_FAIL;
  }

  return S_OK;
}

HRESULT 
AC3Filter::CompleteConnect(PIN_DIRECTION direction, IPin *pin)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CompleteConnect(%s)", this, direction == PINDIR_INPUT? "input": "output"));

  // Applicaqtion may construct several graphs,
  // therefore if we enable the tray icon here we may get several
  // icons with non-working filters.
//  if (tray && (m_pInput->IsConnected() == TRUE) && (m_pOutput->IsConnected() == TRUE))
//    tray_ctl.show();

  return S_OK;
}

HRESULT                     
AC3Filter::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::DecideBufferSize", this));

  ASSERT(pAlloc);
  ASSERT(pProperties);
  HRESULT hr = NOERROR;

  pProperties->cBuffers = DSHOW_BUFFERS;
  pProperties->cbBuffer = MAX_BUFFER_SIZE;

  ALLOCATOR_PROPERTIES Actual;
  if FAILED(hr = pAlloc->SetProperties(pProperties, &Actual))
    return hr;

  if (pProperties->cBuffers > Actual.cBuffers ||
      pProperties->cbBuffer > Actual.cbBuffer)
    return E_FAIL;

  return NOERROR;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///
/// ISpecifyPropertyPages
///
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP 
AC3Filter::GetPages(CAUUID *pPages)
{
  pPages->cElems = 7;
  pPages->pElems = (GUID *) CoTaskMemAlloc(sizeof(GUID) * pPages->cElems);
  if (pPages->pElems == NULL)
    return E_OUTOFMEMORY;

  (pPages->pElems)[0] = CLSID_AC3Filter_main;
  (pPages->pElems)[1] = CLSID_AC3Filter_mixer;
  (pPages->pElems)[2] = CLSID_AC3Filter_gains;
  (pPages->pElems)[3] = CLSID_AC3Filter_eq;
  (pPages->pElems)[4] = CLSID_AC3Filter_spdif;
  (pPages->pElems)[5] = CLSID_AC3Filter_sys;
  (pPages->pElems)[6] = CLSID_AC3Filter_about;
  return NOERROR;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///
/// IAC3Filter
///
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Reinit sound card after seek/pause option
STDMETHODIMP 
AC3Filter::get_tray(bool *_tray)
{
  if (_tray)
    *_tray = tray;
  return S_OK;
}

STDMETHODIMP 
AC3Filter::set_tray(bool  _tray)
{
  tray = _tray;
  RegistryKey reg(REG_KEY);
  reg.set_int32("tray", tray);

  /////////////////////////////////////////////////////////
  // Show tray icon if enabled.
  //
  // We cannot hide tray icon when user disables this
  // option because it may be disabled from config dialog
  // called from tray icon. During hiding config dialog
  // is destructed and caller of this function became
  // invalid.

  if (tray && (m_pInput->IsConnected() == TRUE) && (m_pOutput->IsConnected() == TRUE))
    tray_ctl.show();

  return S_OK;
}

// Reinit sound card after seek/pause option
STDMETHODIMP 
AC3Filter::get_reinit(int *_reinit)
{
  if (_reinit)
    *_reinit = reinit;
  return S_OK;
}

STDMETHODIMP 
AC3Filter::set_reinit(int  _reinit)
{
  reinit = _reinit;
  RegistryKey reg(REG_KEY);
  reg.set_int32("reinit", reinit);
  return S_OK;
}

// Reinit sound card after seek/pause option
STDMETHODIMP 
AC3Filter::get_spdif_no_pcm(bool *_spdif_no_pcm)
{
  if (_spdif_no_pcm)
    *_spdif_no_pcm = spdif_no_pcm;
  return S_OK;
}

STDMETHODIMP 
AC3Filter::set_spdif_no_pcm(bool  _spdif_no_pcm)
{
  spdif_no_pcm = _spdif_no_pcm;
  RegistryKey reg(REG_KEY);
  reg.set_bool("spdif_no_pcm", spdif_no_pcm);
  return S_OK;
}

// Playback time
STDMETHODIMP 
AC3Filter::get_playback_time(vtime_t *_time)
{
  *_time = 0;
  if (m_pClock)
  {
    REFERENCE_TIME t;
    if SUCCEEDED(m_pClock->GetTime(&t))
      *_time = vtime_t(double(t - m_tStart) / 10000000);
  }
  return S_OK;
}

// CPU usage
STDMETHODIMP 
AC3Filter::get_cpu_usage(double *_cpu_usage)
{
  *_cpu_usage = cpu.usage();
  return S_OK;
}

// Build and environment info
STDMETHODIMP
AC3Filter::get_env(char *_buf, size_t _size)
{
  const char *env = valib_build_info();
  size_t len = strlen(env) + 1;
  memcpy(_buf, env, MIN(_size, len));
  cr2crlf(_buf, _size);
  return S_OK;
}
