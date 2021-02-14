#ifndef INVERTED_LIST_H
#define INVERTED_LIST_H

#include <bits/stdc++.h>

using namespace std;

struct termContainer {
	int id;
	int doc;
	int pos;

	termContainer(int id, int doc, int pos) : id(id), doc(doc), pos(pos) {}
};

void mergeInvertedLists();
void buildDictionary();

bool comp(const termContainer &lhs, const termContainer &rhs);

termContainer getContainer(string tuple);

#endif