#include <CkSpider.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>

using namespace std;

void pageDisplay(int idx, string url, string title){
	if (idx == 0) cout << "\n";
	cout << "Page " << idx << "\n";
	cout << "Title: " << title << "\r\n";
	cout << "URL: " << url   << "\r\n" << "\r\n";
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

	//Execution time variable and number of successfully
	//crawled url links 
	double exeTime = 0, k = 0;
	
	//Crawl through n+1 url link webpages, 
	//starting at the url given at the input 
	for (int i = 0; i < n+1; i++){
		auto initialExeTime = chrono::high_resolution_clock::now();
		bool found = spider.CrawlNext();
		auto finalExeTime = std::chrono::high_resolution_clock::now();

		//If a link page is successfully found, we display it,
		//and add the crawling time to our execution time variable
		if (found){
			pageDisplay(i, spider.lastUrl(), spider.lastHtmlTitle());
			exeTime += chrono::duration_cast<std::chrono::microseconds>(finalExeTime - initialExeTime).count();
			k++;
		} 
		//Otherwise,
		else {
			//There're no more page links to be crawled in our inital url
			if (!spider.get_NumUnspidered()){
				cout << "There're no more pages to be crawled" << endl;
				break;
			}
			//An erros has been found
			else
				cout << spider.lastErrorText() << "\r\n" << endl;
		}

		spider.SleepMs(1000);
	}

	//Find total and average program execution time, desconsidering the sleep
	//delay of 1 second per page
	double avgExeTime = (exeTime*1e-6)/k;

	cout << "Successfully crawled links: " << k << endl;
	cout << "Total crawling execution time(s): " << exeTime*1e-6 << endl;
	cout << "Average crawling execution time(s): " << avgExeTime << endl;
}