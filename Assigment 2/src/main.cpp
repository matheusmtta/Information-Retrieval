#include <bits/stdc++.h>
#include <CkSpider.h>
#include "include/taskurl.h"
#include "include/webpage.h"

using namespace std;

//Define global static and constant variables, which will be use
//as counters and to limit the program execution loop to 100000 pages
const int MAX = 1000;
static int searchId = 0, countLevelZero = 0, countLevelOne = 0;

//Set time when the algorithm started
auto initialExeTime = chrono::high_resolution_clock::now();

//Auxiliar data strutures that will hold the set of already visited
//and crawled pages, the search ID of each URL successfully crawled,
//the number of web pages crawled from each searched domain and the
//domains that are currently beeing crawled
set <string> visitedPages;
map <string, int> searchedTable;
map <string, int> domainTable;
map <string, bool> crawlingDomain;

//Define the Long-Term Scheduler as priority queue, check webpage.h and .cpp
//for a deeper understanding
priority_queue <TaskUrl> longTermScheduler;	

//Set mutex variables to and unlock global structures and counters to avoid
//malefic concurrence execution in the multi-thread system designed 
mutex searchTableLock, searchIdLock, domainTableLock, longTermSchedulerLock, writeLock, crawlingDomainLock, visitedPagesLock, levelCountLock;

//Check if we already reached the desired number of pages to be crawled
int crawlingFinished(){
	searchIdLock.lock();
	bool finished = searchId >= MAX;
	searchIdLock.unlock();

	return finished;
}

//Try to crawl a given url at a certain level (0 or 1)
//and check for possible mistakes or url repetition
int toCrawl(string url, CkSpider &spider, int level){
	//lock the structure that maintain information
	//about the already crawled URLs
	visitedPagesLock.lock();

	//If we have not crawled it yet we insert it into
	//the already crawled URLs set and collect the 
	//web page
	if(!visitedPages.count(url)){
		visitedPages.insert(url);
		//Unlock the visited structure
		visitedPagesLock.unlock();

		//Crawl the given URL
		bool successfullyCrawled = spider.CrawlNext();
		spider.SleepMs(100);

		if (successfullyCrawled){
			levelCountLock.lock();
			searchTableLock.lock();
			searchIdLock.lock();
			
			//Count if it is a level 0 or level 1 URL
			level == 0 ? countLevelZero++ : countLevelOne++;

			//Set locally the URL unique identifier 
			//and create a new node map that takes the URL
			//to its own id
			int goId = searchId++;
			searchedTable[url] = goId;
			
			levelCountLock.unlock();
			searchIdLock.unlock();
			searchTableLock.unlock();
			
			//Return the crawled URL id
			return goId;
		} 
		//Crawling failed
		else return -2;
	} 
	//Page already crawled
	visitedPagesLock.unlock();
	return -1;
}

//This function takes all meaningful information from a successfully crawled URL and save its info
//in the local disk
int addWebPage(string url, string domain, int id, CkSpider &spider, double crawlingTime, int level){
	string title = spider.lastHtmlTitle();
	string html = spider.lastHtml();

	int htmlSize = sizeof(char)*html.length();
	
	WebPage *nextWebPage = new WebPage(url, domain, title, id, level, crawlingTime, htmlSize);

	nextWebPage->saveInfo(html);

	delete nextWebPage;

	//return the HTML size in bytes
	return htmlSize;
}

//Here we try to crawl each task URL (level 0 URL) properly taken from the Long-Term Scheduler, and 
//update the priority queue with the outbound links found within the page 
int updateLongTermScheduler(CkSpider &taskSpider, string &currTaskUrl, string &currTaskDomain){
	//Computes the crawling execution time
	auto initialCurrCrawExeTime = chrono::high_resolution_clock::now();
	int currTaskID = toCrawl(currTaskUrl, taskSpider, 0);
	auto finalCurrCrawExeTime = chrono::high_resolution_clock::now();

	//If the crawling fails or the URL is already collected we just return
	if (currTaskID == -1 || currTaskID == -2) 
		return -1;

	//Store the collected page and meaningful crawling information on disk
	double crawlingTime = chrono::duration_cast<chrono::microseconds>(finalCurrCrawExeTime - initialCurrCrawExeTime).count();
	addWebPage(currTaskUrl, currTaskDomain, currTaskID, taskSpider, crawlingTime, 0);

	//Check if we already reached the recquired number of pages to collect
	if (crawlingFinished())
		return -1;

	//Run through the list of outbound links and create new potential task URLs
	int outBoundLinks = taskSpider.get_NumOutboundLinks();
	for (int i = 0; i < outBoundLinks; i++){
		
		string nextSeedURL, nextSeedDomain;

		//Check if there's no problem with the url/domain string, some errors were detected when
		//the page referenced URLs as href="http:///"
		try{
			string failSeedDomain(taskSpider.getUrlDomain(taskSpider.getOutboundLink(i)));
			string failSeedUrl(taskSpider.getOutboundLink(i));
		} catch (exception &e){
			continue;
		}

		//Get the next seed url and domain
		nextSeedURL = taskSpider.getOutboundLink(i);
		nextSeedDomain = taskSpider.getUrlDomain(taskSpider.getOutboundLink(i));

		//If the domain is not in the set of domains already found we add it in our current
		//crawling domains as false
		crawlingDomainLock.lock();
		if (crawlingDomain.count(currTaskDomain))
			crawlingDomain[currTaskDomain] = false;
		crawlingDomainLock.unlock();

		//Add the URL to the Long-Term Scheduler creating a new task URL
		longTermSchedulerLock.lock();
		
		TaskUrl newTaskUrl(nextSeedURL, nextSeedDomain, domainTable[nextSeedDomain]);
		longTermScheduler.push(newTaskUrl);
	
		longTermSchedulerLock.unlock();
	}
	taskSpider.SleepMs(100);

	//Return the task/taskurl/url ID
	return currTaskID;
}

//The Short-Term Scheduler created for each task run the uptade of the Long-Term Scheduler
//and control which pages will be collected
void ShortTermScheduler(){
	while (!crawlingFinished()){
		CkSpider taskSpider;
		string currTaskUrl, currTaskDomain;

		//Set the text collected by the spider as utf-8
		taskSpider.put_Utf8(true);

		//Avoid some URL problematic patterns
		taskSpider.AddAvoidOutboundLinkPattern("*facebook.com*");
		taskSpider.AddAvoidOutboundLinkPattern("*instagram.com*");
		taskSpider.AddAvoidOutboundLinkPattern("*twitter.com*");
		taskSpider.AddAvoidOutboundLinkPattern("*amazon.com*");

		int attempts = 50;
		bool startSearching = false, domainAvailable = false;
		//Thiss function assert that the URL taken from the Long-Term Scheduler
		//respect the search politeness and maintain the Thread searching for a
		//new Task URL when none is found in a iteration
		while (!startSearching && !domainAvailable && attempts--){
			if (crawlingFinished())
				return;

			longTermSchedulerLock.lock();
			startSearching = !longTermScheduler.empty();
			//If we have a Task URL available we run through our priority queue
			//till we find a URL with a domain that is not being collected by
			//another thread
			if (startSearching){
				stack <TaskUrl> unavailableTasks;
				//While there are pages on the scheduler or till we find one available
				//we search through the scheduler
				while (!longTermScheduler.empty() && !domainAvailable){
					TaskUrl currTask = longTermScheduler.top();
					longTermScheduler.pop();
					
					currTaskUrl = currTask.url;
					currTaskDomain = currTask.domain;

					//If the URL have its domain available we assert it 
					//and set the domain as not available anymore
					crawlingDomainLock.lock();
					if (crawlingDomain[currTaskDomain] == false){
						domainAvailable = true;
						crawlingDomain[currTaskDomain] = true;
					}
					//Otherwise we add task to stack to be searched later
					else unavailableTasks.push(currTask);

					crawlingDomainLock.unlock();
				}

				//All tasks that were not available 
				//are added back to the scheduler
				while (!unavailableTasks.empty()){
					TaskUrl tmp = unavailableTasks.top();
					unavailableTasks.pop();
					longTermScheduler.push(tmp);
				}
			}
			longTermSchedulerLock.unlock();
			
			//If we reach the total number of attempts we stop
			//the search and return our function deleting the 
			//occious thread
			if (attempts == 0) return;

			//If we still have attempts but were not able
			//to start the search in available domain, the
			//spider sleeps for 2 seconds
			if (!startSearching || !domainAvailable)
				taskSpider.SleepMs(2000);
		}

		//Initialize the spider with in proper domain adding the first 
		//URL to be crawled in the task
		taskSpider.Initialize(currTaskDomain.c_str());
		taskSpider.AddUnspidered(currTaskUrl.c_str());

		//Crawl the first URL and update the Long-Term Scheduler
		int currTaskID = updateLongTermScheduler(taskSpider, currTaskUrl, currTaskDomain);
 		if (currTaskID != -1){ //If a proper id is found we continue the crawling over the page
			//Set variables for average crawling time and page size for each level 1 webpage that
			//will be crawled in the current page
			double currAvgCrawlingTime = 0, currAvgPageSize = 0;

			//Run through the inbound links encountered within the current page
			int inBoundLinks = taskSpider.get_NumUnspidered(), countInboundCrawled = 0; 
			for (int i = 0; i < inBoundLinks; i++){
				string nextUrl, nextDomain;
				
				//Check if there's no problem with the url/domain string, some errors were detected when
				//the page referenced URLs as href="http:///"
				try{
					string failNextDomain(taskSpider.getUrlDomain(taskSpider.getUnspideredUrl(0)));
					string failnextUrl(taskSpider.getUnspideredUrl(0));
				} catch (exception &e){
					taskSpider.SkipUnspidered(0);
					continue;
				}

				nextUrl = taskSpider.getUnspideredUrl(0);
				nextDomain = taskSpider.getUrlDomain(taskSpider.getUnspideredUrl(0));

				//Get the crawling execution time for each successsfully crawled individual level 1 page
				auto initialCurrCrawExeTime = chrono::high_resolution_clock::now();
				int nextUrlID = toCrawl(nextUrl, taskSpider, 1);
				auto finalCurrCrawExeTime = chrono::high_resolution_clock::now();
				
				//If the crawling fails we just skip the level 1 URL
				if (nextUrlID == -1){
					taskSpider.SkipUnspidered(0);
				}
				//Otherwise if we find a new URL page and successfully crawled it
				//we update and store meaningful data about the current level 1 page on disk
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

			//Free the domain to be searched by another task
			crawlingDomainLock.lock();
			crawlingDomain[currTaskDomain] = false;
			crawlingDomainLock.unlock();

			//Update the current domain weight by the number of pages crawled within it
			domainTableLock.lock();
			domainTable[currTaskDomain] += countInboundCrawled;
			domainTableLock.unlock();

			writeLock.lock();

			//Save all meaningful information about the Task, i.e, information about the level 0 page crawled
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
	//Initial number of seeds
	const int numSeeds = 3;

	//Create the initial tasks from the seed urls
	for (int i = 0; i < numSeeds; i++){
		CkSpider auxSpider;
		string str; cin >> str;

		string seedUrl = str.c_str();
		string seedDomain = auxSpider.getUrlDomain(seedUrl.c_str()); 

		//Set the seed domains as available
		crawlingDomain[seedDomain] = false;

		//Add the initial tasks to the Long-Term Scheduler
		TaskUrl newTaskUrl(seedUrl, seedDomain, 0);
		longTermScheduler.push(newTaskUrl);
	}

	//Create the vector of threads
	vector <thread*> jobs;

	//Initialize each task with a Short-Term Scheduler
	for (int i = 0; i < 20; i++){
		thread *task = new thread(ShortTermScheduler);
		jobs.push_back(task);
	}

	//Run each created task
	for (auto &task : jobs){
		task->join();
		delete task;
	}

	cout << "Total number of successfully crawled pages: " << countLevelOne+countLevelZero << endl;
	cout << "Total number of level 0 pages: " << countLevelZero << endl;
	cout << "Total number of level 1 pages: " << countLevelOne << endl;

	return 0;

}