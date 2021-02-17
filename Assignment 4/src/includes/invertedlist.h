#ifndef INVERTED_LIST_H
#define INVERTED_LIST_H

#include "parser.h"
#include "../rapidjson/document.h"
#include "../rapidjson/stringbuffer.h"

#include <bits/stdc++.h>

using namespace std;

struct termContainer {
	int id;
	int doc;
	int pos;

	termContainer(int id, int doc, int pos) : id(id), doc(doc), pos(pos) {}
};

void checkFile(ifstream &file, string path);

bool comp(const termContainer &lhs, const termContainer &rhs);
termContainer getContainer(string tuple);

void buildInvertedLists();
void mergeInvertedLists();

#endif