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

		int numInboundCrawledPages;
		int numOutboundCrawledPages;
		double avgSearchTime;

		WebPage (string url, string domain, string title, int level, int id) : 
			url(url), domain(domain), title(title), level(level), id(id) {
		}

		void saveInfo(string &html){
			string path = "htmlFiles/" + to_string(id) + ".html";
			ofstream outfile(path);
			outfile << html << endl;
			outfile.close();
		}

		void report(){
			cout << "Page ID: " << id << endl;
			cout << "=> " << title << endl;
			cout << "URL: " << url << endl << endl;
		}
};

#endif