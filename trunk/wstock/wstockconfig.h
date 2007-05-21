#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <wx/config.h>
#include "wstockconst.h"

extern wxConfig config;
class WStockConfig :public wxObject
{
    public:
        static void Flush(){
            config.Flush();
        };
        static wxString GetKeyPath(){
            wxString keyPath = wxT("stocks.xml");
            config.Read(STOCK_KEY_PATH, &keyPath);
            return keyPath;
        };
        static wxString GetMyDayInfoPath(){
            wxString keyPath = wxT("myday.xml");
            config.Read(wxT("General/mydayinfo"), &keyPath);
            return keyPath;
        };
		static wxColour GetGoodColor(){
			return *wxRED;
		}
		static wxColour GetBadColor(){
			return *wxBLUE;
		}
        static wxString SetKeyPath(const wxString& keyPath){
            config.Write(STOCK_KEY_PATH, keyPath);
            return keyPath;
        };
        static wxString GetProxy(){
            wxString keyPath = wxT("");
            config.Read(wxT("General/Proxy"), &keyPath);
            return keyPath;
        };
        static int GetGlobalInfoX(){
            int x= -1;
            config.Read(wxT("General/GlobalX"), &x);
            return x;
        };
        static int GetGlobalInfoY(){
            int x= -1;
            config.Read(wxT("General/GlobalY"), &x);
            return x;
        };
        static void SetGlobalInfoX(int x){
            config.Write(wxT("General/GlobalX"), x);
        };
        static void SetGlobalInfoY(int v){
            config.Write(wxT("General/GlobalY"), v);
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
            wxString DataDir = wxT("");
            config.Read(wxT("Curl/Proxy"), &DataDir);
            return DataDir;
        };
        static wxString GetCurlUserPwd(){
            wxString DataDir = wxT("");
            config.Read(wxT("Curl/UserPwd"), &DataDir);
            return DataDir;
        };
        static int GetCurlAuth(){
            int DataDir = 0;
            config.Read(wxT("Curl/Auth"), &DataDir);
            return DataDir;
        };
        static int GetDataProvider(){
            int DataDir = 1;
            config.Read(wxT("General/DataProvider"), &DataDir);
            return DataDir;
        };
        static bool GetCurlNoProxy(){
            bool DataDir = true;
            config.Read(wxT("Curl/NoProxy"), &DataDir);
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
        static void SetDataProvider(int keyPath){
            config.Write(wxT("General/DataProvider"), keyPath);
        };
        static void SetCurlNoProxy(bool keyPath){
            config.Write(wxT("Curl/NoProxy"), keyPath);
        };
};

#endif // CONFIG_H_INCLUDED
