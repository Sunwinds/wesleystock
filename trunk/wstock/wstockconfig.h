#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <wx/config.h>
#include "wstockconst.h"

extern wxConfig config;
class WStockConfig :public wxObject
{
    public:
        static wxString GetKeyPath(){
            wxString keyPath = wxT("stocks.txt");
            config.Read(STOCK_KEY_PATH, &keyPath);
            return keyPath;
        };

        static wxString SetKeyPath(const wxString& keyPath){
            config.Write(STOCK_KEY_PATH, keyPath);
            return keyPath;
        };
        static wxString GetProxy(){
            wxString keyPath = wxT("");
            config.Read(wxT("General/Proxy"), &keyPath);
            return keyPath;
        };
        static wxString GetHistoryDataDir(){
            wxString DataDir = wxT("./data");
            config.Read(STOCK_HISTORY_DATA_DIR_KEY, &DataDir);
            return DataDir;
        };
        static wxString GetGmailUserName(){
            wxString DataDir = wxT("cnwesleywang@gmail.com");
            config.Read(wxT("Google/UserName"), &DataDir);
            return DataDir;
        };
        static wxString GetGmailPasswd(){
            wxString DataDir = wxT("");
            config.Read(wxT("Google/Passwd"), &DataDir);
            return DataDir;
        };
        static wxString GetGoogleMystockTitle(){
            wxString DataDir = wxT("mystocks");
            config.Read(wxT("Google/MyStockTitle"), &DataDir);
            return DataDir;
        };
        static wxString GetCurlProxy(){
            wxString DataDir = wxT("mystocks");
            config.Read(wxT("Google/MyStockTitle"), &DataDir);
            return DataDir;
        };
        static wxString GetCurlUserPwd(){
            wxString DataDir = wxT("mystocks");
            config.Read(wxT("Google/MyStockTitle"), &DataDir);
            return DataDir;
        };
        static wxString GetCurlAuth(){
            wxString DataDir = wxT("mystocks");
            config.Read(wxT("Google/MyStockTitle"), &DataDir);
            return DataDir;
        };
        static wxString GetCurlNoProxy(){
            wxString DataDir = wxT("mystocks");
            config.Read(wxT("Google/MyStockTitle"), &DataDir);
            return DataDir;
        };
        static void SetHistoryDataDir(const wxString& keyPath){
            config.Write(STOCK_HISTORY_DATA_DIR_KEY, keyPath);
        };
        static void SetGmailUserName(const wxString& keyPath){
            config.Write(wxT("Google/UserName"), keyPath);
        };
        static void SetGmailPasswd(const wxString& keyPath){
            config.Write(wxT("Google/Passwd"), keyPath);
        };
        static void SetGoogleMystockTitle(const wxString& keyPath){
            config.Write(wxT("Google/MyStockTitle"), keyPath);
        };
        static void SetCurlProxy(const wxString& keyPath){
            config.Write(wxT("Curl/Proxy"), keyPath);
        };
        static void SetCurlUserPwd(const wxString& keyPath){
            config.Write(wxT("Curl/UserPwd"), keyPath);
        };
        static void SetCurlAuth(int keyPath){
            config.Write(wxT("Curl/Auth"), keyPath);
        };
        static void SetCurlNoProxy(bool keyPath){
            config.Write(wxT("Curl/NoProxy"), keyPath);
        };
};

#endif // CONFIG_H_INCLUDED
