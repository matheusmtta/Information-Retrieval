#ifndef WEB_PAGE_H
#define WEB_PAGE_H

#include <bits/stdc++.h>

using namespace std;

//This Class is used to save all relevant information about
//each individual crawled URL, (url, domain, title,
//crawling id, level 1 or 0, crawling time and page size)
//that will be stored in our local disk
class WebPage {
	public:
		string url;
		string domain;
		string title;

		bool crawled = false;
		int id;
		int level;

		double crawlingTime;
		double pageSize;

		WebPage (string url, string domain, string title, int id, int level, double crawlingTime, double pageSize) : 
			url(url), domain(domain), title(title), id(id), level(level), crawlingTime(crawlingTime), pageSize(pageSize*0.001) {
		}

		void saveInfo(string &html);
};

#endif