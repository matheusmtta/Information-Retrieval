#ifndef TASK_URL_H
#define TASK_URL_H

#include <bits/stdc++.h>

using namespace std;

class TaskUrl {
	public:
		string url;
		string domain;

		int id;
		int urlWgt;
		int domainWgt; 

		int numCrawledPages;
		double avgCrawlingTime;

		TaskUrl (string url, string domain, int domainWgt) : url(url), domain(domain), domainWgt(domainWgt) {
			for (char ch : url)
				if (ch == '.' || ch == '/')
					this->urlWgt += 1;
		}
};

bool operator<(const TaskUrl &lhs, const TaskUrl &rhs);
void saveInfo(string url, int crawledPages, double AvgCrawlingTime, int id);

#endif