#include "include/webpage.h"

void WebPage::report(){
	cout << "Page ID: " << this->id << endl;
	cout << "=> " << this->title << endl;
	cout << "URL: " << this->url << endl << endl;

	string path = "log/" + to_string(this->id) + ".txt";
	ofstream outfile(path);
	outfile << this->id << " " << this->url << endl;
	outfile << this->title << endl;
	outfile.close(); 
}

void WebPage::saveInfo(string &html){
	string path = "htmlFiles/" + to_string(id) + ".html";
	ofstream outfile(path);
	outfile << html << endl;
	outfile.close();
}