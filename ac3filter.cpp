#include "guids.h"
#include "ac3filter.h"
#include "decss\DeCSSInputPin.h"

// uncomment this to log timing information into DirectShow log
#define LOG_TIMING

// uncomment this to register graph at running objects table
//#define REGISTER_FILTERGRAPH

#define MAX_NSAMPLES 2048
#define MAX_BUFFER_SIZE (MAX_NSAMPLES * NCHANNELS * 4)

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
  dec((IAC3Filter*)this)
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

  config_autoload = false;

  // init decoder
  dec.set_sink(sink);
  dec.load_params(0, AC3FILTER_ALL);
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
    rot.register_graph(m_pGraph);
  #endif

  return CTransformFilter::JoinFilterGraph(pGraph, pName);
}



void 
AC3Filter::reset()
{
  dec.reset();
  // sink->send_discontinuity();
  cpu.reset();
}

bool        
AC3Filter::set_input(const CMediaType &_mt)
{
  Speakers spk_tmp;
  return mt2spk(_mt, spk_tmp) && set_input(spk_tmp);
}

bool        
AC3Filter::set_input(Speakers _in_spk)
{
  if (!dec.query_input(_in_spk))
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_input(%s %s %iHz): format refused", this,
      _in_spk.mode_text(), _in_spk.format_text(), _in_spk.sample_rate));
    return false;
  }

  if (!dec.set_input(_in_spk))
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_input(%s %s %iHz): failed", this,
      _in_spk.mode_text(), _in_spk.format_text(), _in_spk.sample_rate));
    return false;
  }

  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_input(%s %s %iHz): succeeded", this,
    _in_spk.mode_text(), _in_spk.format_text(), _in_spk.sample_rate));
  return true;
}

bool
AC3Filter::process_chunk(const Chunk *_chunk)
{
  // Here we want to measure processor time used by filter only
  // so we cannot use just dec->process_to(_chunk, sink)
  // (time used by downstream filters will be also counted in this case)
  // and we have to write full processing cycle.
  // It's also useful because of extended error reporting.

  cpu.start();
  if (!dec.process(_chunk))
  {
    cpu.stop();
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::process_chunk(): dec.process() failed!", this));
    return false;
  }
  cpu.stop();

  Chunk chunk;
  while (!dec.is_empty())
  {
    cpu.start();
    if (!dec.get_chunk(&chunk))
    {
      cpu.stop();
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::process_chunk(): dec.get_chunk() failed!", this));
      return false;
    }
    cpu.stop();

    if (!sink->process(&chunk))
    {
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::process_chunk(): sink->process() failed!", this));
      return false;
    }
  }
  return true;
}

bool
AC3Filter::flush()
{
  Chunk chunk;
  chunk.set_empty(dec.get_input(), false, 0, true);
  return process_chunk(&chunk);
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
  vtime_t time;

  Chunk chunk;

  /////////////////////////////////////////////////////////
  // Dynamic input format change

  CMediaType *mt;
  if (in->GetMediaType((_AMMediaType**)&mt) == S_OK)
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::Receive(): Input format change", this));
    flush();
    if (!set_input(*mt))
      return VFW_E_INVALIDMEDIATYPE;
  }

  Speakers in_spk = dec.get_input();

  /////////////////////////////////////////////////////////
  // Discontinuity

  if (in->IsDiscontinuity() == S_OK)
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::Receive(): Discontinuity", this));
    sink->send_discontinuity();
    // we have to reset because we may need to drop incomplete frame in the decoder
    flush();
    reset();
  }

  /////////////////////////////////////////////////////////
  // Data

  in->GetPointer((BYTE**)&buf);
  buf_size = in->GetActualDataLength();

  /////////////////////////////////////////////////////////
  // Fill chunk

  chunk.set_rawdata(in_spk, buf, buf_size);

  /////////////////////////////////////////////////////////
  // Timing

  REFERENCE_TIME begin, end;
  switch (in->GetTime(&begin, &end))
  {
    case S_OK:
    case VFW_S_NO_STOP_TIME:
      time = vtime_t(begin) / 10000000;
      chunk.set_sync(true, time);
#ifdef LOG_TIMING
      DbgLog((LOG_TRACE, 3, "-> > timestamp: %ims\t> %.0fsm", int(begin/10000), time * 1000));
#endif
      break;
  }

  /////////////////////////////////////////////////////////
  // Process

  sink->reset_hresult();
  process_chunk(&chunk);

  if FAILED(sink->get_hresult())
    return sink->get_hresult();
  else
    return S_OK;
}

HRESULT 
AC3Filter::StartStreaming()
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::StartStreaming()", this));

  // Reset before starting a new stream
  CAutoLock lock(&m_csReceive);
  reset();

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

  // read 'quick hack' of play/pause problem
  int reinit_samples = 0;
  RegistryKey reg(REG_KEY);
  reg.get_int32("reinit_samples", reinit_samples);

  if (reinit_samples)
  {
    // Quick hack to overcome SPDIF 'play/pause' problem
    //
    // Some sound cards (I have found it on AD1985) have a bug with pausing
    // of SPDIF playback: after pause or seeking SPDIF transmission disappears
    // at all. The reason is a bug in sound card driver: when Pause() is
    // called on DirectSound's SPDIF playback buffer sound card switches to
    // PCM mode and does not switch back to SPDIF when playback is resumed.
    // The only way to continue playback is to reopen SPDIF output. 
    // 
    // To force the renderer to reopen SPDIF output we send a portion of 
    // PCM data so forcing the renderer to close SPDIF output and open 
    // PCM playback. After this we may continue normal operation but should
    // reset DVDGraph's processing chain to force DVDGraph to re-check
    // possibility of SPDIF output.
    //
    // Also discontiniuity flag should be sent with next normal output sample
    // to force the renderer to sync time correctly because excessive PCM
    // output and format switching may produce desynchronization.
    //
    // This method is a 'quick hack' because it breaks normal DirectShow
    // data flow and produces glitches on seeking and pause.

    CAutoLock lock(&m_csReceive);

    uint8_t *buf = new uint8_t[reinit_samples * 4];
    memset(buf, 0, reinit_samples * 4);

    Chunk chunk;
    chunk.set_rawdata(Speakers(FORMAT_PCM16, MODE_STEREO, dec.get_input().sample_rate), buf, sizeof(buf));

    sink->process(&chunk);

    dec.reset();
    // sink->send_discontinuity();
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

  if (!i--) return spk2mt(Speakers(FORMAT_PCM16, MODE_STEREO, dec.get_input().sample_rate), *_mt, false)? NOERROR: E_FAIL;

/*
  /////////////////////////////////////////////////////////////////////////////
  // Depending on current settings output formats may be:
  //
  // i | simple    | ext        | spdif/simple | spdif/ext    
  // --|-----------|------------|--------------|--------------
  // 0 | mt_pcm_wf | mt_pcm_wfx | mt_spdif_wf  | mt_spdif_wf  
  // 1 |           | mt_pcm_wf  | mt_spdif_wfx | mt_spdif_wfx 
  // 2 |           |            | mt_pcm_wf    | mt_pcm_wfx   
  // 3 |           |            |              | mt_pcm_wf    
  //
  // where i - format number
  //
  //       simple - output is set to mono/stereo PCM16 format
  //       ext - output is set to any other format (extended formats)
  //       spdif - 'use spdif if possible' flag set
  //
  //       mt_spdif_wfx - spdif media type using WAVEFORMATEXTENSIBLE
  //       mt_spdif_wf  - spdif media type using WAVEFORMATEX
  //       mt_pcm_wfx   - PCM media type using WAVEFORMATEXTENSIBLE
  //       mt_pcm_wf    - PCM media type using WAVEFORMATEX
  //
  // So even if output cannot support some of formats, it can use other.
  //
  // multichannel mt_pcm_wf formats are nessesary for some old sound cards 
  // that do not understand WAVEFORMATEXTENSIBLE format 
  // (Vortex-based cards for example).

  if (dec.use_spdif)
  {
    // mt_spdif_wf
    if (!i--) return spk2mt(guess_spdif_output(), *_mt, false)? NOERROR: E_FAIL;

    // mt_spdif_wfx
    if (!i--) return spk2mt(guess_spdif_output(), *_mt, true)? NOERROR: E_FAIL;
  }

  Speakers spk = guess_pcm_output();
  if ((spk.mask != MODE_MONO && spk.mask != MODE_STEREO) || spk.format != FORMAT_PCM16)
  {
    // mt_pcm_wfx
    if (!i--) return spk2mt(spk, *_mt, true)? NOERROR: E_FAIL;
  }

  // mt_pcm_wf
  if (!i--) return spk2mt(spk, *_mt, false)? NOERROR: E_FAIL;
*/
  return VFW_S_NO_MORE_ITEMS;
}


HRESULT 
AC3Filter::CheckInputType(const CMediaType *mt)
{
  Speakers spk_tmp;

  if (!mt2spk(*mt, spk_tmp))
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckInputType(): cannot determine format", this));
    return VFW_E_TYPE_NOT_ACCEPTED;
  }

  if (!dec.query_input(spk_tmp))
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckInputType(%s %s %iHz): format refused by decoder", this, spk_tmp.mode_text(), spk_tmp.format_text(), spk_tmp.sample_rate));
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
    // If output is already connected agree 
    // only with current media type
    CMediaType out_mt;
    m_pOutput->ConnectionMediaType(&out_mt);
    if (*mt == out_mt)
    {
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckOutputType: Ok...", this));
      return S_OK;
    }
  }
  else
  {
    // If output is not connected agree
    // only with our proposed media types
    int i = 0;
    CMediaType out_mt;
    while (GetMediaType(i++, &out_mt) == S_OK)
      if (*mt == out_mt)
      {
        DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckOutputType: Ok...", this));
        return S_OK;
      }
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

    if (!set_input(*mt))
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
AC3Filter::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::DecideBufferSize", this));

  ASSERT(pAlloc);
  ASSERT(pProperties);
  HRESULT hr = NOERROR;

  pProperties->cBuffers = 10;
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
  pPages->cElems = 5;
  pPages->pElems = (GUID *) CoTaskMemAlloc(sizeof(GUID) * pPages->cElems);
  if (pPages->pElems == NULL)
    return E_OUTOFMEMORY;

  (pPages->pElems)[0] = CLSID_AC3Filter_main;
  (pPages->pElems)[1] = CLSID_AC3Filter_mixer;
  (pPages->pElems)[2] = CLSID_AC3Filter_gains;
  (pPages->pElems)[3] = CLSID_AC3Filter_sys;
  (pPages->pElems)[4] = CLSID_AC3Filter_about;
  return NOERROR;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///
/// IAC3Filter
///
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

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
AC3Filter::get_env(char *_buf, int _size)
{
  const char *env = valib_build_info();
  int len = strlen(env) + 1;
  memcpy(_buf, env, MIN(_size, len));
  cr2crlf(_buf, _size);
  return S_OK;
}


