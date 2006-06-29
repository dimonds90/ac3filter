/*
  COM-based DVDDecoder
*/


#ifndef COM_DEC_H
#define COM_DEC_H

#include "filters\dvd_graph.h"
#include "win32\thread.h"
#include "guids.h"

class COMDecoder : public IAudioProcessor, public IDecoder, public DVDGraph
{
protected:
  // COM support
  IUnknown *outer;
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) 
    { return outer->QueryInterface(riid, ppvObject); };
  ULONG   STDMETHODCALLTYPE AddRef()   
    { return outer->AddRef();  }
  ULONG   STDMETHODCALLTYPE Release()  
    { return outer->Release(); }

public:
  CritSec config;

  COMDecoder(IUnknown *_outer) 
  { outer = _outer; };

  /////////////////////////////////////////////////////////
  // Filter interface
  // (protect state-changing functions)

  virtual void reset();
  virtual bool set_input(Speakers spk);
  virtual bool process(const Chunk *chunk);
  virtual bool get_chunk(Chunk *chunk);

  /////////////////////////////////////////////////////////
  // IAudioProcessor

  // AGC options
  STDMETHODIMP get_auto_gain    (bool *auto_gain);
  STDMETHODIMP set_auto_gain    (bool  auto_gain);
  STDMETHODIMP get_normalize    (bool *normalize);
  STDMETHODIMP set_normalize    (bool  normalize);
  STDMETHODIMP get_attack       (sample_t *attack);
  STDMETHODIMP set_attack       (sample_t  attack);
  STDMETHODIMP get_release      (sample_t *release);
  STDMETHODIMP set_release      (sample_t  release);
  // Matrix options           
  STDMETHODIMP get_auto_matrix  (bool *auto_matrix);
  STDMETHODIMP set_auto_matrix  (bool  auto_matrix);
  STDMETHODIMP get_normalize_matrix(bool *normalize_matrix);
  STDMETHODIMP set_normalize_matrix(bool  normalize_matrix);
  STDMETHODIMP get_voice_control(bool *voice_control);
  STDMETHODIMP set_voice_control(bool  voice_control);
  STDMETHODIMP get_expand_stereo(bool *expand_stereo);
  STDMETHODIMP set_expand_stereo(bool  expand_stereo);
  // Master gain
  STDMETHODIMP get_master       (sample_t *master);
  STDMETHODIMP set_master       (sample_t  master);
  STDMETHODIMP get_gain         (sample_t *gain);
  // Mix levels               
  STDMETHODIMP get_clev         (sample_t *clev);
  STDMETHODIMP set_clev         (sample_t  clev);
  STDMETHODIMP get_slev         (sample_t *slev);
  STDMETHODIMP set_slev         (sample_t  slev);
  STDMETHODIMP get_lfelev       (sample_t *lfelev);
  STDMETHODIMP set_lfelev       (sample_t  lfelev);
  // Input/output gains
  STDMETHODIMP get_input_gains  (sample_t *input_gains) ;
  STDMETHODIMP set_input_gains  (sample_t *input_gains) ;
  STDMETHODIMP get_output_gains (sample_t *output_gains);
  STDMETHODIMP set_output_gains (sample_t *output_gains);
  // Input/output levels
  STDMETHODIMP get_levels       (vtime_t time, sample_t *input_levels, sample_t *output_levels);
  STDMETHODIMP get_hist         (int count, double *input_hist, double *output_hist);
  // Matrix                     
  STDMETHODIMP get_matrix       (matrix_t *matrix);
  STDMETHODIMP set_matrix       (matrix_t *matrix);
  // DRC                        
  STDMETHODIMP get_drc          (bool *drc);
  STDMETHODIMP set_drc          (bool  drc);
  STDMETHODIMP get_drc_power    (sample_t *drc_power);
  STDMETHODIMP set_drc_power    (sample_t  drc_power);
  STDMETHODIMP get_drc_level    (sample_t *drc_level);
  // Bass redirection
  STDMETHODIMP get_bass_redir   (bool *bass_redir);
  STDMETHODIMP set_bass_redir   (bool  bass_redir);
  STDMETHODIMP get_bass_freq    (int  *bass_freq);
  STDMETHODIMP set_bass_freq    (int   bass_freq);
  // Delay
  STDMETHODIMP get_delay        (bool *delay);
  STDMETHODIMP set_delay        (bool  delay);
  STDMETHODIMP get_delay_units  (int *delay_units);
  STDMETHODIMP set_delay_units  (int  delay_units);
  STDMETHODIMP get_delays       (float *delays);
  STDMETHODIMP set_delays       (float *delays);
  // Syncronization
  STDMETHODIMP get_time_shift   (vtime_t *time_shift);
  STDMETHODIMP set_time_shift   (vtime_t  time_shift);
  STDMETHODIMP get_time_factor  (vtime_t *time_factor);
  STDMETHODIMP set_time_factor  (vtime_t  time_factor);
  STDMETHODIMP get_dejitter     (bool *dejitter);
  STDMETHODIMP set_dejitter     (bool  dejitter);
  STDMETHODIMP get_threshold    (vtime_t *threshold);
  STDMETHODIMP set_threshold    (vtime_t  threshold);
  STDMETHODIMP get_jitter       (vtime_t *jitter);
                                
  STDMETHODIMP get_state        (AudioProcessorState *state, vtime_t time = 0);
  STDMETHODIMP set_state        (AudioProcessorState *state);
                                
  /////////////////////////////////////////////////////////
  // IDecoder

  // Input speakers configuration
  STDMETHODIMP get_spk   (Speakers *spk);

  // Stats
  STDMETHODIMP get_frames(int  *frames, int *errors);
  STDMETHODIMP get_info  (char *info, int len);

  // Downmix coefs locks
  STDMETHODIMP get_locks (bool *clev_lock, bool *slev_lock, bool *lfelev_lock);
  STDMETHODIMP set_locks (bool  clev_lock, bool  slev_lock, bool  lfelev_lock);

  /////////////////////////////////////////////////////////
  // IAC3Decoder

  STDMETHODIMP get_eq_on(bool *eq_on);
  STDMETHODIMP set_eq_on(bool  eq_on);

  STDMETHODIMP get_eq(sample_t *eq);
  STDMETHODIMP set_eq(sample_t *eq);
};


#endif
