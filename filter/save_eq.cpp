#include "guids.h"
#include "save_eq.h"
#include "resource_ids.h"

void
SaveEq::on_create()
{
  CheckDlgButton(hwnd, IDC_RBT_EQ_CUR, (preset == AC3FILTER_EQ_CUR)? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hwnd, IDC_RBT_EQ_MCH, (preset == AC3FILTER_EQ_MCH)? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hwnd, IDC_RBT_EQ_ALL, (preset == AC3FILTER_EQ_ALL)? BST_CHECKED: BST_UNCHECKED);
}

BOOL
SaveEq::on_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_COMMAND)
  {
    int control = LOWORD(wParam);
    int message = HIWORD(wParam);

    switch (control)
    {
      case IDC_RBT_EQ_CUR:
      case IDC_RBT_EQ_MCH:
      case IDC_RBT_EQ_ALL:
        if (IsDlgButtonChecked(hwnd, IDC_RBT_EQ_CUR) == BST_CHECKED)
          preset = AC3FILTER_EQ_CUR;
        if (IsDlgButtonChecked(hwnd, IDC_RBT_EQ_MCH) == BST_CHECKED)
          preset = AC3FILTER_EQ_MCH;
        if (IsDlgButtonChecked(hwnd, IDC_RBT_EQ_ALL) == BST_CHECKED)
          preset = AC3FILTER_EQ_ALL;
        return TRUE;
    }
  }

  return FALSE;
}
