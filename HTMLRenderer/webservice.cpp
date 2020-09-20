#include "webservice.h"
#include <iostream>

WebService::WebService()
{

}

size_t WebService::write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    *((std::string*)stream) += std::string((char*)ptr, size*nmemb);
    //std::cout << "data: " << (char*)ptr << std::endl;
    return nmemb;
}

std::string WebService::htmlFileDownloader(std::string path)
{
    std::string result;
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, path.c_str());

    /* Switch on full protocol/debug output while testing */
    //curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

    /* disable progress meter, set to 0L to enable it */
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.112 Safari/537.36");

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &result);

    /* get it! */
    CURLcode res = curl_easy_perform(curl_handle);
    if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

    curl_easy_cleanup(curl_handle);

    curl_global_cleanup();
    return result;
}
