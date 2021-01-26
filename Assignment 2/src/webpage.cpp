#include "include/webpage.h"

//Saves in disk all relevant information about 
//the successfully crawled webpages and output
//the current output status for each collected page
void WebPage::saveInfo(string &html){
	cout << "Page ID: " << this->id << endl;
	cout << "=> " << this->title << endl;
	cout << "URL: " << this->url << endl << endl;

	//Save on file the web page id and general
	//relevant information
	string pathGeneralCsv = "pageinfo.csv";
	fstream checkGeneralCsvPath;
	checkGeneralCsvPath.open(pathGeneralCsv);

	ofstream generalCsv;
	if (checkGeneralCsvPath.fail()){
		generalCsv.open(pathGeneralCsv);
		generalCsv << "id,time,size,level" << endl;
	}
	else
	   	generalCsv.open(pathGeneralCsv, std::ios_base::app);

	generalCsv << to_string(this->id) << "," << to_string(this->crawlingTime*1e-6) << "," << to_string(pageSize) << "," << to_string(level) << endl;

	//Save id and its respective URL in a .txt file
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

	//Store a individual file for each HTML code from the pages crawled
	//indexed by the page id
	string path = "htmlFiles/" + to_string(id) + ".html";
	ofstream outfile(path);
	outfile << html << endl;
	outfile.close();
}