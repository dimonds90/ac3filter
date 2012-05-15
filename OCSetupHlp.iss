//
// OCSetupHlp.iss
// --------------
//
// OpenCandy Helper Include File
//
// This file defines functions and procedures that need to
// be called from your main installer script in order to
// initialize and setup OpenCandy.
//
// Please consult the accompanying SDK documentation for
// integration details and contact partner support for
// assistance with any advanced integration needs.
//
// IMPORTANT:
// ----------
// Publishers should have no need to modify the content
// of this file. If you are modifying this file for any
// reason other than as directed by partner support
// you are probably making a mistake. Please contact
// partner support instead.
//
// Copyright (c) 2008 - 2012 SweetLabs, Inc.
//



[Code]

//--------------------------------
// OpenCandy types
//--------------------------------

#ifdef UNICODE
type OCWString = String;
type OCAString = AnsiString;
type OCTString = OCWString;
#else
type OCAString = String;
type OCTString = OCAString;
#endif



//--------------------------------
// OpenCandy definitions
//--------------------------------

// Size of strings (including terminating character)
#define OC_STR_CHARS 1024

// Values used with OCInit2A(), OCInit2W() APIs
#define OC_INIT_SUCCESS      0
#define OC_INIT_MODE_NORMAL  0
#define OC_INIT_MODE_REMNANT 1

// Values used with OCGetNoCandy() API
#define OC_CANDY_ENABLED  0
#define OC_CANDY_DISABLED 1

// Offer types returned by OCGetOfferType() API
#define OC_OFFER_TYPE_NORMAL   1
#define OC_OFFER_TYPE_EMBEDDED 2

// Values returned by OCGetBannerInfo() API
#define OC_OFFER_BANNER_FOUNDNEITHER     0
#define OC_OFFER_BANNER_FOUNDTITLE       1
#define OC_OFFER_BANNER_FOUNDDESCRIPTION 2
#define OC_OFFER_BANNER_FOUNDBOTH        3

// User choice indicators returned by OCGetOfferState() API
#define OC_OFFER_CHOICE_ACCEPTED  1
#define OC_OFFER_CHOICE_DECLINED  0
#define OC_OFFER_CHOICE_NOCHOICE -1

// Values used with OCCanLeaveOfferPage() API
#define OC_OFFER_LEAVEPAGE_ALLOWED    1
#define OC_OFFER_LEAVEPAGE_DISALLOWED 0

// Values used for OCGetAsyncOfferStatus() API
#define OC_OFFER_STATUS_CANOFFER_READY    0
#define OC_OFFER_STATUS_CANOFFER_NOTREADY 1
#define OC_OFFER_STATUS_QUERYING_NOTREADY 2
#define OC_OFFER_STATUS_NOOFFERSAVAILABLE 3

// Values returned by OCRunDialog() API
#define OC_OFFER_RUNDLG_FAILURE -1

// Values returned by OCLoadOpenCandyDLL() API
#define OC_LOADOCDLL_FAILURE 0

// Values used with LogDevModeMessage() API
#define OC_DEVMSG_ERROR_TRUE  1
#define OC_DEVMSG_ERROR_FALSE 0

// Values used with the OpenCandyInsert*Page functions
#define OC_PAGE_CREATE_FAIL -1

// Values used with SetUseDefaultColorBkgrnd() API
#define OC_USEDEFAULTCOLORBKGRND_FALSE 0
#define OC_USEDEFAULTCOLORBKGRND_TRUE 1

// Values used in the sample installer script
//
// IMPORTANT:
// Do not modify these definitions or disable the warnings below.
// If you see warnings when you compile your script you must
// modify the values you set where you !insertmacro OpenCandyInit
// (i.e. in your .iss file) before releasing your installer publicly.
#define OC_SAMPLE_PUBLISHERNAME "Open Candy Sample"
#define OC_SAMPLE_KEY "748ad6d80864338c9c03b664839d8161"
#define OC_SAMPLE_SECRET "dfb3a60d6bfdb55c50e1ef53249f1198"

// Compile-time checks and defaults
#if OC_STR_MY_PRODUCT_NAME == OC_SAMPLE_PUBLISHERNAME
	#pragma warning "Do not forget to change the product name from '" + OC_SAMPLE_PUBLISHERNAME + "' to your company's product name before releasing this installer."
#endif
#if OC_STR_KEY == OC_SAMPLE_KEY
	#pragma warning "Do not forget to change the sample key '" + OC_SAMPLE_KEY + "' to your company's product key before releasing this installer."
#endif
#if OC_STR_SECRET == OC_SAMPLE_SECRET
	#pragma warning "Do not forget to change the sample secret '" + OC_SAMPLE_SECRET + "' to your company's product secret before releasing this installer."
#endif
#if Pos(LowerCase("\OCSetupHlp.dll"),LowerCase(OC_OCSETUPHLP_FILE_PATH)) == 0
	#pragma error "The definition OC_OCSETUPHLP_FILE_PATH does not use ""OCSetupHlp.dll"" for the file part."
#endif

// OC_MAX_INIT_TIME is the maximum time in milliseconds that OCInit may block when fetching offers.
// If you intend to override this default do so by defining it in your .iss file before #include'ing this header.
// Be certain to make OpenCandy partner support aware of any override you apply because this can affect your metrics.
#ifndef OC_MAX_INIT_TIME
	#define OC_MAX_INIT_TIME 0
#endif

// OC_MAX_LOADING_TIME is the maximum time in milliseconds that the loading page may be displayed.
// Note that under normal network conditions the loading page may end sooner. Setting this value too low
// may reduce offer rate. Values of at least 5000 are recommended. If you intend to override this default do so by
// defining it in your .iss file before #include'ing this header. Be certain to make OpenCandy partner support aware
// of any override you apply because this can affect your metrics.
#ifndef OC_MAX_LOADING_TIME
	#define OC_MAX_LOADING_TIME 8000
#endif



//--------------------------------
// OpenCandy global variables
//--------------------------------

// IMPORTANT:
// Never modify or reference these directly, they are used
// completely internally to this helper script.

var

	gl_OC_objOCLoadAPIPage: TWizardPage;        // Handle to the LoadDLL placeholder page
	gl_OC_objOCConnectPage: TWizardPage;        // Handle to the Init2 placeholder page
	gl_OC_objLoadingPage: TWizardPage;          // Handle to the Loading placeholder page
	gl_OC_objOCOfferPage: TWizardPage;          // Handle to the offer page wizard page
	gl_OC_init_tszProductName: OCTString;       // Product name passed to Init procedure
	gl_OC_init_tszProductKey: OCTString;        // Product key passed to Init procedure
	gl_OC_init_tszProductSecret: OCTString;     // Product secret passed to Init procedure
	gl_OC_init_tszInstallerLanguage: OCTString; // Installer language passed to Init procedure
	gl_OC_init_bAsync: Boolean;                 // Async enabled? Implied by Init procedure name
	gl_OC_init_iInitMode: Integer;              // Init mode passed to Init procedure
	gl_OC_bAttached:Boolean;                    // Is the OpenCandy offer window attached?
	gl_OC_bHasBeenLoaded: Boolean;              // Has the OpenCandy client DLL been loaded?
	gl_OC_bHasBeenInitialized: Boolean;         // Has the OpenCandy client DLL been initialized?
	gl_OC_bNoCandy: Boolean;                    // Is OpenCandy disabled?
	gl_OC_bOfferIsEnabled: Boolean;             // Is the OpenCandy offer enabled?
	gl_OC_bUseOfferPage: Boolean;               // Should show an offer?
	gl_OC_bHasReachedOCPage: Boolean;           // Has the user reached the OpenCandy offer page?
	gl_OC_bProductInstallSuccess: Boolean;      // Has the publisher product install completed successfully?
	gl_OC_LoadingMaxWaitMilliSecs: Integer;     // Max wait time in milliseconds to show loading screen for
	gl_OC_szLoadingMsg: OCTString;              // Custom loading message
	gl_OC_szFontName: OCTString;                // Custom loading font name
	gl_OC_iFontSize: Integer;                   // Custom loading font size
	gl_OC_bIsShowingLoadingScreen: Boolean;     // Is loading screen currently showing?
	gl_OC_bHasShownLoadingScreen: Boolean;      // Have the loading screen already been shown?
	gl_OC_bCanSkipAfterLoadingScreen: Boolean;  // Can we skip to next page after loading screen was shown?
	gl_OC_bUseDefaultColorBkGround: Boolean;    // Use Windows COLOR_3DFACE system color for background of loading and offer screen?
	gl_OC_szCustomBrushColor: OCTString;        // If not empty string, custom color for background of loading and offer screen as '#RGB' where R,G,B are hex in range 00-FF.
	gl_OC_szCustomImagePath: OCTString;         // If not empty string, custom image to draw as the background for the loading and offer screens. e.g. 'c:\temp\bkground.png'


//-----------------------------------------
// OpenCandy external procedure definitions
//-----------------------------------------

procedure _OCDLL_OCStartDLMgr2Download();
external 'OCPRD1099OpenCandy29@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCLoadOpenCandyDLL():Integer;
external 'OCPRD1099OpenCandy1@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCInit(szPubId, szProdId, szSecret, szInstallLang:OCAString; bAsyncMode:Boolean; iMaxWait, iRemnant:Integer):Integer;
external 'OCPRD1099OpenCandy5@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

#ifdef UNICODE
function _OCDLL_OCInitW(wszPubId, wszProdId, wszSecret, wszInstallLang:OCWString; bAsyncMode:Boolean; iMaxWait, iRemnant:Integer):Integer;
external 'OCPRD1099OpenCandy6@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';
#endif

function _OCDLL_OCGetBannerInfo(szTitle, szDesc:OCAString):Integer;
external 'OCPRD1099OpenCandy7@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

#ifdef UNICODE
function _OCDLL_OCGetBannerInfoW(wszTitle, wszDesc:OCWString):Integer;
external 'OCPRD1099OpenCandy8@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';
#endif

function _OCDLL_OCRunDialog(iHwnd:Integer): Integer;
external 'OCPRD1099OpenCandy9@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCAdjustPage(iHwnd, iX, iY, iW, iH:Integer):Integer;
external 'OCPRD1099OpenCandy13@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCRestorePage(iHwnd:Integer):Integer;
external 'OCPRD1099OpenCandy14@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCGetOfferState():Integer;
external 'OCPRD1099OpenCandy10@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCGetOfferType():Integer;
external 'OCPRD1099OpenCandy17@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCPrepareDownload():Integer;
external 'OCPRD1099OpenCandy18@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCShutdown():Integer;
external 'OCPRD1099OpenCandy11@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCDetach():Integer;
external 'OCPRD1099OpenCandy12@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCSignalProductInstalled():Integer;
external 'OCPRD1099OpenCandy19@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCSignalProductFailed():Integer;
external 'OCPRD1099OpenCandy20@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCGetAsyncOfferStatus(bWantToShowOffer:Boolean):Integer;
external 'OCPRD1099OpenCandy31@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCCanLeaveOfferPage():Integer;
external 'OCPRD1099OpenCandy34@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCSetCmdLineValues(szValue:OCAString):Integer;
external 'OCPRD1099OpenCandy35@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

#ifdef UNICODE
function _OCDLL_OCSetCmdLineValuesW(wszValue:OCWString):Integer;
external 'OCPRD1099OpenCandy36@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';
#endif

function _OCDLL_OCGetNoCandy():Integer;
external 'OCPRD1099OpenCandy32@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function _OCDLL_OCSetNoCandy(bNoCandy:Boolean):Integer;
external 'OCPRD1099OpenCandy33@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

procedure _OCDLL_SetOCOfferEnabled(bEnabled:Boolean);
external 'OCPRD1099OpenCandy37@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

procedure _OCDLL_LogDevModeMessage(szMessage:OCAString; iError, iFaqID:Integer);
external 'OCPRD1099OpenCandy38@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

#ifdef UNICODE
procedure _OCDLL_LogDevModeMessageW(wszMessage:OCWString; iError, iFaqID:Integer);
external 'OCPRD1099OpenCandy39@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';
#endif

procedure _OCDLL_SetClientAdvancedOptions(szOptions:OCAString);
external 'OCPRD1099OpenCandy47@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

#ifdef UNICODE
procedure _OCDLL_SetClientAdvancedOptionsW(szOptions:OCWString);
external 'OCPRD1099OpenCandy48@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';
#endif

function _OCKernel32DLL_GetModuleHandle(szModuleName:OCAString):Integer;
external 'GetModuleHandleA@kernel32.dll stdcall';

#ifdef UNICODE
function _OCKernel32DLL_GetModuleHandleW(szModuleName:OCWString):Integer;
external 'GetModuleHandleW@kernel32.dll stdcall';
#endif

procedure _OCDLL_OCShowLoadingScreen(maxWaitSecs, iHwnd:Integer);
external 'OCPRD1099OpenCandy50@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

procedure _OCDLL_OCShowLoadingScreen2A(maxWaitSecs,iHwnd:Integer; wszLoadingMsg, wszFontName:OCAString; iFontSize:Integer);
external 'OCPRD1099OpenCandy51@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

#ifdef UNICODE
procedure _OCDLL_OCShowLoadingScreen2W(maxWaitSecs,iHwnd:Integer; wszLoadingMsg, wszFontName:OCWString; iFontSize:Integer);
external 'OCPRD1099OpenCandy52@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';
#endif

procedure _OCDLL_OCHideLoadingScreen();
external 'OCPRD1099OpenCandy53@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

procedure _OCDLL_OCSetUseDefaultColorBkgrnd(bUseDefault:Integer);
external 'OCPRD1099OpenCandy44@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

procedure _OCDLL_OCSetCustomBrushColorA(szColorValue:OCAString);
external 'OCPRD1099OpenCandy45@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

#ifdef UNICODE
procedure _OCDLL_OCSetCustomBrushColorW(wszColorValue:OCWString);
external 'OCPRD1099OpenCandy46@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';
#endif

procedure _OCDLL_OCSetBkGrdImagePathA(szImagePath:OCAString);
external 'OCPRD1099OpenCandy54@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

#ifdef UNICODE
procedure _OCDLL_OCSetBkGrdImagePathW(wszImagePath:OCWString);
external 'OCPRD1099OpenCandy55@{tmp}\OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';
#endif



//-------------------------------------------
// OpenCandy runtime functions and procedures
//-------------------------------------------

//
// _OpenCandyPrepareInnoAPI
// ------------------------
// This function is internal to this helper script. Do not
// call it from your own code.
//
// Prepares the OpenCandy API by initializing state for the Inno Setup
// interface layer.
//

procedure _OpenCandyPrepareInnoAPI();
begin
	// Initialize OpenCandy variables
	gl_OC_objOCConnectPage := nil;
	gl_OC_objLoadingPage := nil;
	gl_OC_objOCOfferPage := nil;
	gl_OC_init_tszProductName := '';
	gl_OC_init_tszProductKey := '';
	gl_OC_init_tszProductSecret := '';
	gl_OC_init_tszInstallerLanguage := '';
	gl_OC_init_bAsync := true;
	gl_OC_init_iInitMode := {#OC_INIT_MODE_NORMAL};
	gl_OC_bAttached := false;
	gl_OC_bHasBeenLoaded := false;
	gl_OC_bHasBeenInitialized := false;
	gl_OC_bNoCandy := false;
	gl_OC_bUseOfferPage := false;
	gl_OC_bHasReachedOCPage := false;
	gl_OC_bProductInstallSuccess := false;
	gl_OC_LoadingMaxWaitMilliSecs := {#OC_MAX_LOADING_TIME};
	gl_OC_szLoadingMsg := 'Loading...';
	gl_OC_szFontName := 'Arial';
	gl_OC_iFontSize := 100;	
	gl_OC_bIsShowingLoadingScreen := false;
	gl_OC_bHasShownLoadingScreen := false;
	gl_OC_bOfferIsEnabled := true;
	gl_OC_bCanSkipAfterLoadingScreen := false;
	gl_OC_bUseDefaultColorBkGround := true;
	gl_OC_szCustomBrushColor := '';
	gl_OC_szCustomImagePath := '';
end;



//
// _OCSetNoCandy
// -------------
// This function is internal to this helper script. Do not
// call it from your own code.
//

procedure _OCSetNoCandy();
begin
	if gl_OC_bHasBeenLoaded and not gl_OC_bNoCandy then
		_OCDLL_OCSetNoCandy(true);
	gl_OC_bNoCandy := true;
end;



//
// _OpenCandyAutoSelfDisable
// -------------------------
// This procedure is internal to this helper script. Do not
// call it from your own code.
//

procedure _OpenCandyAutoSelfDisable();
var
	i:Integer;
begin
	if not gl_OC_bNoCandy then
	begin
		// OpenCandy is disabled during a silent installation
		if WizardSilent() then
			_OCSetNoCandy();

		// OpenCandy may be explicitly disabled via command argument	
		for i := 0 to ParamCount() do
		begin
			if AnsiUppercase(Trim(ParamStr(i))) = '/NOCANDY' then
				_OCSetNoCandy();
		end;
	end;
end;



//
// _OpenCandyLoadDLL
// -----------------
// This function is internal to this helper script. Do not
// call it from your own code.
//
// Loads the OpenCandy Network Client DLL. This function must be called
// before any stateful operations. The result will be true on success.
//
// Usage:
//
//   _OpenCandyLoadDLL();
//

function _OpenCandyLoadDLL(): Boolean;
var
	iRes:Integer;
	hMod:Integer;
begin
	Result := gl_OC_bHasBeenLoaded;
	if not (gl_OC_bNoCandy or gl_OC_bHasBeenLoaded) then
	begin
		// Prevent loading multiple instances
		#ifdef UNICODE
		hMod := _OCKernel32DLL_GetModuleHandleW('OCSetupHlp.dll')
		#else
		hMod := _OCKernel32DLL_GetModuleHandle('OCSetupHlp.dll')
		#endif	
		if (0 <> hMod) or FileExists(ExpandConstant('{tmp}\OCSetupHlp.dll')) then
			Exit;

		// Extract and load OpenCandy Network Client library
		try
			ExtractTemporaryFile('OCSetupHlp.dll');
			iRes := _OCDLL_OCLoadOpenCandyDLL();
		except
			iRes := {#OC_LOADOCDLL_FAILURE};
		end;
		
		if {#OC_LOADOCDLL_FAILURE} <> iRes then
		begin
			gl_OC_bHasBeenLoaded := true;
			_OCDLL_SetOCOfferEnabled(gl_OC_bOfferIsEnabled);
			
			// Apply background color settings for loading screen and offer screen
			if not gl_OC_bUseDefaultColorBkGround then
				_OCDLL_OCSetUseDefaultColorBkgrnd({#OC_USEDEFAULTCOLORBKGRND_FALSE});
			if (gl_OC_szCustomBrushColor <> '') then
			begin
				#ifdef UNICODE
				_OCDLL_OCSetCustomBrushColorW(gl_OC_szCustomBrushColor);
				#else
				_OCDLL_OCSetCustomBrushColorA(gl_OC_szCustomBrushColor);
				#endif
			end;
			if (gl_OC_szCustomImagePath <> '') then
			begin
				#ifdef UNICODE
				_OCDLL_OCSetBkGrdImagePathW(gl_OC_szCustomImagePath);
				#else
				_OCDLL_OCSetBkGrdImagePathA(gl_OC_szCustomImagePath);
				#endif
			end;
		end
		else
			gl_OC_bNoCandy := true;
		Result := gl_OC_bHasBeenLoaded;
	end;
end;



//
// _OpenCandyDevModeMsg
// --------------------
// This function is internal to this helper script. Do not
// call it from your own code.
//
// Parameters:
//
//   tszMessage  : Message to display
//   bIsError    : True if the message represents an error
//   iFaqID      : ID of the FAQ associated with the message, or 0 if there is no FAQ associated
//
// Usage:
//
//   _OpenCandyDevModeMsg('This is an error with associated FAQ #500', true, 500);
//

procedure _OpenCandyDevModeMsg(tszMessage: OCTString; bIsError: Boolean; iFaqID: Integer);
var
	iError:Integer;
begin
	if gl_OC_bHasBeenLoaded and not gl_OC_bNoCandy then
	begin
		if bIsError then
		begin
			iError := {#OC_DEVMSG_ERROR_TRUE};
			tszMessage := '{\rtf1 {\colortbl;\red0\green0\blue0;\red255\green0\blue0;}\cf2Status ERROR! \cf1' + tszMessage + '\par}';
		end
		else
			iError := {#OC_DEVMSG_ERROR_FALSE};
			
		 #ifdef UNICODE
		 _OCDLL_LogDevModeMessageW(tszMessage, iError, iFaqID);
		 #else
		 _OCDLL_LogDevModeMessage(tszMessage, iError, iFaqID);
		 #endif
	end;
end;



//
// _OCEnabledAndReady
// ------------------
// This function is internal to this helper script. Do not
// call it from your own code.
//

function _OCEnabledAndReady(): Boolean;
begin
	Result := gl_OC_bHasBeenInitialized and not gl_OC_bNoCandy;
end;


			
//
// _OpenCandyInitInternal
// ----------------------
// This procedure is internal to this helper script. Do not
// call it from your own code. Instead see OpenCandyInit.
//

procedure _OpenCandyInitInternal();
var
	i:Integer;
begin
	if gl_OC_bHasBeenLoaded and not (gl_OC_bHasBeenInitialized or gl_OC_bNoCandy) then
	begin
		// Handle any OpenCandy command line options
		for i := 0 to ParamCount() do
		begin
			#ifdef UNICODE
			_OCDLL_OCSetCmdLineValuesW(ParamStr(i));
			#else
			_OCDLL_OCSetCmdLineValues(ParamStr(i));
			#endif
		end;
		
		// Check if command line options have disabled OpenCandy
		gl_OC_bNoCandy := {#OC_CANDY_DISABLED} = _OCDLL_OCGetNoCandy();

		if not gl_OC_bNoCandy then
		begin
			// Pass advanced options to client
			#ifdef OC_ADV_OPTIONS
				#ifdef UNICODE
				_OCDLL_SetClientAdvancedOptionsW('{#OC_ADV_OPTIONS}');
				#else
				_OCDLL_SetClientAdvancedOptions('{#OC_ADV_OPTIONS}');
				#endif
			#endif

			// Initialize OpenCandy client
			#ifdef UNICODE
			if {#OC_INIT_SUCCESS} = _OCDLL_OCInitW(gl_OC_init_tszProductName, gl_OC_init_tszProductKey, gl_OC_init_tszProductSecret,
			                                       gl_OC_init_tszInstallerLanguage, gl_OC_init_bAsync, {#OC_MAX_INIT_TIME}, gl_OC_init_iInitMode) then
			#else
			if {#OC_INIT_SUCCESS} = _OCDLL_OCInit(gl_OC_init_tszProductName, gl_OC_init_tszProductKey, gl_OC_init_tszProductSecret,
			                                      gl_OC_init_tszInstallerLanguage, gl_OC_init_bAsync, {#OC_MAX_INIT_TIME}, gl_OC_init_iInitMode) then
			#endif
				gl_OC_bHasBeenInitialized := true
			else
			begin
				gl_OC_bNoCandy := true;
				_OCDLL_OCSetNoCandy(true);
			end;
		end;
	end;
end;



// OpenCandyInit
// -------------
// Performs initialization of the OpenCandy DLL
// and checks for offers to present.  This procedure
// must be called before any other.
//
// Parameters:
//
//   tszProductName      : Your produce name
//   tszKey              : Your product key (will be provided by OpenCandy)
//   tszSecret           : Your product secret (will be provided by OpenCandy)
//   tszLanguage         : The installation language as an ISO 639-1 Alpha-2 Code
//   iInitMode           : The operating mode. Pass OC_INIT_MODE_NORMAL for normal operation
//                         or OC_INIT_MODE_REMNANT if OpenCandy should not show offers. Do not
//                         use iInitMode to handle /NOCANDY, this is done automatically for you.
//
// Usage (Using sample values for internal testing purposes only):
//
//   procedure InitializeWizard;
//   var
//     OCtszInstallerLanguage: OCTString;
//   begin
//     // Translate language from the value of the "Name"
//     // parameter assigned in the "[Languages]" section
//     // into ISO 639-1 Alpha-2 codes for the OpenCandy API
//     OCtszInstallerLanguage := ActiveLanguage();
//     if(OCtszInstallerLanguage = 'default') then
//       OCtszInstallerLanguage := 'en';
//     OpenCandyInit('{#OC_STR_MY_PRODUCT_NAME}', '{#OC_STR_KEY}', '{#OC_STR_SECRET}', OCtszInstallerLanguage, {#OC_INIT_MODE_NORMAL});
//   end;
//

procedure OpenCandyInit(tszProductName, tszKey, tszSecret, tszLanguage:OCTString; iInitMode: Integer);
begin
	_OpenCandyPrepareInnoAPI();
	gl_OC_init_tszProductName := tszProductName;
	gl_OC_init_tszProductKey := tszKey;
	gl_OC_init_tszProductSecret := tszSecret;
	gl_OC_init_tszInstallerLanguage := tszLanguage;
	gl_OC_init_bAsync := true;
	gl_OC_init_iInitMode := iInitMode;
	_OpenCandyAutoSelfDisable();
end;



// OpenCandyInsertLoadAPIPage
// --------------------------
// Inserts a placeholder page that is used to load the
// OpenCandy Network Client library. The end user must
// not be able to reach this placeholder page unless they have
// agreed to the OpenCandy End User License Agreement (EULA).
//
// The placeholder page should generally be inserted as soon
// after the EULA as possible in order to maximize the likelihood
// that offers will be ready by the time the end user reaches the
// offer screen.
//
// The function returns the ID of the new placeholder page that
// is inserted, or OC_PAGE_CREATE_FAIL if the page is not created
// successfully. The new page ID can be used to insert other custom
// pages after the placeholder page.
//
// Parameters:
//
//   iAfterPageID : Insert after this page ID
//
// Usage:
//
//   iOpenCandyNewPageID := OpenCandyInsertLoadDLLPage(wpLicense);
//

function OpenCandyInsertLoadDLLPage(iAfterPageID:Integer): Integer;
begin
	Result := {#OC_PAGE_CREATE_FAIL}
	if not (gl_OC_bNoCandy or (gl_OC_objOCLoadAPIPage <> nil)) then
		gl_OC_objOCLoadAPIPage := CreateCustomPage(iAfterPageID, ' ', ' ');
	if gl_OC_objOCLoadAPIPage <> nil then


		Result := gl_OC_objOCLoadAPIPage.ID;
end;



// OpenCandyInsertConnectPage
// --------------------------
// Inserts a placeholder page that is used to connect the OpenCandy
// Network Client to the OpenCandy Network and request available
// offers. This placeholder page must be inserted after
// the page inserted by OpenCandyInsertLoadDLLPage has been reached.
// The end user must not be able to reach this placeholder page until
// they have also reached the placeholder page inserted using
// OpenCandyInsertLoadDLLPage.
//
// The placeholder page should generally be inserted as soon
// after the EULA as possible in order to maximize the likelihood
// that offers will be ready by the time the end user reaches the
// offer screen.
//
// The function returns the ID of the new placeholder page that
// is inserted, or OC_PAGE_CREATE_FAIL if the page is not created
// successfully. The new page ID can be used to insert other custom
// pages after the placeholder page.
//
//
// Parameters:
//
//   iAfterPageID : Insert after this page ID
//
// Usage:
//
//   iOpenCandyNewPageID := OpenCandyInsertLoadDLLPage(wpLicense);
//   iOpenCandyNewPageID := OpenCandyInsertConnectPage(iOpenCandyNewPageID);
//

function OpenCandyInsertConnectPage(iAfterPageID:Integer): Integer;
begin
	Result := {#OC_PAGE_CREATE_FAIL}
	if not (gl_OC_bNoCandy or (gl_OC_objOCConnectPage <> nil)) then
		gl_OC_objOCConnectPage := CreateCustomPage(iAfterPageID, ' ', ' ');
	if gl_OC_objOCConnectPage <> nil then
		Result := gl_OC_objOCConnectPage.ID;
end;


//
// OpenCandyInsertLoadingPage
// --------------------------
// Inserts a placeholder page that is used to display a loading
// screen while the OpenCandy client is retrieving offers from
// the OpenCandy network. This placeholder page must be inserted after
// the page inserted by OpenCandyInsertConnectPage has been reached.
// The end user must not be able to reach this placeholder page until
// they have also reached the placeholder page inserted using
// OpenCandyInsertConnectPage.
//
// The placeholder page should generally be inserted immediately before
// the OpenCandy offer screen to minimize both the likelihood that it
// will need to be displayed, and to minimize the period for which it is
// displayed.
//
// The function returns the ID of the new placeholder page that
// is inserted, or OC_PAGE_CREATE_FAIL if the page is not created
// successfully. The new page ID can be used to insert other custom
// pages after the placeholder page.
//
//
// Parameters:
//
//   iAfterPageID  : Insert after this page ID
//   szCaption     : The page caption, or ' ' to display no caption
//   szDescription : The page description, or ' ' to display no description
//   szMessage     : The message displayed on the loading screen, or ' ' to display no message
//   szFontFace    : The font face used to display the message on the loading screen
//   iFontSize     : The font size used to display the message on the loading screen
//
// Usage:
//
//   iOpenCandyNewPageID := OpenCandyInsertConnectPage(wpLicense);
//   iOpenCandyNewPageID := OpenCandyInsertLoadingPage(wpSelectTasks, ' ', ' ', 'Loading...', 'Arial', 100);
//

function OpenCandyInsertLoadingPage(iAfterPageID:Integer; szCaption, szDescription, szMessage, szFontFace:OCTString; iFontSize:Integer): Integer;
begin
	Result := {#OC_PAGE_CREATE_FAIL}
	if not (gl_OC_bNoCandy or (gl_OC_objLoadingPage <> nil)) then
	begin
		gl_OC_objLoadingPage := CreateCustomPage(iAfterPageID, szCaption, szDescription);
		gl_OC_szLoadingMsg := szMessage;
		gl_OC_szFontName := szFontFace;
		gl_OC_iFontSize := iFontSize;	
	end;	
	if gl_OC_objLoadingPage <> nil then
		Result := gl_OC_objLoadingPage.ID;
end;



// OpenCandyInsertOfferPage
// ------------------------
// Inserts the OpenCandy offer page. The offer page is displayed if
// offers have become ready in the time since the placeholder page
// inserted using OpenCandyInsertConnectPage was reached and a valid
// offer was found for the specific end user system.
//
// The offer page should generally be inserted as late in the installation
// sequence as possible, just before installation begins. This helps to
// maximize the likelihood that offers will be ready.
//
// The function returns the ID of the new placeholder page that
// is inserted, or OC_PAGE_CREATE_FAIL if the page is not created
// successfully. The new page ID can be used to insert other custom
// pages after the offer page.
//
//
// Parameters:
//
//   iAfterPageID : Insert after this page ID
//
// Usage:
//
//   iOpenCandyNewPageID := OpenCandyInsertOfferPage(wpSelectTasks);
//

function OpenCandyInsertOfferPage(iAfterPageID:Integer): Integer;
begin
	Result := {#OC_PAGE_CREATE_FAIL}
	if not (gl_OC_bNoCandy or (gl_OC_objOCOfferPage <> nil)) then
		gl_OC_objOCOfferPage := CreateCustomPage(iAfterPageID, ' ', ' ');
	if gl_OC_objOCOfferPage <> nil then


		Result := gl_OC_objOCOfferPage.ID;
end;



//
// GetOCOfferStatus
// ----------------
// Allows you to determine if an offer is currently available. This is
// done automatically for you before the offer screen is shown. Typically
// you do not need to call this function from your own code directly.
//
// The offer status is placed on the stack and may be one of:
// {#OC_OFFER_STATUS_CANOFFER_READY}    - An OpenCandy offer is available and ready to be shown
// {#OC_OFFER_STATUS_CANOFFER_NOTREADY} - An offer is available but is not yet ready to be shown
// {#OC_OFFER_STATUS_QUERYING_NOTREADY} - The remote API is still being queried for offers
// {#OC_OFFER_STATUS_NOOFFERSAVAILABLE} - No offers are available
//
// When calling this function you must indicate whether the information returned
// will be used to decide whether the OpenCandy offer screen will be shown, e.g.
// if the information may result in a call to SetOCOfferEnabled. This helps
// to optimize future OpenCandy SDKs for better performance with your product.
//
// Usage:
//
//   // Test if OpenCandy is ready to show an offer.
//   // Indicate the result is informative only and does not directly
//   // determine whether offers from OpenCandy are enabled.
//   if {#OC_OFFER_STATUS_CANOFFER_READY} = GetOCOfferStatus(false) then
//

function GetOCOfferStatus(bDeterminesOfferEnabled: Boolean): Integer;
begin
	if _OCEnabledAndReady() then
		Result := _OCDLL_OCGetAsyncOfferStatus(bDeterminesOfferEnabled)
	else
		Result := {#OC_OFFER_STATUS_NOOFFERSAVAILABLE};
end;



//
// SetOCOfferEnabled
// -----------------
// Allows you to disable the OpenCandy offer screen easily from your
// installer code without affecting product installation analytics.
// This must be performed before an offer is shown only.
// Note that this is not the recommended method - you ought to determine
// during initialization whether OpenCandy should be disabled and specify
// an appropriate mode when calling OpenCandyInit or OpenCandyAsyncInit
// in that case. If you must use this method please be sure to inform the
// OpenCandy partner support team. Never directly place logical conditions
// around other OpenCandy functions and macros because this can have
// unforeseen consequences. You should call this procedure only after
// calling OpenCandyInit.
//
// Usage:
//
//  // This turns off offers from the OpenCandy network when called before the offer screen
//  SetOCOfferEnabled(false);
//

procedure SetOCOfferEnabled(bEnabled: Boolean);
begin
	if _OCEnabledAndReady() then
		_OCDLL_SetOCOfferEnabled(bEnabled);
	gl_OC_bOfferIsEnabled := bEnabled;
end;



//
// _OpenCandyUpdateCaptions
// ------------------------
// This function is internal to this helper script. Do not
// call it from your own code.
//
// Update the Title and Description text on the OpenCandy offer screen
// using strings from the server (preferred) or else fall-back strings.
// The result is true if strings from the server were ready.
//
// Parameters:
//
//   objTWizardPage       : The OpenCandy wizard page
//   szDefaultCaption     : The default page caption, if no banner strings from the server are ready
//   szDefaultDescription : The default page description, if no banner strings from the server are ready
//
// Usage:
//
//   _OpenCandyUpdateCaptions(gl_OC_objOCOfferPage, ' ', ' ');
//

function _OpenCandyUpdateCaptions(var objTWizardPage:TWizardPage; szDefaultCaption, szDefaultDescription:OCTString): Boolean;
var
	iBannerInfoResult:Integer;
	tszDesc: OCTString;
	tszTitle: OCTString;	
begin
	iBannerInfoResult := {#OC_OFFER_BANNER_FOUNDNEITHER};
	if _OCEnabledAndReady() then
	begin
		tszTitle := StringOfChar(' ',{#OC_STR_CHARS});
		tszDesc := StringOfChar(' ',{#OC_STR_CHARS});
		#ifdef UNICODE
		iBannerInfoResult := _OCDLL_OCGetBannerInfoW(tszTitle, tszDesc);
		#else
		iBannerInfoResult := _OCDLL_OCGetBannerInfo(tszTitle, tszDesc);
		#endif

		case iBannerInfoResult of
			{#OC_OFFER_BANNER_FOUNDTITLE}: tszDesc := ' ';
			{#OC_OFFER_BANNER_FOUNDDESCRIPTION}: tszTitle := ' ';
			{#OC_OFFER_BANNER_FOUNDNEITHER}:
			begin
				tszTitle := szDefaultCaption;
				tszDesc := szDefaultDescription;
			end;
		end;
		objTWizardPage.Caption := tszTitle;
		objTWizardPage.Description := tszDesc;
	end;
	Result := iBannerInfoResult <> {#OC_OFFER_BANNER_FOUNDNEITHER};
end;



//
// _OpenCandyShowLoadingScreen
// ---------------------------
// This function is internal to this helper script. Do not
// call it from your own code.
//
// Shows the loading screen while the OpenCandy client waits
// for the offer availability state to become final, or the maximum
// wait time to elapse. The user is prevented from clicking 'Next'
// while the loading screen is displayed.
//

procedure _OpenCandyShowLoadingScreen();
begin
	Wizardform.NextButton.Enabled := false;
	Wizardform.CancelButton.Enabled := false;
	gl_OC_bHasShownLoadingScreen  := true;
	gl_OC_bIsShowingLoadingScreen := true;
	gl_OC_bCanSkipAfterLoadingScreen := true;
	#ifdef UNICODE
	_OCDLL_OCShowLoadingScreen2W(gl_OC_LoadingMaxWaitMilliSecs, gl_OC_objLoadingPage.Surface.Handle, gl_OC_szLoadingMsg, gl_OC_szFontName, gl_OC_iFontSize);
	#else
	_OCDLL_OCShowLoadingScreen2A(gl_OC_LoadingMaxWaitMilliSecs, gl_OC_objLoadingPage.Surface.Handle, gl_OC_szLoadingMsg, gl_OC_szFontName, gl_OC_iFontSize);
	#endif
	gl_OC_bIsShowingLoadingScreen := false;
	Wizardform.NextButton.Enabled := true;
	Wizardform.CancelButton.Enabled := true;
end;	



//
// OpenCandyShouldSkipPage()
// -------------------------
//
// This function needs to be called from the ShouldSkipPage Inno script
// event function so that Inno Setup can determine whether the OpenCandy
// offer page should be displayed. The function returns true if the
// current page is the OpenCandy offer page and no offer is to be
// presented.
//
// Usage:
//
//   function ShouldSkipPage(PageID: Integer): Boolean;
//   begin
//     Result := false; // Don't skip pages by default
//
//     if OpenCandyShouldSkipPage(PageID) then
//       Result := true;
//   end;
//

function OpenCandyShouldSkipPage(iCurPageID: Integer) : Boolean;
begin
	Result := false;
	
	// Handle loading the OpenCandy Network Client DLL
	if gl_OC_objOCLoadAPIPage <> nil then
	begin
		if (iCurPageID = gl_OC_objOCLoadAPIPage.ID) then
		begin
			if not gl_OC_bHasBeenLoaded then
				gl_OC_bHasBeenLoaded := _OpenCandyLoadDLL();		
			Result := true;
		end;
	end;
	
	// Handle connecting to the OpenCandy Network
	if gl_OC_objOCConnectPage <> nil then
	begin
		if iCurPageID = gl_OC_objOCConnectPage.ID then
		begin
			if not gl_OC_bHasBeenInitialized then
				_OpenCandyInitInternal();
			Result := true;	
		end;
	end;
	
	// Handle reaching the OpenCandy loading page
	if gl_OC_objLoadingPage <> nil then
	begin
		if iCurPageID = gl_OC_objLoadingPage.ID then
			Result := (not _OCEnabledAndReady()) or gl_OC_bHasShownLoadingScreen;
	end;
	
	// Handle reaching the OpenCandy offer page
	if gl_OC_objOCOfferPage <> nil then
	begin
		if iCurPageID = gl_OC_objOCOfferPage.ID then
		begin
			if _OCEnabledAndReady() then
			begin
				if (not gl_OC_bUseOfferPage) and (not gl_OC_bHasReachedOCPage) then
					gl_OC_bUseOfferPage := {#OC_OFFER_STATUS_CANOFFER_READY} = _OCDLL_OCGetAsyncOfferStatus(true);
			end
			else
				gl_OC_bUseOfferPage := false;
			gl_OC_bHasReachedOCPage := true;
			Result := not gl_OC_bUseOfferPage;
		end;
	end;	
end;



//
// OpenCandyCurPageChanged
// -----------------------
// This function needs to be called from CurPageChanged() Inno script
// event function so that the OpenCandy offer page is displayed correctly.
//
// Usage:
//
//   procedure CurPageChanged(CurPageID: Integer);
//   begin
//     OpenCandyCurPageChanged(CurPageID);
//   end;
//

procedure OpenCandyCurPageChanged(CurPageID: Integer);
begin
	if not _OCEnabledAndReady() then
		Exit;
		
	if (gl_OC_objLoadingPage <> nil) then
	begin
		if (CurPageID = gl_OC_objLoadingPage.ID) and (not gl_OC_bHasShownLoadingScreen) then
		begin
			_OpenCandyShowLoadingScreen();
			if gl_OC_bCanSkipAfterLoadingScreen then		
				Wizardform.NextButton.OnClick(nil);
			Exit;		
		end
	end;

	if (gl_OC_objOCOfferPage <> nil) and gl_OC_bUseOfferPage then
	begin
		if (CurPageID <> gl_OC_objOCOfferPage.ID) and gl_OC_bAttached then
		begin
			_OCDLL_OCDetach();
			gl_OC_bAttached := false;
		end;
			
		if (CurPageID = gl_OC_objOCOfferPage.ID) and not gl_OC_bAttached then
		begin
			_OpenCandyUpdateCaptions(gl_OC_objOCOfferPage, ' ', ' ');
			_OCDLL_OCAdjustPage(gl_OC_objOCOfferPage.Surface.Handle,8,60,480,250);
			if {#OC_OFFER_RUNDLG_FAILURE} <> _OCDLL_OCRunDialog(gl_OC_objOCOfferPage.Surface.Handle) then
				gl_OC_bAttached := true
			else
				gl_OC_bUseOfferPage := false;		
		end;
	end;
end;



//
// OpenCandyNextButtonClick
// ------------------------
// This function needs to be called be called from the NextButtonClick()
// Inno script event function so that Inno Setup does not allow an end user
// to proceed past the OpenCandy offer screen in the event that the user
// must make a selection and hasn't yet done so. The function returns false
// if the user should not be allowed to proceed.
//
// Usage:
//
//   function NextButtonClick(CurPageID: Integer): Boolean;
//   begin
//     Result := true; // Allow action by default
//     if not OpenCandyNextButtonClick(CurPageID) then
//       Result := false;
//   end;
//

function OpenCandyNextButtonClick(CurPageID: Integer): Boolean;
begin
	Result := true;
	if _OCEnabledAndReady() and gl_OC_bUseOfferPage and (gl_OC_objOCOfferPage <> nil) then
	begin
		if (CurPageID = gl_OC_objOCOfferPage.ID) then
		begin
			// User must make a selection
			if {#OC_OFFER_LEAVEPAGE_DISALLOWED} = _OCDLL_OCCanLeaveOfferPage() then
				Result := false
			else
			begin
				_OCDLL_OCRestorePage(gl_OC_objOCOfferPage.Surface.Handle);
				Result := true;
			end;
		end;
	end;
end;



//
// OpenCandyBackButtonClick
// ------------------------
// This function should be called from BackButtonClick() Inno script
// event function. It restores the layout of the installer window after
// an OpenCandy offer page has been displayed.
//
// Usage:
//
//   function BackButtonClick(CurPageID: Integer): Boolean;
//   begin
//     Result := true; // Allow action by default
//     OpenCandyBackButtonClick(CurPageID);
//   end;
//

procedure OpenCandyBackButtonClick(CurPageID: Integer);
begin
	if _OCEnabledAndReady() and gl_OC_bUseOfferPage and (gl_OC_objOCOfferPage <> nil) then
	begin
		if CurPageID = gl_OC_objOCOfferPage.ID then
			_OCDLL_OCRestorePage(gl_OC_objOCOfferPage.Surface.Handle);
	end;
	
	if (gl_OC_objLoadingPage <> nil) then
	begin
		if CurPageID = gl_OC_objLoadingPage.ID then
		begin
			// Do not skip to next page after loading screen
			gl_OC_bCanSkipAfterLoadingScreen := false;
			
			// Unblock the loading screen
			if _OCEnabledAndReady() and gl_OC_bIsShowingLoadingScreen then
				_OCDLL_OCHideLoadingScreen();
		end;
	end;
end;



//
// _OpenCandyExecOfferInternal
// ---------------------------
// This procedure is internal to this helper script. Do not
// call it from your own code.
//

procedure _OpenCandyExecOfferInternal();
begin
	_OCDLL_OCPrepareDownload();
	if _OCDLL_OCGetOfferState() = {#OC_OFFER_CHOICE_ACCEPTED} then
		_OCDLL_OCStartDLMgr2Download();
end;



//
// OpenCandyCurStepChanged
// -----------------------
// This should be called from CurStepChanged() Inno script event function.
// It handles necessary operations at the various different stages of the setup,
// such as installing any offer the user may have accepted.
//
// Usage:
//
//   procedure CurStepChanged(CurStep: TSetupStep);
//   begin
//     OpenCandyCurStepChanged(CurStep);
//   end;
//

procedure OpenCandyCurStepChanged(CurStep: TSetupStep);
begin
	if _OCEnabledAndReady() then
	begin
		// ssInstall is just before the product installation starts
		if (CurStep = ssInstall) and gl_OC_bUseOfferPage then
			if {#OC_OFFER_TYPE_EMBEDDED} = _OCDLL_OCGetOfferType() then
				_OpenCandyExecOfferInternal();

		// ssDone is just before Setup terminates after a successful install
		if CurStep = ssDone then
		begin
			if gl_OC_bUseOfferPage and ({#OC_OFFER_TYPE_NORMAL} = _OCDLL_OCGetOfferType()) then
				_OpenCandyExecOfferInternal();
			gl_OC_bProductInstallSuccess := true;
			_OCDLL_OCSignalProductInstalled();
		end;
	end;
end;



//
// OpenCandyDeinitializeSetup
// --------------------------
// This should be called from DeinitializeSetup() Inno script event function.
// It signals product installation success or failure, and cleans up the
// OpenCandy library.
//
// Usage:
//   procedure DeinitializeSetup();
//   begin
//     OpenCandyDeinitializeSetup();
//   end;
//

procedure OpenCandyDeinitializeSetup();
begin
	if _OCEnabledAndReady() then
	begin
		if gl_OC_bIsShowingLoadingScreen then
			_OCDLL_OCHideLoadingScreen();

		if not gl_OC_bProductInstallSuccess then
			_OCDLL_OCSignalProductFailed();
		if gl_OC_bUseOfferPage and gl_OC_bAttached then
		begin
			_OCDLL_OCDetach();
			gl_OC_bAttached := false;
		end;
	end;
	if gl_OC_bHasBeenLoaded then
		_OCDLL_OCShutdown();
end;



//
// OpenCandySetUseDefaultColorBkgrnd
// ---------------------------------
// Calling this procedure after OpenCandyInit in the Inno Setup
// InitializeWizard callback procedure tells the client whether to
// draw the OpenCandy loading and offer screens on the solid
// Windows system color COLOR_3DFACE.
//
// Parameters:
//
//   bUseDefault : Use the default solid background color?
//
// Usage:
//
//   // Do not use default solid background color for OpenCandy loading and offer screens
//   OpenCandySetUseDefaultColorBkgrnd(false);
//

procedure OpenCandySetUseDefaultColorBkgrnd(bUseDefault: Boolean);
begin
	gl_OC_bUseDefaultColorBkGround := bUseDefault;
end;



//
// OpenCandySetCustomBrushColor
// ----------------------------
// Calling this procedure after OpenCandyInit in the Inno Setup
// InitializeWizard callback procedure tells the client to draw
// the OpenCandy loading and offer screens on the specified
// solid background color.
//
// Parameters:
//
//   szColor : The solid background color to draw on, in '#RGB' form, where R,G,B are hex values in range 00-FF.
//
// Usage:
//
//   // Draw on solid red background color
//   OpenCandySetCustomBrushColor('#FF0000');
//

procedure OpenCandySetCustomBrushColor(szColor: OCTString);
begin
	gl_OC_szCustomBrushColor := szColor;
end;



//
// OpenCandyCustomImagePath
// ------------------------
// Calling this procedure after OpenCandyInit in the Inno Setup
// InitializeWizard callback procedure tells the client to load a
// background image from the specified file, which should be a
// fully-qualified path, and composite the OpenCandy loading and
// offer screens upon it. The image dimensions should match the
// dimensions of the main installer window.
//
// Parameters:
//
//   szImagePath : A fully-qualified path to the background image.
//
// Usage:
//
//   OpenCandyCustomImagePath(ExpandConstant('{tmp}\MyInstallerBackground.png'));
//

procedure OpenCandyCustomImagePath(szImagePath: OCTString);
begin
	gl_OC_szCustomImagePath := szImagePath;
end;



//---------------------------------------------------------------------------//
//                    END of OpenCandy Helper Include file                   //
//---------------------------------------------------------------------------//