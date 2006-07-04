
///////////////////////////////////////////////////////////
// String IDs

#define IDS_AC3FILTER                   1
#define IDS_MAIN                        2
#define IDS_MIXER                       3
#define IDS_GAINS                       4
#define IDS_EQ                          5
#define IDS_SYS                         6
#define IDS_ABOUT                       7
#define IDS_DESC                        8

///////////////////////////////////////////////////////////
// Interface IDs

#define IDI_AC3FILTER                   101

///////////////////////////////////////////////////////////
// Dialog IDs

#define IDD_MAIN                        102
#define IDD_MIXER                       103
#define IDD_GAINS                       104
#define IDD_EQ                          105
#define IDD_SYS                         106
#define IDD_ABOUT                       107

#define IDD_FILE                        108

#define IDD_SIMPLE                      109
#define IDD_TEMPL                       110

///////////////////////////////////////////////////////////
// Audio processing IDs
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// Gain
// Groups: 101

#define IDC_SLIDER_GAIN                10101
#define IDC_SLIDER_MASTER              10102

#define IDC_EDT_MASTER                 10103
#define IDC_EDT_GAIN                   10104

///////////////////////////////////////////////////////////
// AGC
// Groups: 102

#define IDC_CHK_AUTO_GAIN              10201
#define IDC_CHK_NORMALIZE              10202

#define IDC_EDT_ATTACK                 10203
#define IDC_EDT_RELEASE                10204

///////////////////////////////////////////////////////////
// DRC
// Groups: 103

#define IDC_CHK_DRC                    10301

#define IDC_SLIDER_DRC_POWER           10302
#define IDC_SLIDER_DRC_LEVEL           10303

#define IDC_EDT_DRC_POWER              10304
#define IDC_EDT_DRC_LEVEL              10305

///////////////////////////////////////////////////////////
// Matrix
// Groups: 104, 105, 106

// Matrix preset

#define IDC_CMB_MATRIX                 10401
#define IDC_BTN_MATRIX_SAVE            10402
#define IDC_BTN_MATRIX_DELETE          10403

// Matrix options

#define IDC_CHK_AUTO_MATRIX            10501
#define IDC_CHK_NORM_MATRIX            10502
#define IDC_CHK_VOICE_CONTROL          10503
#define IDC_CHK_EXPAND_STEREO          10504

// Matrix vaues

#define IDC_EDT_L_L                    10611
#define IDC_EDT_L_C                    10612
#define IDC_EDT_L_R                    10613
#define IDC_EDT_L_SL                   10614
#define IDC_EDT_L_SR                   10615
#define IDC_EDT_L_LFE                  10616
#define IDC_EDT_C_L                    10621
#define IDC_EDT_C_C                    10622
#define IDC_EDT_C_R                    10623
#define IDC_EDT_C_SL                   10624
#define IDC_EDT_C_SR                   10625
#define IDC_EDT_C_LFE                  10626
#define IDC_EDT_R_L                    10631
#define IDC_EDT_R_C                    10632
#define IDC_EDT_R_R                    10633
#define IDC_EDT_R_SL                   10634
#define IDC_EDT_R_SR                   10635
#define IDC_EDT_R_LFE                  10636
#define IDC_EDT_SL_L                   10641
#define IDC_EDT_SL_C                   10642
#define IDC_EDT_SL_R                   10643
#define IDC_EDT_SL_SL                  10644
#define IDC_EDT_SL_SR                  10645
#define IDC_EDT_SL_LFE                 10646
#define IDC_EDT_SR_L                   10651
#define IDC_EDT_SR_C                   10652
#define IDC_EDT_SR_R                   10653
#define IDC_EDT_SR_SL                  10654
#define IDC_EDT_SR_SR                  10655
#define IDC_EDT_SR_LFE                 10656
#define IDC_EDT_LFE_L                  10661
#define IDC_EDT_LFE_C                  10662
#define IDC_EDT_LFE_R                  10663
#define IDC_EDT_LFE_SL                 10664
#define IDC_EDT_LFE_SR                 10665
#define IDC_EDT_LFE_LFE                10666

///////////////////////////////////////////////////////////
// Mix levels
// Groups: 107

#define IDC_SLIDER_LFE                 10711
#define IDC_SLIDER_VOICE               10712
#define IDC_SLIDER_SUR                 10713

#define IDC_EDT_LFE                    10721
#define IDC_EDT_VOICE                  10722
#define IDC_EDT_SUR                    10723

#define IDC_CHK_LFELOCK                10731
#define IDC_CHK_CLOCK                  10732
#define IDC_CHK_SLOCK                  10733

///////////////////////////////////////////////////////////
// I/O Gains
// Groups: 108

#define IDC_SLIDER_IN_L                10811
#define IDC_SLIDER_IN_C                10812
#define IDC_SLIDER_IN_R                10813
#define IDC_SLIDER_IN_SL               10814
#define IDC_SLIDER_IN_SR               10815
#define IDC_SLIDER_IN_LFE              10816

#define IDC_SLIDER_OUT_L               10821
#define IDC_SLIDER_OUT_C               10822
#define IDC_SLIDER_OUT_R               10823
#define IDC_SLIDER_OUT_SL              10824
#define IDC_SLIDER_OUT_SR              10825
#define IDC_SLIDER_OUT_LFE             10826

#define IDC_EDT_IN_L                   10831
#define IDC_EDT_IN_C                   10832
#define IDC_EDT_IN_R                   10833
#define IDC_EDT_IN_SL                  10834
#define IDC_EDT_IN_SR                  10835
#define IDC_EDT_IN_LFE                 10836

#define IDC_EDT_OUT_L                  10841
#define IDC_EDT_OUT_C                  10842
#define IDC_EDT_OUT_R                  10843
#define IDC_EDT_OUT_SL                 10844
#define IDC_EDT_OUT_SR                 10845
#define IDC_EDT_OUT_LFE                10846

///////////////////////////////////////////////////////////
// I/O Levels
// Groups: 109

#define IDC_IN_L                       10911
#define IDC_IN_C                       10912
#define IDC_IN_R                       10913
#define IDC_IN_SL                      10914
#define IDC_IN_SR                      10915
#define IDC_IN_LFE                     10916

#define IDC_OUT_L                      10921
#define IDC_OUT_C                      10922
#define IDC_OUT_R                      10923
#define IDC_OUT_SL                     10924
#define IDC_OUT_SR                     10925
#define IDC_OUT_LFE                    10926

///////////////////////////////////////////////////////////
// Bass redirection
// Groups: 110

#define IDC_CHK_BASS_REDIR             11001
#define IDC_EDT_BASS_FREQ              11002

///////////////////////////////////////////////////////////
// Delay
// Groups: 111, 112, 113

// Delay preset

#define IDC_CMB_DELAY                  11101
#define IDC_BTN_DELAY_SAVE             11102
#define IDC_BTN_DELAY_DELETE           11103

// Delay options

#define IDC_CHK_DELAY                  11201
#define IDC_CMB_UNITS                  11202

// Delay values

#define IDC_EDT_DL                     11301
#define IDC_EDT_DC                     11302
#define IDC_EDT_DR                     11303
#define IDC_EDT_DSL                    11304
#define IDC_EDT_DSR                    11305
#define IDC_EDT_DLFE                   11306

///////////////////////////////////////////////////////////
// Sync
// Groups: 114

// Time shift

#define IDC_SLIDER_TIME_SHIFT          11401
#define IDC_EDT_TIME_SHIFT             11402

// Dejitter

#define IDC_CHK_JITTER                 11403
#define IDC_EDT_JITTER                 11404

///////////////////////////////////////////////////////////
// Equalizer
// Groups: 115, 116, 117

// Equalizer preset

#define IDC_CMB_EQ                     11501
#define IDC_BTN_EQ_SAVE                11502
#define IDC_BTN_EQ_DELETE              11503

// Equalizer options

#define IDC_CHK_EQ                     11601

// Equalizer values

#define IDC_EQ1                        11701
#define IDC_EQ2                        11702
#define IDC_EQ3                        11703
#define IDC_EQ4                        11704
#define IDC_EQ5                        11705
#define IDC_EQ6                        11706
#define IDC_EQ7                        11707
#define IDC_EQ8                        11708
#define IDC_EQ9                        11709

#define IDC_EDT_EQ1                    11721
#define IDC_EDT_EQ2                    11722
#define IDC_EDT_EQ3                    11723
#define IDC_EDT_EQ4                    11724
#define IDC_EDT_EQ5                    11725
#define IDC_EDT_EQ6                    11726
#define IDC_EDT_EQ7                    11727
#define IDC_EDT_EQ8                    11728
#define IDC_EDT_EQ9                    11729

#define IDC_LEVEL_EQ1                  11741
#define IDC_LEVEL_EQ2                  11742
#define IDC_LEVEL_EQ3                  11743
#define IDC_LEVEL_EQ4                  11744
#define IDC_LEVEL_EQ5                  11745
#define IDC_LEVEL_EQ6                  11746
#define IDC_LEVEL_EQ7                  11747
#define IDC_LEVEL_EQ8                  11748
#define IDC_LEVEL_EQ9                  11749


///////////////////////////////////////////////////////////
// Unsorted (to change)
///////////////////////////////////////////////////////////

// Decoder info controls

#define IDC_EDT_INFO                   19001
#define IDC_LBL_INPUT                  19002
#define IDC_EDT_FRAMES                 19003
#define IDC_EDT_ERRORS                 19004

// CPU usage controls

#define IDC_CPU                        19101
#define IDC_CPU_LABEL                  19102

#define IDC_CMB_SPK                    19201
#define IDC_CMB_FORMAT                 19202

// Link controls

#define IDC_LNK_AC3FILTER              19301
#define IDC_LNK_FORUM                  19302
#define IDC_LNK_LIBA52                 19303
#define IDC_LNK_EMAIL                  19304
#define IDC_LNK_MATRIX_MIXER           19305
#define IDC_LNK_WINAMPAC3              19306
#define IDC_LNK_DONATE                 19307
#define IDC_LNK_HOME                   19308
#define IDC_LNK_HOME2                  19309

// System options

#define IDC_RB_RENDER_DS               19401
#define IDC_RB_RENDER_WO               19402
#define IDC_RB_MERIT_PREFERRED         19403
#define IDC_RB_MERIT_UNLIKELY          19404

#define IDC_CHK_CONFIG_AUTOLOAD        19405

// Preset

#define IDC_CMB_PRESET                 19501
#define IDC_BTN_PRESET_FILE            19502
#define IDC_BTN_PRESET_SAVE            19503
#define IDC_BTN_PRESET_DELETE          19504

// Save preset to file dialog

#define IDC_EDT_FILE_NAME              19601

#define IDC_CHK_FILE_PRESET            19602
#define IDC_CHK_FILE_MATRIX            19603
#define IDC_CHK_FILE_EQ                19604
#define IDC_CHK_FILE_DELAY             19605

#define IDC_BTN_FILE_SAVE              19606
#define IDC_BTN_FILE_LOAD              19607
#define IDC_BTN_FILE_CHOOSE            19608

// Logos

#define IDC_LOGO                       19801
#define IDB_LOGO                       202
#define IDB_FORMAT_PCM                 19811
#define IDB_FORMAT_AC3                 19812
#define IDB_FORMAT_DTS                 19813
#define IDB_FORMAT_MPA                 19814
#define IDB_FORMAT_PES                 19815

// Allowed formats

#define IDC_CHK_PCM                    19901
#define IDC_CHK_AC3                    19902
#define IDC_CHK_DTS                    19903
#define IDC_CHK_MPA                    19904
#define IDC_CHK_PES                    19905

// SPDIF

#define IDC_CHK_SPDIF                  20001
#define IDC_LBL_SPDIF_STATUS           20002

#define IDC_CHK_SPDIF_MPA              20011
#define IDC_CHK_SPDIF_AC3              20012
#define IDC_CHK_SPDIF_DTS              20013

#define IDC_CHK_SPDIF_STEREO_PT        20014
#define IDC_CHK_SPDIF_AS_PCM           20015

// Other

#define IDC_VER                        20101
#define IDC_EDT_ENV                    20102
