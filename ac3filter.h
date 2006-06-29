/*
  AC3Filter class
*/


#ifndef AC3FILTER_H
#define AC3FILTER_H

#include <streams.h>
#include "win32\cpu.h"
#include "sink\sink_dshow.h"
#include "com_dec.h"

#include "rot.h"

class AC3Filter : public CTransformFilter, public IAC3Filter, public ISpecifyPropertyPages
{
protected:
  CPUMeter   cpu;       // CPU usage meter
  COMDecoder dec;       // decoder & processor
  DShowSink  *sink;     // sink
  ROTEntry   rot;       // registred objects table entry

  int  formats;         // formats supported by filter
  bool config_autoload; // auto-load configuration files

//  Speakers in_spk;
//  Speakers out_spk;

  bool set_input(const CMediaType &mt);
  bool set_input(Speakers in_spk);

  Speakers guess_pcm_output();
  Speakers guess_spdif_output();

  bool process_chunk(const Chunk *chunk);
  void reset();


private:
  AC3Filter(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);
  ~AC3Filter();

public:
  DECLARE_IUNKNOWN;
  STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
  static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);


  /////////////////////////////////////////////////////////
  // CTransformFilter

  HRESULT Receive(IMediaSample *in);

  HRESULT EndOfStream();
  HRESULT NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);

  HRESULT StartStreaming();
  HRESULT StopStreaming();

  HRESULT BeginFlush();
  HRESULT EndFlush();

  STDMETHODIMP Stop();
  STDMETHODIMP Pause();
  STDMETHODIMP Run(REFERENCE_TIME tStart);
  STDMETHODIMP JoinFilterGraph(IFilterGraph *pGraph, LPCWSTR pName);

  HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
  HRESULT CheckInputType(const CMediaType *mt);
  HRESULT CheckOutputType(const CMediaType *mt);
  HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
  bool    CheckConnectPin(IPin *pin);
  HRESULT CheckConnect(PIN_DIRECTION dir, IPin *pin);
  HRESULT SetMediaType(PIN_DIRECTION direction, const CMediaType *mt);

  HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties);


  /////////////////////////////////////////////////////////
  // ISpecifyPropertyPages

  STDMETHODIMP GetPages(CAUUID *pPages);

  /////////////////////////////////////////////////////////
  // IAC3Filter

  // Input/output format
  STDMETHODIMP get_in_spk (Speakers *spk);
  STDMETHODIMP get_out_spk(Speakers *spk);

  // User format
  STDMETHODIMP get_user_spk(Speakers *spk);
  STDMETHODIMP set_user_spk(Speakers  spk);

  // Use SPDIF if possible
  STDMETHODIMP get_use_spdif(bool *use_spdif);
  STDMETHODIMP set_use_spdif(bool  use_spdif);

  // SPDIF passthrough (formats bitmask)
  STDMETHODIMP get_spdif_pt(int *spdif_pt);
  STDMETHODIMP set_spdif_pt(int  spdif_pt);

  // SPDIF stereo PCM passthrough
  STDMETHODIMP get_spdif_stereo_pt(bool *spdif_stereo_pt);
  STDMETHODIMP set_spdif_stereo_pt(bool  spdif_stereo_pt);

  // SPDIF status
  STDMETHODIMP get_spdif_status(int *spdif_status);

  // Formats to accept
  STDMETHODIMP get_formats(int *formats);
  STDMETHODIMP set_formats(int  formats);

  // Timing
  STDMETHODIMP get_playback_time (vtime_t *time);

  // CPU usage
  STDMETHODIMP get_cpu_usage(double *cpu_usage);

  // Config
  STDMETHODIMP get_config_file(char *filename, int size);
  STDMETHODIMP get_config_autoload(bool *config_autoload);
  STDMETHODIMP set_config_autoload(bool  config_autoload);

  // Load/save settings
  STDMETHODIMP load_params(Config *config, int what);
  STDMETHODIMP save_params(Config *config, int what);
};

#endif