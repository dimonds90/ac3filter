
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
#define IDD_EQ                          105
#define IDD_SPDIF                       106
#define IDD_SYSTEM                      107
#define IDD_ABOUT                       108
#define IDD_EQ_CUSTOM                   109
#define IDD_EQ_SAVE                     110
#define IDD_ERROR_REPORT                111

#define IDD_SIMPLE                      200
#define IDD_TEMPL                       201

///////////////////////////////////////////////////////////
// Logos (100)

#define IDC_LOGO                       10001
#define IDB_LOGO                       10002

///////////////////////////////////////////////////////////
// Bitmaps (200)

#define IDB_PAUSE_ICON                 20001
#define IDB_PLAY_ICON                  20002
#define IDB_STOP_ICON                  20003

///////////////////////////////////////////////////////////
// Audio processing controls
///////////////////////////////////////////////////////////


// Output format (500)
#define IDC_GRP_OUTPUT                 50001
#define IDC_CMB_SPK                    50002
#define IDC_CMB_FORMAT                 50003
#define IDC_LBL_FORMAT                 50004
#define IDC_CMB_RATE                   50005
#define IDC_LBL_RATE                   50006
#define IDC_CHK_USE_SPDIF              50007
#define IDC_LBL_SPDIF_STATUS           50008

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
#define IDC_LEVEL_IN1                  50411
#define IDC_LEVEL_IN2                  50412
#define IDC_LEVEL_IN3                  50413
#define IDC_LEVEL_IN4                  50414
#define IDC_LEVEL_IN5                  50415
#define IDC_LEVEL_IN6                  50416
#define IDC_LEVEL_IN7                  50417
#define IDC_LEVEL_IN8                  50418
#define IDC_LEVEL_OUT1                 50421
#define IDC_LEVEL_OUT2                 50422
#define IDC_LEVEL_OUT3                 50423
#define IDC_LEVEL_OUT4                 50424
#define IDC_LEVEL_OUT5                 50425
#define IDC_LEVEL_OUT6                 50426
#define IDC_LEVEL_OUT7                 50427
#define IDC_LEVEL_OUT8                 50428


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
#define IDC_GRP_BASS                   51301
#define IDC_CHK_BASS_ENABLE            51302
#define IDC_LBL_BASS_FREQ              51303
#define IDC_EDT_BASS_FREQ              51304
#define IDC_LBL_BASS_ROUTE             51305
#define IDC_RBT_BASS_FRONT             51306
#define IDC_RBT_BASS_SUB               51307
#define IDC_SLI_BASS_LEVEL             51308

// Matrix preset (510)
#define IDC_GRP_MATRIX_PRESET          51001
#define IDC_CMB_MATRIX_PRESET          51002
#define IDC_BTN_MATRIX_SAVE            51003
#define IDC_BTN_MATRIX_DELETE          51004

// Matrix values (511)
#define IDC_GRP_MATRIX                 51100
#define IDC_EDT_MATRIX11               51111
#define IDC_EDT_MATRIX12               51112
#define IDC_EDT_MATRIX13               51113
#define IDC_EDT_MATRIX14               51114
#define IDC_EDT_MATRIX15               51115
#define IDC_EDT_MATRIX16               51116
#define IDC_EDT_MATRIX17               51117
#define IDC_EDT_MATRIX18               51118
#define IDC_EDT_MATRIX21               51121
#define IDC_EDT_MATRIX22               51122
#define IDC_EDT_MATRIX23               51123
#define IDC_EDT_MATRIX24               51124
#define IDC_EDT_MATRIX25               51125
#define IDC_EDT_MATRIX26               51126
#define IDC_EDT_MATRIX27               51127
#define IDC_EDT_MATRIX28               51128
#define IDC_EDT_MATRIX31               51131
#define IDC_EDT_MATRIX32               51132
#define IDC_EDT_MATRIX33               51133
#define IDC_EDT_MATRIX34               51134
#define IDC_EDT_MATRIX35               51135
#define IDC_EDT_MATRIX36               51136
#define IDC_EDT_MATRIX37               51137
#define IDC_EDT_MATRIX38               51138
#define IDC_EDT_MATRIX41               51141
#define IDC_EDT_MATRIX42               51142
#define IDC_EDT_MATRIX43               51143
#define IDC_EDT_MATRIX44               51144
#define IDC_EDT_MATRIX45               51145
#define IDC_EDT_MATRIX46               51146
#define IDC_EDT_MATRIX47               51147
#define IDC_EDT_MATRIX48               51148
#define IDC_EDT_MATRIX51               51151
#define IDC_EDT_MATRIX52               51152
#define IDC_EDT_MATRIX53               51153
#define IDC_EDT_MATRIX54               51154
#define IDC_EDT_MATRIX55               51155
#define IDC_EDT_MATRIX56               51156
#define IDC_EDT_MATRIX57               51157
#define IDC_EDT_MATRIX58               51158
#define IDC_EDT_MATRIX61               51161
#define IDC_EDT_MATRIX62               51162
#define IDC_EDT_MATRIX63               51163
#define IDC_EDT_MATRIX64               51164
#define IDC_EDT_MATRIX65               51165
#define IDC_EDT_MATRIX66               51166
#define IDC_EDT_MATRIX67               51167
#define IDC_EDT_MATRIX68               51168
#define IDC_EDT_MATRIX71               51171
#define IDC_EDT_MATRIX72               51172
#define IDC_EDT_MATRIX73               51173
#define IDC_EDT_MATRIX74               51174
#define IDC_EDT_MATRIX75               51175
#define IDC_EDT_MATRIX76               51176
#define IDC_EDT_MATRIX77               51177
#define IDC_EDT_MATRIX78               51178
#define IDC_EDT_MATRIX81               51181
#define IDC_EDT_MATRIX82               51182
#define IDC_EDT_MATRIX83               51183
#define IDC_EDT_MATRIX84               51184
#define IDC_EDT_MATRIX85               51185
#define IDC_EDT_MATRIX86               51186
#define IDC_EDT_MATRIX87               51187
#define IDC_EDT_MATRIX88               51188

// Matrix labels (533)
#define IDC_LBL_MATRIX_IN1             53311
#define IDC_LBL_MATRIX_IN2             53312
#define IDC_LBL_MATRIX_IN3             53313
#define IDC_LBL_MATRIX_IN4             53314
#define IDC_LBL_MATRIX_IN5             53315
#define IDC_LBL_MATRIX_IN6             53316
#define IDC_LBL_MATRIX_IN7             53317
#define IDC_LBL_MATRIX_IN8             53318
#define IDC_LBL_MATRIX_OUT1            53321
#define IDC_LBL_MATRIX_OUT2            53322
#define IDC_LBL_MATRIX_OUT3            53323
#define IDC_LBL_MATRIX_OUT4            53324
#define IDC_LBL_MATRIX_OUT5            53325
#define IDC_LBL_MATRIX_OUT6            53326
#define IDC_LBL_MATRIX_OUT7            53327
#define IDC_LBL_MATRIX_OUT8            53328

// Input/output gains (512)
#define IDC_GRP_INPUT_GAINS            51201
#define IDC_GRP_OUTPUT_GAINS           51202
#define IDC_LBL_GAIN_IN1               51211
#define IDC_LBL_GAIN_IN2               51212
#define IDC_LBL_GAIN_IN3               51213
#define IDC_LBL_GAIN_IN4               51214
#define IDC_LBL_GAIN_IN5               51215
#define IDC_LBL_GAIN_IN6               51216
#define IDC_LBL_GAIN_IN7               51217
#define IDC_LBL_GAIN_IN8               51218
#define IDC_LBL_GAIN_OUT1              51221
#define IDC_LBL_GAIN_OUT2              51222
#define IDC_LBL_GAIN_OUT3              51223
#define IDC_LBL_GAIN_OUT4              51224
#define IDC_LBL_GAIN_OUT5              51225
#define IDC_LBL_GAIN_OUT6              51226
#define IDC_LBL_GAIN_OUT7              51227
#define IDC_LBL_GAIN_OUT8              51228
#define IDC_EDT_GAIN_IN1               51231
#define IDC_EDT_GAIN_IN2               51232
#define IDC_EDT_GAIN_IN3               51233
#define IDC_EDT_GAIN_IN4               51234
#define IDC_EDT_GAIN_IN5               51235
#define IDC_EDT_GAIN_IN6               51236
#define IDC_EDT_GAIN_IN7               51237
#define IDC_EDT_GAIN_IN8               51238
#define IDC_EDT_GAIN_OUT1              51241
#define IDC_EDT_GAIN_OUT2              51242
#define IDC_EDT_GAIN_OUT3              51243
#define IDC_EDT_GAIN_OUT4              51244
#define IDC_EDT_GAIN_OUT5              51245
#define IDC_EDT_GAIN_OUT6              51246
#define IDC_EDT_GAIN_OUT7              51247
#define IDC_EDT_GAIN_OUT8              51248
#define IDC_SLI_GAIN_IN1               51251
#define IDC_SLI_GAIN_IN2               51252
#define IDC_SLI_GAIN_IN3               51253
#define IDC_SLI_GAIN_IN4               51254
#define IDC_SLI_GAIN_IN5               51255
#define IDC_SLI_GAIN_IN6               51256
#define IDC_SLI_GAIN_IN7               51257
#define IDC_SLI_GAIN_IN8               51258
#define IDC_SLI_GAIN_OUT1              51261
#define IDC_SLI_GAIN_OUT2              51262
#define IDC_SLI_GAIN_OUT3              51263
#define IDC_SLI_GAIN_OUT4              51264
#define IDC_SLI_GAIN_OUT5              51265
#define IDC_SLI_GAIN_OUT6              51266
#define IDC_SLI_GAIN_OUT7              51267
#define IDC_SLI_GAIN_OUT8              51268

// Delay preset (514)
#define IDC_GRP_DELAY_PRESET           51401
#define IDC_CMB_DELAY_PRESET           51402
#define IDC_BTN_DELAY_SAVE             51403
#define IDC_BTN_DELAY_DELETE           51404

// Delay (515)
#define IDC_GRP_DELAYS                 51501
#define IDC_CHK_DELAYS                 51502
#define IDC_LBL_DELAY_UNITS            51503
#define IDC_CMB_DELAY_UNITS            51504
#define IDC_BTN_DELAYS_RESET           51505
#define IDC_LBL_DELAY1                 51511
#define IDC_LBL_DELAY2                 51512
#define IDC_LBL_DELAY3                 51513
#define IDC_LBL_DELAY4                 51514
#define IDC_LBL_DELAY5                 51515
#define IDC_LBL_DELAY6                 51516
#define IDC_LBL_DELAY7                 51517
#define IDC_LBL_DELAY8                 51518
#define IDC_EDT_DELAY1                 51521
#define IDC_EDT_DELAY2                 51522
#define IDC_EDT_DELAY3                 51523
#define IDC_EDT_DELAY4                 51524
#define IDC_EDT_DELAY5                 51525
#define IDC_EDT_DELAY6                 51526
#define IDC_EDT_DELAY7                 51527
#define IDC_EDT_DELAY8                 51528

// Channel routing (534)
#define IDC_GRP_ROUTE                  53401
#define IDC_CHK_ROUTE_ENABLE           53402
#define IDC_BTN_ROUTE_RESET            53403
#define IDC_LBL_ROUTE1                 53411
#define IDC_LBL_ROUTE2                 53412
#define IDC_LBL_ROUTE3                 53413
#define IDC_LBL_ROUTE4                 53414
#define IDC_LBL_ROUTE5                 53415
#define IDC_LBL_ROUTE6                 53416
#define IDC_LBL_ROUTE7                 53417
#define IDC_LBL_ROUTE8                 53418
#define IDC_CMB_ROUTE1                 53421
#define IDC_CMB_ROUTE2                 53422
#define IDC_CMB_ROUTE3                 53423
#define IDC_CMB_ROUTE4                 53424
#define IDC_CMB_ROUTE5                 53425
#define IDC_CMB_ROUTE6                 53426
#define IDC_CMB_ROUTE7                 53427
#define IDC_CMB_ROUTE8                 53428


// Allowed formats (516)
#define IDC_GRP_FORMATS                51601
#define IDC_LST_FORMATS                51602

// Filter merit (517)
#define IDC_GRP_MERIT                  51701
#define IDC_RBT_MERIT_PREFERRED        51702
#define IDC_RBT_MERIT_UNLIKELY         51703
#define IDC_RBT_MERIT_DO_NOT_USE       51704

// Default renderer (518)
#define IDC_GRP_RENDERER               51801
#define IDC_RBT_RENDER_DS              51802
#define IDC_RBT_RENDER_WO              51803

// Sync (519)
#define IDC_GRP_SYNC                   51901
#define IDC_SLI_TIME_SHIFT             51902
#define IDC_LBL_TIME_SHIFT             51903
#define IDC_EDT_TIME_SHIFT             51904
#define IDC_LBL_TIME_SHIFT_MINUS       51905
#define IDC_LBL_TIME_SHIFT_PLUS        51906
#define IDC_CHK_JITTER                 51907
#define IDC_LBL_JITTER                 51908
#define IDC_EDT_JITTER                 51909

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

// SPDIF/DTS conversion (526)
#define IDC_GRP_DTS_CONV               52601
#define IDC_RBT_DTS_CONV_NONE          52602
#define IDC_RBT_DTS_CONV_14BIT         52603
#define IDC_RBT_DTS_CONV_16BIT         52604

// SPDIF options (522)
#define IDC_GRP_SPDIF                  52201
#define IDC_CHK_USE_DETECTOR           52202
#define IDC_CHK_SPDIF_ENCODE           52203
#define IDC_CMB_SPDIF_BITRATE          52204
#define IDC_LBL_SPDIF_BITRATE          52205
#define IDC_CHK_SPDIF_STEREO_PT        52206
#define IDC_CHK_SPDIF_AS_PCM           52207
#define IDC_CHK_SPDIF_CHECK_SR         52208
#define IDC_CHK_SPDIF_ALLOW_48         52209
#define IDC_CHK_SPDIF_ALLOW_44         52210
#define IDC_CHK_SPDIF_ALLOW_32         52211

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
#define IDC_LNK_TRANSLATE              52705

// About dialog (528)
#define IDC_GRP_ABOUT                  52801
#define IDC_LBL_ABOUT1                 52802
#define IDC_LBL_ABOUT2                 52803
#define IDC_GRP_LICENSE                52804
#define IDC_LBL_LICENSE                52805
#define IDC_GRP_CONTACT                52806
#define IDC_LBL_CONTACT                52807
#define IDC_GRP_CREDITS                52808
#define IDC_EDT_CREDITS                52809

// Link controls (525)
#define IDC_LNK_HOME                   52501
#define IDC_LNK_FORUM                  52502
#define IDC_LNK_EMAIL                  52503

// Equalizer preset (530)
#define IDC_GRP_EQ_PRESET              53001
#define IDC_CMB_EQ_PRESET              53002
#define IDC_BTN_EQ_SAVE                53003
#define IDC_BTN_EQ_DELETE              53004

// Equalizer controls (531)
#define IDC_GRP_EQ                     53101
#define IDC_GRP_EQ_OPTIONS             53102
#define IDC_CHK_EQ                     53103
#define IDC_CHK_EQ_LOG                 53104
#define IDC_CMB_EQ_CH                  53105
#define IDC_BTN_EQ_RESET               53106
#define IDC_BTN_EQ_CUSTOM              53107
#define IDC_LBL_EQ_FREQ                53108
#define IDC_LBL_EQ_GAIN                53109
#define IDC_LBL_EQ_LEN                 53110
#define IDC_EDT_EQ_LEN                 53111
#define IDC_SPECTRUM                   53112

#define IDC_EDT_EQ1                    53120
#define IDC_EDT_EQ2                    53121
#define IDC_EDT_EQ3                    53122
#define IDC_EDT_EQ4                    53123
#define IDC_EDT_EQ5                    53124
#define IDC_EDT_EQ6                    53125
#define IDC_EDT_EQ7                    53126
#define IDC_EDT_EQ8                    53127
#define IDC_EDT_EQ9                    53128
#define IDC_EDT_EQ10                   53129

#define IDC_SLI_EQ1                    53130
#define IDC_SLI_EQ2                    53131
#define IDC_SLI_EQ3                    53132
#define IDC_SLI_EQ4                    53133
#define IDC_SLI_EQ5                    53134
#define IDC_SLI_EQ6                    53135
#define IDC_SLI_EQ7                    53136
#define IDC_SLI_EQ8                    53137
#define IDC_SLI_EQ9                    53138
#define IDC_SLI_EQ10                   53139

#define IDC_LBL_EQ1                    53140
#define IDC_LBL_EQ2                    53141
#define IDC_LBL_EQ3                    53142
#define IDC_LBL_EQ4                    53143
#define IDC_LBL_EQ5                    53144
#define IDC_LBL_EQ6                    53145
#define IDC_LBL_EQ7                    53146
#define IDC_LBL_EQ8                    53147
#define IDC_LBL_EQ9                    53148
#define IDC_LBL_EQ10                   53149

#define IDC_EDT_EQ_FREQ1               53160
#define IDC_EDT_EQ_FREQ2               53161
#define IDC_EDT_EQ_FREQ3               53162
#define IDC_EDT_EQ_FREQ4               53163
#define IDC_EDT_EQ_FREQ5               53164
#define IDC_EDT_EQ_FREQ6               53165
#define IDC_EDT_EQ_FREQ7               53166
#define IDC_EDT_EQ_FREQ8               53167
#define IDC_EDT_EQ_FREQ9               53168
#define IDC_EDT_EQ_FREQ10              53169

#define IDC_EDT_EQ_GAIN1               53170
#define IDC_EDT_EQ_GAIN2               53171
#define IDC_EDT_EQ_GAIN3               53172
#define IDC_EDT_EQ_GAIN4               53173
#define IDC_EDT_EQ_GAIN5               53174
#define IDC_EDT_EQ_GAIN6               53175
#define IDC_EDT_EQ_GAIN7               53176
#define IDC_EDT_EQ_GAIN8               53177
#define IDC_EDT_EQ_GAIN9               53178
#define IDC_EDT_EQ_GAIN10              53179

// Save equalizer dialog (532)
#define IDC_RBT_EQ_CUR                 53201
#define IDC_RBT_EQ_MCH                 53202
#define IDC_RBT_EQ_ALL                 53203

// Debug
#define IDC_GRP_DEBUG                  53501
#define IDC_LBL_FEEDBACK               53502
#define IDC_EDT_FEEDBACK               53503
#define IDC_CHK_SAVE_LOGS              53504
#define IDC_BTN_ERROR_REPORT           53505
#define IDC_LNK_ERROR_REPORT           53506
#define IDC_LBL_ERROR_DESC             53507
#define IDC_EDT_ERROR_DESC             53508
#define IDC_CHK_SEND_AUDIO_DATA        53509


// Other
#define IDC_VER                        52901
#define IDC_BTN_DONATE                 52902
#define IDC_BTN_HELP                   52903
