#include <bits/stdc++.h>
#include <CkSpider.h>
#include "include/taskurl.h"
#include "include/webpage.h"

using namespace std;

const int MAX = 100000;
static int searchId = 0, countLevelZero = 0, countLevelOne = 0;
static double totalLevelOnePageSize = 0;

auto initialExeTime = chrono::high_resolution_clock::now();

set <string> visitedPages;
map <string, int> searchedTable;
map <string, int> domainTable;
map <string, bool> crawlingDomain;

priority_queue <TaskUrl> longTermScheduler;	

mutex searchTableLock, searchIdLock, domainTableLock, longTermSchedulerLock, writeLock, crawlingDomainLock, visitedPagesLock, levelCountLock;

int crawlingFinished(){
	searchIdLock.lock();
	bool finished = searchId >= MAX;
	searchIdLock.unlock();

	return finished;
}

int toCrawl(string url, CkSpider &spider, int level){
	visitedPagesLock.lock();

	if(!visitedPages.count(url)){
		visitedPages.insert(url);
		visitedPagesLock.unlock();

		bool successfullyCrawled = spider.CrawlNext();
		spider.SleepMs(100);

		if (successfullyCrawled){
			levelCountLock.lock();
			searchTableLock.lock();
			searchIdLock.lock();
			
			level == 0 ? countLevelZero++ : countLevelOne++;

			int goId = searchId++;
			searchedTable[url] = goId;
			
			if ((goId >= 1000 && goId%1000 == 0)){
				auto finalExeTime = chrono::high_resolution_clock::now();
				double totalExeTime = chrono::duration_cast<chrono::microseconds>(finalExeTime - initialExeTime).count()*1e-6;

				string path = "exetime.csv";

				fstream checkExistance;
				checkExistance.open(path);
				
				ofstream outfile;
				if (checkExistance.fail()){
					outfile.open(path);
			 		outfile << "id,totalTime,pageSize,leve0,level1" << endl;
			 	}
				else
				   	outfile.open(path, std::ios_base::app);

				outfile << to_string(goId) << "," << to_string(totalExeTime/60) << "," <<
				           to_string(totalLevelOnePageSize*1e-9) << "," << 
				           to_string(countLevelZero) << "," << to_string(countLevelOne) << endl;
				outfile.close();
			}

			levelCountLock.unlock();
			searchIdLock.unlock();
			searchTableLock.unlock();
			
			return goId;
		} 
		else return -2;
	} 
	
	visitedPagesLock.unlock();
	return -1;
}

int addWebPage(string url, string domain, int id, CkSpider &spider, double crawlingTime, int level){
	string title = spider.lastHtmlTitle();
	string html = spider.lastHtml();

	int htmlSize = sizeof(char)*html.length();
	
	WebPage *nextWebPage = new WebPage(url, domain, title, id, level, crawlingTime, htmlSize);

	nextWebPage->saveInfo(html);

	delete nextWebPage;

	return htmlSize;
}

int updateLongTermScheduler(CkSpider &taskSpider, string &currTaskUrl, string &currTaskDomain){
	auto initialCurrCrawExeTime = chrono::high_resolution_clock::now();
	int currTaskID = toCrawl(currTaskUrl, taskSpider, 0);
	auto finalCurrCrawExeTime = chrono::high_resolution_clock::now();

	if (currTaskID == -1 || currTaskID == -2) 
		return -1;

	double crawlingTime = chrono::duration_cast<chrono::microseconds>(finalCurrCrawExeTime - initialCurrCrawExeTime).count();
	addWebPage(currTaskUrl, currTaskDomain, currTaskID, taskSpider, crawlingTime, 0);

	if (crawlingFinished())
		return -1;

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

		crawlingDomainLock.lock();
		crawlingDomain[currTaskDomain] = false;
		crawlingDomainLock.unlock();

		longTermSchedulerLock.lock();
		
		TaskUrl newTaskUrl(nextSeedURL, nextSeedDomain, domainTable[nextSeedDomain]);
		longTermScheduler.push(newTaskUrl);
	
		longTermSchedulerLock.unlock();
	}
	taskSpider.SleepMs(100);

	return currTaskID;
}

void Crawler(){
	while (!crawlingFinished()){
		CkSpider taskSpider;
		string currTaskUrl, currTaskDomain;

		taskSpider.put_Utf8(true);

		taskSpider.AddAvoidOutboundLinkPattern("*facebook.com*");
		taskSpider.AddAvoidOutboundLinkPattern("*instagram.com*");
		taskSpider.AddAvoidOutboundLinkPattern("*twitter.com*");
		taskSpider.AddAvoidOutboundLinkPattern("*amazon.com*");
		taskSpider.AddAvoidOutboundLinkPattern("*wikipedia*");
		taskSpider.AddAvoidOutboundLinkPattern("*blogspot*");

		int attempts = 50;
		bool startSearching = false, domainAvailable = false;
		while (!startSearching && !domainAvailable && attempts--){
			if (crawlingFinished())
				return;

			longTermSchedulerLock.lock();
			startSearching = !longTermScheduler.empty();
			if (startSearching){
				stack <TaskUrl> unavailableTasks;
				while (!longTermScheduler.empty() && !domainAvailable){
					TaskUrl currTask = longTermScheduler.top();
					longTermScheduler.pop();
					
					currTaskUrl = currTask.url;
					currTaskDomain = currTask.domain;

					crawlingDomainLock.lock();
					if (crawlingDomain[currTaskDomain] == false){
						domainAvailable = true;
						crawlingDomain[currTaskDomain] = true;
					}
					else unavailableTasks.push(currTask);

					crawlingDomainLock.unlock();
				}

				while (!unavailableTasks.empty()){
					TaskUrl tmp = unavailableTasks.top();
					unavailableTasks.pop();
					longTermScheduler.push(tmp);
				}
			}
			longTermSchedulerLock.unlock();
			
			if (attempts == 0) return;

			if (!startSearching || !domainAvailable)
				taskSpider.SleepMs(2000);
		}

		taskSpider.Initialize(currTaskDomain.c_str());
		taskSpider.AddUnspidered(currTaskUrl.c_str());

		int currTaskID = updateLongTermScheduler(taskSpider, currTaskUrl, currTaskDomain);
 		if (currTaskID != -1){
			double currAvgCrawlingTime = 0, currAvgPageSize = 0;

			int inBoundLinks = taskSpider.get_NumUnspidered(), countInboundCrawled = 0; 
			for (int i = 0; i < inBoundLinks; i++){
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

				auto initialCurrCrawExeTime = chrono::high_resolution_clock::now();
				int nextUrlID = toCrawl(nextUrl, taskSpider, 1);
				auto finalCurrCrawExeTime = chrono::high_resolution_clock::now();
				
				if (nextUrlID == -1){
					taskSpider.SkipUnspidered(0);
				}
				else if (nextUrlID != -2){
					double crawlingTime = chrono::duration_cast<chrono::microseconds>(finalCurrCrawExeTime - initialCurrCrawExeTime).count();
					
					int nextUrlPageSize = addWebPage(nextUrl, nextDomain, nextUrlID, taskSpider, crawlingTime, 1);
					
					currAvgCrawlingTime += chrono::duration_cast<chrono::microseconds>(finalCurrCrawExeTime - initialCurrCrawExeTime).count();
					currAvgPageSize += nextUrlPageSize;

					countInboundCrawled++;
				}

				if (crawlingFinished())
					break;
			}

			crawlingDomainLock.lock();
			crawlingDomain[currTaskDomain] = false;
			crawlingDomainLock.unlock();

			domainTableLock.lock();
			domainTable[currTaskDomain] += countInboundCrawled;
			domainTableLock.unlock();

			writeLock.lock();

			totalLevelOnePageSize += currAvgPageSize;

			currAvgCrawlingTime = countInboundCrawled > 0 ? (currAvgCrawlingTime*1e-6)/(countInboundCrawled*1.0) : 0;
			currAvgPageSize = currAvgPageSize > 0 ? currAvgPageSize/(countInboundCrawled*1.0) : 0;

			TaskUrl *currLevelZeroTask = new TaskUrl(currTaskID, currTaskUrl, currTaskDomain,
										             countInboundCrawled, currAvgPageSize, currAvgCrawlingTime);
			currLevelZeroTask->report();

			delete currLevelZeroTask;

			writeLock.unlock();
		}
	}
}

int main(){
	const int numSeeds = 3;

	for (int i = 0; i < numSeeds; i++){
		CkSpider auxSpider;
		string str; cin >> str;

		string seedUrl = str.c_str();
		string seedDomain = auxSpider.getUrlDomain(seedUrl.c_str()); 

		crawlingDomain[seedDomain] = false;

		TaskUrl newTaskUrl(seedUrl, seedDomain, 0);
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

	return 0;

}