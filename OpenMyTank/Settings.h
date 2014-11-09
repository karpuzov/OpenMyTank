//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include "tstring.h"
#include "Resource.h"

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

    unsigned clickersDelay;

    std::string language;

    UINT32 windowState;
    RECT mainWindowRect;

    tstring screenshotPath;  //  without '\' at end
    tstring screenshotDateFormat;
    tstring screenshotTimeFormat;
    tstring screenshotFormat;
    unsigned jpegQuality;
    bool beepOnScreenshot;

    struct ChatMessage
    {
        enum
        {
            MaxTextSizeWithNull = 299 + 1
        };
        UINT key;
        tstring text;
        bool team;
        ChatMessage()
        {
        }
        ChatMessage(const UINT key, const tstring& text, const bool team)
            : key(key), text(text), team(team)
        {
        }
        bool operator==(const ChatMessage& rhs) const
        {
            return bool(key == rhs.key && text == rhs.text && team == rhs.team);
        }

    };
    typedef std::vector<ChatMessage> ChatMessagesVector;
    ChatMessagesVector chatMessages;

    unsigned snapshotKey;

    unsigned serverButtonsCount;

    static const tstring DefaultScreenshotDateFormat;
    static const tstring DefaultScreenshotTimeFormat;
    static const tstring DefaultScreenshotFormat;

    static const unsigned DefaultJpegQuality = 90;
    static const bool DefaultBeepOnScreenshot = true;

    static const unsigned DefaultSnapshotKey = VK_SNAPSHOT;

    static const unsigned DefaultServerButtonsCount = 30;

    tstring account; // slot, prefix in url

    Settings()
        : clickersDelay(10), language(GetDefaultLanguage()), windowState(0)
        , screenshotPath(GetDefaultScreenshotPath())
        , screenshotDateFormat(DefaultScreenshotDateFormat)
        , screenshotTimeFormat(DefaultScreenshotTimeFormat)
        , screenshotFormat(DefaultScreenshotFormat), jpegQuality(DefaultJpegQuality)
        , beepOnScreenshot(DefaultBeepOnScreenshot), snapshotKey(DefaultSnapshotKey)
        , serverButtonsCount(DefaultServerButtonsCount)
    {
        mainWindowRect.left = mainWindowRect.right = 0;
        mainWindowRect.top = mainWindowRect.bottom = 0;

        chatMessages.push_back(ChatMessage(VK_F1, GetString(IDS_F1_MESSAGE), true));
        chatMessages.push_back(ChatMessage(VK_F2, GetString(IDS_F2_MESSAGE), true));
        chatMessages.push_back(ChatMessage(VK_F3, GetString(IDS_F3_MESSAGE), true));
        chatMessages.push_back(ChatMessage(VK_F4, GetString(IDS_F4_MESSAGE), true));
        chatMessages.push_back(ChatMessage(VK_F5, GetString(IDS_F5_MESSAGE), true));
        chatMessages.push_back(ChatMessage(VK_F6, GetString(IDS_F6_MESSAGE), true));
        chatMessages.push_back(ChatMessage(VK_F7, GetString(IDS_F7_MESSAGE), true));
        chatMessages.push_back(ChatMessage(VK_F8, GetString(IDS_F8_MESSAGE), true));
        chatMessages.push_back(ChatMessage(VK_F9, GetString(IDS_F9_MESSAGE), true));
        chatMessages.push_back(ChatMessage(VK_F10, GetString(IDS_F10_MESSAGE), false));

        {
            std::ifstream file(AccountsFileMame);
            readString(file, account);
        }
        if (account.empty())
        {
            std::srand(unsigned(time(NULL)));
            for (int i = 6 + std::rand() % 5; i >= 0; --i)
            {
                switch (rand() % 3)
                {
                    case 0:
                        account += (TEXT('0') + rand() % (TEXT('9') - TEXT('0') + 1));
                        break;
                    case 1:
                        account += (TEXT('A') + rand() % (TEXT('Z') - TEXT('A') + 1));
                        break;
                    case 2:
                        account += (TEXT('a') + rand() % (TEXT('z') - TEXT('a') + 1));
                        break;
                }
            }
            std::ofstream file(AccountsFileMame, std::fstream::trunc);
            file << TCharToOem(account) << std::endl;
        }
    }

    bool operator!=(const Settings& other) const
    {
        return bool(clickersDelay != other.clickersDelay || language != other.language
                    // WindowState and MainWindowRect are not compared
                    || screenshotPath != other.screenshotPath
                    || screenshotDateFormat != other.screenshotDateFormat
                    || screenshotTimeFormat != other.screenshotTimeFormat
                    || screenshotFormat != other.screenshotFormat
                    || jpegQuality != other.jpegQuality
                    || beepOnScreenshot != other.beepOnScreenshot
                    || chatMessages != other.chatMessages || snapshotKey != other.snapshotKey
                    || serverButtonsCount != other.serverButtonsCount);
    }

    void resetToDefault()
    {
    }

    void load()
    {
        std::ifstream file(SettingsFileMame);
        bool broken = true;
        if (file)
        {
            std::time_t lastUpdateCheck; // deprecated
            std::time_t updateChecksPeriod; // deprecated
            file >> lastUpdateCheck;
            file >> updateChecksPeriod;
            file >> clickersDelay;
            file >> language;
            file >> windowState;
            file >> mainWindowRect.left >> mainWindowRect.top >> mainWindowRect.right
                 >> mainWindowRect.bottom;
            readString(file, screenshotPath);
            readString(file, screenshotDateFormat);
            readString(file, screenshotTimeFormat);
            readString(file, screenshotFormat);
            file >> jpegQuality;
            file >> beepOnScreenshot;
            unsigned availableButtons; // deprecated
            file >> availableButtons;

            int messagesCount;
            file >> messagesCount;
            if (!file.fail()) // from previous version don't clear new default messages
            {
                chatMessages.clear();
                for (int i = 0; i < messagesCount; ++i)
                {
                    ChatMessage message;
                    file >> message.key;
                    readString(file, message.text);
                    file >> message.team;
                    chatMessages.push_back(message);
                }
            }

            file >> snapshotKey;
            file >> serverButtonsCount;

            broken = file.fail();
            file.close();
        }
        if (broken)
        {
            save();
        }
    }

    void save()
    {
        std::ofstream file(SettingsFileMame, std::fstream::trunc);
        std::time_t lastUpdateCheck(0); // deprecated
        std::time_t updateChecksPeriod(0); // deprecated
        file << lastUpdateCheck << std::endl;
        file << updateChecksPeriod << std::endl;
        file << clickersDelay << std::endl;
        file << language << std::endl;
        file << windowState << std::endl;
        file << mainWindowRect.left << std::endl << mainWindowRect.top << std::endl
             << mainWindowRect.right << std::endl << mainWindowRect.bottom << std::endl;
        file << TCharToOem(screenshotPath) << std::endl;
        file << TCharToOem(screenshotDateFormat) << std::endl;
        file << TCharToOem(screenshotTimeFormat) << std::endl;
        file << TCharToOem(screenshotFormat) << std::endl;
        file << jpegQuality << std::endl;
        file << beepOnScreenshot << std::endl;
        unsigned availableButtons(0); // deprecated
        file << availableButtons << std::endl;

        file << chatMessages.size() << std::endl;
        for (ChatMessagesVector::const_iterator it = chatMessages.begin(); it != chatMessages.end();
                ++it)
        {
            file << it->key << std::endl;
            file << TCharToOem(it->text) << std::endl;
            file << it->team << std::endl;
        }

        file << snapshotKey << std::endl;
        file << serverButtonsCount << std::endl;
    }

    void saveCurrentWindowRect(const HWND window)
    {
        RECT rect;
        if (::GetWindowRect(window, &rect))
        {
            mainWindowRect = rect;
        }
    }

private:

    void skipAllReturs(std::ifstream& file) const
    {
        while (file.peek() == '\r' || file.peek() == '\n')
        {
            file.get();
        }
    }

    bool readString(std::ifstream& file, tstring& value) const
    {
        std::string line;
        skipAllReturs(file);
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
