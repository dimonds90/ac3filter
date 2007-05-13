
///////////////////////////////////////////////////////////
// String IDs

#define IDS_AC3FILTER                   1
#define IDS_MAIN                        2
#define IDS_MIXER                       3
#define IDS_GAINS                       4
#define IDS_SPDIF                       5
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
#define IDD_SPDIF                       105
#define IDD_SYS                         106
#define IDD_ABOUT                       107

#define IDD_FILE                        108

#define IDD_SIMPLE                      109
#define IDD_TEMPL                       110





///////////////////////////////////////////////////////////
// Audio processing controls
///////////////////////////////////////////////////////////


// Output format (500)
#define IDC_GRP_OUTPUT                 50001
#define IDC_CMB_SPK                    50002
#define IDC_CMB_FORMAT                 50003
#define IDC_CHK_USE_SPDIF              50004
#define IDC_LBL_SPDIF_STATUS           50005

// Preset (501)
#define IDC_GRP_PRESET                 50101
#define IDC_CMB_PRESET                 50102
#define IDC_BTN_PRESET_FILE            50103
#define IDC_BTN_PRESET_SAVE            50104
#define IDC_BTN_PRESET_DELETE          50105

// Save preset to file dialog (502)
#define IDC_EDT_FILE_NAME              50201
#define IDC_CHK_FILE_PRESET            50202
#define IDC_CHK_FILE_MATRIX            50203
#define IDC_CHK_FILE_EQ                50204
#define IDC_CHK_FILE_DELAY             50205
#define IDC_BTN_FILE_SAVE              50206
#define IDC_BTN_FILE_LOAD              50207
#define IDC_BTN_FILE_CHOOSE            50208

// Decoder info (503)
#define IDC_GRP_DECODER_INFO           50301
#define IDC_LBL_INPUT                  50302
#define IDC_EDT_INFO                   50303
#define IDC_LBL_FRAMES_ERRORS          50304
#define IDC_EDT_FRAMES                 50305
#define IDC_EDT_ERRORS                 50306

// Input/output levels (504)
#define IDC_GRP_LEVELS                 50401
#define IDC_LBL_INPUT_LEVELS           50402
#define IDC_LBL_OUTPUT_LEVELS          50403
#define IDC_GRP_INPUT_LEVELS           50404
#define IDC_GRP_OUTPUT_LEVELS          50405
#define IDC_LBL_IN_L                   50411
#define IDC_LBL_IN_C                   50412
#define IDC_LBL_IN_R                   50413
#define IDC_LBL_IN_SL                  50414
#define IDC_LBL_IN_SR                  50415
#define IDC_LBL_IN_LFE                 50416
#define IDC_LBL_OUT_L                  50421
#define IDC_LBL_OUT_C                  50422
#define IDC_LBL_OUT_R                  50423
#define IDC_LBL_OUT_SL                 50424
#define IDC_LBL_OUT_SR                 50425
#define IDC_LBL_OUT_SW                 50426
#define IDC_IN_L                       50431
#define IDC_IN_C                       50432
#define IDC_IN_R                       50433
#define IDC_IN_SL                      50434
#define IDC_IN_SR                      50435
#define IDC_IN_LFE                     50436
#define IDC_OUT_L                      50441
#define IDC_OUT_C                      50442
#define IDC_OUT_R                      50443
#define IDC_OUT_SL                     50444
#define IDC_OUT_SR                     50445
#define IDC_OUT_SW                     50446

// Gains (505)
#define IDC_GRP_GAIN                   50501
#define IDC_GRP_GAINS                  50502
#define IDC_LBL_MASTER                 50511
#define IDC_LBL_GAIN                   50512
#define IDC_LBL_LFE                    50513
#define IDC_LBL_VOICE                  50514
#define IDC_LBL_SUR                    50515
#define IDC_SLI_MASTER                 50521
#define IDC_SLI_GAIN                   50522
#define IDC_SLI_LFE                    50523
#define IDC_SLI_VOICE                  50524
#define IDC_SLI_SUR                    50525
#define IDC_EDT_MASTER                 50531
#define IDC_EDT_GAIN                   50532
#define IDC_EDT_LFE                    50533
#define IDC_EDT_VOICE                  50534
#define IDC_EDT_SUR                    50535

// CPU usage (506)
#define IDC_GRP_CPU                    50601
#define IDC_CPU                        50602
#define IDC_CPU_LABEL                  50603

// AGC options (507)
#define IDC_GRP_AGC                    50701
#define IDC_CHK_AUTO_GAIN              50702
#define IDC_CHK_NORMALIZE              50703
#define IDC_LBL_ATTACK_RELEASE         50704
#define IDC_EDT_ATTACK                 50705
#define IDC_EDT_RELEASE                50706

// DRC (509)
#define IDC_GRP_DRC                    50901
#define IDC_CHK_DRC                    50902
#define IDC_SLI_DRC_POWER              50903
#define IDC_SLI_DRC_LEVEL              50904
#define IDC_EDT_DRC_POWER              50905
#define IDC_EDT_DRC_LEVEL              50906

// Mixer options (508)
#define IDC_GRP_MIXER_OPTIONS          50801
#define IDC_CHK_AUTO_MATRIX            50802
#define IDC_CHK_NORM_MATRIX            50803
#define IDC_CHK_VOICE_CONTROL          50804
#define IDC_CHK_EXPAND_STEREO          50805

// Bass redirection (513)
#define IDC_CHK_BASS_REDIR             51301
#define IDC_EDT_BASS_FREQ              51302

// Matrix preset (510)
#define IDC_GRP_MATRIX_PRESET          51001
#define IDC_CMB_MATRIX_PRESET          51002
#define IDC_BTN_MATRIX_SAVE            51003
#define IDC_BTN_MATRIX_DELETE          51004

// Matrix values (511)
#define IDC_GRP_MATRIX                 51101
#define IDC_EDT_L_L                    51111
#define IDC_EDT_L_C                    51112
#define IDC_EDT_L_R                    51113
#define IDC_EDT_L_SL                   51114
#define IDC_EDT_L_SR                   51115
#define IDC_EDT_L_LFE                  51116
#define IDC_EDT_C_L                    51121
#define IDC_EDT_C_C                    51122
#define IDC_EDT_C_R                    51123
#define IDC_EDT_C_SL                   51124
#define IDC_EDT_C_SR                   51125
#define IDC_EDT_C_LFE                  51126
#define IDC_EDT_R_L                    51131
#define IDC_EDT_R_C                    51132
#define IDC_EDT_R_R                    51133
#define IDC_EDT_R_SL                   51134
#define IDC_EDT_R_SR                   51135
#define IDC_EDT_R_LFE                  51136
#define IDC_EDT_SL_L                   51141
#define IDC_EDT_SL_C                   51142
#define IDC_EDT_SL_R                   51143
#define IDC_EDT_SL_SL                  51144
#define IDC_EDT_SL_SR                  51145
#define IDC_EDT_SL_LFE                 51146
#define IDC_EDT_SR_L                   51151
#define IDC_EDT_SR_C                   51152
#define IDC_EDT_SR_R                   51153
#define IDC_EDT_SR_SL                  51154
#define IDC_EDT_SR_SR                  51155
#define IDC_EDT_SR_LFE                 51156
#define IDC_EDT_LFE_L                  51161
#define IDC_EDT_LFE_C                  51162
#define IDC_EDT_LFE_R                  51163
#define IDC_EDT_LFE_SL                 51164
#define IDC_EDT_LFE_SR                 51165
#define IDC_EDT_LFE_LFE                51166

// Input/output gains (512)
#define IDC_GRP_INPUT_GAINS            51201
#define IDC_GRP_OUTPUT_GAINS           51202
#define IDC_SLI_IN_L                   51211
#define IDC_SLI_IN_C                   51212
#define IDC_SLI_IN_R                   51213
#define IDC_SLI_IN_SL                  51214
#define IDC_SLI_IN_SR                  51215
#define IDC_SLI_IN_LFE                 51216
#define IDC_SLI_OUT_L                  51221
#define IDC_SLI_OUT_C                  51222
#define IDC_SLI_OUT_R                  51223
#define IDC_SLI_OUT_SL                 51224
#define IDC_SLI_OUT_SR                 51225
#define IDC_SLI_OUT_LFE                51226
#define IDC_EDT_IN_L                   51231
#define IDC_EDT_IN_C                   51232
#define IDC_EDT_IN_R                   51233
#define IDC_EDT_IN_SL                  51234
#define IDC_EDT_IN_SR                  51235
#define IDC_EDT_IN_LFE                 51236
#define IDC_EDT_OUT_L                  51241
#define IDC_EDT_OUT_C                  51242
#define IDC_EDT_OUT_R                  51243
#define IDC_EDT_OUT_SL                 51244
#define IDC_EDT_OUT_SR                 51245
#define IDC_EDT_OUT_LFE                51246

// Delay preset (514)
#define IDC_GRP_DELAY_PRESET           51401
#define IDC_CMB_DELAY_PRESET           51402
#define IDC_BTN_DELAY_SAVE             51403
#define IDC_BTN_DELAY_DELETE           51404

// Delay (515)
#define IDC_GRP_DELAYS                 51501
#define IDC_CHK_DELAYS                 51502
#define IDC_LBL_UNITS                  51503
#define IDC_CMB_UNITS                  51504
#define IDC_EDT_DL                     51511
#define IDC_EDT_DC                     51512
#define IDC_EDT_DR                     51513
#define IDC_EDT_DSL                    51514
#define IDC_EDT_DSR                    51515
#define IDC_EDT_DLFE                   51516

// Allowed formats (516)
#define IDC_GRP_FORMATS                51601
#define IDC_CHK_PCM                    51602
#define IDC_CHK_AC3                    51603
#define IDC_CHK_DTS                    51604
#define IDC_CHK_MPA                    51605
#define IDC_CHK_PES                    51606
#define IDC_CHK_SPDIF                  51607

// Filter merit (517)
#define IDC_GRP_MERIT                  51701
#define IDC_RBT_MERIT_PREFERRED        51702
#define IDC_RBT_MERIT_UNLIKELY         51703

// Default renderer (518)
#define IDC_GRP_RENDERER               51801
#define IDC_RBT_RENDER_DS              51802
#define IDC_RBT_RENDER_WO              51803

// Sync (519)
#define IDC_GRP_SYNC                   51901
#define IDC_SLI_TIME_SHIFT             51902
#define IDC_LBL_TIME_SHIFT             51903
#define IDC_EDT_TIME_SHIFT             51904
#define IDC_CHK_JITTER                 51905
#define IDC_LBL_JITTER                 51906
#define IDC_EDT_JITTER                 51907

// SPDIF passthrough (520)
#define IDC_GRP_SPDIF_PT               52001
#define IDC_CHK_SPDIF_MPA              52002
#define IDC_CHK_SPDIF_AC3              52003
#define IDC_CHK_SPDIF_DTS              52004

// SPDIF/DTS output mode (521)
#define IDC_GRP_DTS_MODE               52101
#define IDC_RBT_DTS_MODE_AUTO          52102
#define IDC_RBT_DTS_MODE_WRAPPED       52103
#define IDC_RBT_DTS_MODE_PADDED        52104
#define IDC_CHK_USE_DTS14              52105

// SPDIF/DTS conversion (526)
#define IDC_GRP_DTS_CONV               52601
#define IDC_RBT_DTS_CONV_NONE          52602
#define IDC_RBT_DTS_CONV_14BIT         52603
#define IDC_RBT_DTS_CONV_16BIT         52604

// SPDIF options (522)
#define IDC_GRP_SPDIF                  52201
#define IDC_CHK_USE_DETECTOR           52202
#define IDC_CHK_SPDIF_ENCODE           52203
#define IDC_CHK_SPDIF_STEREO_PT        52204
#define IDC_CHK_SPDIF_AS_PCM           52205
#define IDC_CHK_SPDIF_CHECK_SR         52206
#define IDC_CHK_SPDIF_ALLOW_48         52207
#define IDC_CHK_SPDIF_ALLOW_44         52208
#define IDC_CHK_SPDIF_ALLOW_32         52209

// DirectShow options (523)
#define IDC_GRP_DSHOW                  52301
#define IDC_CHK_REINIT                 52302
#define IDC_CHK_QUERY_SINK             52303
#define IDC_CHK_SPDIF_NO_PCM           52304

// Interface options (524)
#define IDC_GRP_INTERFACE              52401
#define IDC_CHK_TRAY                   52402
#define IDC_CHK_INVERT_LEVELS          52403
#define IDC_CHK_TOOLTIPS               52404
#define IDC_LBL_REFRESH_TIME           52405 
#define IDC_EDT_REFRESH_TIME           52406

// Translation options (527)
#define IDC_GRP_TRANS                  52701
#define IDC_CMB_LANG                   52702
#define IDC_LBL_LANG                   52703
#define IDC_EDT_TRANS_INFO             52704

// About dialog (528)
#define IDC_GRP_ABOUT                  52801
#define IDC_LBL_ABOUT                  52802
#define IDC_GRP_LICENSE                52803
#define IDC_LBL_LICENSE                52804
#define IDC_GRP_CONTACT                52805
#define IDC_LBL_CONTACT                52806
#define IDC_GRP_ENV                    52807
#define IDC_EDT_ENV                    52808
#define IDC_GRP_CREDITS                52809
#define IDC_EDT_CREDITS                52810

// Link controls (525)
#define IDC_LNK_HOME                   52501
#define IDC_LNK_FORUM                  52502
#define IDC_LNK_EMAIL                  52503

// Logos (100)
#define IDC_LOGO                       10001
#define IDB_LOGO                       10002

// Other
#define IDC_VER                        52901
#define IDC_BTN_DONATE                 52902
