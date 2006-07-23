#include "com_dec.h"
#include <stdio.h>

COMDecoder::COMDecoder(IUnknown *_outer) 
{ 
  outer = _outer; 
  formats = FORMAT_CLASS_PCM | FORMAT_MASK_AC3 | FORMAT_MASK_MPA | FORMAT_MASK_DTS | FORMAT_MASK_PES;
  dvd.proc.set_input_order(win_order);
  dvd.proc.set_output_order(win_order);
};

///////////////////////////////////////////////////////////////////////////////
// DVDGraph interface

void COMDecoder::set_sink(const Sink *_sink)
{
  dvd.set_sink(_sink);
}

const Sink *COMDecoder::get_sink() const
{
  return dvd.get_sink();
}

///////////////////////////////////////////////////////////////////////////////
// Filter interface

void COMDecoder::reset()
{
  AutoLock config_lock(&config);
  dvd.reset();
}
bool COMDecoder::is_ofdd() const
{
  return dvd.is_ofdd();
}
bool COMDecoder::query_input(Speakers _spk) const
{
  return ((FORMAT_MASK(_spk.format) & formats) != 0) && dvd.query_input(_spk);
}
bool COMDecoder::set_input(Speakers _spk)
{
  AutoLock config_lock(&config);
  return dvd.set_input(_spk);
}
Speakers COMDecoder::get_input() const
{
  return dvd.get_input();
}
bool COMDecoder::process(const Chunk *_chunk)
{
  AutoLock config_lock(&config);
  return dvd.process(_chunk);
}
Speakers COMDecoder::get_output() const
{
  return dvd.get_output();
}
bool COMDecoder::is_empty() const
{
  return dvd.is_empty();
}
bool COMDecoder::get_chunk(Chunk *_chunk)
{
  AutoLock config_lock(&config);
  return dvd.get_chunk(_chunk);
}

///////////////////////////////////////////////////////////////////////////////
// IDecoder

// Input/output format
STDMETHODIMP COMDecoder::get_in_spk(Speakers *_spk)
{
  if (_spk) *_spk = dvd.get_input();
  return S_OK;
}
STDMETHODIMP COMDecoder::get_out_spk(Speakers *_spk)
{
  if (_spk) *_spk = dvd.get_output();
  return S_OK;
}

// User format
STDMETHODIMP COMDecoder::get_user_spk(Speakers *_spk)
{
  if (_spk) *_spk = dvd.get_user();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_user_spk(Speakers  _spk)
{
  return dvd.set_user(_spk)? S_OK: E_FAIL;
}

// Formats to accept
STDMETHODIMP COMDecoder::get_formats(int *_formats)
{
  if (_formats) *_formats = formats;
  return S_OK;
}
STDMETHODIMP COMDecoder::set_formats(int  _formats)
{
  formats = _formats;
  return S_OK;
}

// Query sink about output format support
STDMETHODIMP COMDecoder::get_query_sink(bool *_query_sink)
{
  if (_query_sink) *_query_sink = dvd.get_query_sink();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_query_sink(bool  _query_sink)
{
  dvd.set_query_sink(_query_sink);
  return S_OK;
}

// Use SPDIF if possible
STDMETHODIMP COMDecoder::get_use_spdif(bool *_use_spdif)
{
  if (_use_spdif) *_use_spdif = dvd.get_use_spdif();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_use_spdif(bool  _use_spdif)
{
  dvd.set_use_spdif(_use_spdif);
  return S_OK;
}

// SPDIF passthrough (formats bitmask)
STDMETHODIMP COMDecoder::get_spdif_pt(int *_spdif_pt)
{
  if (_spdif_pt) *_spdif_pt = dvd.get_spdif_pt();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_pt(int  _spdif_pt)
{
  dvd.set_spdif_pt(_spdif_pt);
  return S_OK;
}

// SPDIF as PCM output
STDMETHODIMP COMDecoder::get_spdif_as_pcm(bool *_spdif_as_pcm)
{
  if (_spdif_as_pcm) *_spdif_as_pcm = dvd.get_spdif_as_pcm();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_as_pcm(bool  _spdif_as_pcm)
{
  dvd.set_spdif_as_pcm(_spdif_as_pcm);
  return S_OK;
}

// SPDIF encode
STDMETHODIMP COMDecoder::get_spdif_encode(bool *_spdif_encode)
{
  if (_spdif_encode) *_spdif_encode = dvd.get_spdif_encode();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_encode(bool  _spdif_encode)
{
  dvd.set_spdif_encode(_spdif_encode);
  return S_OK;
}

// SPDIF stereo PCM passthrough
STDMETHODIMP COMDecoder::get_spdif_stereo_pt(bool *_spdif_stereo_pt)
{
  if (_spdif_stereo_pt) *_spdif_stereo_pt = dvd.get_spdif_stereo_pt();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_stereo_pt(bool  _spdif_stereo_pt)
{
  dvd.set_spdif_stereo_pt(_spdif_stereo_pt);
  return S_OK;
}

// SPDIF check sample rate
STDMETHODIMP COMDecoder::get_spdif_check_sr(bool *_spdif_check_sr)
{
  if (_spdif_check_sr) *_spdif_check_sr = dvd.get_spdif_check_sr();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_check_sr(bool  _spdif_check_sr)
{
  dvd.set_spdif_check_sr(_spdif_check_sr);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_spdif_allow_48(bool *_spdif_allow_48)
{
  if (_spdif_allow_48) *_spdif_allow_48 = dvd.get_spdif_allow_48();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_allow_48(bool  _spdif_allow_48)
{
  dvd.set_spdif_allow_48(_spdif_allow_48);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_spdif_allow_44(bool *_spdif_allow_44)
{
  if (_spdif_allow_44) *_spdif_allow_44 = dvd.get_spdif_allow_44();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_allow_44(bool  _spdif_allow_44)
{
  dvd.set_spdif_allow_44(_spdif_allow_44);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_spdif_allow_32(bool *_spdif_allow_32)
{
  if (_spdif_allow_32) *_spdif_allow_32 = dvd.get_spdif_allow_32();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_allow_32(bool  _spdif_allow_32)
{
  dvd.set_spdif_allow_32(_spdif_allow_32);
  return S_OK;
}

// SPDIF status
STDMETHODIMP COMDecoder::get_spdif_status(int *_spdif_status)
{
  if (_spdif_status) *_spdif_status = dvd.get_spdif_status();
  return S_OK;
}

STDMETHODIMP COMDecoder::get_frames(int *_frames, int *_errors)
{
  *_frames = dvd.dec.get_frames() + dvd.spdifer_pt.get_frames() + dvd.spdifer_enc.get_frames();
  *_errors = dvd.dec.get_errors();
  return S_OK;
}

STDMETHODIMP COMDecoder::get_info(char *_info, int _len)
{
  dvd.get_info(_info, _len);
  cr2crlf(_info, _len);
  return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// IAudioProcessor

// AGC options
STDMETHODIMP COMDecoder::get_auto_gain (bool *_auto_gain)
{
  if (_auto_gain) *_auto_gain = dvd.proc.get_auto_gain();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_auto_gain (bool _auto_gain)
{
  AutoLock config_lock(&config);
  dvd.proc.set_auto_gain(_auto_gain);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_normalize (bool *_normalize)
{
  if (_normalize) *_normalize = dvd.proc.get_normalize();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_normalize (bool _normalize)
{
  AutoLock config_lock(&config);
  dvd.proc.set_normalize(_normalize);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_attack(sample_t *_attack)
{
  if (_attack) *_attack = dvd.proc.get_attack();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_attack(sample_t _attack)
{
  AutoLock config_lock(&config);
  dvd.proc.set_attack(_attack);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_release(sample_t *_release)
{
  if (_release) *_release = dvd.proc.get_release();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_release(sample_t _release)
{
  AutoLock config_lock(&config);
  dvd.proc.set_release(_release);
  return S_OK;
}

// Matrix options
STDMETHODIMP COMDecoder::get_auto_matrix(bool *_auto_matrix)
{
  if (_auto_matrix) *_auto_matrix = dvd.proc.get_auto_matrix();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_auto_matrix(bool _auto_matrix)
{
  AutoLock config_lock(&config);
  dvd.proc.set_auto_matrix(_auto_matrix);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_normalize_matrix(bool *_normalize_matrix)
{
  if (_normalize_matrix) *_normalize_matrix = dvd.proc.get_normalize_matrix();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_normalize_matrix(bool _normalize_matrix)
{
  AutoLock config_lock(&config);
  dvd.proc.set_normalize_matrix(_normalize_matrix);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_voice_control(bool *_voice_control)
{
  if (_voice_control) *_voice_control = dvd.proc.get_voice_control();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_voice_control(bool _voice_control)
{
  AutoLock config_lock(&config);
  dvd.proc.set_voice_control(_voice_control);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_expand_stereo(bool *_expand_stereo)
{
  if (_expand_stereo) *_expand_stereo = dvd.proc.get_expand_stereo();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_expand_stereo(bool _expand_stereo)
{
  AutoLock config_lock(&config);
  dvd.proc.set_expand_stereo(_expand_stereo);
  return S_OK;
}

// Master gain
STDMETHODIMP COMDecoder::get_master(sample_t *_master)
{
  if (_master) *_master = dvd.proc.get_master();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_master(sample_t _master)
{
  AutoLock config_lock(&config);
  dvd.proc.set_master(_master);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_gain(sample_t *_gain)
{
  if (_gain) *_gain = dvd.proc.get_gain();
  return S_OK;
}

// Mix levels
STDMETHODIMP COMDecoder::get_clev(sample_t *_clev)
{
  if (_clev) *_clev = dvd.proc.get_clev();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_clev(sample_t _clev)
{
  AutoLock config_lock(&config);
  dvd.proc.set_clev(_clev);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_slev(sample_t *_slev)
{
  if (_slev) *_slev = dvd.proc.get_slev();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_slev(sample_t _slev)
{
  AutoLock config_lock(&config);
  dvd.proc.set_slev(_slev);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_lfelev(sample_t *_lfelev)
{
  if (_lfelev) *_lfelev = dvd.proc.get_lfelev();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_lfelev(sample_t _lfelev)
{
  AutoLock config_lock(&config);
  dvd.proc.set_lfelev(_lfelev);
  return S_OK;
}

// Input/output gains
STDMETHODIMP COMDecoder::get_input_gains(sample_t *_input_gains)
{
  if (_input_gains) dvd.proc.get_input_gains(_input_gains);
  return S_OK;
}
STDMETHODIMP COMDecoder::set_input_gains(sample_t *_input_gains)
{
  AutoLock config_lock(&config);
  dvd.proc.set_input_gains(_input_gains);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_output_gains(sample_t *_output_gains)
{
  if (_output_gains) dvd.proc.get_output_gains(_output_gains);
  return S_OK;
}
STDMETHODIMP COMDecoder::set_output_gains(sample_t *_output_gains)
{
  AutoLock config_lock(&config);
  dvd.proc.set_output_gains(_output_gains);
  return S_OK;
}

// Input/output levels
STDMETHODIMP COMDecoder::get_levels(vtime_t _time, sample_t *_input_levels, sample_t *_output_levels)
{
  if (_input_levels)  dvd.proc.get_input_levels(_time, _input_levels);
  if (_output_levels) dvd.proc.get_output_levels(_time, _output_levels);
  return S_OK;
}

// Matrix
STDMETHODIMP COMDecoder::get_matrix(matrix_t *_matrix)
{
  if (_matrix) dvd.proc.get_matrix(_matrix);
  return S_OK;
}
STDMETHODIMP COMDecoder::set_matrix(matrix_t *_matrix)
{
  AutoLock config_lock(&config);
  dvd.proc.set_matrix(_matrix);
  return S_OK;
}

// DRC
STDMETHODIMP COMDecoder::get_drc(bool *_drc)
{
  if (_drc) *_drc = dvd.proc.get_drc();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_drc(bool _drc)
{
  AutoLock config_lock(&config);
  dvd.proc.set_drc(_drc);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_drc_power(sample_t *_drc_power)
{
  if (_drc_power) *_drc_power = dvd.proc.get_drc_power();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_drc_power(sample_t _drc_power)
{
  AutoLock config_lock(&config);
  dvd.proc.set_drc_power(_drc_power);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_drc_level(sample_t *_drc_level)
{
  if (_drc_level) *_drc_level = dvd.proc.get_drc_level();
  return S_OK;
}

// Bass redirection
STDMETHODIMP COMDecoder::get_bass_redir(bool *_bass_redir)
{
  if (_bass_redir) *_bass_redir = dvd.proc.get_bass_redir();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_bass_redir(bool _bass_redir)
{
  AutoLock config_lock(&config);
  dvd.proc.set_bass_redir(_bass_redir);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_bass_freq(int *_bass_freq)
{
  if (_bass_freq) *_bass_freq = dvd.proc.get_bass_freq();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_bass_freq(int bass_freq)
{
  AutoLock config_lock(&config);
  dvd.proc.set_bass_freq(bass_freq);
  return S_OK;
}

// Delay
STDMETHODIMP COMDecoder::get_delay(bool *_delay)
{
  if (_delay) *_delay = dvd.proc.get_delay();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_delay(bool _delay)
{
  AutoLock config_lock(&config);
  dvd.proc.set_delay(_delay);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_delay_units(int *_delay_units)
{
  if (_delay_units) *_delay_units = dvd.proc.get_delay_units();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_delay_units(int _delay_units)
{
  AutoLock config_lock(&config);
  dvd.proc.set_delay_units(_delay_units);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_delays(float *_delays)
{
  if (_delays) dvd.proc.get_delays(_delays);
  return S_OK;
}
STDMETHODIMP COMDecoder::set_delays(float *_delays)
{
  AutoLock config_lock(&config);
  dvd.proc.set_delays(_delays);
  return S_OK;
}

// Syncronization
STDMETHODIMP COMDecoder::get_time_shift(vtime_t *_time_shift)
{
  if (_time_shift) *_time_shift = dvd.syncer.get_time_shift();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_time_shift(vtime_t _time_shift)
{
  AutoLock config_lock(&config);
  dvd.syncer.set_time_shift(_time_shift);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_time_factor(vtime_t *_time_factor)
{
  if (_time_factor) *_time_factor = dvd.syncer.get_time_factor();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_time_factor(vtime_t _time_factor)
{
  AutoLock config_lock(&config);
  dvd.syncer.set_time_factor(_time_factor);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_dejitter(bool *_dejitter)
{
  if (_dejitter) *_dejitter = dvd.syncer.get_dejitter();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_dejitter(bool _dejitter)
{
  AutoLock config_lock(&config);
  dvd.syncer.set_dejitter(_dejitter);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_threshold(vtime_t *_threshold)
{
  if (_threshold) *_threshold = dvd.syncer.get_threshold();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_threshold(vtime_t _threshold)
{
  AutoLock config_lock(&config);
  dvd.syncer.set_threshold(_threshold);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_jitter(vtime_t *_input_mean, vtime_t *_input_stddev, vtime_t *_output_mean, vtime_t *_output_stddev)
{
  if (_input_mean)    *_input_mean    = dvd.syncer.get_input_mean();
  if (_input_stddev)  *_input_stddev  = dvd.syncer.get_input_stddev();
  if (_output_mean)   *_output_mean   = dvd.syncer.get_output_mean();
  if (_output_stddev) *_output_stddev = dvd.syncer.get_output_stddev();
  return S_OK;
}



STDMETHODIMP COMDecoder::get_state(AudioProcessorState *_state, vtime_t _time)
{
  AutoLock config_lock(&config);

  // AGC options
  get_auto_gain(&_state->auto_gain);
  get_normalize(&_state->normalize);
  get_attack(&_state->attack);
  get_release(&_state->release);

  // Matrix options
  get_auto_matrix(&_state->auto_matrix);
  get_normalize_matrix(&_state->normalize_matrix);
  get_voice_control(&_state->voice_control);
  get_expand_stereo(&_state->expand_stereo);

  // Master gain
  get_master(&_state->master);
  get_gain(&_state->gain);

  // Mix levels
  get_clev(&_state->clev);
  get_slev(&_state->slev);
  get_lfelev(&_state->lfelev);

  // Input/output gains
  get_input_gains(_state->input_gains);
  get_output_gains(_state->output_gains);

  // Input/output levels
  get_levels(_time, _state->input_levels, _state->output_levels);

  // Matrix
  get_matrix(&_state->matrix);

  // DRC
  get_drc(&_state->drc);
  get_drc_power(&_state->drc_power);
  get_drc_level(&_state->drc_level);

  // Bass redirection
  get_bass_redir(&_state->bass_redir);
  get_bass_freq(&_state->bass_freq);

  // Delay
  get_delay(&_state->delay);
  get_delay_units(&_state->delay_units);
  get_delays(_state->delays);
/*
  // Syncronization
  get_time_shift(&_state->time_shift);
  get_time_factor(&_state->time_factor);
  get_dejitter(&_state->dejitter);
  get_threshold(&_state->threshold);
  get_jitter(&_state->input_mean, &_state->input_stddev, &_state->output_mean, &_state->output_stddev);
*/
  return S_OK;
};

STDMETHODIMP COMDecoder::set_state     (AudioProcessorState *_state)
{
  AutoLock config_lock(&config);

  // AGC options
  set_auto_gain(_state->auto_gain);
  set_normalize(_state->normalize);
  set_attack(_state->attack);
  set_release(_state->release);

  // Matrix options
  set_auto_matrix(_state->auto_matrix);
  set_normalize_matrix(_state->normalize_matrix);
  set_voice_control(_state->voice_control);
  set_expand_stereo(_state->expand_stereo);

  // Master gain
  set_master(_state->master);

  // Mix levels
  set_clev(_state->clev);
  set_slev(_state->slev);
  set_lfelev(_state->lfelev);

  // Input/output gains
  set_input_gains(_state->input_gains);
  set_output_gains(_state->output_gains);

  // Matrix
  set_matrix(&_state->matrix);

  // DRC
  set_drc(_state->drc);
  set_drc_power(_state->drc_power);

  // Bass redirection
  set_bass_redir(_state->bass_redir);
  set_bass_freq(_state->bass_freq);

  // Delay
  set_delay(_state->delay);
  set_delay_units(_state->delay_units);
  set_delays(_state->delays);
/*
  // Syncronization
  set_time_shift(_state->time_shift);
  set_time_factor(_state->time_factor);
  set_dejitter(_state->dejitter);
  set_threshold(_state->threshold);
*/
  return S_OK;
};





// Load/save settings

STDMETHODIMP COMDecoder::load_params(Config *_conf, int _what)
{
  AutoLock config_lock(&config);
  AudioProcessorState state;
  get_state(&state);

  RegistryKey reg;
  if (!_conf)
  {
    _conf = &reg;
    reg.open_key(REG_KEY_PRESET"\\Default");
  }

  if (_what & AC3FILTER_SPK)
  {
    Speakers user_spk = dvd.get_user();
    bool use_spdif = dvd.get_use_spdif();

    _conf->get_int32("format"           ,user_spk.format  );
    _conf->get_int32("mask"             ,user_spk.mask    );
    _conf->get_int32("relation"         ,user_spk.relation);
    _conf->get_bool ("use_spdif"        ,use_spdif        );

    switch (user_spk.format)
    {
      case FORMAT_PCM16_BE:
      case FORMAT_PCM16: 
        user_spk.level = 32767;
        break;

      case FORMAT_PCM24_BE:
      case FORMAT_PCM24: 
        user_spk.level = 8388607;
        break;

      case FORMAT_PCM32_BE:
      case FORMAT_PCM32: 
        user_spk.level = 2147483647;      
        break;

      case FORMAT_PCMFLOAT:
        user_spk.level = 1.0; 
        break;

      default: 
        user_spk.level = 1.0; 
        break;
    }

    dvd.set_user(user_spk);
    dvd.set_use_spdif(use_spdif);
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
    _conf->get_float("attack"           ,state.attack          );
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
    vtime_t time_shift  = dvd.syncer.get_time_shift();
    vtime_t time_factor = dvd.syncer.get_time_factor();
    bool    dejitter    = dvd.syncer.get_dejitter();
    vtime_t threshold   = dvd.syncer.get_threshold();

    _conf->get_float("time_shift"       ,time_shift      );
    _conf->get_float("time_factor"      ,time_factor     );
    _conf->get_bool ("dejitter"         ,dejitter        );
    _conf->get_float("threshold"        ,threshold       );

    dvd.syncer.set_time_shift(time_shift);
    dvd.syncer.set_time_factor(time_factor);
    dvd.syncer.set_dejitter(dejitter);
    dvd.syncer.set_threshold(threshold);
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

  set_state(&state);

  if (_what & AC3FILTER_SYS)
  {
    bool query_sink = dvd.get_query_sink();

    int  spdif_pt = dvd.get_spdif_pt();
    bool spdif_as_pcm = dvd.get_spdif_as_pcm();
    bool spdif_encode = dvd.get_spdif_encode();
    bool spdif_stereo_pt = dvd.get_spdif_stereo_pt();

    bool spdif_check_sr = dvd.get_spdif_check_sr();
    bool spdif_allow_48 = dvd.get_spdif_allow_48();
    bool spdif_allow_44 = dvd.get_spdif_allow_44();
    bool spdif_allow_32 = dvd.get_spdif_allow_32();

    _conf->get_int32("formats"          ,formats         );
    _conf->get_bool ("query_sink"       ,query_sink      );

    _conf->get_int32("spdif_pt"         ,spdif_pt        );
    _conf->get_bool ("spdif_as_pcm"     ,spdif_as_pcm    );
    _conf->get_bool ("spdif_encode"     ,spdif_encode    );
    _conf->get_bool ("spdif_stereo_pt"  ,spdif_stereo_pt );

    _conf->get_bool ("spdif_check_sr"   ,spdif_check_sr  );
    _conf->get_bool ("spdif_allow_48"   ,spdif_allow_48  );
    _conf->get_bool ("spdif_allow_44"   ,spdif_allow_44  );
    _conf->get_bool ("spdif_allow_32"   ,spdif_allow_32  );

    dvd.set_query_sink(query_sink);

    dvd.set_spdif_pt(spdif_pt);
    dvd.set_spdif_stereo_pt(spdif_stereo_pt);
    dvd.set_spdif_encode(spdif_encode);
    dvd.set_spdif_as_pcm(spdif_as_pcm);

    dvd.set_spdif_check_sr(spdif_check_sr);
    dvd.set_spdif_allow_48(spdif_allow_48);
    dvd.set_spdif_allow_44(spdif_allow_44);
    dvd.set_spdif_allow_32(spdif_allow_32);

    dvd.set_spdif_check_sr(spdif_check_sr);
    dvd.set_spdif_allow_48(spdif_allow_48);
    dvd.set_spdif_allow_44(spdif_allow_44);
    dvd.set_spdif_allow_32(spdif_allow_32);                      
  }


  return S_OK;
}

STDMETHODIMP COMDecoder::save_params(Config *_conf, int _what)
{
  AudioProcessorState state;
  Speakers user_spk;
  bool use_spdif;

  get_state(&state);
  user_spk = dvd.get_user();
  use_spdif = dvd.get_use_spdif();

  RegistryKey reg;
  if (!_conf)
  {
    _conf = &reg;
    reg.create_key(REG_KEY_PRESET"\\Default");
  }

  if (_what & AC3FILTER_SPK)
  {
    _conf->set_int32("format"           ,user_spk.format  );
    _conf->set_int32("mask"             ,user_spk.mask    );
    _conf->set_int32("relation"         ,user_spk.relation);
    _conf->set_bool ("use_spdif"        ,use_spdif        );
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
    _conf->set_float("attack"           ,state.attack          );
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
    vtime_t time_shift  = dvd.syncer.get_time_shift();
    vtime_t time_factor = dvd.syncer.get_time_factor();
    bool    dejitter    = dvd.syncer.get_dejitter();
    vtime_t threshold   = dvd.syncer.get_threshold();

    _conf->set_float("time_shift"       ,time_shift      );
    _conf->set_float("time_factor"      ,time_factor     );
    _conf->set_bool ("dejitter"         ,dejitter        );
    _conf->set_float("threshold"        ,threshold       );
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
    bool query_sink = dvd.get_query_sink();

    int  spdif_pt = dvd.get_spdif_pt();
    bool spdif_as_pcm = dvd.get_spdif_as_pcm();
    bool spdif_encode = dvd.get_spdif_encode();
    bool spdif_stereo_pt = dvd.get_spdif_stereo_pt();

    bool spdif_check_sr = dvd.get_spdif_check_sr();
    bool spdif_allow_48 = dvd.get_spdif_allow_48();
    bool spdif_allow_44 = dvd.get_spdif_allow_44();
    bool spdif_allow_32 = dvd.get_spdif_allow_32();

    _conf->set_int32("formats"          ,formats         );
    _conf->set_bool ("query_sink"       ,query_sink      );

    _conf->set_int32("spdif_pt"         ,spdif_pt        );
    _conf->set_bool ("spdif_as_pcm"     ,spdif_as_pcm    );
    _conf->set_bool ("spdif_encode"     ,spdif_encode    );
    _conf->set_bool ("spdif_stereo_pt"  ,spdif_stereo_pt );

    _conf->set_bool ("spdif_check_sr"   ,spdif_check_sr  );
    _conf->set_bool ("spdif_allow_48"   ,spdif_allow_48  );
    _conf->set_bool ("spdif_allow_44"   ,spdif_allow_44  );
    _conf->set_bool ("spdif_allow_32"   ,spdif_allow_32  );
  }

  return S_OK;
}
