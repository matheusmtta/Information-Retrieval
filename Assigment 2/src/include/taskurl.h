#ifndef TASK_URL_H
#define TASK_URL_H

#include <bits/stdc++.h>

using namespace std;

class TaskUrl {
	public:
		string url;
		string domain;

		bool brDomain = false;

		int id;
		int urlWgt;
		int domainWgt; 

		int numCrawledPages;
		double avgCrawlingTime;
		double avgPageSize;

		TaskUrl (string url, string domain, int domainWgt) : url(url), domain(domain), domainWgt(domainWgt) {
			for (char ch : url)
				if (ch == '.' || ch == '/')
					this->urlWgt += 1;

			for (int i = 0; i < (int)url.size()-2; i++){
				if (url[i] == '.' && url[i+1] == 'b' && url[i+2] == 'r'){
					brDomain = true;
					break;
				}
			}
		}

		TaskUrl(int id, string url, string domain,
				int numCrawledPages, double avgPageSize, double avgCrawlingTime) {
			this->id = id;
			this->url = url; 
			this->domain = domain;
			this->numCrawledPages = numCrawledPages;
			this->avgPageSize = avgPageSize*0.001; //bytes to kilobytes
			this->avgCrawlingTime = avgCrawlingTime;
		}

		void report();
};

bool operator<(const TaskUrl &lhs, const TaskUrl &rhs);

#endif