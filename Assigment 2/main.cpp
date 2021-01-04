#include <bits/stdc++.h>
#include "webpage.h"
#include <CkSpider.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;

const int MAX = 1000;

int countSearched = 0;

map <string, int> searchedTable;
map <string, int> domainsTable;
map <string, int> isSearchingDomain;

priority_queue <webPage> longTermScheduler;	

bool updateLongTermScheduler(CkSpider &stsSpider, string &currURL, string &currDomain){
	bool firstCrawl = stsSpider.CrawlNext();
	if (!firstCrawl)
		return false;

	countSearched++;
	searchedTable[currURL]++;
	cout << "<== Searching " << countSearched  << " : " << currURL << ", within " << currDomain << " (SHORT TERM SCHEDULER)" << endl << endl; 

	for (int i = 0; i < stsSpider.get_NumOutboundLinks(); i++){
		CkSpider auxSpider;
		string nextSeedURL, nextSeedDomain;

		try{
			nextSeedDomain = auxSpider.getUrlDomain(stsSpider.getOutboundLink(i));
			nextSeedURL = auxSpider.canonicalizeUrl(stsSpider.getOutboundLink(i));
		} catch (exception &e){
			continue;
		}

		if (searchedTable.find(nextSeedURL) != searchedTable.end())
			continue;

		//cout << "==> Adding: " << nextSeedURL << ", within: " << nextSeedDomain << " (LONG TERM SCHEDULER)" << endl;
		webPage newWebPage(nextSeedURL, nextSeedDomain, domainsTable[nextSeedDomain]++);
		longTermScheduler.push(newWebPage);
	}
	stsSpider.SleepMs(100);

	return true;
}

void STScheduler(webPage currWebPage){
	CkSpider stsSpider;

	string currURL = currWebPage.url;
	string currDomain = currWebPage.domain;

	stsSpider.Initialize(currDomain.c_str());
	stsSpider.AddUnspidered(currURL.c_str());

	bool firstCrawl = updateLongTermScheduler(stsSpider, currURL, currDomain);
	if (firstCrawl){
		int inBoundLinks = stsSpider.get_NumUnspidered();
		for (int i = 0; i < inBoundLinks && countSearched < MAX; i++){
			string nextURL;
			
			try{
				if (stsSpider.get_NumUnspidered())
					nextURL = stsSpider.getUnspideredUrl(0);
				else continue;
			} catch (exception &e){
				stsSpider.SkipUnspidered(0);
				continue;
			}

			if (searchedTable.find(nextURL) != searchedTable.end())
				stsSpider.SkipUnspidered(0);
			else{
				bool inBoundCrawling = stsSpider.CrawlNext();
				if (inBoundCrawling){
					countSearched++;
					searchedTable[nextURL]++;
					cout << "<== Searching " << countSearched  << " : " << nextURL << ", within " << currDomain << "." << endl << endl; 
				}
				stsSpider.SleepMs(100);
			}
		}
	}
}

int main(){
	const int numSeeds = 5;

	for (int i = 0; i < numSeeds; i++){
		CkSpider auxSpider;
		string str; cin >> str;

		string seedUrl = auxSpider.canonicalizeUrl(str.c_str());
		string seedDomain = auxSpider.getUrlDomain(seedUrl.c_str()); 

		cout << "==> Adding " << seedUrl << " within: " << seedDomain << " (LONG TERM SCHEDULER)" << endl;

		webPage newWebPage(seedUrl, seedDomain, domainsTable[seedDomain]++);
		longTermScheduler.push(newWebPage);
	}

	while (!longTermScheduler.empty() && countSearched < MAX){
		webPage search = longTermScheduler.top();
		longTermScheduler.pop();
		
		STScheduler(search);
	}

	return 0;
}