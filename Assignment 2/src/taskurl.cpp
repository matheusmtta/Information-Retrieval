#include "include/taskurl.h"

//Defines the task priority queue priority to pages with the
//lowest url weight and lowest domain weight (prioritizing brazilian pages)
bool operator<(const TaskUrl &lhs, const TaskUrl &rhs){
	if (rhs.urlWgt != lhs.urlWgt) 
		return rhs.urlWgt < lhs.urlWgt;
	return rhs.domainWgt < lhs.domainWgt;
}

//Report task information result into a csv file
//page id, number of successfully crawled urls from that page
//average crawling time per page and average HTML page size in kilobytes
void TaskUrl::report(){
	string path = "tasksInfo.csv";

	fstream checkExistance;
	checkExistance.open(path);
	
	ofstream outfile;
	if (checkExistance.fail()){
		outfile.open(path);
 		outfile << "id,numcrawled,avgCrawlingTime,avgPageSize" << endl;
 	}
	else
	   	outfile.open(path, std::ios_base::app);

 
 	outfile << this->id << "," << this->numCrawledPages << "," << this->avgCrawlingTime << "," << this->avgPageSize << endl;  	

	outfile.close();
}

//Computes the URL weight by taking the number of "." and "/"
double TaskUrl::getWeight(){
	const int INF = 0x3f3f3f3f;
	double weight = 0;

	for (char ch : url)
		if (ch == '.' || ch == '/')
			weight += 1;

	//check if the substring ".br" appears on the URL, since it
	//is a prevalent suffix on brazilian web pages
	for (int i = 0; i < (int)url.size()-2; i++){
		if (url[i] == '.' && url[i+1] == 'b' && url[i+2] == 'r'){
			brDomain = true;
			break;
		}
	}

	//Decrease the weight of potential brazilian URLs by a factor of one
	//or, a more agressive approach, decrease the weight of a potential URL
	//by arbitrarily big number
	
	weight -= (brDomain == true);
	//if (brDomain) return weight - INF;

	return weight;
}

//Set the information for task information reporting
//page id, number of successfully crawled urls from that page
//average crawling time per page and average HTML page size in kilobytes
TaskUrl::TaskUrl(int id, string url, string domain,
		int numCrawledPages, double avgPageSize, double avgCrawlingTime) {
	this->id = id;
	this->url = url; 
	this->domain = domain;
	this->numCrawledPages = numCrawledPages;
	this->avgPageSize = avgPageSize*0.001; //bytes to kilobytes
	this->avgCrawlingTime = avgCrawlingTime;
}