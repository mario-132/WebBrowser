#ifndef WEBSERVICE_H
#define WEBSERVICE_H
#include <string>
#include <curl/curl.h>
#include <vector>

struct WebCache
{
    std::string path;
    std::string data;
};

class WebService
{
public:
    WebService();

    static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
    static CURL *curl_handle;
    static std::string htmlFileDownloader(std::string path);

    static std::vector<WebCache> webCaches;

    static void init();
    static void destroy();
};

#endif // WEBSERVICE_H
