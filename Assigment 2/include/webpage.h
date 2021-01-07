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

		WebPage (string url, string domain, string title, int level, int id) : 
			url(url), domain(domain), title(title), level(level), id(id) {
		}

		void saveInfo(string &html);
		void report();
};

#endif