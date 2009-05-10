#include <stdio.h>
#include "filters/proc_state.h"
#include "com_dec.h"

#define EQ_TYPE_SINGLE 1
#define EQ_TYPE_MULTICHANNEL 2
#define EQ_TYPE_ALL 3

static const char *ch_names[NCHANNELS] = 
{ "L", "C", "R", "SL", "SR", "LFE" };

COMDecoder::COMDecoder(IUnknown *_outer, int _nsamples): dvd(_nsamples)
{ 
  outer = _outer; 
  cur_ch = CH_NONE; // Master is the default equalizer channel
  formats = FORMAT_CLASS_PCM | FORMAT_MASK_AC3 | FORMAT_MASK_MPA | FORMAT_MASK_DTS | FORMAT_MASK_PES |  FORMAT_MASK_SPDIF;
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

// Use detector
STDMETHODIMP COMDecoder::get_use_detector(bool *_use_detector)
{
  if (_use_detector) *_use_detector = dvd.get_use_detector();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_use_detector(bool  _use_detector)
{
  dvd.set_use_detector(_use_detector);
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

// SPDIF bitrate
STDMETHODIMP COMDecoder::get_spdif_bitrate(int *_spdif_bitrate)
{
  if (_spdif_bitrate) *_spdif_bitrate = dvd.get_spdif_bitrate();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_bitrate(int  _spdif_bitrate)
{
  dvd.set_spdif_bitrate(_spdif_bitrate);
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

// SPDIF/DTS output mode
STDMETHODIMP COMDecoder::get_dts_mode(int *_dts_mode)
{
  if (_dts_mode) *_dts_mode = dvd.get_dts_mode();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_dts_mode(int  _dts_mode)
{
  dvd.set_dts_mode(_dts_mode);
  return S_OK;
}

// SPDIF/DTS conversion
STDMETHODIMP COMDecoder::get_dts_conv(int *_dts_conv)
{
  if (_dts_conv) *_dts_conv = dvd.get_dts_conv();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_dts_conv(int  _dts_conv)
{
  dvd.set_dts_conv(_dts_conv);
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
  *_errors = dvd.dec.get_errors() + dvd.spdifer_pt.get_errors() + dvd.spdifer_enc.get_errors();
  return S_OK;
}

STDMETHODIMP COMDecoder::get_info(char *_info, size_t _len)
{
  dvd.get_info(_info, _len);
  cr2crlf(_info, _len);
  return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// IAudioProcessor

// Formats
STDMETHODIMP COMDecoder::get_proc_in_spk(Speakers *spk)
{
  if (spk) *spk = dvd.proc.get_input();
  return S_OK;
}

STDMETHODIMP COMDecoder::get_proc_out_spk(Speakers *spk)
{
  if (spk) *spk = dvd.proc.get_output();
  return S_OK;
}

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
  if (_matrix) dvd.proc.get_matrix(*_matrix);
  return S_OK;
}
STDMETHODIMP COMDecoder::set_matrix(matrix_t *_matrix)
{
  AutoLock config_lock(&config);
  dvd.proc.set_matrix(*_matrix);
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

// Equalizer
STDMETHODIMP COMDecoder::get_eq(bool *_eq)
{
  if (_eq) *_eq = dvd.proc.get_eq();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_eq(bool _eq)
{
  AutoLock config_lock(&config);
  dvd.proc.set_eq(_eq);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_eq_nbands(int ch_name, size_t *nbands)
{
  if (nbands) *nbands = dvd.proc.get_eq_nbands(ch_name);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_eq_bands(int ch_name, EqBand *bands, size_t first_band, size_t nbands)
{
  dvd.proc.get_eq_bands(ch_name, bands, first_band, nbands);
  return S_OK;
}
STDMETHODIMP COMDecoder::set_eq_bands(int ch_name, EqBand *bands, size_t nbands)
{
  AutoLock config_lock(&config);
  dvd.proc.set_eq_bands(ch_name, bands, nbands);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_eq_channel(int *_ch)
{
  if (_ch) *_ch = cur_ch;
  return S_OK;
}
STDMETHODIMP COMDecoder::set_eq_channel(int _ch)
{
  if (_ch == CH_NONE || _ch >= 0 && _ch < NCHANNELS)
    cur_ch = _ch;
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

// Cache
STDMETHODIMP COMDecoder::get_input_cache_size(vtime_t *size)
{
  if (size) *size = dvd.proc.get_input_cache_size();
  return S_OK;
}

STDMETHODIMP COMDecoder::set_input_cache_size(vtime_t size)
{
  AutoLock config_lock(&config);
  dvd.proc.set_input_cache_size(size);
  return S_OK;
}

STDMETHODIMP COMDecoder::get_output_cache_size(vtime_t *size)
{
  if (size) *size = dvd.proc.get_output_cache_size();
  return S_OK;
}

STDMETHODIMP COMDecoder::set_output_cache_size(vtime_t size)
{
  AutoLock config_lock(&config);
  dvd.proc.set_output_cache_size(size);
  return S_OK;
}

STDMETHODIMP COMDecoder::get_input_cache_time(vtime_t *time)
{
  if (time) *time = dvd.proc.get_input_time();
  return S_OK;
}

STDMETHODIMP COMDecoder::get_output_cache_time(vtime_t *time)
{
  if (time) *time = dvd.proc.get_output_time();
  return S_OK;
}

STDMETHODIMP COMDecoder::get_input_cache(vtime_t time, samples_t buf, size_t size, size_t *out_size)
{
  AutoLock config_lock(&config);
  size_t result = dvd.proc.get_input_cache(time, buf, size);
  if (out_size) *out_size = result;
  return S_OK;
}

STDMETHODIMP COMDecoder::get_output_cache(vtime_t time, samples_t buf, size_t size, size_t *out_size)
{
  AutoLock config_lock(&config);
  size_t result = dvd.proc.get_output_cache(time, buf, size);
  if (out_size) *out_size = result;
  return S_OK;
}



// Load/save settings

void COMDecoder::save_spk(Config *conf)
{
  assert(conf);
  Speakers user_spk = dvd.get_user();
  bool use_spdif = dvd.get_use_spdif();

  conf->set_int32("format",      user_spk.format);
  conf->set_int32("mask",        user_spk.mask);
  conf->set_int32("sample_rate", user_spk.sample_rate);
  conf->set_int32("relation",    user_spk.relation);
  conf->set_bool ("use_spdif",   use_spdif);
}

void COMDecoder::load_spk(Config *conf)
{
  assert(conf);
  Speakers user_spk = dvd.get_user();
  bool use_spdif = dvd.get_use_spdif();

  conf->get_int32("format",      user_spk.format);
  conf->get_int32("mask",        user_spk.mask);
  conf->get_int32("sample_rate", user_spk.sample_rate);
  conf->get_int32("relation",    user_spk.relation);
  conf->get_bool ("use_spdif",   use_spdif);

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


void COMDecoder::save_eq(Config *conf, int preset)
{
  assert(conf);
  char param_str[32];
  EqBand band;

  int eq_type = EQ_TYPE_SINGLE;
  switch (preset & AC3FILTER_EQ_MASK)
  {
    case AC3FILTER_EQ_MCH: eq_type = EQ_TYPE_MULTICHANNEL; break;
    case AC3FILTER_EQ_ALL: eq_type = EQ_TYPE_ALL; break;
  }
  conf->set_int32("eq_type", eq_type);

  if (eq_type == EQ_TYPE_SINGLE || eq_type == EQ_TYPE_ALL)
  {
    int eq_channel = cur_ch;
    if (eq_type == EQ_TYPE_ALL)
      eq_channel = CH_NONE;

    size_t nbands = dvd.proc.get_eq_nbands(eq_channel);
    conf->set_int32("eq_nbands", (int)nbands);
    for (size_t i = 0; i < nbands; i++)
    {
      dvd.proc.get_eq_bands(eq_channel, &band, i, 1);
      sprintf(param_str, "eq_freq_%i", i);
      conf->set_int32(param_str, band.freq);
      sprintf(param_str, "eq_gain_%i", i);
      conf->set_float(param_str, band.gain);
    }
  }

  if (eq_type == EQ_TYPE_MULTICHANNEL || eq_type == EQ_TYPE_ALL)
    for (int ch = 0; ch < NCHANNELS; ch++)
    {
      size_t nbands = dvd.proc.get_eq_nbands(ch);
      sprintf(param_str, "eq_%s_nbands", ch_names[ch]);
      conf->set_int32(param_str, (int)nbands);

      for (size_t i = 0; i < nbands; i++)
      {
        dvd.proc.get_eq_bands(ch, &band, i, 1);
        sprintf(param_str, "eq_%s_freq_%i", ch_names[ch], i);
        conf->set_int32(param_str, band.freq);
        sprintf(param_str, "eq_%s_gain_%i", ch_names[ch], i);
        conf->set_float(param_str, band.gain);
      }
    }
}

void COMDecoder::load_eq(Config *conf)
{
  assert(conf);
  AutoBuf<EqBand> bands;
  char param_str[32];

  int eq_type = 1;
  conf->get_int32("eq_type", eq_type);

  if (eq_type == EQ_TYPE_SINGLE || eq_type == EQ_TYPE_ALL)
  {
    int32_t nbands = 10;
    conf->get_int32("eq_nbands", nbands);

    bands.allocate(nbands);
    if (!bands.is_allocated())
      nbands = 0;

    for (int32_t i = 0; i < nbands; i++)
    {
      bands[i].freq = 0;
      bands[i].gain = 0;
      sprintf(param_str, "eq_freq_%i", i);
      conf->get_int32(param_str, bands[i].freq);
      sprintf(param_str, "eq_gain_%i", i);
      conf->get_float(param_str, bands[i].gain);
    }

    if (eq_type == EQ_TYPE_ALL)
      dvd.proc.set_eq_bands(CH_NONE, bands, nbands);
    else
      dvd.proc.set_eq_bands(cur_ch, bands, nbands);
  }

  if (eq_type == EQ_TYPE_MULTICHANNEL || eq_type == EQ_TYPE_ALL)
    for (int ch = 0; ch < NCHANNELS; ch++)
    {
      int32_t nbands = 0;
      sprintf(param_str, "eq_%s_nbands", ch_names[ch]);
      conf->get_int32(param_str, nbands);

      bands.allocate(nbands);
      if (!bands.is_allocated())
        nbands = 0;

      for (int32_t i = 0; i < nbands; i++)
      {
        bands[i].freq = 0;
        bands[i].gain = 0;
        sprintf(param_str, "eq_%s_freq_%i", ch_names[ch], i);
        conf->get_int32(param_str, bands[i].freq);
        sprintf(param_str, "eq_%s_gain_%i", ch_names[ch], i);
        conf->get_float(param_str, bands[i].gain);
      }
      dvd.proc.set_eq_bands(ch, bands, nbands);
    }
}

void COMDecoder::save_sync(Config *conf)
{
  vtime_t time_shift  = dvd.syncer.get_time_shift();
  vtime_t time_factor = dvd.syncer.get_time_factor();
  bool    dejitter    = dvd.syncer.get_dejitter();
  vtime_t threshold   = dvd.syncer.get_threshold();

  conf->set_float("time_shift",  time_shift);
  conf->set_float("time_factor", time_factor);
  conf->set_bool ("dejitter",    dejitter);
  conf->set_float("threshold",   threshold);
}

void COMDecoder::load_sync(Config *conf)
{
  vtime_t time_shift  = dvd.syncer.get_time_shift();
  vtime_t time_factor = dvd.syncer.get_time_factor();
  bool    dejitter    = dvd.syncer.get_dejitter();
  vtime_t threshold   = dvd.syncer.get_threshold();

  conf->get_float("time_shift",  time_shift);
  conf->get_float("time_factor", time_factor);
  conf->get_bool ("dejitter",    dejitter);
  conf->get_float("threshold",   threshold);

  dvd.syncer.set_time_shift(time_shift);
  dvd.syncer.set_time_factor(time_factor);
  dvd.syncer.set_dejitter(dejitter);
  dvd.syncer.set_threshold(threshold);
}

void COMDecoder::save_sys(Config *conf)
{
  bool query_sink = dvd.get_query_sink();
  bool use_detector = dvd.get_use_detector();

  int  spdif_pt = dvd.get_spdif_pt();
  bool spdif_as_pcm = dvd.get_spdif_as_pcm();
  bool spdif_encode = dvd.get_spdif_encode();
  bool spdif_stereo_pt = dvd.get_spdif_stereo_pt();
  int  spdif_bitrate = dvd.get_spdif_bitrate();

  bool spdif_check_sr = dvd.get_spdif_check_sr();
  bool spdif_allow_48 = dvd.get_spdif_allow_48();
  bool spdif_allow_44 = dvd.get_spdif_allow_44();
  bool spdif_allow_32 = dvd.get_spdif_allow_32();

  int  dts_mode = dvd.get_dts_mode();
  int  dts_conv = dvd.get_dts_conv();

  conf->set_int32("formats"          ,formats         );
  conf->set_bool ("query_sink"       ,query_sink      );
  conf->set_bool ("use_detector"     ,use_detector    );

  conf->set_int32("spdif_pt"         ,spdif_pt        );
  conf->set_bool ("spdif_as_pcm"     ,spdif_as_pcm    );
  conf->set_bool ("spdif_encode"     ,spdif_encode    );
  conf->set_bool ("spdif_stereo_pt"  ,spdif_stereo_pt );
  conf->set_int32("spdif_bitrate"    ,spdif_bitrate   );

  conf->set_bool ("spdif_check_sr"   ,spdif_check_sr  );
  conf->set_bool ("spdif_allow_48"   ,spdif_allow_48  );
  conf->set_bool ("spdif_allow_44"   ,spdif_allow_44  );
  conf->set_bool ("spdif_allow_32"   ,spdif_allow_32  );

  conf->set_int32("dts_mode"         ,dts_mode        );
  conf->set_int32("dts_conv"         ,dts_conv        );
}

void COMDecoder::load_sys(Config *conf)
{
  bool query_sink = dvd.get_query_sink();
  bool use_detector = dvd.get_use_detector();

  int  spdif_pt = dvd.get_spdif_pt();
  bool spdif_as_pcm = dvd.get_spdif_as_pcm();
  bool spdif_encode = dvd.get_spdif_encode();
  bool spdif_stereo_pt = dvd.get_spdif_stereo_pt();
  int  spdif_bitrate = dvd.get_spdif_bitrate();

  bool spdif_check_sr = dvd.get_spdif_check_sr();
  bool spdif_allow_48 = dvd.get_spdif_allow_48();
  bool spdif_allow_44 = dvd.get_spdif_allow_44();
  bool spdif_allow_32 = dvd.get_spdif_allow_32();

  int  dts_mode = dvd.get_dts_mode();
  int  dts_conv = dvd.get_dts_conv();

  conf->get_int32("formats"          ,formats         );
  conf->get_bool ("query_sink"       ,query_sink      );
  conf->get_bool ("use_detector"     ,use_detector    );

  conf->get_int32("spdif_pt"         ,spdif_pt        );
  conf->get_bool ("spdif_as_pcm"     ,spdif_as_pcm    );
  conf->get_bool ("spdif_encode"     ,spdif_encode    );
  conf->get_bool ("spdif_stereo_pt"  ,spdif_stereo_pt );
  conf->get_int32("spdif_bitrate"    ,spdif_bitrate   );

  conf->get_bool ("spdif_check_sr"   ,spdif_check_sr  );
  conf->get_bool ("spdif_allow_48"   ,spdif_allow_48  );
  conf->get_bool ("spdif_allow_44"   ,spdif_allow_44  );
  conf->get_bool ("spdif_allow_32"   ,spdif_allow_32  );

  conf->get_int32("dts_mode"         ,dts_mode        );
  conf->get_int32("dts_conv"         ,dts_conv        );

  dvd.set_query_sink(query_sink);
  dvd.set_use_detector(use_detector);

  dvd.set_spdif_pt(spdif_pt);
  dvd.set_spdif_as_pcm(spdif_as_pcm);
  dvd.set_spdif_encode(spdif_encode);
  dvd.set_spdif_stereo_pt(spdif_stereo_pt);
  dvd.set_spdif_bitrate(spdif_bitrate);

  dvd.set_spdif_check_sr(spdif_check_sr);
  dvd.set_spdif_allow_48(spdif_allow_48);
  dvd.set_spdif_allow_44(spdif_allow_44);
  dvd.set_spdif_allow_32(spdif_allow_32);

  dvd.set_dts_mode(dts_mode);
  dvd.set_dts_conv(dts_conv);
}

STDMETHODIMP COMDecoder::load_params(Config *_conf, int _preset)
{
  AutoLock config_lock(&config);
  AudioProcessorState *state = dvd.proc.get_state(0);

  RegistryKey reg;
  if (!_conf)
  {
    _conf = &reg;
    reg.open_key(REG_KEY_PRESET"\\Default");
  }

  if (_preset & AC3FILTER_SPK)
    load_spk(_conf);

  if (state && (_preset & AC3FILTER_PROC))
  {
    // Options
    _conf->get_bool ("auto_gain"        ,state->auto_gain       );
    _conf->get_bool ("normalize"        ,state->normalize       );
    _conf->get_bool ("normalize_matrix" ,state->normalize_matrix);
    _conf->get_bool ("auto_matrix"      ,state->auto_matrix     );
    _conf->get_bool ("expand_stereo"    ,state->expand_stereo   );
    _conf->get_bool ("voice_control"    ,state->voice_control   );
    _conf->get_float("attack"           ,state->attack          );
    _conf->get_float("release"          ,state->release         );
    // Gains
    _conf->get_float("master"           ,state->master          );
    _conf->get_float("clev"             ,state->clev            );
    _conf->get_float("slev"             ,state->slev            );
    _conf->get_float("lfelev"           ,state->lfelev          );
    // DRC
    _conf->get_bool ("drc"              ,state->drc             );
    _conf->get_float("drc_power"        ,state->drc_power       );
    // Bass redirection
    _conf->get_bool ("bass_redir"       ,state->bass_redir      );
    _conf->get_int32("bass_freq"        ,state->bass_freq       );
  }

  if (state && (_preset & AC3FILTER_GAINS))
  {
    // I/O Gains
    _conf->get_float("gain_input_L"     ,state->input_gains[CH_L]   );
    _conf->get_float("gain_input_C"     ,state->input_gains[CH_C]   );
    _conf->get_float("gain_input_R"     ,state->input_gains[CH_R]   );
    _conf->get_float("gain_input_SL"    ,state->input_gains[CH_SL]  );
    _conf->get_float("gain_input_SR"    ,state->input_gains[CH_SR]  );
    _conf->get_float("gain_input_LFE"   ,state->input_gains[CH_LFE] );

    _conf->get_float("gain_output_L"    ,state->output_gains[CH_L]  );
    _conf->get_float("gain_output_C"    ,state->output_gains[CH_C]  );
    _conf->get_float("gain_output_R"    ,state->output_gains[CH_R]  );
    _conf->get_float("gain_output_SL"   ,state->output_gains[CH_SL] );
    _conf->get_float("gain_output_SR"   ,state->output_gains[CH_SR] );
    _conf->get_float("gain_output_LFE"  ,state->output_gains[CH_LFE]);
  }

  if (state && (_preset & AC3FILTER_DELAY))
  {
    // Delays
    _conf->get_bool ("delay"            ,state->delay           );
    _conf->get_int32("delay_units"      ,state->delay_units     );
    _conf->get_float("delay_L"          ,state->delays[CH_L]    );
    _conf->get_float("delay_C"          ,state->delays[CH_C]    );
    _conf->get_float("delay_R"          ,state->delays[CH_R]    );
    _conf->get_float("delay_SL"         ,state->delays[CH_SL]   );
    _conf->get_float("delay_SR"         ,state->delays[CH_SR]   );
    _conf->get_float("delay_LFE"        ,state->delays[CH_LFE]  );
  }

  if (state && (_preset & AC3FILTER_MATRIX))
  {
    // Matrix
    char element_str[32];
    for (int ch1 = 0; ch1 < NCHANNELS; ch1++)
      for (int ch2 = 0; ch2 < NCHANNELS; ch2++)
      {
        state->matrix[ch1][ch2] = 0;
        sprintf(element_str, "matrix_%s_%s", ch_names[ch1], ch_names[ch2]);
        _conf->get_float(element_str, state->matrix[ch1][ch2]);
      }
  }

  if ((_preset & AC3FILTER_EQ_MASK) && (_preset & AC3FILTER_PROC))
  {
    bool eq = dvd.proc.get_eq();
    _conf->get_bool ("eq", eq);
    dvd.proc.set_eq(eq);
  }

  if (_preset & AC3FILTER_EQ_MASK)
  {
    load_eq(_conf);
    state->eq = dvd.proc.get_eq();
    state->eq_master_nbands = 0;
    safe_delete(state->eq_master_bands);
    for (int ch = 0; ch < NCHANNELS; ch++)
    {
      state->eq_nbands[ch] = 0;
      safe_delete(state->eq_bands[ch]);
    }
  }

  if (_preset & AC3FILTER_SYNC)
    load_sync(_conf);

  if (_preset & AC3FILTER_SYS)
    load_sys(_conf);

  if (state)
  {
    dvd.proc.set_state(state);
    safe_delete(state);
  }

  return S_OK;
}

STDMETHODIMP COMDecoder::save_params(Config *_conf, int _preset)
{
  AudioProcessorState *state = dvd.proc.get_state(0);

  RegistryKey reg;
  if (!_conf)
  {
    _conf = &reg;
    reg.create_key(REG_KEY_PRESET"\\Default");
  }

  if (_preset & AC3FILTER_SPK)
    save_spk(_conf);

  if (state && (_preset & AC3FILTER_PROC))
  {
    // Options
    _conf->set_bool ("auto_gain"        ,state->auto_gain       );
    _conf->set_bool ("normalize"        ,state->normalize       );
    _conf->set_bool ("normalize_matrix" ,state->normalize_matrix);
    _conf->set_bool ("auto_matrix"      ,state->auto_matrix     );
    _conf->set_bool ("expand_stereo"    ,state->expand_stereo   );
    _conf->set_bool ("voice_control"    ,state->voice_control   );
    _conf->set_float("attack"           ,state->attack          );
    _conf->set_float("release"          ,state->release         );
    // Gains
    _conf->set_float("master"           ,state->master          );
    _conf->set_float("clev"             ,state->clev            );
    _conf->set_float("slev"             ,state->slev            );
    _conf->set_float("lfelev"           ,state->lfelev          );
    // DRC
    _conf->set_bool ("drc"              ,state->drc             );
    _conf->set_float("drc_power"        ,state->drc_power       );
    // Bass redirection
    _conf->set_bool ("bass_redir"       ,state->bass_redir      );
    _conf->set_int32("bass_freq"        ,state->bass_freq       );
  }

  if (state && (_preset & AC3FILTER_GAINS))
  {
    // I/O Gains
    _conf->set_float("gain_input_L"     ,state->input_gains[CH_L]   );
    _conf->set_float("gain_input_C"     ,state->input_gains[CH_C]   );
    _conf->set_float("gain_input_R"     ,state->input_gains[CH_R]   );
    _conf->set_float("gain_input_SL"    ,state->input_gains[CH_SL]  );
    _conf->set_float("gain_input_SR"    ,state->input_gains[CH_SR]  );
    _conf->set_float("gain_input_LFE"   ,state->input_gains[CH_LFE] );

    _conf->set_float("gain_output_L"    ,state->output_gains[CH_L]  );
    _conf->set_float("gain_output_C"    ,state->output_gains[CH_C]  );
    _conf->set_float("gain_output_R"    ,state->output_gains[CH_R]  );
    _conf->set_float("gain_output_SL"   ,state->output_gains[CH_SL] );
    _conf->set_float("gain_output_SR"   ,state->output_gains[CH_SR] );
    _conf->set_float("gain_output_LFE"  ,state->output_gains[CH_LFE]);
  }

  if (state && (_preset & AC3FILTER_DELAY))
  {
    // Delays
    _conf->set_bool ("delay"            ,state->delay           );
    _conf->set_int32("delay_units"      ,state->delay_units     );
    _conf->set_float("delay_L"          ,state->delays[CH_L]    );
    _conf->set_float("delay_C"          ,state->delays[CH_C]    );
    _conf->set_float("delay_R"          ,state->delays[CH_R]    );
    _conf->set_float("delay_SL"         ,state->delays[CH_SL]   );
    _conf->set_float("delay_SR"         ,state->delays[CH_SR]   );
    _conf->set_float("delay_LFE"        ,state->delays[CH_LFE]  );
  }

  if (state && (_preset & AC3FILTER_MATRIX))
  {
    // Matrix
    char element_str[32];
    for (int ch1 = 0; ch1 < NCHANNELS; ch1++)
      for (int ch2 = 0; ch2 < NCHANNELS; ch2++)
      {
        sprintf(element_str, "matrix_%s_%s", ch_names[ch1], ch_names[ch2]);
        _conf->set_float(element_str, state->matrix[ch1][ch2]);
      }
  }

  if ((_preset & AC3FILTER_EQ_MASK) && (_preset & AC3FILTER_PROC))
    _conf->set_bool("eq", dvd.proc.get_eq());

  if (_preset & AC3FILTER_EQ_MASK)
    save_eq(_conf, _preset & AC3FILTER_EQ_MASK);

  if (_preset & AC3FILTER_SYNC)
    save_sync(_conf);

  if (_preset & AC3FILTER_SYS)
    save_sys(_conf);

  safe_delete(state);
  return S_OK;
}
