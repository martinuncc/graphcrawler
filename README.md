# graphcrawler

Author: Martin Anuonye

This program performs a BFS across movie and actor data given a starting node 
as a string and then an integer for the depth for the search.

./crawler "Tom Hanks" 2

for example, was the input that resulted in the attacked crawler_log.txt file.
The makefile is configured to include curl as long as it is downloaded on your system.
Also, download rapidjson and place it in ~/graphcrawler/rapidjson/include; or reconfigure the makefile
according to where your rapidjson library is.