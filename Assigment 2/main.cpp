#include <bits/stdc++.h>
#include "include/taskurl.h"
#include "include/webpage.h"
#include <CkSpider.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;

const int MAX = 10000;
static int countSearched = 0, countLevelZero = 0, countLevelOne = 0;

map <string, int> searchedTable;
map <string, int> domainTable;

priority_queue <TaskUrl> longTermScheduler;	

mutex stLock, countLock, dtLock, ltsLock,  crLock, writeLock;

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

void addWebPage(string url, string domain, int level, CkSpider &spider){
	string title = spider.lastHtmlTitle();
	string html = spider.lastHtml();
	
	stLock.lock();
	WebPage *nextWebPage = new WebPage(url, domain, title, level, searchedTable[url]);
	stLock.unlock();

	nextWebPage->report();
	nextWebPage->saveInfo(html);

	delete nextWebPage;
}

int addCrawledUrl(string &url, string &domain){
	stLock.lock();
	countLock.lock();
	dtLock.lock();

	int id = countSearched++;
	searchedTable[url] = countSearched;
	domainTable[domain]++;

	dtLock.unlock();
	stLock.unlock();
	countLock.unlock();

	return id;
}

bool updateLongTermScheduler(CkSpider &taskSpider, string &currTaskUrl, string &currTaskDomain){
	if (isVisited(currTaskUrl))
		return false;

	bool firstCrawl = taskSpider.CrawlNext();
	if (!firstCrawl || crawlingFinished())
		return false;

	addCrawledUrl(currTaskUrl, currTaskDomain);
	addWebPage(currTaskUrl, currTaskDomain, 0, taskSpider);

	int outBoundLinks = taskSpider.get_NumOutboundLinks();
	for (int i = 0; i < outBoundLinks; i++){
		
		string nextSeedURL, nextSeedDomain;

		try{
			string failSeedDomain(taskSpider.getUrlDomain(taskSpider.getOutboundLink(i)));
			string failSeedUrl(taskSpider.getOutboundLink(i));
		} catch (exception &e){
			continue;
		}

		nextSeedURL = taskSpider.getOutboundLink(i);
		nextSeedDomain = taskSpider.getUrlDomain(taskSpider.getOutboundLink(i));

		if (isVisited(nextSeedURL))
			continue;

		ltsLock.lock();
		dtLock.lock();
		
		TaskUrl newTaskUrl(nextSeedURL, nextSeedDomain, domainTable[nextSeedDomain]++);
		longTermScheduler.push(newTaskUrl);
	
		ltsLock.unlock();
		dtLock.unlock();
	}
	taskSpider.SleepMs(100);

	return true;
}

void Crawler(){
	while (!crawlingFinished()){
		CkSpider taskSpider;
		string currTaskUrl, currTaskDomain;

		taskSpider.AddAvoidOutboundLinkPattern("*facebook.com*");
		taskSpider.AddAvoidOutboundLinkPattern("*twitter.com*");

		int attempts = 50;
		bool startSearching = false;
		while (!startSearching && attempts--){
			if (crawlingFinished())
				return;

			ltsLock.lock();
			
			startSearching = !longTermScheduler.empty();
			if (startSearching){
					TaskUrl currTask = longTermScheduler.top();
					longTermScheduler.pop();
					
					currTaskUrl = currTask.url;
					currTaskDomain = currTask.domain;
			}
			else if (attempts == 0) return;
			
			ltsLock.unlock();

			if (!startSearching)
				taskSpider.SleepMs(2000);
		}

		taskSpider.Initialize(currTaskDomain.c_str());
		taskSpider.AddUnspidered(currTaskUrl.c_str());

		string title;

		bool runTask = updateLongTermScheduler(taskSpider, currTaskUrl, currTaskDomain);
 		if (runTask){
			double currAvgCrawlingTime = 0;
			int inBoundLinks = taskSpider.get_NumUnspidered(), countInboundCrawled = 0;
			for (int i = 0; i < inBoundLinks; i++){
				if (crawlingFinished())
					break;

				string nextURL, nextDomain;
				
				try{
					string failNextDomain(taskSpider.getUrlDomain(taskSpider.getUnspideredUrl(0)));
					string failNextUrl(taskSpider.getUnspideredUrl(0));
				} catch (exception &e){
					taskSpider.SkipUnspidered(0);
					continue;
				}

				nextURL = taskSpider.getUnspideredUrl(0);
				nextDomain = taskSpider.getUrlDomain(taskSpider.getUnspideredUrl(0));

				if (isVisited(nextURL)){
					taskSpider.SkipUnspidered(0);
					//AddEdge
				}
				else{
					auto initialExeTime = chrono::high_resolution_clock::now();
					bool inBoundCrawling = taskSpider.CrawlNext();
					auto finalExeTime = chrono::high_resolution_clock::now();
					if (inBoundCrawling){
						addCrawledUrl(nextURL, nextDomain);
						addWebPage(nextURL, nextDomain, 1, taskSpider);
						
						currAvgCrawlingTime += chrono::duration_cast<chrono::microseconds>(finalExeTime - initialExeTime).count();
						countInboundCrawled++;
					}
					taskSpider.SleepMs(100);
				}
			}

			currAvgCrawlingTime = countInboundCrawled > 0 ? (currAvgCrawlingTime*1e-6)/(countInboundCrawled*1.0) : 0;
			
			stLock.lock();
			int currTaskID = searchedTable[currTaskUrl];
			stLock.unlock();

			writeLock.lock();

			saveInfo(currTaskUrl, countInboundCrawled, currAvgCrawlingTime, currTaskID);
			countLevelZero += 1;
			countLevelOne += countInboundCrawled;
			
			writeLock.unlock();

			dtLock.lock();
			domainTable[currTaskDomain] += countInboundCrawled;
			dtLock.unlock();

			if (crawlingFinished())
				break;
		}
	}
}

int main(){
	const int numSeeds = 5;

	for (int i = 0; i < numSeeds; i++){
		CkSpider auxSpider;
		string str; cin >> str;

		string seedUrl = str.c_str();
		string seedDomain = auxSpider.getUrlDomain(seedUrl.c_str()); 

		TaskUrl newTaskUrl(seedUrl, seedDomain, domainTable[seedDomain]++);
		longTermScheduler.push(newTaskUrl);
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

	bool success = countLevelZero+countLevelOne == countSearched;
	cout << "Count results: " << countLevelZero+countLevelOne << "/" << countSearched;
	cout << ((success) ? "   SUCCESS!" : "   FAILED!") << endl;
	if (success){
		cout << "Pages crawled (level 0): " << countLevelZero << endl;
		cout << "Pages crawled (level 1): " << countLevelOne << endl;
	}
	else cout << "GO FIX YOUR CODE DUMB ASS" << endl;

	return 0;

}