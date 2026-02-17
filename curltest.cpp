#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <iostream>

using namespace std;

size_t my_write_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
    std::string* mystring = (std::string*)userdata;

    for (size_t i = 0; i < nmemb; ++i) {
        mystring->push_back(ptr[i]);
    }

    return nmemb;
}


int main(int argc, char *argv[]) {
    
    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, "https://curl.se/libcurl/c/libcurl-tutorial.html");

    std::string myoutstring;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &myoutstring);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write_data);
    curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    cout << myoutstring << endl;
    return 0;

}