#include <stdio.h>
#include "filters/proc_state.h"
#include "com_dec.h"

#define EQ_TYPE_SINGLE 1
#define EQ_TYPE_MULTICHANNEL 2
#define EQ_TYPE_ALL 3

static const char *ch_names[CH_NAMES] = 
{ "L", "C", "R", "SL", "SR", "LFE", "CL", "CR", "BL", "BC", "BR" };

static struct
{
  int format_mask;
  const char *reg_key;
} format_list[] =
{
  { FORMAT_CLASS_PCM,  "format_pcm" },
  { FORMAT_CLASS_LPCM, "format_lpcm" },
  { FORMAT_MASK_MPA,   "format_mpa" },
  { FORMAT_MASK_DOLBY, "format_dolby" },
  { FORMAT_MASK_AAC,   "format_aac" },
  { FORMAT_MASK_DTS,   "format_dts" },
  { FORMAT_MASK_VORBIS,"format_vorbis" },
  { FORMAT_MASK_FLAC,  "format_flac" },
  { FORMAT_MASK_TRUEHD,"format_truehd" },
  { FORMAT_MASK_PES,   "format_mpeg_pes" },
  { FORMAT_MASK_SPDIF, "fomrat_spdif" },
};

COMDecoder::COMDecoder(IUnknown *_outer, int _nsamples): dvd(_nsamples)
{ 
  outer = _outer; 
  cur_ch = CH_NONE; // Master is the default equalizer channel
  formats = FORMAT_CLASS_PCM | FORMAT_CLASS_LPCM |
            FORMAT_MASK_MPA | FORMAT_MASK_DTS | FORMAT_MASK_VORBIS |
            FORMAT_MASK_AAC |
            FORMAT_MASK_AC3 | FORMAT_MASK_EAC3 | FORMAT_MASK_DOLBY |
            FORMAT_MASK_FLAC | FORMAT_MASK_MLP | FORMAT_MASK_TRUEHD |
            FORMAT_MASK_PES | FORMAT_MASK_SPDIF;
  dvd.proc.set_input_order(win_order);
  dvd.proc.set_output_order(win_order);
}

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

bool
COMDecoder::can_open(Speakers spk) const
{
  AutoLock config_lock(&config);
  return ((FORMAT_MASK(spk.format) & formats) != 0) && dvd.can_open(spk);
}
bool
COMDecoder::open(Speakers spk)
{
  AutoLock config_lock(&config);
  return dvd.open(spk);
}
void
COMDecoder::close()
{
  AutoLock config_lock(&config);
  dvd.close();
}
void
COMDecoder::reset()
{
  AutoLock config_lock(&config);
  dvd.reset();
}
bool
COMDecoder::process(Chunk &in, Chunk &out)
{
  AutoLock config_lock(&config);
  return dvd.process(in, out);
}
bool
COMDecoder::flush(Chunk &out)
{
  AutoLock config_lock(&config);
  return dvd.flush(out);
}
bool
COMDecoder::new_stream() const
{
  AutoLock config_lock(&config);
  return dvd.new_stream();
}
bool
COMDecoder::is_open() const
{
  AutoLock config_lock(&config);
  return dvd.is_open();
}
Speakers
COMDecoder::get_input() const
{
  AutoLock config_lock(&config);
  return dvd.get_input();
}
Speakers
COMDecoder::get_output() const
{
  AutoLock config_lock(&config);
  return dvd.get_output();
}
string
COMDecoder::info() const
{
  AutoLock config_lock(&config);
  return dvd.info();
}

///////////////////////////////////////////////////////////////////////////////
// IDecoder

// Input/output format
STDMETHODIMP COMDecoder::get_in_spk(Speakers *_spk)
{
  AutoLock config_lock(&config);
  if (_spk) *_spk = dvd.get_input();
  return S_OK;
}
STDMETHODIMP COMDecoder::get_out_spk(Speakers *_spk)
{
  AutoLock config_lock(&config);
  if (_spk) *_spk = dvd.get_output();
  return S_OK;
}

// User format
STDMETHODIMP COMDecoder::get_user_spk(Speakers *_spk)
{
  AutoLock config_lock(&config);
  if (_spk) *_spk = dvd.get_user();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_user_spk(Speakers  _spk)
{
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
  if (_query_sink) *_query_sink = dvd.get_query_sink();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_query_sink(bool  _query_sink)
{
  AutoLock config_lock(&config);
  dvd.set_query_sink(_query_sink);
  return S_OK;
}

// Use detector
STDMETHODIMP COMDecoder::get_use_detector(bool *_use_detector)
{
  AutoLock config_lock(&config);
  if (_use_detector) *_use_detector = dvd.get_use_detector();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_use_detector(bool  _use_detector)
{
  AutoLock config_lock(&config);
  dvd.set_use_detector(_use_detector);
  return S_OK;
}

// Use SPDIF if possible
STDMETHODIMP COMDecoder::get_use_spdif(bool *_use_spdif)
{
  AutoLock config_lock(&config);
  if (_use_spdif) *_use_spdif = dvd.get_use_spdif();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_use_spdif(bool  _use_spdif)
{
  AutoLock config_lock(&config);
  dvd.set_use_spdif(_use_spdif);
  return S_OK;
}

// SPDIF passthrough (formats bitmask)
STDMETHODIMP COMDecoder::get_spdif_pt(int *_spdif_pt)
{
  AutoLock config_lock(&config);
  if (_spdif_pt) *_spdif_pt = dvd.get_spdif_pt();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_pt(int  _spdif_pt)
{
  AutoLock config_lock(&config);
  dvd.set_spdif_pt(_spdif_pt);
  return S_OK;
}

// SPDIF as PCM output
STDMETHODIMP COMDecoder::get_spdif_as_pcm(bool *_spdif_as_pcm)
{
  AutoLock config_lock(&config);
  if (_spdif_as_pcm) *_spdif_as_pcm = dvd.get_spdif_as_pcm();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_as_pcm(bool  _spdif_as_pcm)
{
  AutoLock config_lock(&config);
  dvd.set_spdif_as_pcm(_spdif_as_pcm);
  return S_OK;
}

// SPDIF encode
STDMETHODIMP COMDecoder::get_spdif_encode(bool *_spdif_encode)
{
  AutoLock config_lock(&config);
  if (_spdif_encode) *_spdif_encode = dvd.get_spdif_encode();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_encode(bool  _spdif_encode)
{
  AutoLock config_lock(&config);
  dvd.set_spdif_encode(_spdif_encode);
  return S_OK;
}

// SPDIF stereo PCM passthrough
STDMETHODIMP COMDecoder::get_spdif_stereo_pt(bool *_spdif_stereo_pt)
{
  AutoLock config_lock(&config);
  if (_spdif_stereo_pt) *_spdif_stereo_pt = dvd.get_spdif_stereo_pt();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_stereo_pt(bool  _spdif_stereo_pt)
{
  AutoLock config_lock(&config);
  dvd.set_spdif_stereo_pt(_spdif_stereo_pt);
  return S_OK;
}

// SPDIF bitrate
STDMETHODIMP COMDecoder::get_spdif_bitrate(int *_spdif_bitrate)
{
  AutoLock config_lock(&config);
  if (_spdif_bitrate) *_spdif_bitrate = dvd.get_spdif_bitrate();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_bitrate(int  _spdif_bitrate)
{
  AutoLock config_lock(&config);
  dvd.set_spdif_bitrate(_spdif_bitrate);
  return S_OK;
}

// SPDIF check sample rate
STDMETHODIMP COMDecoder::get_spdif_check_sr(bool *_spdif_check_sr)
{
  AutoLock config_lock(&config);
  if (_spdif_check_sr) *_spdif_check_sr = dvd.get_spdif_check_sr();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_check_sr(bool  _spdif_check_sr)
{
  AutoLock config_lock(&config);
  dvd.set_spdif_check_sr(_spdif_check_sr);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_spdif_allow_48(bool *_spdif_allow_48)
{
  AutoLock config_lock(&config);
  if (_spdif_allow_48) *_spdif_allow_48 = dvd.get_spdif_allow_48();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_allow_48(bool  _spdif_allow_48)
{
  AutoLock config_lock(&config);
  dvd.set_spdif_allow_48(_spdif_allow_48);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_spdif_allow_44(bool *_spdif_allow_44)
{
  AutoLock config_lock(&config);
  if (_spdif_allow_44) *_spdif_allow_44 = dvd.get_spdif_allow_44();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_allow_44(bool  _spdif_allow_44)
{
  AutoLock config_lock(&config);
  dvd.set_spdif_allow_44(_spdif_allow_44);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_spdif_allow_32(bool *_spdif_allow_32)
{
  AutoLock config_lock(&config);
  if (_spdif_allow_32) *_spdif_allow_32 = dvd.get_spdif_allow_32();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_spdif_allow_32(bool  _spdif_allow_32)
{
  AutoLock config_lock(&config);
  dvd.set_spdif_allow_32(_spdif_allow_32);
  return S_OK;
}

// SPDIF/DTS output mode
STDMETHODIMP COMDecoder::get_dts_mode(int *_dts_mode)
{
  AutoLock config_lock(&config);
  if (_dts_mode) *_dts_mode = dvd.get_dts_mode();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_dts_mode(int  _dts_mode)
{
  AutoLock config_lock(&config);
  dvd.set_dts_mode(_dts_mode);
  return S_OK;
}

// SPDIF/DTS conversion
STDMETHODIMP COMDecoder::get_dts_conv(int *_dts_conv)
{
  AutoLock config_lock(&config);
  if (_dts_conv) *_dts_conv = dvd.get_dts_conv();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_dts_conv(int  _dts_conv)
{
  AutoLock config_lock(&config);
  dvd.set_dts_conv(_dts_conv);
  return S_OK;
}

// SPDIF status
STDMETHODIMP COMDecoder::get_spdif_status(int *_spdif_status)
{
  AutoLock config_lock(&config);
  if (_spdif_status) *_spdif_status = dvd.get_spdif_status();
  return S_OK;
}

STDMETHODIMP COMDecoder::get_frames(int *_frames, int *_errors)
{
  AutoLock config_lock(&config);
  if (_errors) *_errors = 0;
  if (_frames) *_frames = dvd.dec.get_frames() + dvd.spdifer_pt.get_frames() + dvd.spdifer_enc.get_frames();
//  *_errors = dvd.dec.get_errors() + dvd.spdifer_pt.get_errors() + dvd.spdifer_enc.get_errors();
  return S_OK;
}

STDMETHODIMP COMDecoder::get_info(char *_info, size_t _len)
{
  AutoLock config_lock(&config);
  string info = dvd.info();
  size_t str_len = _len;
  if (info.length() + 1 < _len)
    str_len = info.length() + 1;
  memcpy(_info, info.c_str(), str_len - 1);
  _info[_len - 1] = 0;
  cr2crlf(_info, _len);
  return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// IAudioProcessor

// Formats
STDMETHODIMP COMDecoder::get_proc_in_spk(Speakers *spk)
{
  AutoLock config_lock(&config);
  if (spk) *spk = dvd.proc.get_input();
  return S_OK;
}

STDMETHODIMP COMDecoder::get_proc_out_spk(Speakers *spk)
{
  AutoLock config_lock(&config);
  if (spk) *spk = dvd.proc.get_output();
  return S_OK;
}

// AGC options
STDMETHODIMP COMDecoder::get_auto_gain (bool *_auto_gain)
{
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
  if (_gain) *_gain = dvd.proc.get_gain();
  return S_OK;
}

// Mix levels
STDMETHODIMP COMDecoder::get_clev(sample_t *_clev)
{
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
  if (_input_levels)  dvd.proc.get_input_levels(_time, _input_levels);
  if (_output_levels) dvd.proc.get_output_levels(_time, _output_levels);
  return S_OK;
}

// Matrix
STDMETHODIMP COMDecoder::get_matrix(matrix_t *_matrix)
{
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
  if (_drc_level) *_drc_level = dvd.proc.get_drc_level();
  return S_OK;
}

// Bass redirection
STDMETHODIMP COMDecoder::get_bass_redir(bool *_bass_redir)
{
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
  if (_bass_freq) *_bass_freq = dvd.proc.get_bass_freq();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_bass_freq(int _bass_freq)
{
  AutoLock config_lock(&config);
  dvd.proc.set_bass_freq(_bass_freq);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_bass_channels(int *_bass_channels)
{
  AutoLock config_lock(&config);
  if (_bass_channels) *_bass_channels = dvd.proc.get_bass_channels();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_bass_channels(int _bass_channels)
{
  AutoLock config_lock(&config);
  dvd.proc.set_bass_channels(_bass_channels);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_bass_level(sample_t *_bass_level)
{
  AutoLock config_lock(&config);
  if (_bass_level) *_bass_level = dvd.proc.get_bass_level();
  return S_OK;
}

// SRC
STDMETHODIMP COMDecoder::get_src_quality(double *_src_quality)
{
  AutoLock config_lock(&config);
  if (_src_quality) *_src_quality = dvd.proc.get_src_quality();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_src_quality(double _src_quality)
{
  AutoLock config_lock(&config);
  dvd.proc.set_src_quality(_src_quality);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_src_att(double *_src_att)
{
  AutoLock config_lock(&config);
  if (_src_att) *_src_att = dvd.proc.get_src_att();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_src_att(double _src_att)
{
  AutoLock config_lock(&config);
  dvd.proc.set_src_att(_src_att);
  return S_OK;
}

// Equalizer
STDMETHODIMP COMDecoder::get_eq(bool *_eq)
{
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
  if (nbands) *nbands = dvd.proc.get_eq_nbands(ch_name);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_eq_bands(int ch_name, EqBand *bands, size_t first_band, size_t nbands)
{
  AutoLock config_lock(&config);
  dvd.proc.get_eq_bands(ch_name, bands, first_band, nbands);
  return S_OK;
}
STDMETHODIMP COMDecoder::set_eq_bands(int ch_name, EqBand *bands, size_t nbands)
{
  AutoLock config_lock(&config);
  dvd.proc.set_eq_bands(ch_name, bands, nbands);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_eq_equalized(int ch_name, bool *is_equalized)
{
  AutoLock config_lock(&config);
  if (is_equalized) *is_equalized = dvd.proc.get_eq_equalized(ch_name);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_eq_channel(int *_ch)
{
  AutoLock config_lock(&config);
  if (_ch) *_ch = cur_ch;
  return S_OK;
}
STDMETHODIMP COMDecoder::set_eq_channel(int _ch)
{
  AutoLock config_lock(&config);
  if (_ch == CH_NONE || _ch >= 0 && _ch < CH_NAMES)
    cur_ch = _ch;
  return S_OK;
}


// Delay
STDMETHODIMP COMDecoder::get_delay(bool *_delay)
{
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
  if (_delays) dvd.proc.get_delays(_delays);
  return S_OK;
}
STDMETHODIMP COMDecoder::set_delays(float *_delays)
{
  AutoLock config_lock(&config);
  dvd.proc.set_delays(_delays);
  return S_OK;
}

// Dithering
STDMETHODIMP COMDecoder::get_dithering(int *_dithering)
{
  AutoLock config_lock(&config);
  if (_dithering) *_dithering = dvd.proc.get_dithering();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_dithering(int _dithering)
{
  AutoLock config_lock(&config);
  dvd.proc.set_dithering(_dithering);
  return S_OK;
}

// Syncronization
STDMETHODIMP COMDecoder::get_time_shift(vtime_t *_time_shift)
{
  AutoLock config_lock(&config);
  if (_time_shift) *_time_shift = dvd.dejitter.get_time_shift();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_time_shift(vtime_t _time_shift)
{
  AutoLock config_lock(&config);
  dvd.dejitter.set_time_shift(_time_shift);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_time_factor(vtime_t *_time_factor)
{
  AutoLock config_lock(&config);
  if (_time_factor) *_time_factor = dvd.dejitter.get_time_factor();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_time_factor(vtime_t _time_factor)
{
  AutoLock config_lock(&config);
  dvd.dejitter.set_time_factor(_time_factor);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_dejitter(bool *_dejitter)
{
  AutoLock config_lock(&config);
  if (_dejitter) *_dejitter = dvd.dejitter.get_dejitter();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_dejitter(bool _dejitter)
{
  AutoLock config_lock(&config);
  dvd.dejitter.set_dejitter(_dejitter);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_threshold(vtime_t *_threshold)
{
  AutoLock config_lock(&config);
  if (_threshold) *_threshold = dvd.dejitter.get_threshold();
  return S_OK;
}
STDMETHODIMP COMDecoder::set_threshold(vtime_t _threshold)
{
  AutoLock config_lock(&config);
  dvd.dejitter.set_threshold(_threshold);
  return S_OK;
}
STDMETHODIMP COMDecoder::get_jitter(vtime_t *_input_mean, vtime_t *_input_stddev, vtime_t *_output_mean, vtime_t *_output_stddev)
{
  AutoLock config_lock(&config);
  if (_input_mean)    *_input_mean    = dvd.dejitter.get_input_mean();
  if (_input_stddev)  *_input_stddev  = dvd.dejitter.get_input_stddev();
  if (_output_mean)   *_output_mean   = dvd.dejitter.get_output_mean();
  if (_output_stddev) *_output_stddev = dvd.dejitter.get_output_stddev();
  return S_OK;
}

// Cache
STDMETHODIMP COMDecoder::get_input_cache_size(vtime_t *size)
{
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
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
  AutoLock config_lock(&config);
  if (time) *time = dvd.proc.get_input_cache_time();
  return S_OK;
}

STDMETHODIMP COMDecoder::get_output_cache_time(vtime_t *time)
{
  AutoLock config_lock(&config);
  if (time) *time = dvd.proc.get_output_cache_time();
  return S_OK;
}

STDMETHODIMP COMDecoder::get_input_cache(int ch_name, vtime_t time, sample_t *buf, size_t size, size_t *out_size)
{
  AutoLock config_lock(&config);
  size_t result = dvd.proc.get_input_cache(ch_name, time, buf, size);
  if (out_size) *out_size = result;
  return S_OK;
}

STDMETHODIMP COMDecoder::get_output_cache(int ch_name, vtime_t time, sample_t *buf, size_t size, size_t *out_size)
{
  AutoLock config_lock(&config);
  size_t result = dvd.proc.get_output_cache(ch_name, time, buf, size);
  if (out_size) *out_size = result;
  return S_OK;
}



///////////////////////////////////////////////////////////////////////////////
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

  int format = user_spk.format;
  int mask = user_spk.mask;
  int sample_rate = user_spk.sample_rate;
  int relation = user_spk.relation;

  conf->get_int32("format",      format);
  conf->get_int32("mask",        mask);
  conf->get_int32("sample_rate", sample_rate);
  conf->get_int32("relation",    relation);
  conf->get_bool ("use_spdif",   use_spdif);

  user_spk = Speakers(format, mask, sample_rate, -1, relation);

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
    for (int ch_name = 0; ch_name < CH_NAMES; ch_name++)
    {
      size_t nbands = dvd.proc.get_eq_nbands(ch_name);
      sprintf(param_str, "eq_%s_nbands", ch_names[ch_name]);
      conf->set_int32(param_str, (int)nbands);

      for (size_t i = 0; i < nbands; i++)
      {
        dvd.proc.get_eq_bands(ch_name, &band, i, 1);
        sprintf(param_str, "eq_%s_freq_%i", ch_names[ch_name], i);
        conf->set_int32(param_str, band.freq);
        sprintf(param_str, "eq_%s_gain_%i", ch_names[ch_name], i);
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
    for (int ch_name = 0; ch_name < CH_NAMES; ch_name++)
    {
      int32_t nbands = 0;
      sprintf(param_str, "eq_%s_nbands", ch_names[ch_name]);
      conf->get_int32(param_str, nbands);

      bands.allocate(nbands);
      if (!bands.is_allocated())
        nbands = 0;

      for (int32_t i = 0; i < nbands; i++)
      {
        bands[i].freq = 0;
        bands[i].gain = 0;
        sprintf(param_str, "eq_%s_freq_%i", ch_names[ch_name], i);
        conf->get_int32(param_str, bands[i].freq);
        sprintf(param_str, "eq_%s_gain_%i", ch_names[ch_name], i);
        conf->get_float(param_str, bands[i].gain);
      }
      dvd.proc.set_eq_bands(ch_name, bands, nbands);
    }
}

void COMDecoder::save_sync(Config *conf)
{
  vtime_t time_shift  = dvd.dejitter.get_time_shift();
  vtime_t time_factor = dvd.dejitter.get_time_factor();
  bool    dejitter    = dvd.dejitter.get_dejitter();
  vtime_t threshold   = dvd.dejitter.get_threshold();

  conf->set_float("time_shift",  time_shift);
  conf->set_float("time_factor", time_factor);
  conf->set_bool ("dejitter",    dejitter);
  conf->set_float("threshold",   threshold);
}

void COMDecoder::load_sync(Config *conf)
{
  vtime_t time_shift  = dvd.dejitter.get_time_shift();
  vtime_t time_factor = dvd.dejitter.get_time_factor();
  bool    dejitter    = dvd.dejitter.get_dejitter();
  vtime_t threshold   = dvd.dejitter.get_threshold();

  conf->get_float("time_shift",  time_shift);
  conf->get_float("time_factor", time_factor);
  conf->get_bool ("dejitter",    dejitter);
  conf->get_float("threshold",   threshold);

  dvd.dejitter.set_time_shift(time_shift);
  dvd.dejitter.set_time_factor(time_factor);
  dvd.dejitter.set_dejitter(dejitter);
  dvd.dejitter.set_threshold(threshold);
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

  sample_t attack = dvd.proc.get_attack();
  sample_t release = dvd.proc.get_release();
  int dithering = dvd.proc.get_dithering();

  double src_quality = dvd.proc.get_src_quality();
  double src_att = dvd.proc.get_src_att();

  for (int i = 0; i < array_size(format_list); i++)
    conf->set_bool(format_list[i].reg_key, (formats & format_list[i].format_mask) != 0);
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

  conf->set_float("attack"           ,attack          );
  conf->set_float("release"          ,release         );
  conf->set_int32("dithering"        ,dithering       );

  conf->set_float("src_quality"      ,src_quality     );
  conf->set_float("src_att"          ,src_att         );
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

  sample_t attack = dvd.proc.get_attack();
  sample_t release = dvd.proc.get_release();
  int dithering = dvd.proc.get_dithering();

  double src_quality = dvd.proc.get_src_quality();
  double src_att = dvd.proc.get_src_att();

  for (int i = 0; i < array_size(format_list); i++)
  {
    bool enabled = (formats & format_list[i].format_mask) != 0;
    conf->get_bool(format_list[i].reg_key, enabled);
    if (enabled)
      formats |= format_list[i].format_mask;
    else
      formats &= ~format_list[i].format_mask;
  }
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

  conf->get_float("attack"           ,attack          );
  conf->get_float("release"          ,release         );
  conf->get_int32("dithering"        ,dithering       );

  conf->get_float("src_quality"      ,src_quality     );
  conf->get_float("src_att"          ,src_att         );

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

  dvd.proc.set_attack(attack);
  dvd.proc.set_release(release);
  dvd.proc.set_dithering(dithering);

  dvd.proc.set_src_quality(src_quality);
  dvd.proc.set_src_att(src_att);
}

void COMDecoder::save_proc(Config *conf, AudioProcessorState *state)
{
  // Options
  conf->set_bool ("auto_gain"        ,state->auto_gain       );
  conf->set_bool ("normalize"        ,state->normalize       );
  conf->set_bool ("normalize_matrix" ,state->normalize_matrix);
  conf->set_bool ("auto_matrix"      ,state->auto_matrix     );
  conf->set_bool ("expand_stereo"    ,state->expand_stereo   );
  conf->set_bool ("voice_control"    ,state->voice_control   );
  // Gains
  conf->set_float("master"           ,state->master          );
  conf->set_float("clev"             ,state->clev            );
  conf->set_float("slev"             ,state->slev            );
  conf->set_float("lfelev"           ,state->lfelev          );
  // DRC
  conf->set_bool ("drc"              ,state->drc             );
  conf->set_float("drc_power"        ,state->drc_power       );
  // Bass redirection
  conf->set_bool ("bass_redir"       ,state->bass_redir      );
  conf->set_int32("bass_freq"        ,state->bass_freq       );
  conf->set_int32("bass_channels"    ,state->bass_channels   );
}

void COMDecoder::load_proc(Config *conf, AudioProcessorState *state)
{
  // Options
  conf->get_bool ("auto_gain"        ,state->auto_gain       );
  conf->get_bool ("normalize"        ,state->normalize       );
  conf->get_bool ("normalize_matrix" ,state->normalize_matrix);
  conf->get_bool ("auto_matrix"      ,state->auto_matrix     );
  conf->get_bool ("expand_stereo"    ,state->expand_stereo   );
  conf->get_bool ("voice_control"    ,state->voice_control   );
  // Gains
  conf->get_float("master"           ,state->master          );
  conf->get_float("clev"             ,state->clev            );
  conf->get_float("slev"             ,state->slev            );
  conf->get_float("lfelev"           ,state->lfelev          );
  // DRC
  conf->get_bool ("drc"              ,state->drc             );
  conf->get_float("drc_power"        ,state->drc_power       );
  // Bass redirection
  conf->get_bool ("bass_redir"       ,state->bass_redir      );
  conf->get_int32("bass_freq"        ,state->bass_freq       );
  conf->get_int32("bass_channels"    ,state->bass_channels   );
}

void COMDecoder::save_gains(Config *conf, AudioProcessorState *state)
{
  char str[32];
  for (int ch_name = 0; ch_name < CH_NAMES; ch_name++)
  {
    sprintf(str, "gain_input_%s", ch_names[ch_name]);
    conf->set_float(str, state->input_gains[ch_name]);
  }
  for (int ch_name = 0; ch_name < CH_NAMES; ch_name++)
  {
    sprintf(str, "gain_output_%s", ch_names[ch_name]);
    conf->set_float(str, state->output_gains[ch_name]);
  }
}

void COMDecoder::load_gains(Config *conf, AudioProcessorState *state)
{
  char str[32];
  for (int ch_name = 0; ch_name < CH_NAMES; ch_name++)
  {
    sprintf(str, "gain_input_%s", ch_names[ch_name]);
    conf->get_float(str, state->input_gains[ch_name]);
  }
  for (int ch_name = 0; ch_name < CH_NAMES; ch_name++)
  {
    sprintf(str, "gain_output_%s", ch_names[ch_name]);
    conf->get_float(str, state->output_gains[ch_name]);
  }
}

void COMDecoder::save_delays(Config *conf, AudioProcessorState *state)
{
  char str[32];
  conf->set_int32("delay_units", state->delay_units     );
  for (int ch_name = 0; ch_name < CH_NAMES; ch_name++)
  {
    sprintf(str, "delay_%s", ch_names[ch_name]);
    conf->set_float(str, state->delays[ch_name]);
  }
}

void COMDecoder::load_delays(Config *conf, AudioProcessorState *state)
{
  char str[32];
  conf->get_int32("delay_units", state->delay_units     );
  for (int ch_name = 0; ch_name < CH_NAMES; ch_name++)
  {
    sprintf(str, "delay_%s", ch_names[ch_name]);
    conf->get_float(str, state->delays[ch_name]);
  }
}

void COMDecoder::save_matrix(Config *conf, AudioProcessorState *state)
{
  char element_str[32];
  for (int ch1 = 0; ch1 < CH_NAMES; ch1++)
    for (int ch2 = 0; ch2 < CH_NAMES; ch2++)
    {
      sprintf(element_str, "matrix_%s_%s", ch_names[ch1], ch_names[ch2]);
      conf->set_float(element_str, state->matrix[ch1][ch2]);
    }
}

void COMDecoder::load_matrix(Config *conf, AudioProcessorState *state)
{
  char element_str[32];
  for (int ch1 = 0; ch1 < CH_NAMES; ch1++)
    for (int ch2 = 0; ch2 < CH_NAMES; ch2++)
    {
      state->matrix[ch1][ch2] = 0;
      sprintf(element_str, "matrix_%s_%s", ch_names[ch1], ch_names[ch2]);
      conf->get_float(element_str, state->matrix[ch1][ch2]);
    }
}

STDMETHODIMP COMDecoder::load_params(Config *_conf, int _preset)
{
  AutoLock config_lock(&config);

  RegistryKey reg;
  if (!_conf)
  {
    _conf = &reg;
    reg.open_key(REG_KEY_PRESET"\\Default");
  }

  if (_preset & AC3FILTER_SPK)
    load_spk(_conf);

  if (_preset & AC3FILTER_SYNC)
    load_sync(_conf);

  if (_preset & AC3FILTER_SYS)
    load_sys(_conf);

  if (_preset & AC3FILTER_EQ_MASK)
    load_eq(_conf);

  AudioProcessorState *state = dvd.proc.get_state(0);
  if (state)
  {
    if (_preset & AC3FILTER_PROC)
      load_proc(_conf, state);

    if (_preset & AC3FILTER_GAINS)
      load_gains(_conf, state);

    if (_preset & AC3FILTER_DELAY)
      load_delays(_conf, state);

    if (_preset & AC3FILTER_MATRIX)
      load_matrix(_conf, state);

    // Load equalizer/delay switch ONLY when both
    // equalizer/delay and processor states are loaded

    if ((_preset & AC3FILTER_EQ_MASK) && (_preset & AC3FILTER_PROC))
      _conf->get_bool ("eq"               ,state->eq              );

    if ((_preset & AC3FILTER_DELAY) && (_preset & AC3FILTER_PROC))
      _conf->get_bool ("delay"            ,state->delay           );

    dvd.proc.set_state(state);
    safe_delete(state);
  }

  return S_OK;
}

STDMETHODIMP COMDecoder::save_params(Config *_conf, int _preset)
{
  RegistryKey reg;
  if (!_conf)
  {
    _conf = &reg;
    reg.create_key(REG_KEY_PRESET"\\Default");
  }

  if (_preset & AC3FILTER_SPK)
    save_spk(_conf);

  if (_preset & AC3FILTER_SYNC)
    save_sync(_conf);

  if (_preset & AC3FILTER_SYS)
    save_sys(_conf);

  if (_preset & AC3FILTER_EQ_MASK)
    save_eq(_conf, _preset & AC3FILTER_EQ_MASK);

  AudioProcessorState *state = dvd.proc.get_state(0);
  if (state)
  {
    if (_preset & AC3FILTER_PROC)
      save_proc(_conf, state);

    if (_preset & AC3FILTER_GAINS)
      save_gains(_conf, state);

    if (_preset & AC3FILTER_DELAY)
      save_delays(_conf, state);

    if (_preset & AC3FILTER_MATRIX)
      save_matrix(_conf, state);

    // Save equalizer/delay switch ONLY when both
    // equalizer/delay and processor states are saved

    if ((_preset & AC3FILTER_EQ_MASK) && (_preset & AC3FILTER_PROC))
      _conf->set_bool ("eq"               ,state->eq              );

    if ((_preset & AC3FILTER_DELAY) && (_preset & AC3FILTER_PROC))
      _conf->set_bool ("delay"            ,state->delay           );

    safe_delete(state);
  }
  return S_OK;
}
