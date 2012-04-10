/*
  Class GUIDs
  Interface GUIDs
  Interface definitions
*/

#ifndef AC3FILTER_GUIDS_H
#define AC3FILTER_GUIDS_H

#include <objbase.h>
#include "spk.h"
#include "fir/eq_fir.h"
#include "registry.h"

extern HINSTANCE ac3filter_instance;

void cr2crlf(char *buf, size_t size);

// Application info
#define APP_NAME      "AC3Filter"
#define SUPPORT_EMAIL "support@ac3filter.net"
#define WEB_SITE_URL  "http://ac3filter.net"
#define BUG_TRAP_URL  "http://ac3filter.net/bugtrap.php"
#define FORUM_URL     "http://ac3filter.net/forum"

// Registry keys
#define REG_KEY        "Software\\AC3Filter"
#define REG_KEY_PRESET "Software\\AC3Filter\\preset"
#define REG_KEY_MATRIX "Software\\AC3Filter\\matrix"
#define REG_KEY_EQ     "Software\\AC3Filter\\equalizer"

// preset settings
#define AC3FILTER_SPK     0x0001 // Speakers configuration: 
                                 // spk, use_spdif
#define AC3FILTER_PROC    0x0002 // Audio processor settings:
                                 // auto_gain, normalize, auto_matrix, normalize_matrix, voice_control, expand_stereo,
                                 // drc, drc_power, master, clev, slev, lfelev
#define AC3FILTER_GAINS   0x0004 // Input/output gains: 
                                 // input_gains, output_gains
#define AC3FILTER_MATRIX  0x0008 // Mixing matrix:
                                 // matrix
#define AC3FILTER_DELAY   0x0010 // Delay settings:
                                 // delay, delay_units, delays
#define AC3FILTER_EQ_MASK 0x0060 // Equalizer type mask
#define AC3FILTER_EQ_ALL  0x0060 // Full equalizer settings (master & multichannel)
#define AC3FILTER_EQ_CUR  0x0020 // Current equalizer channel
#define AC3FILTER_EQ_MCH  0x0040 // Multichannel equalizer
#define AC3FILTER_SYNC    0x0080 // Syncronization settings:
                                 // time_shift, time_factor, dejitter, threshold
#define AC3FILTER_SYS     0x0100 // System settings: 
                                 // formats, spdif, config_autoload, 

#define AC3FILTER_ALL     0x01ff // all settings
#define AC3FILTER_PRESET  0x007f // settings that saved into preset (all except system settings and sync)

// Constants from dvd_graph.h
#define SPDIF_MODE_NONE                0
#define SPDIF_MODE_DISABLED            1
#define SPDIF_MODE_PASSTHROUGH         2
#define SPDIF_MODE_ENCODE              3

// Constants from spdif_wrapper.h
#define DTS_MODE_AUTO    0
#define DTS_MODE_WRAPPED 1
#define DTS_MODE_PADDED  2

#define DTS_CONV_NONE    0
#define DTS_CONV_16BIT   1
#define DTS_CONV_14BIT   2

// Notification messages for AC3Filter tray
#define WM_TRAY_ICON (WM_USER + 10)
#define WM_PRESET    (WM_USER + 11)

///////////////////////////////////////////////////////////////////////////////
// Interface GUIDs
///////////////////////////////////////////////////////////////////////////////

// {E4539501-C609-46ea-AD2A-0E9700245683}
DEFINE_GUID(IID_IAC3Filter, 
0xe4539501, 0xc609, 0x46ea, 0xad, 0x2a, 0xe, 0x97, 0x0, 0x24, 0x56, 0x83);

// {64388F26-933E-4b64-97A9-F9DFEDBE2E73}
DEFINE_GUID(IID_IAudioProcessor, 
0x64388f26, 0x933e, 0x4b64, 0x97, 0xa9, 0xf9, 0xdf, 0xed, 0xbe, 0x2e, 0x73);

// {71F2E1AF-9C87-48f7-A2C0-5E2D08327ADA}
DEFINE_GUID(IID_IDecoder, 
0x71f2e1af, 0x9c87, 0x48f7, 0xa2, 0xc0, 0x5e, 0x2d, 0x8, 0x32, 0x7a, 0xda);

// {799ACEE3-8943-4238-89C8-7D3348CDD4B0}
DEFINE_GUID(IID_IAC3Dec, 
0x4c3df4d3, 0x721b, 0x42d0, 0xa3, 0x48, 0x8b, 0xbb, 0xf6, 0xc7, 0xaa, 0x72);

// {D2969EC8-1AA8-4554-8D0C-4190C2874256}
DEFINE_GUID(IID_IMPADec, 
0xd2969ec8, 0x1aa8, 0x4554, 0x8d, 0xc, 0x41, 0x90, 0xc2, 0x87, 0x42, 0x56);

// {C439C9B0-9471-494d-A530-0E69E9082884}
DEFINE_GUID(IID_IDTSDec, 
0xc439c9b0, 0x9471, 0x494d, 0xa5, 0x30, 0xe, 0x69, 0xe9, 0x8, 0x28, 0x84);

// {AFC57835-2FD1-4541-A6D9-0DB71856E589}
DEFINE_GUID(IID_IMatrixMixer, 
0xafc57835, 0x2fd1, 0x4541, 0xa6, 0xd9, 0xd, 0xb7, 0x18, 0x56, 0xe5, 0x89);

///////////////////////////////////////////////////////////////////////////////
// CLSID GUIDs
///////////////////////////////////////////////////////////////////////////////

// {A753A1EC-973E-4718-AF8E-A3F554D45C44}
DEFINE_GUID(CLSID_AC3Filter, 
0xa753a1ec, 0x973e, 0x4718, 0xaf, 0x8e, 0xa3, 0xf5, 0x54, 0xd4, 0x5c, 0x44);

// {FBA5FB05-58C3-45cb-8B0D-C2313EA048CF}
DEFINE_GUID(CLSID_AC3Filter_main, 
0xfba5fb05, 0x58c3, 0x45cb, 0x8b, 0xd, 0xc2, 0x31, 0x3e, 0xa0, 0x48, 0xcf);

// {F0B801B1-A239-473b-B6B4-6AE3DB3ABBD3}
DEFINE_GUID(CLSID_AC3Filter_mixer, 
0xf0b801b1, 0xa239, 0x473b, 0xb6, 0xb4, 0x6a, 0xe3, 0xdb, 0x3a, 0xbb, 0xd3);

// {02AFA80F-4BEE-41fd-8572-214B58A9EF90}
DEFINE_GUID(CLSID_AC3Filter_gains, 
0x2afa80f, 0x4bee, 0x41fd, 0x85, 0x72, 0x21, 0x4b, 0x58, 0xa9, 0xef, 0x90);

// {8643B615-6A76-4060-8A29-C2C6BDF5D70F}
DEFINE_GUID(CLSID_AC3Filter_eq, 
0x8643b615, 0x6a76, 0x4060, 0x8a, 0x29, 0xc2, 0xc6, 0xbd, 0xf5, 0xd7, 0xf);

// {A6A695A2-B1AD-49a2-AD6F-FFB82E2A7832}
DEFINE_GUID(CLSID_AC3Filter_spdif, 
0xa6a695a2, 0xb1ad, 0x49a2, 0xad, 0x6f, 0xff, 0xb8, 0x2e, 0x2a, 0x78, 0x32);

// {363F46BE-27B4-4c8d-99E7-B1E049B84376}
DEFINE_GUID(CLSID_AC3Filter_sys, 
0x363f46be, 0x27b4, 0x4c8d, 0x99, 0xe7, 0xb1, 0xe0, 0x49, 0xb8, 0x43, 0x76);

// {90A9B7D2-3794-45ea-9E23-140E3938D2D9}
DEFINE_GUID(CLSID_AC3Filter_about, 
0x90a9b7d2, 0x3794, 0x45ea, 0x9e, 0x23, 0x14, 0xe, 0x39, 0x38, 0xd2, 0xd9);

///////////////////////////////////////////////////////////////////////////////
// Interfaces
///////////////////////////////////////////////////////////////////////////////

DECLARE_INTERFACE_(IAC3Filter, IUnknown)
{
  // Tray icon
  STDMETHOD (get_tray)(bool *tray) = 0;
  STDMETHOD (set_tray)(bool  tray) = 0;

  // Reinit after seek/pause option
  STDMETHOD (get_reinit)(int *reinit) = 0;
  STDMETHOD (set_reinit)(int  reinit) = 0;

  // Do not allow PCM in SPDIF mode
  STDMETHOD (get_spdif_no_pcm)(bool *spdif_no_pcm) = 0;
  STDMETHOD (set_spdif_no_pcm)(bool  spdif_no_pcm) = 0;

  // Timing
  STDMETHOD (get_playback_time)(vtime_t *time) = 0;

  // CPU usage
  STDMETHOD (get_cpu_usage)(double *cpu_usage) = 0;
};

DECLARE_INTERFACE_(IDecoder, IUnknown)
{
  // Input/output format
  STDMETHOD (get_in_spk)  (Speakers *spk) = 0;
  STDMETHOD (get_out_spk) (Speakers *spk) = 0;

  // User format
  STDMETHOD (get_user_spk) (Speakers *spk) = 0;
  STDMETHOD (set_user_spk) (Speakers  spk) = 0;

  // Input formats to accept (formats bitmask)
  STDMETHOD (get_formats) (int *formats) = 0;
  STDMETHOD (set_formats) (int  formats) = 0;

  // Query sink about output format support
  STDMETHOD (get_query_sink) (bool *query_sink) = 0;
  STDMETHOD (set_query_sink) (bool  query_sink) = 0;

  // Use detector
  STDMETHOD (get_use_detector) (bool *use_detector) = 0;
  STDMETHOD (set_use_detector) (bool  use_detector) = 0;

  // Use SPDIF if possible
  STDMETHOD (get_use_spdif) (bool *use_spdif) = 0;
  STDMETHOD (set_use_spdif) (bool  use_spdif) = 0;

  // SPDIF passthrough (formats bitmask)
  STDMETHOD (get_spdif_pt)(int *spdif_pt) = 0;
  STDMETHOD (set_spdif_pt)(int  spdif_pt) = 0;

  // SPDIF as PCM output
  STDMETHOD (get_spdif_as_pcm)(bool *spdif_as_pcm) = 0;
  STDMETHOD (set_spdif_as_pcm)(bool  spdif_as_pcm) = 0;

  // SPDIF encode
  STDMETHOD (get_spdif_encode)(bool *spdif_encode) = 0;
  STDMETHOD (set_spdif_encode)(bool  spdif_encode) = 0;

  // SPDIF stereo PCM passthrough
  STDMETHOD (get_spdif_stereo_pt)(bool *spdif_stereo_pt) = 0;
  STDMETHOD (set_spdif_stereo_pt)(bool  spdif_stereo_pt) = 0;

  // SPDIF bitrate
  STDMETHOD (get_spdif_bitrate)(int *spdif_bitrate) = 0;
  STDMETHOD (set_spdif_bitrate)(int  spdif_bitrate) = 0;

  // SPDIF check sample rate
  STDMETHOD (get_spdif_check_sr)(bool *spdif_check_sr) = 0;
  STDMETHOD (set_spdif_check_sr)(bool  spdif_check_sr) = 0;
  STDMETHOD (get_spdif_allow_48)(bool *spdif_allow_48) = 0;
  STDMETHOD (set_spdif_allow_48)(bool  spdif_allow_48) = 0;
  STDMETHOD (get_spdif_allow_44)(bool *spdif_allow_44) = 0;
  STDMETHOD (set_spdif_allow_44)(bool  spdif_allow_44) = 0;
  STDMETHOD (get_spdif_allow_32)(bool *spdif_allow_32) = 0;
  STDMETHOD (set_spdif_allow_32)(bool  spdif_allow_32) = 0;

  // SPDIF/DTS output mode
  STDMETHOD (get_dts_mode) (int *dts_mode) = 0;
  STDMETHOD (set_dts_mode) (int  dts_mode) = 0;

  // SPDIF/DTS conversion
  STDMETHOD (get_dts_conv) (int *dts_conv) = 0;
  STDMETHOD (set_dts_conv) (int  dts_conv) = 0;

  // SPDIF status
  STDMETHOD (get_spdif_status)(int *spdif_status) = 0;

  // Linear time transform
  STDMETHOD (get_time_shift)   (vtime_t *time_shift) = 0;
  STDMETHOD (set_time_shift)   (vtime_t  time_shift) = 0;
  STDMETHOD (get_time_factor)  (vtime_t *time_factor) = 0;
  STDMETHOD (set_time_factor)  (vtime_t  time_factor) = 0;

  // Jitter correction
  STDMETHOD (get_dejitter)     (bool *dejitter) = 0;
  STDMETHOD (set_dejitter)     (bool  dejitter) = 0;
  STDMETHOD (get_threshold)    (vtime_t *threshold) = 0;
  STDMETHOD (set_threshold)    (vtime_t  threshold) = 0;
  STDMETHOD (get_jitter)       (vtime_t *input_mean, vtime_t *input_stddev, vtime_t *output_mean, vtime_t *output_stddev) = 0;
                               
  // Stats
  STDMETHOD (get_frames)(int  *frames, int *errors) = 0;
  STDMETHOD (get_info)  (char *info, size_t len) = 0;

  // Load/save settings
  STDMETHOD (load_params) (Config *config, int what) = 0;
  STDMETHOD (save_params) (Config *config, int what) = 0;
};

DECLARE_INTERFACE_(IAudioProcessor, IUnknown)
{
  STDMETHOD (get_proc_in_spk)  (Speakers *spk) = 0;
  STDMETHOD (get_proc_out_spk) (Speakers *spk) = 0;

  // AGC options
  STDMETHOD (get_auto_gain)    (bool *auto_gain) = 0;
  STDMETHOD (set_auto_gain)    (bool  auto_gain) = 0;
  STDMETHOD (get_normalize)    (bool *normalize) = 0;
  STDMETHOD (set_normalize)    (bool  normalize) = 0;
  STDMETHOD (get_attack)       (sample_t *attack) = 0;
  STDMETHOD (set_attack)       (sample_t  attack) = 0;
  STDMETHOD (get_release)      (sample_t *release) = 0;
  STDMETHOD (set_release)      (sample_t  release) = 0;
  // Matrix options
  STDMETHOD (get_auto_matrix)  (bool *auto_matrix) = 0;
  STDMETHOD (set_auto_matrix)  (bool  auto_matrix) = 0;
  STDMETHOD (get_normalize_matrix)(bool *normalize_matrix) = 0;
  STDMETHOD (set_normalize_matrix)(bool  normalize_matrix) = 0;
  STDMETHOD (get_voice_control)(bool *voice_control) = 0;
  STDMETHOD (set_voice_control)(bool  voice_control) = 0;
  STDMETHOD (get_expand_stereo)(bool *expand_stereo) = 0;
  STDMETHOD (set_expand_stereo)(bool  expand_stereo) = 0;
  // Master gain
  STDMETHOD (get_master)       (sample_t *master) = 0;
  STDMETHOD (set_master)       (sample_t  master) = 0;
  STDMETHOD (get_gain)         (sample_t *gain) = 0;
  // Mix levels              
  STDMETHOD (get_clev)         (sample_t *clev) = 0;
  STDMETHOD (set_clev)         (sample_t  clev) = 0;
  STDMETHOD (get_slev)         (sample_t *slev) = 0;
  STDMETHOD (set_slev)         (sample_t  slev) = 0;
  STDMETHOD (get_lfelev)       (sample_t *lfelev) = 0;
  STDMETHOD (set_lfelev)       (sample_t  lfelev) = 0;
  // Input/output gains
  STDMETHOD (get_input_gains)  (sample_t *input_gains)  = 0;
  STDMETHOD (set_input_gains)  (sample_t *input_gains)  = 0;
  STDMETHOD (get_output_gains) (sample_t *output_gains) = 0;
  STDMETHOD (set_output_gains) (sample_t *output_gains) = 0;
  // Input/output levels
  STDMETHOD (get_levels)       (vtime_t time, sample_t *input_levels, sample_t *output_levels) = 0;
  // Matrix                    
  STDMETHOD (get_matrix)       (matrix_t *matrix) = 0;
  STDMETHOD (set_matrix)       (matrix_t *matrix) = 0;
  // DRC                       
  STDMETHOD (get_drc)          (bool *drc) = 0;
  STDMETHOD (set_drc)          (bool  drc) = 0;
  STDMETHOD (get_drc_power)    (sample_t *drc_power) = 0;
  STDMETHOD (set_drc_power)    (sample_t  drc_power) = 0;
  STDMETHOD (get_drc_level)    (sample_t *drc_level) = 0;
  // Bass redirection
  STDMETHOD (get_bass_redir)   (bool *bass_redir) = 0;
  STDMETHOD (set_bass_redir)   (bool  bass_redir) = 0;
  STDMETHOD (get_bass_freq)    (int  *bass_freq) = 0;
  STDMETHOD (set_bass_freq)    (int   bass_freq) = 0;
  STDMETHOD (get_bass_channels)(int  *bass_channels) = 0;
  STDMETHOD (set_bass_channels)(int   bass_channels) = 0;
  STDMETHOD (get_bass_level)   (sample_t *bass_level) = 0;
  // SRC
  STDMETHOD (get_src_quality)  (double *src_quality) = 0;
  STDMETHOD (set_src_quality)  (double  src_quality) = 0;
  STDMETHOD (get_src_att)      (double *src_att) = 0;
  STDMETHOD (set_src_att)      (double  src_att) = 0;
  // Equalizer
  STDMETHOD (get_eq)           (bool *eq) = 0;
  STDMETHOD (set_eq)           (bool  eq) = 0;
  STDMETHOD (get_eq_nbands)    (int ch, size_t *nbands) = 0;
  STDMETHOD (get_eq_bands)     (int ch, EqBand *bands, size_t first_band, size_t nbands) = 0;
  STDMETHOD (set_eq_bands)     (int ch, EqBand *bands, size_t nbands) = 0;
  STDMETHOD (get_eq_equalized) (int ch, bool *is_equalized) = 0;
  STDMETHOD (get_eq_channel)   (int *ch) = 0;
  STDMETHOD (set_eq_channel)   (int  ch) = 0;
  // Delay
  STDMETHOD (get_delay)        (bool *delay) = 0;
  STDMETHOD (set_delay)        (bool  delay) = 0;
  STDMETHOD (get_delay_units)  (int *delay_units) = 0;
  STDMETHOD (set_delay_units)  (int  delay_units) = 0;
  STDMETHOD (get_delays)       (float *delays) = 0;
  STDMETHOD (set_delays)       (float *delays) = 0;
  // Dithering
  STDMETHOD (get_dithering)    (int *dithering) = 0;
  STDMETHOD (set_dithering)    (int  dithering) = 0;
  // Cache
  STDMETHOD (get_input_cache_size)(vtime_t *size) = 0;
  STDMETHOD (set_input_cache_size)(vtime_t size) = 0;
  STDMETHOD (get_output_cache_size)(vtime_t *size) = 0;
  STDMETHOD (set_output_cache_size)(vtime_t size) = 0;
  STDMETHOD (get_input_cache_time)(vtime_t *time) = 0;
  STDMETHOD (get_output_cache_time)(vtime_t *time) = 0;
  STDMETHOD (get_input_cache)(int ch_name, vtime_t time, sample_t *buf, size_t size, size_t *out_size) = 0;
  STDMETHOD (get_output_cache)(int ch_name, vtime_t time, sample_t *buf, size_t size, size_t *out_size) = 0;
};

#endif
