#include <wx/url.h>
#include <wx/sstream.h>
#include "wstockgeturl.h"
#include "wstockconfig.h"
#include "wx/wfstream.h"
#include "wx/datstrm.h"

const wxEventType wxEVT_URL_GET_DONE = wxNewEventType();
IMPLEMENT_DYNAMIC_CLASS(wxUrlGetDoneEvent, wxNotifyEvent)

/*  //这个版本使用wxUrl来从远程获取数据，确实可以用不过竟然占用100%的CPU使用率。 这怎么接受的了呢。
void *WStockGetUrl::Entry(){
    wxURL url(Url);
    wxString rtnString = wxT("");
    wxString proxy = WStockConfig::GetProxy();
    if (proxy.Length()>0){
        url.SetProxy(proxy);
    }
    wxStringOutputStream out_stream(&rtnString);
    int ret=url.GetError();
    if (url.GetError() == wxURL_NOERR){
        wxInputStream *in_stream = url.GetInputStream();
        try{
            while (!in_stream->Eof()){
                if (in_stream->CanRead()) in_stream->Read(out_stream);
            }
        }
        catch (...){
            ret = -1;
        }
        delete in_stream;
    }

    if (Parent){
        wxUrlGetDoneEvent event(wxEVT_URL_GET_DONE, -1,UserData);
        event.RetCode = ret;
        event.Result = rtnString;
        Parent->AddPendingEvent(event);
    }

    return NULL;
}*/


//下面尝试使用libcurl
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
struct MemoryStruct {
  char *memory;
  size_t size;
};

void *myrealloc(void *ptr, size_t size)
{
  /* There might be a realloc() out there that doesn't like reallocing
     NULL pointers, so we take care of it here */
  if(ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}

size_t
WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)data;

  mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory) {
    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
  }
  return realsize;
}

void *WStockGetUrl::Entry(){
  CURL *curl_handle;

  struct MemoryStruct chunk;
  chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
  chunk.size = 0;    /* no data at this point */

  curl_global_init(CURL_GLOBAL_ALL);
  /* init the curl session */
  curl_handle = curl_easy_init();

/*    wxString Proxy = WStockConfig::GetProxy();
    if (Proxy.Length()>0){
      char proxy[255]="";
      strcpy(proxy,(const char*)Proxy.mb_str());
      curl_easy_setopt(curl_handle, CURLOPT_PROXY, proxy);
      curl_easy_setopt(curl_handle, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
    }
*/

  char url[255]="";
  strcpy(url,(const char*)Url.mb_str());
  /* specify URL to get */
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  /*if (PostData.size()>0){
      char post[255]="";
      strcpy(post,(const char*)PostData.mb_str());
      curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post);
  }*/
  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  /* we pass our 'chunk' struct to the callback function */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  /* get it! */
  curl_easy_perform(curl_handle);
  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);

    if (Parent){
        //printf("%s\n",(char*)chunk.memory);
        wxUrlGetDoneEvent event(wxEVT_URL_GET_DONE, -1,UserData);
        wxCSConv cs(wxT("GB2312"));
        event.Result = wxString(cs.cMB2WC((char*)chunk.memory),*wxConvCurrent);
		wxLogMessage(wxT("%d %d"),strlen((char*)chunk.memory),event.Result.Length());

		wxFileOutputStream output(wxT("c:\\test.log"));
		wxDataOutputStream store(output);
		store << event.Result;

        //event.RetCode = ret;
        //event.Result = rtnString;
        Parent->AddPendingEvent(event);
    }

  if(chunk.memory)
    free(chunk.memory);

    return NULL;
}
