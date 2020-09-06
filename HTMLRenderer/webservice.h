#ifndef WEBSERVICE_H
#define WEBSERVICE_H
#include <string>
#include <curl/curl.h>

class WebService
{
public:
    WebService();

    static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

    static std::string htmlFileDownloader(std::string path);
};

#endif // WEBSERVICE_H
