
XPStyle "on"

;--------------------------------

!define REG_UNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenMyTank"
!define REG_INSTALL_DIR "Software\OpenMyTank"
!define VERSION "1.0"

;--------------------------------

Name "OpenMyTank ${VERSION}"
Caption "OpenMyTank ${VERSION} Setup"

OutFile "OpenMyTankSetup.exe"

;--------------------------------

; The default installation directory
;InstallDir "$PROGRAMFILES\OpenMyTank"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "${REG_INSTALL_DIR}" "InstallDir"

;--------------------------------
; Functions
;--------------------------------


Function .onInit

  ReadRegStr $0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion" 
  StrCpy $1 $0 1 0
  IntCmpU $1 6 Vista Xp Vista
  Xp:
    ExpandEnvStrings $INSTDIR "$PROGRAMFILES\OpenMyTank"
    goto WinVerEnd
  Vista:
    ExpandEnvStrings $INSTDIR "$LOCALAPPDATA\OpenMyTank"
  WinVerEnd:

FunctionEnd


;--------------------------------


Function .onInstSuccess

  HideWindow
  MessageBox MB_YESNO|MB_ICONQUESTION "Do you want run OpenMyTank now?" IDNO +2 
    Exec "$INSTDIR\OpenMyTank.exe"

FunctionEnd


;--------------------------------
; Pages
;--------------------------------

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles


;--------------------------------
; Sections
;--------------------------------

section "OpenMyTank ${VERSION} (required)"

  ; Section cannot be unchecked
  SectionIn RO

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Put files there
  File OpenMyTank.exe
;  File setup.exe
  File Uninstaller.exe

  ; Write the installation path into the registry
  WriteRegStr HKLM "${REG_INSTALL_DIR}" "InstallDir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "${REG_UNINSTALL}" "DisplayName" "OpenMyTank"
  WriteRegStr HKLM "${REG_UNINSTALL}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "${REG_UNINSTALL}" "Publisher" "Karpuzov Valery"
  WriteRegStr HKLM "${REG_UNINSTALL}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "${REG_UNINSTALL}" "NoModify" 1
  WriteRegDWORD HKLM "${REG_UNINSTALL}" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"

sectionEnd

;--------------------------------

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\OpenMyTank"
  CreateShortCut "$SMPROGRAMS\OpenMyTank\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\OpenMyTank\OpenMyTank.lnk" "$INSTDIR\OpenMyTank.exe" "" "$INSTDIR\OpenMyTank.exe" 0
  
SectionEnd

;--------------------------------

; Optional section (can be disabled by the user)
Section "Desktop Shortcut"

  CreateShortCut "$DESKTOP\OpenMyTank.lnk" "$INSTDIR\OpenMyTank.exe" "" "$INSTDIR\OpenMyTank.exe" 0
  
SectionEnd


;--------------------------------
; Uninstaller
;--------------------------------


section "Uninstall"

  ; Start internal uninstaller
  ExecWait '"$INSTDIR\Uninstaller.exe"'

  ; Remove registry keys
  DeleteRegKey HKLM "${REG_UNINSTALL}"
  DeleteRegKey HKLM "${REG_INSTALL_DIR}"

  ; Remove files and uninstaller
  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\*.*"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\OpenMyTank\*.*"
  Delete "$DESKTOP\OpenMyTank.lnk"

  ; Remove directories used
  RMDir "$SMPROGRAMS\OpenMyTank"
  RMDir "$INSTDIR"

sectionEnd


;--------------------------------
