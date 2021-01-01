#include <bits/stdc++.h>
#include <CkSpider.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;

const int MAX = 100;

map <string, int> searchedTable;
map <string, int> domainsTable;
set <string> currSearchDomains; 

class URL {
	public:
		string url;
		int id;
		int urlWeight;
		int domainWeight; 

		URL(string url, int id, int domainWeight) : url(url), id(id), domainWeight(domainWeight) {
			for (char ch : url)
				if (ch == '.' || ch == '/')
					this->urlWeight += 1;
		}

		bool operator > (const URL &x){
			if (this->urlWeight != x.urlWeight) return this->urlWeight < x.urlWeight;
			return this->domainWeight < x.domainWeight;
		}	
};

priority_queue <URL> longTermScheduler;
mutex ltsLock, dtLock;

//void STScheduler(){
	// while (totalCollected < MAX){
	// 	CkSpider stSpider;

	// 	while (longTermScheduler.empty()) stSpider.SleepMs(1000);

	// 	ltsLock.lock();
		
	// 	pair <int, string> nextUrl = longTermScheduler.top();
	// 	string currDomain = stSpider.getUrlDomain(nextUrl.second.c_str());
	// 	//int currLevel = -nextUrl.first;
	// 	longTermScheduler.pop();
		
	// 	ltsLock.unlock();

	// 	stSpider.Initialize(currDomain.c_str());
	// 	stSpider.AddUnspidered(nextUrl.second.c_str());

	// 	bool searching = true;

	// 	while (searching){
	// 		countLock.lock();
	// 		if (totalCollected >= MAX)
	// 			break;
	// 		countLock.unlock();

	// 		//bool found = stSpider.CrawlNext();
	// 		//bool validUrl = validateUrl(stSpider.lastUrl());

	// 		string foundDomain = stSpider.domain();

	// 		dtLock.lock();
	// 		if (checkDomain(foundDomain)){
	// 			domainTable[foundDomain]++;

	// 			countLock.lock();
	// 			totalCollected++;
	// 			countLock.unlock();

	// 			ltsLock.lock();
	// 			longTermScheduler.push({-getUrlWgt(foundDomain), foundDomain});
	// 			ltsLock.unlock();
	// 		}
	// 		dtLock.unlock();

	// 		stSpider.SleepMs(100);
	// 	}
	// }
//}

int main(){
	const int numSeeds = 5;

	int id = 0;
	for (int i = 0; i < numSeeds; i++){
		CkSpider auxSpider;
		string str; cin >> str;

		string currDomain = auxSpider.getUrlDomain(str.c_str());
		string seedUrl = auxSpider.canonicalizeUrl(str.c_str());

		if (searchedTable.find(seedUrl) != searchedTable.end())
			continue;

		URL currURL(seedUrl, id++, 0);

		longTermScheduler.push(currURL);
	}

	return 0;
}