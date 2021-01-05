#ifndef WEB_PAGE_H
#define WEB_PAGE_H

#include <bits/stdc++.h>

using namespace std;

class webPage {
	public:
		string url;
		string domain;
		string title;

		int crawlingID;
		int parentID;
		int searchID;
		int urlWgt;
		int domainWgt; 

		webPage (string url, string domain, int domainWgt = 0) : url(url), domain(domain), domainWgt(domainWgt) {
			getUrlWeight();
		}

		int getUrlWeight(){
			int weight = 0;
			for (char ch : url)
				if (ch == '.' || ch == '/')
					weight += 1;
			return weight;
		}

		// void setCrawlingID(int crawlingID){
		// 	this->crawlingId = id;
		// }

		// void setSearchID(int searchID){
		// 	this->searchId = id;
		// }

		// void setParent(int parentID){
		// 	this->parentID = parentID;
		// }

		// void setTitle(string title){
		// 	this->title = title;
		// }

		//void saveInfo(){

		//}
};

bool operator<(const webPage &lhs, const webPage &rhs);

#endif