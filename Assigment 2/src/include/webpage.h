#ifndef WEB_PAGE_H
#define WEB_PAGE_H

#include <bits/stdc++.h>

using namespace std;

class WebPage {
	public:
		string url;
		string domain;
		string title;

		bool crawled = false;
		int level;
		int id;

		double crawlingTime;
		double pageSize;

		WebPage (string url, string domain, string title, int id, double crawlingTime, double pageSize) : 
			url(url), domain(domain), title(title), id(id), crawlingTime(crawlingTime), pageSize(pageSize*0.001) {
		}

		void saveInfo(string &html);
};

#endif