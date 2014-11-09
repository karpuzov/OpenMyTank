//-----------------------------------------------------------------------------

#include "resource.h"
#include "tstring.h"
#include "settings.h"
#include "hwnd_utils.h"
#include "file_utils.h"

#include <windows.h>
#include <Windowsx.h>
#include <Shlobj.h>
#include <Shlwapi.h>

#include <memory>

#undef max
#undef min
#include <limits>

#pragma comment(lib, "comctl32.lib")

//-----------------------------------------------------------------------------

extern HINSTANCE Instance;
extern HWND MainWindow;

void CreateServerButtons();  // main.cpp

//-----------------------------------------------------------------------------

const tstring Settings::DefaultScreenshotDateFormat(TEXT("dd'.'MM'.'yyyy' '"));
const tstring Settings::DefaultScreenshotTimeFormat(TEXT("HH'-'mm'-'ss'.'"));
const tstring Settings::DefaultScreenshotFormat(TEXT("jpeg"));

//-----------------------------------------------------------------------------

Settings* GetSettings()
{
    static std::auto_ptr<Settings> settings;
    if (settings.get() == NULL)
    {
        Settings* newSettings = new Settings();
        newSettings->load();
        settings.reset(newSettings);
    }
    return settings.get();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Settings CurrentSettings;

//-----------------------------------------------------------------------------
//    General settings:
//-----------------------------------------------------------------------------

void GeneralSettingsToControls(HWND dialog, const Settings& settings)
{
    SetEditNumber(::GetDlgItem(dialog, IDC_CLICKERS_DELAY_EDIT), settings.clickersDelay);

    INT languageCheckBox = IDC_ENGLISH;
    if (settings.language.compare("ru") == 0)
    {
        languageCheckBox = IDC_RUSSIAN;
    }
    else if (settings.language.compare("de") == 0)
    {
        languageCheckBox = IDC_GERMAN;
    }
    ::SendMessage(::GetDlgItem(dialog, languageCheckBox), BM_CLICK, 0, 0);
}

//-----------------------------------------------------------------------------

void ControlsToGeneralSettings(HWND dialog, Settings& settings)
{
    settings.clickersDelay = GetEditNumber(::GetDlgItem(dialog, IDC_CLICKERS_DELAY_EDIT));
    if (settings.clickersDelay < 1)
    {
        settings.clickersDelay = 1;
    }

    if (BST_CHECKED == Button_GetCheck(::GetDlgItem(dialog, IDC_RUSSIAN)))
    {
        settings.language = "ru";
    }
    else if (BST_CHECKED == Button_GetCheck(::GetDlgItem(dialog, IDC_GERMAN)))
    {
        settings.language = "de";
    }
    else // if (BST_CHECKED == Button_GetCheck(::GetDlgItem(dialog, IDC_ENGLISH)))
    {
        settings.language = "en";
    }
}

//-----------------------------------------------------------------------------

INT_PTR CALLBACK GeneralSettingsDialogProc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            MoveToCenterOfParentWindow(::GetParent(dialog));
            Edit_LimitText(::GetDlgItem(dialog, IDC_CLICKERS_DELAY_EDIT), 3);
            GeneralSettingsToControls(dialog, CurrentSettings);
            return TRUE;
        }
        case WM_NOTIFY:
        {
            if (((NMHDR*)lParam)->code == PSN_APPLY)
            {
                ControlsToGeneralSettings(dialog, CurrentSettings);
            }
            return TRUE;
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
//    Screenshot settings:
//-----------------------------------------------------------------------------

void ScreenshotSettingsToControls(HWND dialog, const Settings& settings)
{
    ::SetWindowText(::GetDlgItem(dialog, IDC_PATH_EDIT), settings.screenshotPath.c_str());

    INT dateFormatControlId = IDC_DD_MM_YYYY;
    if (settings.screenshotDateFormat.at(0) == TEXT('y'))
    {
        dateFormatControlId = IDC_YYYY_MM_DD;
    }
    ::SendMessage(::GetDlgItem(dialog, dateFormatControlId), BM_CLICK, 0, 0);

    INT formatControlId = IDC_JPEG;
    if (settings.screenshotFormat == tstring(TEXT("png")))
    {
        formatControlId = IDC_PNG;
    }
    else if (settings.screenshotFormat == tstring(TEXT("bmp")))
    {
        formatControlId = IDC_BMP;
    }
    ::SendMessage(::GetDlgItem(dialog, formatControlId), BM_CLICK, 0, 0);

    if (settings.beepOnScreenshot)
    {
        ::SendMessage(::GetDlgItem(dialog, IDC_BEEP_CHECKBOX), BM_CLICK, 0, 0);
    }

    SetEditNumber(::GetDlgItem(dialog, IDC_QUALITY_EDIT), settings.jpegQuality);
}

//-----------------------------------------------------------------------------

void ControlsToScreenshotSettings(HWND dialog, Settings& settings)
{
    settings.screenshotPath = GetWindowString(::GetDlgItem(dialog, IDC_PATH_EDIT));

    settings.screenshotDateFormat = Settings::DefaultScreenshotDateFormat;
    if (BST_CHECKED == Button_GetCheck(::GetDlgItem(dialog, IDC_YYYY_MM_DD)))
    {
        settings.screenshotDateFormat = TEXT("yyyy'-'MM'-'dd' '");
    }
    settings.screenshotTimeFormat = Settings::DefaultScreenshotTimeFormat;

    settings.screenshotFormat = Settings::DefaultScreenshotFormat;
    if (BST_CHECKED == Button_GetCheck(::GetDlgItem(dialog, IDC_PNG)))
    {
        settings.screenshotFormat = TEXT("png");
    }
    else if (BST_CHECKED == Button_GetCheck(::GetDlgItem(dialog, IDC_BMP)))
    {
        settings.screenshotFormat = TEXT("bmp");
    }

    settings.jpegQuality = GetEditNumber(::GetDlgItem(dialog, IDC_QUALITY_EDIT));
    if (settings.jpegQuality > 100)
    {
        settings.jpegQuality = 100;
    }

    settings.beepOnScreenshot = bool(
            BST_CHECKED == Button_GetCheck(::GetDlgItem(dialog, IDC_BEEP_CHECKBOX)));
}

//-----------------------------------------------------------------------------

INT_PTR CALLBACK ScreenshotSettingsDialogProc(
    HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            ScreenshotSettingsToControls(dialog, CurrentSettings);
            Edit_LimitText(::GetDlgItem(dialog, IDC_QUALITY_EDIT), 3);
            return TRUE;
        }
        case WM_NOTIFY:
        {
            if (((NMHDR*)lParam)->code == PSN_APPLY)
            {
                ControlsToScreenshotSettings(dialog, CurrentSettings);
            }
            return TRUE;
        }
        case WM_COMMAND:
        {
            if (LOWORD(wParam) == IDC_PATH_BUTTON)
            {
                const tstring newPath = GetFolder(GetString(IDS_SCREENSHOT_PATH).c_str(),
                        CurrentSettings.screenshotPath.c_str(), dialog);
                if (CurrentSettings.screenshotPath != newPath)
                {
                    CurrentSettings.screenshotPath = newPath;
                    ::SetWindowText(::GetDlgItem(dialog, IDC_PATH_EDIT), newPath.c_str());
                }
            }
            else if (LOWORD(wParam) == IDC_JPEG)
            {
                ::EnableWindow(::GetDlgItem(dialog, IDC_QUALITY_EDIT), TRUE);
            }
            else if (LOWORD(wParam) == IDC_PNG || LOWORD(wParam) == IDC_BMP)
            {
                ::EnableWindow(::GetDlgItem(dialog, IDC_QUALITY_EDIT), FALSE);
            }
            return TRUE;
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
//    Interface settings:
//-----------------------------------------------------------------------------

void InterfaceSettingsToControls(HWND dialog, const Settings& settings)
{
    INT snapshotControlId = IDC_KEY_PRTSC;
    if (settings.snapshotKey == VK_ESCAPE)
    {
        snapshotControlId = IDC_KEY_ESC;
    }
    else if (settings.snapshotKey == VK_OEM_3)
    {
        snapshotControlId = IDC_KEY_TILDE;
    }
    ::SendMessage(::GetDlgItem(dialog, snapshotControlId), BM_CLICK, 0, 0);

    SetEditNumber(::GetDlgItem(dialog, IDC_SERVERBUTTONS_EDIT), settings.serverButtonsCount);
}

//-----------------------------------------------------------------------------

void ControlsToInterfaceSettings(HWND dialog, Settings& settings)
{
    if (BST_CHECKED == Button_GetCheck(::GetDlgItem(dialog, IDC_KEY_ESC)))
    {
        settings.snapshotKey = VK_ESCAPE;
    }
    else if (BST_CHECKED == Button_GetCheck(::GetDlgItem(dialog, IDC_KEY_TILDE)))
    {
        settings.snapshotKey = VK_OEM_3;
    }
    else
    {
        settings.snapshotKey = VK_SNAPSHOT;
    }

    settings.serverButtonsCount = GetEditNumber(::GetDlgItem(dialog, IDC_SERVERBUTTONS_EDIT));
    if (settings.serverButtonsCount < 1)
    {
        settings.serverButtonsCount = 1;
    }
}

//-----------------------------------------------------------------------------

INT_PTR CALLBACK InterfaceSettingsDialogProc(
    HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            Edit_LimitText(::GetDlgItem(dialog, IDC_SERVERBUTTONS_EDIT), 2);
            InterfaceSettingsToControls(dialog, CurrentSettings);
            return TRUE;
        }
        case WM_NOTIFY:
        {
            if (((NMHDR*)lParam)->code == PSN_APPLY)
            {
                ControlsToInterfaceSettings(dialog, CurrentSettings);
            }
            return TRUE;
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
//    Chat settings:
//-----------------------------------------------------------------------------

void ChatSettingsToControls(HWND dialog, const Settings& settings)
{
    for (Settings::ChatMessagesVector::const_iterator it = settings.chatMessages.begin();
            it != settings.chatMessages.end(); ++it)
    {
        const Settings::ChatMessage& chatMessage = *it;
        const int key = chatMessage.key - VK_F1;
        if (chatMessage.team)
        {
            ::SendMessage(::GetDlgItem(dialog, IDC_CHAT_CHECK1 + key * 2), BM_CLICK, 0, 0);
        }
        ::SetWindowText(::GetDlgItem(dialog, IDC_CHAT_EDIT1 + key * 2), chatMessage.text.c_str());
    }
}

//-----------------------------------------------------------------------------

void ControlsToChatSettings(HWND dialog, Settings& settings)
{
    settings.chatMessages.clear();
    for (int key = 0; key < 10; ++key)
    {
        Settings::ChatMessage chatMessage;
        chatMessage.text = GetWindowString(::GetDlgItem(dialog, IDC_CHAT_EDIT1 + key * 2));
        if (!chatMessage.text.empty())
        {
            chatMessage.key = VK_F1 + key;
            chatMessage.team = bool(BST_CHECKED == Button_GetCheck(::GetDlgItem(dialog, IDC_CHAT_CHECK1 + key * 2)));
            settings.chatMessages.push_back(chatMessage);
        }
    }
}

//-----------------------------------------------------------------------------

INT_PTR CALLBACK ChatSettingsDialogProc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            for (int i = 0; i < 10; ++i)
            {
                Edit_LimitText(::GetDlgItem(dialog, IDC_CHAT_EDIT1 + i * 2),
                        Settings::ChatMessage::MaxTextSizeWithNull - 1);
            }
            ChatSettingsToControls(dialog, CurrentSettings);
            return TRUE;
        }
        case WM_NOTIFY:
        {
            if (((NMHDR*)lParam)->code == PSN_APPLY)
            {
                ControlsToChatSettings(dialog, CurrentSettings);
            }
            return TRUE;
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------

bool PropertySheetExecute(CONST HWND hwnd)
{
    PROPSHEETHEADER pshead = { 0 };

    pshead.dwSize = sizeof(PROPSHEETHEADER);
    pshead.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP;
    pshead.hwndParent = hwnd;
    pshead.hInstance = Instance;
    pshead.pszIcon = NULL;
    const tstring title(GetString(IDS_SETTINGS_TITLE));
    pshead.pszCaption = title.c_str();
    pshead.nStartPage = 0;

    pshead.nPages = 4;
    PROPSHEETPAGE pspage[4] = { 0 };

    pshead.ppsp = pspage;
    pspage[0].dwSize = sizeof(PROPSHEETPAGE);
    pspage[0].hInstance = Instance;

    pspage[0].pszTemplate = MAKEINTRESOURCE(ID_GENERAL_SETTINGS_PAGE);
    pspage[0].pfnDlgProc = (DLGPROC)GeneralSettingsDialogProc;

    pspage[1] = pspage[0];

    pspage[1].pszTemplate = MAKEINTRESOURCE(ID_SCREENSHOT_SETTINGS_PAGE);
    pspage[1].pfnDlgProc = (DLGPROC)ScreenshotSettingsDialogProc;

    pspage[2] = pspage[0];

    pspage[2].pszTemplate = MAKEINTRESOURCE(ID_CHAT_SETTINGS_PAGE);
    pspage[2].pfnDlgProc = (DLGPROC)ChatSettingsDialogProc;

    pspage[3] = pspage[0];

    pspage[3].pszTemplate = MAKEINTRESOURCE(ID_INTERFACE_SETTINGS_PAGE);
    pspage[3].pfnDlgProc = (DLGPROC)InterfaceSettingsDialogProc;

    return bool(0 < ::PropertySheet(&pshead));
}

//-----------------------------------------------------------------------------

bool ChangeSettingsByUser()
{
    const Settings oldSettings(*GetSettings());
    CurrentSettings = oldSettings;

    if (PropertySheetExecute(MainWindow))
    {
        if (CurrentSettings != oldSettings)
        {
            *GetSettings() = CurrentSettings;
            GetSettings()->save();
            if (CurrentSettings.serverButtonsCount != oldSettings.serverButtonsCount)
            {
                CreateServerButtons();
            }
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

std::string GetDefaultLanguage()
{
    static std::string language;
    if (language.empty())
    {
        const tstring tLanguage = GetString(IDS_LANGUAGE);
        language.assign(tLanguage.begin(), tLanguage.end());
    }
    return language;
}

//-----------------------------------------------------------------------------

tstring GetDefaultScreenshotPath()
{
    TCHAR path[MAX_PATH] = { TEXT('\0') };
    ::SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, path);
    ::PathAddBackslash(path);
    ::PathAppend(path, GetString(IDS_SCREENSHOT_DIR).c_str());
    return tstring(path);
}

//-----------------------------------------------------------------------------
