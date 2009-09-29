#include <stdio.h>
#include <windows.h>
#include "sink\sink_dsound.h"
#include "resource.h"

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_INITDIALOG:
    SetDlgItemText(hwnd, IDC_EDT_TEST, "Hit 'Test' button to start test");
    return TRUE;

  case WM_COMMAND:
    switch (LOWORD(wParam))
    {
      case IDOK:
        EndDialog(hwnd, wParam); 
        return TRUE; 

      case IDC_BTN_TEST:
      {
        DSoundSink ds;
        ds.open_dsound(hwnd);
        bool test48 = ds.open(Speakers(FORMAT_SPDIF, MODE_STEREO, 48000));
        bool test44 = ds.open(Speakers(FORMAT_SPDIF, MODE_STEREO, 44100));
        bool test32 = ds.open(Speakers(FORMAT_SPDIF, MODE_STEREO, 32000));

        char buf[1024];
        sprintf(buf, "DirectSound test:\r\n"
                "  48000Hz support: %s\r\n"
                "  44100Hz support: %s\r\n"
                "  32000Hz support: %s\r\n",
                test48? "YES": "no",
                test44? "YES": "no",
                test32? "YES": "no"
                );
        SetDlgItemText(hwnd, IDC_EDT_TEST, buf);
        return TRUE;
      }
    }
  }

  return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  DialogBox(hInstance, MAKEINTRESOURCE(IDD_TEST), 0, DialogProc);
  return 0;
}
