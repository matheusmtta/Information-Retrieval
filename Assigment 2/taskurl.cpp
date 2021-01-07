#include "include/taskurl.h"

bool operator<(const TaskUrl &lhs, const TaskUrl &rhs){
	if (rhs.urlWgt != lhs.urlWgt) return rhs.urlWgt < lhs.urlWgt;
	return rhs.domainWgt < lhs.domainWgt;
}

void saveInfo(string url, int crawledPages, double AvgCrawlingTime, int id){
	string path = "crawlingInfo.txt";

	fstream checkExistance;
	checkExistance.open(path);
	
	ofstream outfile;
	if (checkExistance.fail())
		outfile.open(path);
	else
	   	outfile.open(path, std::ios_base::app);

   	string dataID = to_string(id) + " " + url;
   	string dataNum = to_string(crawledPages);
   	outfile << dataID << endl;
   	outfile << dataNum << endl;
   	outfile << AvgCrawlingTime << endl << endl;
	
	outfile.close();
}