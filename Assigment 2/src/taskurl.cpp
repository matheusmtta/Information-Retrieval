#include "include/taskurl.h"

bool operator<(const TaskUrl &lhs, const TaskUrl &rhs){
	if (rhs.urlWgt != lhs.urlWgt) return rhs.urlWgt - (rhs.brDomain == true) > lhs.urlWgt - (lhs.brDomain == true);
	return rhs.domainWgt > lhs.domainWgt;
}

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