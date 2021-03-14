#ifndef PARSER_H
#define PARSER_H

#include <bits/stdc++.h>
#include "gumbo.h"

using namespace std;

class Parser {
	public:
		string cleantext(GumboNode* node);
		
		string getCleanHTML(string);
		string getCleanQuery(string);
		string getCleanText(string);
};

#endif