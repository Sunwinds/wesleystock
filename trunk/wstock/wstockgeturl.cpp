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


#ifdef __WXDEBUG__
extern FILE *fp;
#endif

void *WStockGetUrl::Entry(){
  CURL *curl_handle;

  struct MemoryStruct chunk;
  chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
  chunk.size = 0;    /* no data at this point */

  curl_global_init(CURL_GLOBAL_ALL);
  /* init the curl session */
  curl_handle = curl_easy_init();

  if ((!WStockConfig::GetCurlNoProxy()) && (!WStockConfig::GetCurlProxy().IsEmpty())){
    char post[255]="";
    strcpy(post,(const char*)WStockConfig::GetCurlProxy().mb_str());
	curl_easy_setopt(curl_handle, CURLOPT_PROXY, post);
	if (!WStockConfig::GetCurlUserPwd().IsEmpty()){
	    char post[255]="";
		strcpy(post,(const char*)WStockConfig::GetCurlUserPwd().mb_str());
		curl_easy_setopt(curl_handle, CURLOPT_PROXYUSERPWD, post);
	}
	curl_easy_setopt(curl_handle, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
	switch (WStockConfig::GetCurlAuth()){
	case 0:
		curl_easy_setopt(curl_handle, CURLOPT_PROXYAUTH,CURLAUTH_NONE);
		break;
	case 1:
		curl_easy_setopt(curl_handle, CURLOPT_PROXYAUTH,CURLAUTH_BASIC);
		break;
	case 2:
		curl_easy_setopt(curl_handle, CURLOPT_PROXYAUTH,CURLAUTH_DIGEST);
		break;
	case 3:
		curl_easy_setopt(curl_handle, CURLOPT_PROXYAUTH,CURLAUTH_GSSNEGOTIATE);
		break;
	case 4:
		curl_easy_setopt(curl_handle, CURLOPT_PROXYAUTH,CURLAUTH_NTLM);
		break;
	case 5:
		curl_easy_setopt(curl_handle, CURLOPT_PROXYAUTH,CURLAUTH_ANY);
		break;
	case 6:
		curl_easy_setopt(curl_handle, CURLOPT_PROXYAUTH,CURLAUTH_ANYSAFE);
		break;
	}
  }

  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, FALSE);
  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 1);

  char url[255]="";
  strcpy(url,(const char*)Url.mb_str());
  /* specify URL to get */
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  /* we pass our 'chunk' struct to the callback function */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl_handle, CURLOPT_HEADER,0);
  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  if (CustomHeads.size()>0){
        struct curl_slist *headers=NULL; /* init to NULL is important */
        for (size_t i=0;i<CustomHeads.size();i++){
            char post[1024]="";
            strcpy(post,(const char*)CustomHeads[i].mb_str());
            headers = curl_slist_append(headers, post);
        }
        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
  }
  if (PostData.size()>0){
      char post[1024]="";
      strcpy(post,(const char*)PostData.mb_str());
      curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post);
  }

  if (!CusCmd.IsEmpty()){
      char post[255]="";
      strcpy(post,(const char*)CusCmd.mb_str());
      curl_easy_setopt(curl_handle,   CURLOPT_CUSTOMREQUEST, post);
  }

  /* get it! */
  curl_easy_perform(curl_handle);
  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);

    if (Parent){
        wxUrlGetDoneEvent event(wxEVT_URL_GET_DONE, -1,UserData);
		event.requestTime = requestTime;
        if (WantXml){
            event.doc = xmlReadMemory((char*)chunk.memory,
                        strlen((char*)chunk.memory), "noname.xml",NULL,0);
        }
        else {
            wxCSConv cs(wxT("GB2312"));
            event.Result = wxString(cs.cMB2WC((char*)chunk.memory),*wxConvCurrent);
			//wxLogMessage(event.Result);
            /*wxLogMessage(wxT("%d %d"),strlen((char*)chunk.memory),event.Result.Length());
            wxFileOutputStream output(wxT("c:\\test.log"));
            wxDataOutputStream store(output);
            store << event.Result;*/

        }
        Parent->AddPendingEvent(event);
    }
#ifdef __WXDEBUG__
	else{
		fwrite(chunk.memory, 1, strlen(chunk.memory) , fp);
		fflush(fp);
	}
#endif

  if(chunk.memory)
    free(chunk.memory);

    return NULL;
}
