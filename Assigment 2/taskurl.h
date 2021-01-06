#ifndef TASK_URL_H
#define TASK_URL_H

#include <bits/stdc++.h>

using namespace std;

class TaskUrl {
	public:
		string url;
		string domain;

		int id;
		//int parentID
		int urlWgt;
		int domainWgt; 

		TaskUrl (string url, string domain, int domainWgt) : url(url), domain(domain), domainWgt(domainWgt) {
			getUrlWeight();
		}

		int getUrlWeight(){
			int weight = 0;
			for (char ch : url)
				if (ch == '.' || ch == '/')
					weight += 1;
			return weight;
		}
};

bool operator<(const TaskUrl &lhs, const TaskUrl &rhs);

#endif