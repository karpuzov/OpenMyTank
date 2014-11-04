
//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include "tstring.h"
#include "resource.h"

#include <windows.h>

#include <vector>
#include <fstream>
#include <ctime>

//-----------------------------------------------------------------------------

std::string GetDefaultLanguage();

tstring GetDefaultScreenshotPath();

static const std::string SettingsFileMame = "openmytank.cfg";
static const std::string AccountsFileMame = "accounts.cfg";

//-----------------------------------------------------------------------------

struct Settings
{
public:

  unsigned  ClickersDelay;

  std::string  Language;

  UINT32 WindowState;
  RECT MainWindowRect;

  tstring ScreenshotPath;  //  without '\' at end
  tstring ScreenshotDateFormat;
  tstring ScreenshotTimeFormat;
  tstring ScreenshotFormat;
  unsigned JpegQuality;
  bool BeepOnScreenshot;

  struct ChatMessage
  {
    enum { MaxTextSizeWithNull = 299 + 1 };
    UINT Key;
    tstring Text;
    bool Team;
    ChatMessage()
    {
    }
    ChatMessage(const UINT key, const tstring& text, const bool team)
      : Key(key), Text(text), Team(team)
    {
    }
    bool operator==(const ChatMessage& rhs) const
    {
      return bool(Key == rhs.Key && Text == rhs.Text && Team == rhs.Team);
    }

  };
  typedef  std::vector<ChatMessage>  ChatMessagesVector;
  ChatMessagesVector  ChatMessages;

  unsigned SnapshotKey;

  unsigned ServerButtonsCount;

  static const tstring DefaultScreenshotDateFormat;
  static const tstring DefaultScreenshotTimeFormat;
  static const tstring DefaultScreenshotFormat;

  static const unsigned DefaultJpegQuality = 90;
  static const bool DefaultBeepOnScreenshot = true;

  static const unsigned DefaultSnapshotKey = VK_SNAPSHOT;

  static const unsigned DefaultServerButtonsCount = 30;

  tstring Account; // slot, prefix in url

  Settings()
    : ClickersDelay(10)
    , Language(GetDefaultLanguage())
    , WindowState(0)
    , ScreenshotPath(GetDefaultScreenshotPath())
    , ScreenshotDateFormat(DefaultScreenshotDateFormat)
    , ScreenshotTimeFormat(DefaultScreenshotTimeFormat)
    , ScreenshotFormat(DefaultScreenshotFormat)
    , JpegQuality(DefaultJpegQuality)
    , BeepOnScreenshot(DefaultBeepOnScreenshot)
    , SnapshotKey(DefaultSnapshotKey)
    , ServerButtonsCount(DefaultServerButtonsCount)
  {
    MainWindowRect.left = MainWindowRect.right = 0;
    MainWindowRect.top = MainWindowRect.bottom = 0;

    ChatMessages.push_back(ChatMessage(VK_F1, GetString(IDS_F1_MESSAGE), true));
    ChatMessages.push_back(ChatMessage(VK_F2, GetString(IDS_F2_MESSAGE), true));
    ChatMessages.push_back(ChatMessage(VK_F3, GetString(IDS_F3_MESSAGE), true));
    ChatMessages.push_back(ChatMessage(VK_F4, GetString(IDS_F4_MESSAGE), true));
    ChatMessages.push_back(ChatMessage(VK_F5, GetString(IDS_F5_MESSAGE), true));
    ChatMessages.push_back(ChatMessage(VK_F6, GetString(IDS_F6_MESSAGE), true));
    ChatMessages.push_back(ChatMessage(VK_F7, GetString(IDS_F7_MESSAGE), true));
    ChatMessages.push_back(ChatMessage(VK_F8, GetString(IDS_F8_MESSAGE), true));
    ChatMessages.push_back(ChatMessage(VK_F9, GetString(IDS_F9_MESSAGE), true));
    ChatMessages.push_back(ChatMessage(VK_F10, GetString(IDS_F10_MESSAGE), false));

    {
      std::ifstream file(AccountsFileMame);
      ReadString(file, Account);
    }
    if (Account.empty())
    {
      std::srand(unsigned(time(NULL)));
      for (int i = 6 + std::rand() % 5; i >= 0; --i)
      {
        switch (rand() % 3)
        {
          case 0: Account += (TEXT('0') + rand() % (TEXT('9') - TEXT('0') + 1)); break;
          case 1: Account += (TEXT('A') + rand() % (TEXT('Z') - TEXT('A') + 1)); break;
          case 2: Account += (TEXT('a') + rand() % (TEXT('z') - TEXT('a') + 1)); break;
        }
      }
      std::ofstream file(AccountsFileMame, std::fstream::trunc);
      file << TCharToOem(Account) << std::endl;
    }
  }

  bool operator!=(const Settings& other) const
  {
    return bool(ClickersDelay != other.ClickersDelay
                || Language != other.Language
                // WindowState and MainWindowRect are not compared
                || ScreenshotPath != other.ScreenshotPath
                || ScreenshotDateFormat != other.ScreenshotDateFormat
                || ScreenshotTimeFormat != other.ScreenshotTimeFormat
                || ScreenshotFormat != other.ScreenshotFormat
                || JpegQuality != other.JpegQuality
                || BeepOnScreenshot != other.BeepOnScreenshot
                || ChatMessages != other.ChatMessages
                || SnapshotKey != other.SnapshotKey
                || ServerButtonsCount != other.ServerButtonsCount);
  }

  void ResetToDefault()
  {
  }

  void Load()
  {
    std::ifstream file(SettingsFileMame);
    bool broken = true;
    if (file)
    {
      std::time_t lastUpdateCheck; // deprecated
      std::time_t updateChecksPeriod; // deprecated
      file >> lastUpdateCheck;
      file >> updateChecksPeriod;
      file >> ClickersDelay;
      file >> Language;
      file >> WindowState;
      file >> MainWindowRect.left
           >> MainWindowRect.top
           >> MainWindowRect.right
           >> MainWindowRect.bottom;
      ReadString(file, ScreenshotPath);
      ReadString(file, ScreenshotDateFormat);
      ReadString(file, ScreenshotTimeFormat);
      ReadString(file, ScreenshotFormat);
      file >> JpegQuality;
      file >> BeepOnScreenshot;
      unsigned availableButtons; // deprecated
      file >> availableButtons;

      int messagesCount;
      file >> messagesCount;
      if (!file.fail()) // from previous version don't clear new default messages
      {
        ChatMessages.clear();
        for (int i = 0; i < messagesCount; ++i)
        {
          ChatMessage message;
          file >> message.Key;
          ReadString(file, message.Text);
          file >> message.Team;
          ChatMessages.push_back(message);
        }
      }

      file >> SnapshotKey;
      file >> ServerButtonsCount;

      broken = file.fail();
      file.close();
    }
    if (broken)
    {
      Save();
    }
  }

  void Save()
  {
    std::ofstream file(SettingsFileMame, std::fstream::trunc);
    std::time_t lastUpdateCheck(0); // deprecated
    std::time_t updateChecksPeriod(0); // deprecated
    file << lastUpdateCheck << std::endl;
    file << updateChecksPeriod << std::endl;
    file << ClickersDelay << std::endl;
    file << Language << std::endl;
    file << WindowState << std::endl;
    file << MainWindowRect.left   << std::endl
         << MainWindowRect.top    << std::endl
         << MainWindowRect.right  << std::endl
         << MainWindowRect.bottom << std::endl;
    file << TCharToOem(ScreenshotPath) << std::endl;
    file << TCharToOem(ScreenshotDateFormat) << std::endl;
    file << TCharToOem(ScreenshotTimeFormat) << std::endl;
    file << TCharToOem(ScreenshotFormat) << std::endl;
    file << JpegQuality << std::endl;
    file << BeepOnScreenshot << std::endl;
    unsigned availableButtons(0); // deprecated
    file << availableButtons << std::endl;

    file << ChatMessages.size() << std::endl;
    for (ChatMessagesVector::const_iterator it = ChatMessages.begin();
         it != ChatMessages.end();
         ++it)
    {
      file << it->Key << std::endl;
      file << TCharToOem(it->Text) << std::endl;
      file << it->Team << std::endl;
    }

    file << SnapshotKey << std::endl;
    file << ServerButtonsCount << std::endl;
  }

  void SaveCurrentWindowRect(const HWND window)
  {
    RECT rect;
    if (::GetWindowRect(window, &rect))
    {
      MainWindowRect = rect;
    }
  }

private:

  void SkipAllReturs(std::ifstream& file) const
  {
    while (file.peek() == '\r' || file.peek() == '\n')
    {
      file.get();
    }
  }

  bool ReadString(std::ifstream& file, tstring& value) const
  {
    std::string line;
    SkipAllReturs(file);
    std::getline(file, line);
    if (!file.fail())
    {
      value = OemToTChar(line);
      return true;
    }
    return false;
  }

};

//-----------------------------------------------------------------------------

Settings* GetSettings();

bool ChangeSettingsByUser();

//-----------------------------------------------------------------------------
