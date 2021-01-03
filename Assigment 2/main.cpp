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

int id = 0;
map <string, int> searchedTable;
map <string, int> domainsTable;
set <string> currSearchDomains; 

priority_queue <webPage> longTermScheduler;	

void STScheduler(webPage currWebPage){
	CkSpider stsSpider;

	string currURL = currWebPage.url;
	string currDomain = currWebPage.domain;

	stsSpider.Initialize(currDomain.c_str());
	stsSpider.AddUnspidered(currURL.c_str());

	bool firstCrawl = stsSpider.CrawlNext();
	if (firstCrawl){
		countSearched++;
		searchedTable[currURL]++;
		//cout << "Searched " << countSearched << ": " << currURL << " within domain " << currDomain << endl << endl;

		for (int i = 0; i < stsSpider.get_NumOutboundLinks(); i++){
			CkSpider auxSpider;
			string nextSeedURL, nextSeedDomain;

			try{
				cout << "IN " << i << ' ' << stsSpider.get_NumOutboundLinks() << endl;
				nextSeedDomain = auxSpider.getUrlDomain(stsSpider.getOutboundLink(i));
				nextSeedURL = auxSpider.canonicalizeUrl(stsSpider.getOutboundLink(i));
				cout << nextSeedURL << endl;
				cout << "OUT" << endl;
			} catch (exception &e){
				continue;
			}

			webPage newWebPage(nextSeedURL, nextSeedDomain, id++, domainsTable[nextSeedDomain]++);
			longTermScheduler.push(newWebPage);
			//cout << "=>> Adding: " << nextSeedURL << " (LongTerm Scheduler) within domain " << nextSeedDomain << endl << endl;
		}
		stsSpider.SleepMs(100);

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

			bool inBoundCrawling = stsSpider.CrawlNext();
			if (inBoundCrawling){
				countSearched++;
				searchedTable[nextURL]++;
			}

			stsSpider.SleepMs(200);
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

		webPage newWebPage(seedUrl, seedDomain, id++, domainsTable[seedDomain]++);
		longTermScheduler.push(newWebPage);
	}

	while (!longTermScheduler.empty() && countSearched < MAX){
		webPage search = longTermScheduler.top();
		longTermScheduler.pop();
		
		STScheduler(search);
	}

	return 0;
}