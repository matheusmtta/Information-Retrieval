#include "include/taskurl.h"

bool operator<(const TaskUrl &lhs, const TaskUrl &rhs){
	if (rhs.urlWgt != lhs.urlWgt) return rhs.urlWgt < lhs.urlWgt;
	return rhs.domainWgt < lhs.domainWgt;
}

void TaskUrl::report(){
	string path = "tasksInfo.txt";

	fstream checkExistance;
	checkExistance.open(path);
	
	ofstream outfile;
	if (checkExistance.fail())
		outfile.open(path);
	else
	   	outfile.open(path, std::ios_base::app);

   	string dataID = "URL ID: ";
   	string dataURL = "URL: '" + url + "'";
   	string dataNum = "Level 1 pages crawled: ";
   	string dataTime = "Average crawling time ";
   	string dataSize = "Average pages size ";
   	
   	outfile << dataID << id << endl;
   	outfile << dataURL << endl;
   	outfile << dataNum << numCrawledPages << endl;
   	outfile << dataTime << avgCrawlingTime << " seconds." << endl;
   	outfile << dataSize << avgPageSize << " kilobytes." << endl << endl;
	
	outfile.close();
}