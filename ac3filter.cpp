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
  
  in_spk = def_spk;
  out_spk = stereo_spk;

  spdif = false;
  spdif_on = false;

  config_autoload = false;
  formats = FORMAT_CLASS_PCM | FORMAT_MASK_AC3 | FORMAT_MASK_MPA | FORMAT_MASK_DTS | FORMAT_MASK_PES;

  dec.set_input(in_spk);
  dec.set_output(out_spk);
  dec.proc.set_input_order(win_order);
  dec.proc.set_output_order(win_order);
  dec.reset();

  sink->set_input(out_spk);

  // load params
  load_params(0, AC3FILTER_ALL);
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
AC3Filter::set_input(CMediaType &_mt)
{
  Speakers spk_tmp;
  return mt2spk(_mt, spk_tmp) && set_input(spk_tmp);
}

bool        
AC3Filter::set_input(Speakers _spk)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_input(%s %s %iHz)...", this, _spk.mode_text(), _spk.format_text(), _spk.sample_rate));
  if (in_spk == _spk)
    return true;

  if (!dec.query_input(_spk))
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_input(%s %s %iHz): FAILED", this, _spk.mode_text(), _spk.format_text(), _spk.sample_rate));
    return false;
  }

  Speakers old_in_spk = in_spk;
  in_spk = _spk;

  // update output sample rate
  if (_spk.sample_rate != out_spk.sample_rate)
  {
    Speakers spk_tmp = out_spk;
    spk_tmp.sample_rate = _spk.sample_rate;
    if (!set_output(spk_tmp, spdif))
    {
      in_spk = old_in_spk;
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_input(%s %s %iHz): FAILED", this, _spk.mode_text(), _spk.format_text(), _spk.sample_rate));
      return false;
    }
  }

  in_spk = _spk;
  dec.set_input(in_spk);
  dec.proc.set_input_order(win_order);
  dec.reset();

  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_input(%s %s %iHz): Ok", this, _spk.mode_text(), _spk.format_text(), _spk.sample_rate));
  return true;
}

bool        
AC3Filter::set_output(Speakers _spk, bool _spdif)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_output(%s %s %iHz%s)...", this, _spk.mode_text(), _spk.format_text(), _spk.sample_rate, _spdif? " (spdif if possible)": ""));
  // output sample rate should be equal to the input sample rate
  _spk.sample_rate = in_spk.sample_rate;

  Speakers spk_spdif = _spk;
  spk_spdif.format = FORMAT_SPDIF;

  if (_spdif && dec.query_output(spk_spdif) && sink->query_input(spk_spdif))
    spdif_on = true;
  else if (dec.query_output(_spk) && sink->query_input(_spk))
    spdif_on = false;
  else
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_output(%s %s %iHz%s): FAILED", this, _spk.mode_text(), _spk.format_text(), _spk.sample_rate, _spdif? " (spdif if possible)": ""));
    return false;
  }

  spdif     = _spdif;
  out_spk   = _spk;
  out_spdif = spk_spdif;

  if (spdif_on)
    dec.set_output(out_spdif);
  else
    dec.set_output(out_spk);

  dec.proc.set_output_order(win_order);
  dec.reset();
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_output(%s %s %iHz%s): Ok", this, _spk.mode_text(), _spk.format_text(), _spk.sample_rate, _spdif? " (spdif if possible)": ""));
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




STDMETHODIMP 
AC3Filter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
  CheckPointer(ppv, E_POINTER);

  if (riid == IID_IAC3Filter)
    return GetInterface((IAC3Filter *) this, ppv);

  if (riid == IID_IAudioProcessor)
    return GetInterface((IAudioProcessor *) &dec, ppv);

  if (riid == IID_IDecoder)
    return GetInterface((IDecoder *) &dec, ppv);

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

  {
    CMediaType *mt;
    if (in->GetMediaType((_AMMediaType**)&mt) == S_OK)
    {
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::Receive(): Input format change", this));
      if (!set_input(*mt))
        return VFW_E_INVALIDMEDIATYPE;
    }
  }

  /////////////////////////////////////////////////////////
  // Discontinuity

  if (in->IsDiscontinuity() == S_OK)
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::Receive(): Discontinuity", this));
    // we have to reset because we may need to drop incomplete frame in the decoder
    reset();
  }

  /////////////////////////////////////////////////////////
  // Data

  in->GetPointer((BYTE**)&buf);
  buf_size = in->GetActualDataLength();

  /////////////////////////////////////////////////////////
  // Fill chunk

  chunk.set(in_spk, buf, buf_size);

  /////////////////////////////////////////////////////////
  // Timing

  REFERENCE_TIME begin, end;
  switch (in->GetTime(&begin, &end))
  {
    case S_OK:
    case VFW_S_NO_STOP_TIME:
      time = vtime_t(begin) * in_spk.sample_rate / 10000000;
      chunk.set_sync(true, time);
#ifdef LOG_TIMING
      DbgLog((LOG_TRACE, 3, "-> > timestamp: %ims\t> %.0fsm", int(begin/10000), time));
#endif
      break;
  }

  /////////////////////////////////////////////////////////
  // Process

  process_chunk(&chunk);

  return S_OK;
}

HRESULT 
AC3Filter::StartStreaming()
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::StartStreaming()", this));

  // switch between spdif/analog
  set_output(out_spk, spdif);

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

  Chunk chunk;
  chunk.set(in_spk, 0, 0, false, 0, true);
  process_chunk(&chunk);
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
  return CTransformFilter::Run(tStart);
}



///////////////////////////////////////////////////////////////////////////////
////////////////////////////// PIN CONNECTIION ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


HRESULT 
AC3Filter::GetMediaType(int i, CMediaType *_mt)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::GetMediaType #%i", this, i));

  // todo: remove this?
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
  // (Vortex-based cards, for example).

  if (spdif)
  {
    // mt_spdif_wf
    if (i != 0)
      i--;
    else
      if (spk2mt(out_spdif, *_mt, false))
        return NOERROR;
      else
        return E_FAIL;

    // mt_spdif_wfx
    if (i != 0)
      i--;
    else
      if (spk2mt(out_spdif, *_mt, true))
        return NOERROR;
      else
        return E_FAIL;
  }

  // mt_pcm_wfx
  if ((out_spk.mask != MODE_MONO && out_spk.mask != MODE_STEREO) || 
      out_spk.format != FORMAT_PCM16)
  {
    if (i != 0)
      i--;
    else
      if (spk2mt(out_spk, *_mt, true))
        return NOERROR;
      else
        return E_FAIL;
  }

  // mt_pcm_wf
  if (i != 0)
    i--;
  else
    if (spk2mt(out_spk, *_mt, false))
      return NOERROR;
    else
      return E_FAIL;

  return VFW_S_NO_MORE_ITEMS;
}


HRESULT 
AC3Filter::CheckInputType(const CMediaType *mt)
{
  Speakers spk_tmp;
  if (mt2spk(*mt, spk_tmp) && dec.query_input(spk_tmp))
  {
    if ((FORMAT_MASK(spk_tmp.format) & formats) != 0)
    {
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckInputType(%s %s %iHz): Ok...", this, spk_tmp.mode_text(), spk_tmp.format_text(), spk_tmp.sample_rate));
      return S_OK;
    }
    else
    {
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckInputType(%s %s %iHz): Format refused...", this, spk_tmp.mode_text(), spk_tmp.format_text(), spk_tmp.sample_rate));
      return VFW_E_TYPE_NOT_ACCEPTED;
    }
  }
  else 
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckInputType: Cannot determine format...", this));
    return VFW_E_TYPE_NOT_ACCEPTED;
  }
} 

HRESULT 
AC3Filter::CheckOutputType(const CMediaType *mt)
{
  CMediaType mt_tmp1 = *mt;
  CMediaType mt_tmp2;
  CMediaType mt_tmp3;
  /////////////////////////////////////////////////////////
  // Verify SPDIF output

  if (spdif)
  {
    spk2mt(out_spdif, mt_tmp2, false);
    spk2mt(out_spdif, mt_tmp3, true);

    // Media Player Classics bug hack: do not check sample rates
    if (*mt_tmp1.FormatType() == FORMAT_WaveFormatEx) ((WAVEFORMATEX *)mt_tmp1.Format())->nSamplesPerSec = 0;
    if (*mt_tmp2.FormatType() == FORMAT_WaveFormatEx) ((WAVEFORMATEX *)mt_tmp2.Format())->nSamplesPerSec = 0;
    if (*mt_tmp3.FormatType() == FORMAT_WaveFormatEx) ((WAVEFORMATEX *)mt_tmp3.Format())->nSamplesPerSec = 0;

    if (mt_tmp1 == mt_tmp2 || mt_tmp1 == mt_tmp3)
    {
      DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckOutputType(): Ok...", this));
      return S_OK;
    }
  }

  /////////////////////////////////////////////////////////
  // Verify PCM output
  // (used also in case when SPDIF is not available)

  spk2mt(out_spk, mt_tmp2, false);
  spk2mt(out_spk, mt_tmp3, true);

  // Media Player Classics bug hack: do not check sample rates
  if (*mt_tmp1.FormatType() == FORMAT_WaveFormatEx) ((WAVEFORMATEX *)mt_tmp1.Format())->nSamplesPerSec = 0;
  if (*mt_tmp2.FormatType() == FORMAT_WaveFormatEx) ((WAVEFORMATEX *)mt_tmp2.Format())->nSamplesPerSec = 0;
  if (*mt_tmp3.FormatType() == FORMAT_WaveFormatEx) ((WAVEFORMATEX *)mt_tmp3.Format())->nSamplesPerSec = 0;

  if (mt_tmp1 == mt_tmp2 || mt_tmp1 == mt_tmp3)
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckOutputType(): Ok...", this));
    return S_OK;
  }
  else
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckOutputType(): Format refused...", this));
    return VFW_E_TYPE_NOT_ACCEPTED;
  }
}

HRESULT 
AC3Filter::CheckTransform(const CMediaType *mt_in, const CMediaType *mt_out)
{
  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckTransform", this));

  if FAILED(CheckInputType(mt_in))
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckTransform(): Input type rejected!", this));
    return VFW_E_TYPE_NOT_ACCEPTED;
  }
  if FAILED(CheckOutputType(mt_out))
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckTransform(): Output type rejected!!", this));
    return VFW_E_TYPE_NOT_ACCEPTED;
  }

  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::CheckTransform: Ok...", this));
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

  bool result = true;

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

    Speakers spk_tmp;
    if (!mt2spk(*mt, spk_tmp) || !set_input(spk_tmp))
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
/// ISpecifyPropertyPages
///
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Speakers
STDMETHODIMP 
AC3Filter::get_in_spk(Speakers *_spk)
{
  *_spk = in_spk;
  return S_OK;
}
STDMETHODIMP 
AC3Filter::get_out_spk(Speakers *_spk)
{
  *_spk = out_spk;
  return S_OK;
}
STDMETHODIMP 
AC3Filter::set_out_spk(Speakers  _spk)
{
  AutoLock config_lock(&dec.config);

  DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_out_spk(%s %s %iHz)", this, _spk.mode_text(), _spk.format_text(), _spk.sample_rate));
  if (set_output(_spk, spdif))
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_out_spk(): Ok", this));
    return S_OK;
  }
  else
  {
    DbgLog((LOG_TRACE, 3, "AC3Filter(%x)::set_out_spk(): Failed!", this));
    return E_FAIL;
  }
}

// SPDIF
STDMETHODIMP 
AC3Filter::get_spdif(bool *_spdif, int* _spdif_mode)
{
  *_spdif = spdif;
  *_spdif_mode = dec.get_spdif_mode();

  return S_OK;
}
STDMETHODIMP 
AC3Filter::set_spdif(bool _spdif)
{
  AutoLock config_lock(&dec.config);
  set_output(out_spk, _spdif);
  return S_OK;
}

STDMETHODIMP 
AC3Filter::get_spdif_pt(int *_spdif_pt)
{
  *_spdif_pt = dec.get_spdif();
  return S_OK;
}
STDMETHODIMP 
AC3Filter::set_spdif_pt(int _spdif_pt)
{
  AutoLock config_lock(&dec.config);
  dec.set_spdif(_spdif_pt);
  return S_OK;
}

// Formats to accept
STDMETHODIMP 
AC3Filter::get_formats(int *_formats)
{
  *_formats = formats;
  return S_OK;
}
STDMETHODIMP 
AC3Filter::set_formats(int  _formats)
{
  formats = _formats;
  return S_OK;
}


STDMETHODIMP 
AC3Filter::get_playback_time(vtime_t *_time)
{
  *_time = 0;
  if (m_pClock)
  {
    REFERENCE_TIME t;
    if SUCCEEDED(m_pClock->GetTime(&t))
      *_time = vtime_t(double(t - m_tStart) * in_spk.sample_rate / 10000000);
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

// Config
STDMETHODIMP 
AC3Filter::get_config_file(char *_filename, int _size)
{
  _filename[0] = 0;
  return S_OK;
}
STDMETHODIMP 
AC3Filter::get_config_autoload(bool *_config_autoload)
{
  *_config_autoload = config_autoload;
  return S_OK;
}
STDMETHODIMP 
AC3Filter::set_config_autoload(bool  _config_autoload)
{
  config_autoload = _config_autoload;
  return S_OK;
}

// Load/save settings
STDMETHODIMP AC3Filter::load_params(Config *_conf, int _what)
{
  AutoLock config_lock(&dec.config);
  AudioProcessorState state;
  dec.get_state(&state);

  RegistryKey reg;
  if (!_conf)
  {
    _conf = &reg;
    reg.open_key(REG_KEY_PRESET"\\Default");
  }

  if (_what & AC3FILTER_SPK)
  {
    Speakers spk_tmp = out_spk;
    bool spdif_tmp = spdif;
    _conf->get_int32("format"           ,spk_tmp.format  );
    _conf->get_int32("mask"             ,spk_tmp.mask    );
    _conf->get_int32("relation"         ,spk_tmp.relation);
    _conf->get_bool ("spdif"            ,spdif_tmp       );

    switch (spk_tmp.format)
    {
      case FORMAT_PCM16_LE:
      case FORMAT_PCM16: 
        spk_tmp.level = 32767;
        break;

      case FORMAT_PCM24_LE:
      case FORMAT_PCM24: 
        spk_tmp.level = 8388607;
        break;

      case FORMAT_PCM32_LE:
      case FORMAT_PCM32: 
        spk_tmp.level = 2147483647;      
        break;

      case FORMAT_PCMFLOAT:
      case FORMAT_PCMFLOAT_LE:
        spk_tmp.level = 1.0; 
        break;

      default: 
        spk_tmp.level = 1.0; 
        break;
    }

    set_out_spk(spk_tmp);
    set_spdif(spdif_tmp);
  }

  if (_what & AC3FILTER_PROC)
  {
    // Options
    _conf->get_bool ("auto_gain"        ,state.auto_gain       );
    _conf->get_bool ("normalize"        ,state.normalize       );
    _conf->get_bool ("normalize_matrix" ,state.normalize_matrix);
    _conf->get_bool ("auto_matrix"      ,state.auto_matrix     );
    _conf->get_bool ("expand_stereo"    ,state.expand_stereo   );
    _conf->get_bool ("voice_control"    ,state.voice_control   );
    _conf->get_float("release"          ,state.release         );
    // Gains
    _conf->get_float("master"           ,state.master          );
    _conf->get_float("clev"             ,state.clev            );
    _conf->get_float("slev"             ,state.slev            );
    _conf->get_float("lfelev"           ,state.lfelev          );
    // DRC
    _conf->get_bool ("drc"              ,state.drc             );
    _conf->get_float("drc_power"        ,state.drc_power       );
    // Bass redirection
    _conf->get_bool ("bass_redir"       ,state.bass_redir      );
    _conf->get_int32("bass_freq"        ,state.bass_freq       );
  }

  if (_what & AC3FILTER_GAINS)
  {
    // I/O Gains
    _conf->get_float("gain_input_L"     ,state.input_gains[CH_L]   );
    _conf->get_float("gain_input_C"     ,state.input_gains[CH_C]   );
    _conf->get_float("gain_input_R"     ,state.input_gains[CH_R]   );
    _conf->get_float("gain_input_SL"    ,state.input_gains[CH_SL]  );
    _conf->get_float("gain_input_SR"    ,state.input_gains[CH_SR]  );
    _conf->get_float("gain_input_LFE"   ,state.input_gains[CH_LFE] );

    _conf->get_float("gain_output_L"    ,state.output_gains[CH_L]  );
    _conf->get_float("gain_output_C"    ,state.output_gains[CH_C]  );
    _conf->get_float("gain_output_R"    ,state.output_gains[CH_R]  );
    _conf->get_float("gain_output_SL"   ,state.output_gains[CH_SL] );
    _conf->get_float("gain_output_SR"   ,state.output_gains[CH_SR] );
    _conf->get_float("gain_output_LFE"  ,state.output_gains[CH_LFE]);
  }

  if (_what & AC3FILTER_DELAY)
  {
    // Delays
    _conf->get_bool ("delay"            ,state.delay           );
    _conf->get_int32("delay_units"      ,state.delay_units     );
    _conf->get_float("delay_L"          ,state.delays[CH_L]    );
    _conf->get_float("delay_C"          ,state.delays[CH_C]    );
    _conf->get_float("delay_R"          ,state.delays[CH_R]    );
    _conf->get_float("delay_SL"         ,state.delays[CH_SL]   );
    _conf->get_float("delay_SR"         ,state.delays[CH_SR]   );
    _conf->get_float("delay_LFE"        ,state.delays[CH_LFE]  );
  }

  if (_what & AC3FILTER_SYNC)
  {
    _conf->get_float("time_shift"       ,state.time_shift      );
    _conf->get_float("time_factor"      ,state.time_factor     );
    _conf->get_bool ("dejitter"         ,state.dejitter        );
    _conf->get_float("threshold"        ,state.threshold       );
  }

  if (_what & AC3FILTER_MATRIX)
  {
    // state.matrix
    _conf->get_float("matrix_L_L",    state.matrix[0][0]);
    _conf->get_float("matrix_L_C",    state.matrix[0][1]);
    _conf->get_float("matrix_L_R",    state.matrix[0][2]);
    _conf->get_float("matrix_L_SL",   state.matrix[0][3]);
    _conf->get_float("matrix_L_SR",   state.matrix[0][4]);
    _conf->get_float("matrix_L_LFE",  state.matrix[0][5]);
                                       
    _conf->get_float("matrix_C_L",    state.matrix[1][0]);
    _conf->get_float("matrix_C_C",    state.matrix[1][1]);
    _conf->get_float("matrix_C_R",    state.matrix[1][2]);
    _conf->get_float("matrix_C_SL",   state.matrix[1][3]);
    _conf->get_float("matrix_C_SR",   state.matrix[1][4]);
    _conf->get_float("matrix_C_LFE",  state.matrix[1][5]);
                                       
    _conf->get_float("matrix_R_L",    state.matrix[2][0]);
    _conf->get_float("matrix_R_C",    state.matrix[2][1]);
    _conf->get_float("matrix_R_R",    state.matrix[2][2]);
    _conf->get_float("matrix_R_SL",   state.matrix[2][3]);
    _conf->get_float("matrix_R_SR",   state.matrix[2][4]);
    _conf->get_float("matrix_R_LFE",  state.matrix[2][5]);
      
    _conf->get_float("matrix_SL_L",   state.matrix[3][0]);
    _conf->get_float("matrix_SL_C",   state.matrix[3][1]);
    _conf->get_float("matrix_SL_R",   state.matrix[3][2]);
    _conf->get_float("matrix_SL_SL",  state.matrix[3][3]);
    _conf->get_float("matrix_SL_SR",  state.matrix[3][4]);
    _conf->get_float("matrix_SL_LFE", state.matrix[3][5]);
                                       
    _conf->get_float("matrix_SR_L",   state.matrix[4][0]);
    _conf->get_float("matrix_SR_C",   state.matrix[4][1]);
    _conf->get_float("matrix_SR_R",   state.matrix[4][2]);
    _conf->get_float("matrix_SR_SL",  state.matrix[4][3]);
    _conf->get_float("matrix_SR_SR",  state.matrix[4][4]);
    _conf->get_float("matrix_SR_LFE", state.matrix[4][5]);

    _conf->get_float("matrix_LFE_L",  state.matrix[5][0]);
    _conf->get_float("matrix_LFE_C",  state.matrix[5][1]);
    _conf->get_float("matrix_LFE_R",  state.matrix[5][2]);
    _conf->get_float("matrix_LFE_SL", state.matrix[5][3]);
    _conf->get_float("matrix_LFE_SR", state.matrix[5][4]);
    _conf->get_float("matrix_LFE_LFE",state.matrix[5][5]);
  }

  dec.set_state(&state);

  if (_what & AC3FILTER_SYS)
  {
    int spdif_pt = dec.get_spdif();
    _conf->get_int32("formats"          ,formats         );
    _conf->get_int32("spdif_pt"         ,spdif_pt        );
    dec.set_spdif(spdif_pt);
//    conf->get_bool   ("generate_timestamps", generate_timestamps);
//    conf->get_int32  ("time_shift"       ,time_shift      );
//    conf->get_bool   ("jitter"           ,jitter_on       );
//    conf->get_bool   ("config_autoload"  ,config_autoload );
                       
  }


  return S_OK;
}

STDMETHODIMP AC3Filter::save_params(Config *_conf, int _what)
{
  AudioProcessorState state;
  dec.get_state(&state);

  RegistryKey reg;
  if (!_conf)
  {
    _conf = &reg;
    reg.create_key(REG_KEY_PRESET"\\Default");
  }

  if (_what & AC3FILTER_SPK)
  {
    _conf->set_int32("format"           ,out_spk.format  );
    _conf->set_int32("mask"             ,out_spk.mask    );
    _conf->set_int32("relation"         ,out_spk.relation);
    _conf->set_bool ("spdif"            ,spdif           );
  }

  if (_what & AC3FILTER_PROC)
  {
    // Options
    _conf->set_bool ("auto_gain"        ,state.auto_gain       );
    _conf->set_bool ("normalize"        ,state.normalize       );
    _conf->set_bool ("normalize_matrix" ,state.normalize_matrix);
    _conf->set_bool ("auto_matrix"      ,state.auto_matrix     );
    _conf->set_bool ("expand_stereo"    ,state.expand_stereo   );
    _conf->set_bool ("voice_control"    ,state.voice_control   );
    _conf->set_float("release"          ,state.release         );
    // Gains
    _conf->set_float("master"           ,state.master          );
    _conf->set_float("clev"             ,state.clev            );
    _conf->set_float("slev"             ,state.slev            );
    _conf->set_float("lfelev"           ,state.lfelev          );
    // DRC
    _conf->set_bool ("drc"              ,state.drc             );
    _conf->set_float("drc_power"        ,state.drc_power       );
    // Bass redirection
    _conf->set_bool ("bass_redir"       ,state.bass_redir      );
    _conf->set_int32("bass_freq"        ,state.bass_freq       );
  }

  if (_what & AC3FILTER_GAINS)
  {
    // I/O Gains
    _conf->set_float("gain_input_L"     ,state.input_gains[CH_L]   );
    _conf->set_float("gain_input_C"     ,state.input_gains[CH_C]   );
    _conf->set_float("gain_input_R"     ,state.input_gains[CH_R]   );
    _conf->set_float("gain_input_SL"    ,state.input_gains[CH_SL]  );
    _conf->set_float("gain_input_SR"    ,state.input_gains[CH_SR]  );
    _conf->set_float("gain_input_LFE"   ,state.input_gains[CH_LFE] );

    _conf->set_float("gain_output_L"    ,state.output_gains[CH_L]  );
    _conf->set_float("gain_output_C"    ,state.output_gains[CH_C]  );
    _conf->set_float("gain_output_R"    ,state.output_gains[CH_R]  );
    _conf->set_float("gain_output_SL"   ,state.output_gains[CH_SL] );
    _conf->set_float("gain_output_SR"   ,state.output_gains[CH_SR] );
    _conf->set_float("gain_output_LFE"  ,state.output_gains[CH_LFE]);
  }

  if (_what & AC3FILTER_DELAY)
  {
    // Delays
    _conf->set_bool ("delay"            ,state.delay           );
    _conf->set_int32("delay_units"      ,state.delay_units     );
    _conf->set_float("delay_L"          ,state.delays[CH_L]    );
    _conf->set_float("delay_C"          ,state.delays[CH_C]    );
    _conf->set_float("delay_R"          ,state.delays[CH_R]    );
    _conf->set_float("delay_SL"         ,state.delays[CH_SL]   );
    _conf->set_float("delay_SR"         ,state.delays[CH_SR]   );
    _conf->set_float("delay_LFE"        ,state.delays[CH_LFE]  );
  }

  if (_what & AC3FILTER_SYNC)
  {
    _conf->set_float("time_shift"       ,state.time_shift      );
    _conf->set_float("time_factor"      ,state.time_factor     );
    _conf->set_bool ("dejitter"         ,state.dejitter        );
    _conf->set_float("threshold"        ,state.threshold       );
  }

  if (_what & AC3FILTER_MATRIX)
  {
    // state.matrix
    _conf->set_float("matrix_L_L",    state.matrix[0][0]);
    _conf->set_float("matrix_L_C",    state.matrix[0][1]);
    _conf->set_float("matrix_L_R",    state.matrix[0][2]);
    _conf->set_float("matrix_L_SL",   state.matrix[0][3]);
    _conf->set_float("matrix_L_SR",   state.matrix[0][4]);
    _conf->set_float("matrix_L_LFE",  state.matrix[0][5]);
                                       
    _conf->set_float("matrix_C_L",    state.matrix[1][0]);
    _conf->set_float("matrix_C_C",    state.matrix[1][1]);
    _conf->set_float("matrix_C_R",    state.matrix[1][2]);
    _conf->set_float("matrix_C_SL",   state.matrix[1][3]);
    _conf->set_float("matrix_C_SR",   state.matrix[1][4]);
    _conf->set_float("matrix_C_LFE",  state.matrix[1][5]);
                                       
    _conf->set_float("matrix_R_L",    state.matrix[2][0]);
    _conf->set_float("matrix_R_C",    state.matrix[2][1]);
    _conf->set_float("matrix_R_R",    state.matrix[2][2]);
    _conf->set_float("matrix_R_SL",   state.matrix[2][3]);
    _conf->set_float("matrix_R_SR",   state.matrix[2][4]);
    _conf->set_float("matrix_R_LFE",  state.matrix[2][5]);
      
    _conf->set_float("matrix_SL_L",   state.matrix[3][0]);
    _conf->set_float("matrix_SL_C",   state.matrix[3][1]);
    _conf->set_float("matrix_SL_R",   state.matrix[3][2]);
    _conf->set_float("matrix_SL_SL",  state.matrix[3][3]);
    _conf->set_float("matrix_SL_SR",  state.matrix[3][4]);
    _conf->set_float("matrix_SL_LFE", state.matrix[3][5]);
                                       
    _conf->set_float("matrix_SR_L",   state.matrix[4][0]);
    _conf->set_float("matrix_SR_C",   state.matrix[4][1]);
    _conf->set_float("matrix_SR_R",   state.matrix[4][2]);
    _conf->set_float("matrix_SR_SL",  state.matrix[4][3]);
    _conf->set_float("matrix_SR_SR",  state.matrix[4][4]);
    _conf->set_float("matrix_SR_LFE", state.matrix[4][5]);

    _conf->set_float("matrix_LFE_L",  state.matrix[5][0]);
    _conf->set_float("matrix_LFE_C",  state.matrix[5][1]);
    _conf->set_float("matrix_LFE_R",  state.matrix[5][2]);
    _conf->set_float("matrix_LFE_SL", state.matrix[5][3]);
    _conf->set_float("matrix_LFE_SR", state.matrix[5][4]);
    _conf->set_float("matrix_LFE_LFE",state.matrix[5][5]);
  }

  if (_what & AC3FILTER_SYS)
  {
    int spdif_pt = dec.get_spdif();
    _conf->set_int32("formats"          ,formats         );
    _conf->set_int32("spdif_pt"         ,spdif_pt        );
//    conf->set_bool   ("generate_timestamps", generate_timestamps);
//    conf->set_int32  ("time_shift"       ,time_shift      );
//    conf->set_bool   ("jitter"           ,jitter_on       );
//    conf->set_bool   ("config_autoload"  ,config_autoload );
                       
  }

  return S_OK;
}
