#include "include/webpage.h"

void WebPage::saveInfo(string &html){
	cout << "Page ID: " << this->id << endl;
	cout << "=> " << this->title << endl;
	cout << "URL: " << this->url << endl << endl;

	string pathGeneralCsv = "pageinfo.csv";
	fstream checkGeneralCsvPath;
	checkGeneralCsvPath.open(pathGeneralCsv);

	ofstream generalCsv;
	if (checkGeneralCsvPath.fail()){
		generalCsv.open(pathGeneralCsv);
		generalCsv << "id,time,size" << endl;
	}
	else
	   	generalCsv.open(pathGeneralCsv, std::ios_base::app);

	generalCsv << to_string(this->id) << "," << this->crawlingTime*1e-6 << "," << pageSize << endl;

	string pathCompleteLog = "completeLog.txt";
	fstream checkpathCompleteLog;
	checkpathCompleteLog.open(pathCompleteLog);

	ofstream completeLogTxt;
	if (checkpathCompleteLog.fail())
		completeLogTxt.open(pathCompleteLog);
	else
	   	completeLogTxt.open(pathCompleteLog, std::ios_base::app);

	completeLogTxt << this->id << " " << this->url << endl << endl;
	completeLogTxt.close();

	string path = "htmlFiles/" + to_string(id) + ".html";
	ofstream outfile(path);
	outfile << html << endl;
	outfile.close();
}