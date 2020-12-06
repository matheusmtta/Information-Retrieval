#include <CkSpider.h>
#include <iostream>
#include <chrono>
#include <string>

using namespace std;

void pageDisplay(int idx, string url, string title){
	if (idx == 0) cout << "\n";
	cout << "Page " << idx << "\n";
	cout << title << "\r\n";
	cout << url   << "\r\n" << "\r\n";
}

int main(){
	//Input given parameters, inital url
	//and number of n link pages to be crawled
	//from it
	string firstURL; cin >> firstURL;
	int n; cin >> n;

	//Initialize the spider object
	//with the input link url
	CkSpider spider;

	spider.Initialize(firstURL.c_str());
	spider.AddUnspidered(firstURL.c_str());

	auto initialExeTime = chrono::high_resolution_clock::now();

	//Crawl through n+1 link webpages, 
	//beggining at the url given at the input 
	for (int i = 0; i < n+1; i++){
		bool found = spider.CrawlNext();
		
		//If a link page is found, we display it
		if (found)
			pageDisplay(i, spider.lastUrl(), spider.lastHtmlTitle());
		//Otherwise,
		else {
			//There're no more page links to be crawled in our inital url
			if (!spider.get_NumUnspidered())
				cout << "There're no more pages to be crawled" << endl;
			//An erros has been found
			else
				cout << spider.lastErrorText() << "\r\n" << endl;
		}

		spider.SleepMs(1000);
	}

	auto finalExeTime = std::chrono::high_resolution_clock::now();

	//Find total and average program execution time, desconsidering the sleep
	//delay of 1 second per page
	double exeTime = chrono::duration_cast<std::chrono::microseconds>(finalExeTime - initialExeTime).count(); 
	double avgExeTime = (exeTime*1e-6)/(n+1) - 1;

	cout << "Average crawling time in seconds: " << avgExeTime << endl;
}