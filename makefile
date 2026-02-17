LDFLAGS=-lcurl
CXXFLAGS=-I ~/graphcrawler/rapidjson/include -lcurl
LD=g++

crawler: crawler.o
	$(LD) -o $@ $< $(LDFLAGS)

crawler.o: crawler.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<