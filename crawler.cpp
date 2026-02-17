#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <iostream>
#include "rapidjson/document.h"
#include <cassert>
#include <algorithm>

using namespace std;


size_t my_write_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
    std::string* mystring = (std::string*)userdata;

    for (size_t i = 0; i < nmemb; ++i) {
        mystring->push_back(ptr[i]);
    }

    return nmemb;
}

int main(int argc, char *argv[]) {

    std::string starting_node;
    int depth;

    if (argc != 3) {
        std::cout << "Need 2 arguments: starting node and depth" << std::endl;
        return 0;
    } else {
        starting_node = argv[1];
        depth = atoi(argv[2]);
    }
    

    char char_to_replace = ' ';
    std::string replacement_str = "%20";
    std::string baseURL = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/";
    starting_node.replace(starting_node.find(char_to_replace), 1, replacement_str);
    std::cout << "Starting node: " << starting_node << std::endl;
    std::string url = baseURL + starting_node;

    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    std::string myoutstring;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &myoutstring);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write_data);

    curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    //cout << myoutstring << endl;

    Document document;
    std::cout << "Parsing JSON response..." << std::endl;
    document.Parse(myoutstring.c_str());

    std::cout << "Creating neighbors list..." << std::endl;
    const Value& neighbors = document["neighbors"];
    std::vector<std::string> neighbors_visited;
    std::cout << "Checking if neighbors exist for the starting node..." << std::endl;
    if (neighbors.IsNull()) {
        std::cout << "No neighbors found for the starting node " << argv[1] << "." << std::endl;
        return 0;
    }

    std::cout <<"Attempting push starting node to neighbors list..." << std::endl;
    neighbors_visited.push_back(argv[1]);
    std::cout << "Attempting iteration of neighbors for node " << argv[1] << std::endl;
    for (SizeType i = 0; i < neighbors.Size(); i++) {
        std::string neighbor_name = neighbors[i].GetString();
        neighbors_visited.push_back(neighbor_name);
        std::cout << "Visited neighbor: " << neighbors_visited[i] << std::endl;
    }

    for (int i = 0; i < depth - 1; i++) {
        std::vector<std::string> new_neighbors;
        for (const auto& neighbor : neighbors_visited) {
            std::string url = baseURL + neighbor;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            myoutstring.clear();
            curl_easy_perform(curl);
            document.Parse(myoutstring.c_str());
            const Value& neighbors = document["neighbors"];
            if (!neighbors.IsNull()) {
                for (SizeType j = 0; j < neighbors.Size(); j++) {
                    std::string neighbor_name = neighbors[j].GetString();
                    if (std::find(neighbors_visited.begin(), neighbors_visited.end(), neighbor_name) == neighbors_visited.end()) {
                        new_neighbors.push_back(neighbor_name);
                        std::cout << "Visited neighbor: " << neighbor_name << std::endl;
                    }
                }
            }
        }
        neighbors_visited.insert(neighbors_visited.end(), new_neighbors.begin(), new_neighbors.end());
    }


    return 0;
  
}