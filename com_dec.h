/*
  COM-based DVDDecoder
*/


#ifndef COM_DEC_H
#define COM_DEC_H

#include "filters\dvd_graph.h"
#include "win32\thread.h"
#include "guids.h"

class COMDecoder : public Filter, public IDecoder, public IAudioProcessor
{
protected:
  int  formats; // formats allowed
  DVDGraph dvd;

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

  COMDecoder(IUnknown *_outer);

  /////////////////////////////////////////////////////////
  // DVDGraph interface

  void set_sink(const Sink *sink);
  const Sink *get_sink() const;

  /////////////////////////////////////////////////////////
  // Filter interface
  // (protect state-changing functions)

  virtual void reset();

  virtual bool is_ofdd() const;
  virtual bool query_input(Speakers spk) const;
  virtual bool set_input(Speakers spk);
  virtual Speakers get_input() const;

  virtual bool process(const Chunk *chunk);
  virtual Speakers get_output() const;
  virtual bool is_empty() const;
  virtual bool get_chunk(Chunk *chunk);

  /////////////////////////////////////////////////////////
  // IDecoder

  /////////////////////////////////////
  // Input/output control

  // Input/output format
  STDMETHODIMP get_in_spk (Speakers *spk);
  STDMETHODIMP get_out_spk(Speakers *spk);

  // User format
  STDMETHODIMP get_user_spk(Speakers *spk);
  STDMETHODIMP set_user_spk(Speakers  spk);

  // Formats to accept
  STDMETHODIMP get_formats(int *formats);
  STDMETHODIMP set_formats(int  formats);

  // Query sink about output format support
  STDMETHODIMP get_query_sink(bool *query_sink);
  STDMETHODIMP set_query_sink(bool  query_sink);

  /////////////////////////////////////
  // SPDIF options

  // Use SPDIF if possible
  STDMETHODIMP get_use_spdif(bool *use_spdif);
  STDMETHODIMP set_use_spdif(bool  use_spdif);

  // SPDIF passthrough (formats bitmask)
  STDMETHODIMP get_spdif_pt(int *spdif_pt);
  STDMETHODIMP set_spdif_pt(int  spdif_pt);

  // SPDIF as PCM output
  STDMETHODIMP get_spdif_as_pcm(bool *spdif_as_pcm);
  STDMETHODIMP set_spdif_as_pcm(bool  spdif_as_pcm);

  // SPDIF encode
  STDMETHODIMP get_spdif_encode(bool *spdif_encode);
  STDMETHODIMP set_spdif_encode(bool  spdif_encode);

  // SPDIF stereo PCM passthrough
  STDMETHODIMP get_spdif_stereo_pt(bool *spdif_stereo_pt);
  STDMETHODIMP set_spdif_stereo_pt(bool  spdif_stereo_pt);

  // SPDIF check sample rate
  STDMETHODIMP get_spdif_check_sr(bool *spdif_check_sr);
  STDMETHODIMP set_spdif_check_sr(bool  spdif_check_sr);
  STDMETHODIMP get_spdif_allow_48(bool *spdif_allow_48);
  STDMETHODIMP set_spdif_allow_48(bool  spdif_allow_48);
  STDMETHODIMP get_spdif_allow_44(bool *spdif_allow_44);
  STDMETHODIMP set_spdif_allow_44(bool  spdif_allow_44);
  STDMETHODIMP get_spdif_allow_32(bool *spdif_allow_32);
  STDMETHODIMP set_spdif_allow_32(bool  spdif_allow_32);

  // SPDIF status
  STDMETHODIMP get_spdif_status(int *spdif_status);

  /////////////////////////////////////
  // Other

  // Stats
  STDMETHODIMP get_frames(int  *frames, int *errors);
  STDMETHODIMP get_info  (char *info, int len);

  // Load/save settings
  STDMETHODIMP load_params(Config *config, int what);
  STDMETHODIMP save_params(Config *config, int what);

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
  // Linear time transform
  STDMETHODIMP get_time_shift   (vtime_t *time_shift);
  STDMETHODIMP set_time_shift   (vtime_t  time_shift);
  STDMETHODIMP get_time_factor  (vtime_t *time_factor);
  STDMETHODIMP set_time_factor  (vtime_t  time_factor);
  // Dejitter
  STDMETHODIMP get_dejitter     (bool *dejitter);
  STDMETHODIMP set_dejitter     (bool  dejitter);
  STDMETHODIMP get_threshold    (vtime_t *threshold);
  STDMETHODIMP set_threshold    (vtime_t  threshold);
  STDMETHODIMP get_jitter       (vtime_t *input_mean, vtime_t *input_stddev, vtime_t *output_mean, vtime_t *output_stddev);
                                
  STDMETHODIMP get_state        (AudioProcessorState *state, vtime_t time = 0);
  STDMETHODIMP set_state        (AudioProcessorState *state);
};


#endif
