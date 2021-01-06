#include <bits/stdc++.h>
#include "webpage.h"
#include <CkSpider.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;

const int MAX = 100000;

int idCrawler = 0;
int countSearched = 0;

map <string, int> searchedTable;
map <string, int> domainsTable;
map <string, bool> currSearchingDomain;

priority_queue <webPage> longTermScheduler;	

mutex stLock, countLock, dtLock, csdLock, ltsLock, idCra;

int getCount(){
	countLock.lock();
	int rsp = countSearched;
	countLock.unlock();
	return rsp;
}

bool crawlingFinished(){
	countLock.lock();
	int currCount = countSearched;
	countLock.unlock();

	return currCount >= MAX;
}

bool isVisited(string url){
	stLock.lock();
	bool found = searchedTable.find(url) != searchedTable.end();
	stLock.unlock();

	return found;
}

void addSearchedURL(string url){
	stLock.lock();
	countLock.lock();

	countSearched++;
	searchedTable[url]++;

	stLock.unlock();
	countLock.unlock();
}

bool updateLongTermScheduler(CkSpider &stsSpider, string &currURL){
	bool firstCrawl = stsSpider.CrawlNext();
	if (!firstCrawl || crawlingFinished())
		return false;

	addSearchedURL(currURL);

	int outBoundLinks = stsSpider.get_NumOutboundLinks();
	for (int i = 0; i < outBoundLinks; i++){
		
		string nextSeedURL, nextSeedDomain;

		try{
			string failSeedDomain(stsSpider.getUrlDomain(stsSpider.getOutboundLink(i)));
			string failSeedUrl(stsSpider.getOutboundLink(i));
		} catch (exception &e){
			continue;
		}

		nextSeedURL = stsSpider.getOutboundLink(i);
		nextSeedDomain = stsSpider.getUrlDomain(stsSpider.getOutboundLink(i));

		if (isVisited(nextSeedURL))
			continue;

		ltsLock.lock();
		dtLock.lock();
		
		webPage newWebPage(nextSeedURL, nextSeedDomain, domainsTable[nextSeedDomain]++);
		longTermScheduler.push(newWebPage);
	
		ltsLock.unlock();
		dtLock.unlock();
	}
	stsSpider.SleepMs(100);

	return true;
}

void Crawler(){
	idCra.lock();
	int currIdCrawler = idCrawler++;
	idCra.unlock();
	while (!crawlingFinished()){
		CkSpider stsSpider;
		string currURL, currDomain;

		int attempts = 50;
		bool startSearching = false, domainAvailable = false;
		while (!startSearching && !domainAvailable && attempts--){
			if (crawlingFinished())
				return;

			ltsLock.lock();
			startSearching = !longTermScheduler.empty();
			if (startSearching){
				stack <webPage> tmpStk;

				while (!longTermScheduler.empty() && !domainAvailable){
					webPage currWebPage = longTermScheduler.top();
					longTermScheduler.pop();
					
					currURL = currWebPage.url;
					currDomain = currWebPage.domain;
					
					csdLock.lock();
					
					bool isDomainInThread = currSearchingDomain[currDomain];
					if (!isDomainInThread){
						currSearchingDomain[currDomain] = true;
						domainAvailable = true;
					}
					else 
						tmpStk.push(currWebPage);

					csdLock.unlock();
				}

				while (!tmpStk.empty()){
					webPage tmpWebPage = tmpStk.top();
					tmpStk.pop();
					longTermScheduler.push(tmpWebPage);
				}
			}
			ltsLock.unlock();

			if (!startSearching)
				stsSpider.SleepMs(2000);
		}
		if (!domainAvailable)
			return;

		stsSpider.Initialize(currDomain.c_str());
		stsSpider.AddUnspidered(currURL.c_str());

		bool firstCrawl = updateLongTermScheduler(stsSpider, currURL);
		if (firstCrawl){
			int inBoundLinks = stsSpider.get_NumUnspidered();
			for (int i = 0; i < inBoundLinks; i++){
				if (crawlingFinished())
					return;

				string nextURL;
				
				try{
					string failNextDomain(stsSpider.getUrlDomain(stsSpider.getUnspideredUrl(0)));
					string failNextUrl(stsSpider.getUnspideredUrl(0));
				} catch (exception &e){
					stsSpider.SkipUnspidered(0);
					continue;
				}

				nextURL = stsSpider.getUnspideredUrl(0);

				if (isVisited(nextURL))
					stsSpider.SkipUnspidered(0);
				else{
					bool inBoundCrawling = stsSpider.CrawlNext();
					if (inBoundCrawling){
						addSearchedURL(nextURL);
						cout << "==> From thread " << currIdCrawler << ": " << nextURL << ", ID = " << getCount() << "." << endl << endl;
					}

					stsSpider.SleepMs(100);
				}
			}
		}

		csdLock.lock();
		currSearchingDomain[currDomain] = false;
		csdLock.unlock();
	}
}

int main(){
	const int numSeeds = 5;

	for (int i = 0; i < numSeeds; i++){
		CkSpider auxSpider;
		string str; cin >> str;

		string seedUrl = str.c_str();
		string seedDomain = auxSpider.getUrlDomain(seedUrl.c_str()); 

		currSearchingDomain[seedDomain] = false;

		webPage newWebPage(seedUrl, seedDomain, domainsTable[seedDomain]++);
		longTermScheduler.push(newWebPage);
	}

	vector <thread*> jobs;

	for (int i = 0; i < 20; i++){
		thread *task = new thread(Crawler);
		jobs.push_back(task);
	}

	for (auto &task : jobs){
		task->join();
		delete task;
	}

	return 0;

}