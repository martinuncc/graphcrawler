#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <iostream>
#include "rapidjson/document.h"
#include <cassert>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <vector>

using namespace std;

size_t my_write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    std::string *mystring = (std::string *)userdata;

    for (size_t i = 0; i < nmemb; ++i)
    {
        mystring->push_back(ptr[i]);
    }

    return nmemb;
}

int main(int argc, char *argv[])
{

    std::string starting_node;
    int depth;

    if (argc != 3)
    {
        std::cout << "Need 2 arguments: starting node and depth" << std::endl;
        return 0;
    }
    else
    {
        starting_node = argv[1];
        depth = atoi(argv[2]);
    }

    std::ofstream logFile("crawler_log.txt");
    if (!logFile)
    {
        std::cerr << "Error opening log file." << std::endl;
        return 1;
    }
    std::cout << "Starting node: " << starting_node << std::endl;

    char char_to_replace = ' ';
    std::string replacement_str = "%20";
    std::string baseURL = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/";
    size_t pos;
    while ((pos = starting_node.find(char_to_replace)) != std::string::npos)
    {
        starting_node.replace(pos, 1, replacement_str);
    }
    std::string url = baseURL + starting_node;

    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    std::string myoutstring;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&myoutstring);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write_data);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "curl failed: " << curl_easy_strerror(res) << std::endl;
        continue;
    }

    rapidjson::Document document;
    document.Parse(myoutstring.c_str());

    const rapidjson::Value &neighbors = document["neighbors"];
    std::vector<std::string> neighbors_visited;
    if (neighbors.IsNull())
    {
        std::cout << "No neighbors found for the starting node " << argv[1] << "." << std::endl;
        return 0;
    }
    neighbors_visited.push_back(argv[1]);
    for (rapidjson::SizeType i = 0; i < neighbors.Size(); i++)
    {
        std::string neighbor_name = neighbors[i].GetString();
        neighbors_visited.push_back(neighbor_name);
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < depth; i++)
    {
        std::vector<std::string> new_neighbors;
        for (const auto &neighbor : neighbors_visited)
        {
            size_t pos;
            std::string neighbor_copy = neighbor;
            while ((pos = neighbor_copy.find(char_to_replace)) != std::string::npos)
            {
                neighbor_copy.replace(pos, 1, replacement_str);
            }
            std::string url = baseURL + neighbor_copy;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            myoutstring.clear();
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                std::cerr << "curl failed: " << curl_easy_strerror(res) << std::endl;
                continue;
            }
            document.Parse(myoutstring.c_str());
            const rapidjson::Value &neighbors = document["neighbors"];
            if (!neighbors.IsNull())
            {
                for (rapidjson::SizeType j = 0; j < neighbors.Size(); j++)
                {
                    std::string neighbor_name = neighbors[j].GetString();
                    if (std::find(neighbors_visited.begin(), neighbors_visited.end(), neighbor_name) == neighbors_visited.end())
                    {
                        new_neighbors.push_back(neighbor_name);
                        logFile << "Visited neighbor: " << neighbor_name << " at depth " << i + 1 << std::endl;
                    }
                }
            }
        }
        neighbors_visited.insert(neighbors_visited.end(), new_neighbors.begin(), new_neighbors.end());
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Time taken: " << duration.count() << " microseconds" << std::endl;
    logFile << "Time taken: " << duration.count() << " microseconds" << std::endl;
    logFile.close();
    return 0;
}