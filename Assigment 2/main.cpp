#include <bits/stdc++.h>
#include "webpage.h"
#include <CkSpider.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;

const int MAX = 10000;

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
			string failNextSeedDomain(stsSpider.getUrlDomain(stsSpider.getOutboundLink(i)));
			string failNextSeedUrl(stsSpider.getOutboundLink(i));
			string failCanonicalize(stsSpider.getUrlDomain(stsSpider.getOutboundLink(i)));
		} catch (exception &e){
			continue;
		}

		nextSeedURL = stsSpider.canonicalizeUrl(stsSpider.getOutboundLink(i));
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

void STScheduler(){
	idCra.lock();
	int currIdCrawler = idCrawler++;
	idCra.unlock();
	while (!crawlingFinished()){
		CkSpider stsSpider;
		string currURL, currDomain;

		bool startSearching = false, domainAvailable = false;
		while (!startSearching && !domainAvailable){
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
					string failNexUrl(stsSpider.getUnspideredUrl(0));
					string failCanonicalize(stsSpider.canonicalizeUrl(stsSpider.getUnspideredUrl(0)));
				} catch (exception &e){
					stsSpider.SkipUnspidered(0);
					continue;
				}

				nextURL = stsSpider.canonicalizeUrl(stsSpider.getUnspideredUrl(0));

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

		string seedUrl = auxSpider.canonicalizeUrl(str.c_str());
		string seedDomain = auxSpider.getUrlDomain(seedUrl.c_str()); 

		currSearchingDomain[seedDomain] = false;

		webPage newWebPage(seedUrl, seedDomain, domainsTable[seedDomain]++);
		longTermScheduler.push(newWebPage);
	}

	thread t1(STScheduler);
	thread t2(STScheduler);
	thread t3(STScheduler);
	thread t4(STScheduler);
	thread t5(STScheduler);
	thread t6(STScheduler);
	thread t7(STScheduler);
	thread t8(STScheduler);
	thread t9(STScheduler);
	thread t10(STScheduler);
	thread t11(STScheduler);
	thread t12(STScheduler);
	thread t13(STScheduler);
	thread t14(STScheduler);
	thread t15(STScheduler);
	thread t16(STScheduler);
	thread t17(STScheduler);
	thread t18(STScheduler);
	thread t19(STScheduler);
	thread t20(STScheduler);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();
	t9.join();
	t10.join();
	t11.join();
	t12.join();
	t13.join();
	t14.join();
	t15.join();
	t16.join();
	t17.join();
	t18.join();
	t19.join();
	t20.join();

	return 0;

}