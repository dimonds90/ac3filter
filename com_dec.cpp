#include "com_dec.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////
// Filter interface

void COMDecoder::reset()
{
  AutoLock config_lock(&config);
  DVDDecoder::reset();
}

bool COMDecoder::query_input(Speakers _spk)
{
  AutoLock config_lock(&config);
  return DVDDecoder::query_input(_spk);
}
bool COMDecoder::set_input(Speakers _spk)
{
  AutoLock config_lock(&config);
  return DVDDecoder::set_input(_spk);
}
bool COMDecoder::process(const Chunk *_chunk)
{
  AutoLock config_lock(&config);
  return DVDDecoder::process(_chunk);
}

Speakers COMDecoder::get_output()
{
  AutoLock config_lock(&config);
  return DVDDecoder::get_output();
}
bool COMDecoder::is_empty()
{
  AutoLock config_lock(&config);
  return DVDDecoder::is_empty();
}
bool COMDecoder::get_chunk(Chunk *_chunk)
{
  AutoLock config_lock(&config);
  return DVDDecoder::get_chunk(_chunk);
}

///////////////////////////////////////////////////////////////////////////////
// IAudioProcessor

// AGC options
STDMETHODIMP COMDecoder::get_auto_gain (bool *_auto_gain)
{
  *_auto_gain = proc.get_auto_gain();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_auto_gain (bool _auto_gain)
{
  AutoLock config_lock(&config);
  proc.set_auto_gain(_auto_gain);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_normalize (bool *_normalize)
{
  *_normalize = proc.get_normalize();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_normalize (bool _normalize)
{
  AutoLock config_lock(&config);
  proc.set_normalize(_normalize);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_release(sample_t *_release)
{
  *_release = proc.get_release();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_release(sample_t _release)
{
  AutoLock config_lock(&config);
  proc.set_release(_release);
  return S_OK;
}

// Matrix options
STDMETHODIMP COMDecoder::get_auto_matrix(bool *_auto_matrix)
{
  *_auto_matrix = proc.get_auto_matrix();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_auto_matrix(bool _auto_matrix)
{
  AutoLock config_lock(&config);
  proc.set_auto_matrix(_auto_matrix);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_normalize_matrix(bool *_normalize_matrix)
{
  *_normalize_matrix = proc.get_normalize_matrix();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_normalize_matrix(bool _normalize_matrix)
{
  AutoLock config_lock(&config);
  proc.set_normalize_matrix(_normalize_matrix);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_voice_control(bool *_voice_control)
{
  *_voice_control = proc.get_voice_control();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_voice_control(bool _voice_control)
{
  AutoLock config_lock(&config);
  proc.set_voice_control(_voice_control);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_expand_stereo(bool *_expand_stereo)
{
  *_expand_stereo = proc.get_expand_stereo();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_expand_stereo(bool _expand_stereo)
{
  AutoLock config_lock(&config);
  proc.set_expand_stereo(_expand_stereo);
  return S_OK;
}

// Master gain
STDMETHODIMP COMDecoder::get_master(sample_t *_master)
{
  *_master = proc.get_master();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_master(sample_t _master)
{
  AutoLock config_lock(&config);
  proc.set_master(_master);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_gain(sample_t *_gain)
{
  *_gain = proc.get_gain();
  return S_OK;
}

// Mix levels
STDMETHODIMP COMDecoder::get_clev(sample_t *_clev)
{
  *_clev = proc.get_clev();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_clev(sample_t _clev)
{
  AutoLock config_lock(&config);
  proc.set_clev(_clev);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_slev(sample_t *_slev)
{
  *_slev = proc.get_slev();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_slev(sample_t _slev)
{
  AutoLock config_lock(&config);
  proc.set_slev(_slev);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_lfelev(sample_t *_lfelev)
{
  *_lfelev = proc.get_lfelev();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_lfelev(sample_t _lfelev)
{
  AutoLock config_lock(&config);
  proc.set_lfelev(_lfelev);
  return S_OK;
}

// Input/output gains
STDMETHODIMP COMDecoder::get_input_gains(sample_t *_input_gains)
{
  proc.get_input_gains(_input_gains);
  return S_OK;
}
STDMETHODIMP COMDecoder::set_input_gains(sample_t *_input_gains)
{
  AutoLock config_lock(&config);
  proc.set_input_gains(_input_gains);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_output_gains(sample_t *_output_gains)
{
  proc.get_output_gains(_output_gains);
  return S_OK;
}
STDMETHODIMP COMDecoder::set_output_gains(sample_t *_output_gains)
{
  AutoLock config_lock(&config);
  proc.set_output_gains(_output_gains);
  return S_OK;
}

// Input/output levels
STDMETHODIMP COMDecoder::get_levels(time_t _time, sample_t *_input_levels, sample_t *_output_levels)
{
  proc.get_input_levels(_time, _input_levels);
  proc.get_output_levels(_time, _output_levels);
  return S_OK;
}

// Matrix
STDMETHODIMP COMDecoder::get_matrix(matrix_t *_matrix)
{
  proc.get_matrix(_matrix);
  return S_OK;
}
STDMETHODIMP COMDecoder::set_matrix(matrix_t *_matrix)
{
  AutoLock config_lock(&config);
  proc.set_matrix(_matrix);
  return S_OK;
}

// DRC
STDMETHODIMP COMDecoder::get_drc(bool *_drc)
{
  *_drc = proc.get_drc();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_drc(bool _drc)
{
  AutoLock config_lock(&config);
  proc.set_drc(_drc);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_drc_power(sample_t *_drc_power)
{
  *_drc_power = proc.get_drc_power();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_drc_power(sample_t _drc_power)
{
  AutoLock config_lock(&config);
  proc.set_drc_power(_drc_power);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_drc_level(sample_t *_drc_level)
{
  *_drc_level = proc.get_drc_level();
  return S_OK;
}

// Bass redirection
STDMETHODIMP COMDecoder::get_bass_redir(bool *_bass_redir)
{
  *_bass_redir = proc.get_bass_redir();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_bass_redir(bool _bass_redir)
{
  AutoLock config_lock(&config);
  proc.set_bass_redir(_bass_redir);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_bass_freq(int *_bass_freq)
{
  *_bass_freq = proc.get_bass_freq();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_bass_freq(int bass_freq)
{
  AutoLock config_lock(&config);
  proc.set_bass_freq(bass_freq);
  return S_OK;
}

// Delay
STDMETHODIMP COMDecoder::get_delay(bool *_delay)
{
  *_delay = proc.get_delay();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_delay(bool _delay)
{
  AutoLock config_lock(&config);
  proc.set_delay(_delay);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_delay_units(int *_delay_units)
{
  *_delay_units = proc.get_delay_units();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_delay_units(int _delay_units)
{
  AutoLock config_lock(&config);
  proc.set_delay_units(_delay_units);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_delays(float *_delays)
{
  proc.get_delays(_delays);
  return S_OK;
}
STDMETHODIMP COMDecoder::set_delays(float *_delays)
{
  AutoLock config_lock(&config);
  proc.set_delays(_delays);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_delay_ms(float *_delay_ms)
{
  *_delay_ms = proc.get_delay_ms();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_delay_ms(float  _delay_ms)
{
  AutoLock config_lock(&config);
  proc.set_delay_ms(_delay_ms);
  return S_OK;
}



STDMETHODIMP COMDecoder::get_state(AudioProcessorState *_state, time_t _time)
{
  AutoLock config_lock(&config);

  // AGC options
  get_auto_gain(&_state->auto_gain);
  get_normalize(&_state->normalize);
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
  get_delay_ms(&_state->delay_ms);

  return S_OK;
};

STDMETHODIMP COMDecoder::set_state     (AudioProcessorState *_state)
{
  AutoLock config_lock(&config);

  // AGC options
  set_auto_gain(_state->auto_gain);
  set_normalize(_state->normalize);
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
  set_delay_ms(_state->delay_ms);

  return S_OK;
};



///////////////////////////////////////////////////////////////////////////////
// IDecoder

STDMETHODIMP 
COMDecoder::get_spk(Speakers *_spk)
{
  *_spk = dec.get_output();
  return S_OK;
}

STDMETHODIMP 
COMDecoder::get_frames(int *_frames, int *_errors)
{
  *_frames = dec.mpa.get_frames() + dec.ac3.get_frames() + dec.dts.get_frames() + spdifer.get_frames();
  *_errors = dec.mpa.get_errors() + dec.ac3.get_errors() + dec.dts.get_errors();
  return S_OK;
}

STDMETHODIMP 
COMDecoder::get_info(char *_info, int _len)
{
/*
  double hist[MAX_HISTOGRAM];
  char   hist_buf[2048];
  char  *ptr = hist_buf;

  int dbpb;
  int i, j;

  proc.get_output_histogram(MAX_HISTOGRAM, hist);
  dbpb = proc.get_dbpb();

  ptr += sprintf(ptr, "------------------------------------------------------------------------------\r\n");
  for (i = 0; i*dbpb < 100 && i < MAX_HISTOGRAM; i++)
  {
    ptr += sprintf(ptr, "%2idB: %4.1f ", i * dbpb, hist[i] * 100);
    for (j = 0; j < 67 && j < hist[i] * 67; j++)
      *ptr++ = '*';
    ptr += sprintf(ptr, "\r\n");
  }
  ptr += sprintf(ptr, "------------------------------------------------------------------------------\r\n");
  ptr += sprintf(ptr, "dbpb;%i\r\nhistogram;", dbpb);
  for (i = 0; i*dbpb < 100 && i < MAX_HISTOGRAM; i++)
    ptr += sprintf(ptr, "%.4f;", hist[i]);
  ptr += sprintf(ptr, "\r\n");
  ptr += sprintf(ptr, "------------------------------------------------------------------------------\r\n\0");
  strncpy(_info, hist_buf, _len);
  return S_OK;
*/
  // windows controls require '\n' to be replaced with '\r\n'
  DVDDecoder::get_info(_info, _len);

  int len = strlen(_info);
  int cnt = 0;

  for (int i = 0; i < len; i++)
    if (_info[i+1] == '\n')
      cnt++;

  char *src = _info + len - 1;
  char *dst = src + cnt + 1;
  if (dst > _info + _len)
    dst = _info + len;
  *dst-- = 0;

  while (src != dst)
    if (src[0] != '\r' && src[1] == '\n')
    {
      *dst-- = '\r';
      *dst-- = *src--;
    }
    else
      *dst-- = *src--;
  return S_OK;
}

STDMETHODIMP 
COMDecoder::get_locks (bool *_clev_lock, bool *_slev_lock, bool *_lfelev_lock)
{
  return S_OK;
}
STDMETHODIMP 
COMDecoder::set_locks(bool _clev_lock, bool _slev_lock, bool _lfelev_lock)
{
  AutoLock config_lock(&config);
  return S_OK;
}


/////////////////////////////////////////////////////////
// IAC3Decoder

STDMETHODIMP 
COMDecoder::get_eq_on(bool *_eq_on)
{
  *_eq_on = false;
//  *_eq_on = dec.ac3.get_eq_on();
  return S_OK;
}
STDMETHODIMP 
COMDecoder::set_eq_on(bool  _eq_on)
{
//  dec.ac3.set_eq_on(_eq_on);
  return S_OK;
}

STDMETHODIMP 
COMDecoder::get_eq(sample_t *_eq)
{
//  dec.ac3.get_eq(_eq);
  return S_OK;
}
STDMETHODIMP 
COMDecoder::set_eq(sample_t *_eq)
{
//  dec.ac3.set_eq(_eq);
  return S_OK;
}
