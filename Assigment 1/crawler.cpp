#include <CkSpider.h>
#include <iostream>
#include <chrono>
#include <string>

using namespace std;

void pageDisplay(int idx, string url, string title){ //Display Title and URL
	cout << "Page " << idx << "\n";
	cout << "Title: " << title << "\r\n";
	cout << "URL: " << url   << "\r\n" << "\r\n";
}

int main(){
	string firstURL; cin >> firstURL; //Read URL
	int n; cin >> n; //Read number of additional pages to be crawled

	CkSpider spider; //Create spider and add the first URL to be collected
	spider.Initialize(firstURL.c_str());
	spider.AddUnspidered(firstURL.c_str());

  //Initialize execution time counter and successful crawled pages counter 
	double exeTime = 0, k = 0;
	
	for (int i = 0; i < n+1; i++){ 	//Crawl through n+1 url link webpages
	  //set initial crawling time and final crawling time
		auto initialExeTime = chrono::high_resolution_clock::now();
		bool found = spider.CrawlNext();
		auto finalExeTime = chrono::high_resolution_clock::now();
      
		//If a link page is successfully found, we display it,
		if (found){ //and add the crawling time to our execution time counter
			pageDisplay(i, spider.lastUrl(), spider.lastHtmlTitle());
			exeTime += chrono::duration_cast<chrono::microseconds>(finalExeTime - initialExeTime).count();
			k++;
		} 
		else { //There're no more page links to be crawled in our inital url
			if (!spider.get_NumUnspidered()){
				cout << "There're no more pages to be crawled" << endl;
				break;
			}
			else //Or an error has been found
				cout << spider.lastErrorText() << "\r\n" << endl;
		}

		spider.SleepMs(1000);
	}
  //Find total and average program execution time
	double avgExeTime = (exeTime*1e-6)/k;

	cout << "Successfully crawled links: " << k << endl;
	cout << "Total crawling execution time(s): " << exeTime*1e-6 << endl;
	cout << "Average crawling execution time(s): " << avgExeTime << endl;
	
	return 0;
}