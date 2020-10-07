#include "webservice.h"
#include <iostream>
#include "debugger.h"

CURL *WebService::curl_handle;
std::vector<WebCache> WebService::webCaches;

WebService::WebService()
{

}

size_t WebService::write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    *((std::string*)stream) += std::string((char*)ptr, size*nmemb);
    return nmemb;
}

std::string WebService::htmlFileDownloader(std::string path)
{
    if (Debugger::getCheckboxEnabled("use_webcache"))
    {
        for (int i = 0; i < webCaches.size(); i++)
        {
            if (webCaches[i].path == path)
            {
                std::string txt = Debugger::getTextBoxText("network_textview");
                Debugger::setTextBoxText("network_textview", txt + "Cached: " + path + "\n");
                Debugger::loop();
                return webCaches[i].data;
            }
        }
    }
    std::string txt = Debugger::getTextBoxText("network_textview");
    Debugger::setTextBoxText("network_textview", txt + "Download: " + path + "\n");
    Debugger::loop();

    std::string result;
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, path.c_str());

    /* Switch on full protocol/debug output while testing */
    //curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

    /* disable progress meter, set to 0L to enable it */
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

    if (Debugger::getCheckboxEnabled("useragent_chrome"))
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.112 Safari/537.36");
    else
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "");

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &result);

    /* get it! */
    CURLcode res = curl_easy_perform(curl_handle);
    if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    curl_easy_cleanup(curl_handle);

    if (Debugger::getCheckboxEnabled("use_webcache"))
    {
        WebCache cache;
        cache.path = path;
        cache.data = result;
        webCaches.push_back(cache);
    }

    return result;
}

void WebService::init()
{
    curl_global_init(CURL_GLOBAL_ALL);

}

void WebService::destroy()
{

    curl_global_cleanup();

}
