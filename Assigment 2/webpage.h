#ifndef WEB_PAGE_H
#define WEB_PAGE_H

#include <bits/stdc++.h>

using namespace std;

class webPage {
	public:
		string url;
		string domain;

		int id;
		int urlWgt;
		int domainWgt; 

		webPage (string url, string domain, int id, int domainWgt) : url(url), domain(domain), id(id), domainWgt(domainWgt) {
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

bool operator<(const webPage &lhs, const webPage &rhs);

#endif