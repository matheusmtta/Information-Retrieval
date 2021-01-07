#include <bits/stdc++.h>
#include <CkSpider.h>
#include "include/taskurl.h"
#include "include/webpage.h"

using namespace std;

const int MAX = 10000;
static int countSearched = 0, countLevelZero = 0, countLevelOne = 0;

map <string, int> searchedTable;
map <string, int> domainTable;

priority_queue <TaskUrl> longTermScheduler;	

mutex stLock, countLock, dtLock, ltsLock,  crLock, writeLock;

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

int addWebPage(string url, string domain, int id, CkSpider &spider){
	string title = spider.lastHtmlTitle();
	string html = spider.lastHtml();

	int htmlSize = sizeof(char)*html.length();
	
	WebPage *nextWebPage = new WebPage(url, domain, title, id);

	nextWebPage->report();
	nextWebPage->saveInfo(html);

	delete nextWebPage;

	return htmlSize;
}

int setVisited(string &url, string &domain){
	countLock.lock();
	int id = countSearched++;
	countLock.unlock();

	stLock.lock();
	searchedTable[url] = id;
	stLock.unlock();

	dtLock.lock();
	domainTable[domain]++;
	dtLock.unlock();

	return id;
}

int updateLongTermScheduler(CkSpider &taskSpider, string &currTaskUrl, string &currTaskDomain){
	if (isVisited(currTaskUrl))
		return -1;

	bool firstCrawl = taskSpider.CrawlNext();
	if (!firstCrawl || crawlingFinished())
		return -1;

	int currTaskID = setVisited(currTaskUrl, currTaskDomain);
	addWebPage(currTaskUrl, currTaskDomain, currTaskID, taskSpider);

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
	
		dtLock.unlock();
		ltsLock.unlock();
	}
	taskSpider.SleepMs(100);

	return currTaskID;
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
			ltsLock.unlock();
			
			if (attempts == 0) return;

			if (!startSearching)
				taskSpider.SleepMs(2000);
		}

		taskSpider.Initialize(currTaskDomain.c_str());
		taskSpider.AddUnspidered(currTaskUrl.c_str());

		int currTaskID = updateLongTermScheduler(taskSpider, currTaskUrl, currTaskDomain);
 		if (currTaskID != -1){
			double currAvgCrawlingTime = 0, currAvgPageSize = 0;

			int inBoundLinks = taskSpider.get_NumUnspidered(), countInboundCrawled = 0;
			for (int i = 0; i < inBoundLinks; i++){
				if (crawlingFinished())
					break;

				string nextUrl, nextDomain;
				
				try{
					string failNextDomain(taskSpider.getUrlDomain(taskSpider.getUnspideredUrl(0)));
					string failnextUrl(taskSpider.getUnspideredUrl(0));
				} catch (exception &e){
					taskSpider.SkipUnspidered(0);
					continue;
				}

				nextUrl = taskSpider.getUnspideredUrl(0);
				nextDomain = taskSpider.getUrlDomain(taskSpider.getUnspideredUrl(0));

				if (isVisited(nextUrl)){
					taskSpider.SkipUnspidered(0);
					//AddEdge
				}
				else{
					auto initialExeTime = chrono::high_resolution_clock::now();
					bool inBoundCrawling = taskSpider.CrawlNext();
					auto finalExeTime = chrono::high_resolution_clock::now();
					if (inBoundCrawling){
						int nextUrlID = setVisited(nextUrl, nextDomain);
						int nextUrlPageSize = addWebPage(nextUrl, nextDomain, nextUrlID, taskSpider);
						
						currAvgCrawlingTime += chrono::duration_cast<chrono::microseconds>(finalExeTime - initialExeTime).count();
						currAvgPageSize += nextUrlPageSize;

						countInboundCrawled++;
					}
					taskSpider.SleepMs(100);
				}
			}

			currAvgCrawlingTime = countInboundCrawled > 0 ? (currAvgCrawlingTime*1e-6)/(countInboundCrawled*1.0) : 0;
			currAvgPageSize = currAvgPageSize > 0 ? currAvgPageSize/(countInboundCrawled*1.0) : 0;

			writeLock.lock();
			TaskUrl *currLevelZeroTask = new TaskUrl(currTaskID, currTaskUrl, currTaskDomain,
										             countInboundCrawled, currAvgPageSize, currAvgCrawlingTime);
			currLevelZeroTask->report();

			delete currLevelZeroTask;

			countLevelZero += 1;
			countLevelOne += countInboundCrawled;

			writeLock.unlock();

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