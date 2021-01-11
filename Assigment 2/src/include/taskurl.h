#ifndef TASK_URL_H
#define TASK_URL_H

#include <bits/stdc++.h>

using namespace std;

//The priority queue, long-term scheduler will run with
//objects from this class, which will hold all relevant information
//about a URL that has not been crawled yet
class TaskUrl {
	public:
		string url;
		string domain;

		//check if it is a potential brazilian url
		bool brDomain = false;

		int id;
		int urlWgt;
		int domainWgt; 

		int numCrawledPages;
		double avgCrawlingTime;
		double avgPageSize;


		TaskUrl (string url, string domain, int domainWgt) : url(url), domain(domain), domainWgt(domainWgt) {
			this->urlWgt = getWeight();
		}

		TaskUrl(int id, string url, string domain, int numCrawledPages, double avgPageSize, double avgCrawlingTime);

		void report();
		double getWeight();
};

bool operator<(const TaskUrl &lhs, const TaskUrl &rhs);

#endif