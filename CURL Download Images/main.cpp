//
//  main.cpp
//  CURLLoadImage
//
//  Created by Frederik Riedel on 10.09.15.
//  Copyright (c) 2015 Quappi. All rights reserved.
//

//compilieren mit g++ main.cpp -o dw -lcurl

#include <iostream>
#include <curl/curl.h>

size_t callbackfunction(void *ptr, size_t size, size_t nmemb, void* userdata)
{
    FILE* stream = (FILE*)userdata;
    if (!stream)
    {
        printf("!!! No stream\n");
        return 0;
    }
    
    size_t written = fwrite((FILE*)ptr, size, nmemb, stream);
    return written;
}

bool download_jpeg(const char* url)
{
    FILE* fp = fopen("out.jpg", "wb");
    if (!fp)
    {
        printf("!!! Failed to create file on the disk\n");
        return false;
    }
    
    CURL* curlCtx = curl_easy_init();
    curl_easy_setopt(curlCtx, CURLOPT_URL, url);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, callbackfunction);
    curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);
    
    CURLcode rc = curl_easy_perform(curlCtx);
    if (rc)
    {
        printf("!!! Failed to download: %s\n", url);
        return false;
    }
    
    int res_code = 0;
    curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
    if (!((res_code == 200 || res_code == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
    {
        printf("!!! Response code: %d\n", res_code);
        return false;
    }
    
    curl_easy_cleanup(curlCtx);
    
    fclose(fp);
    
    return true;
}



int main(int argc, const char * argv[]) {
    
    for(int i=0; i<100; i++) {
        download_jpeg("http://map1.vis.earthdata.nasa.gov/wmts-geo/MODIS_Terra_CorrectedReflectance_TrueColor/default/2014-02-01/EPSG4326_250m/7/8/8.jpg");
    }
    
    return 0;
}
