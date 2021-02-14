#ifndef INVERTED_LIST_H
#define INVERTED_LIST_H

#include <bits/stdc++.h>

using namespace std;

struct termContainer {
	//int id;
	string word;
	int doc;
	int pos;

	//termContainer(int id, int doc, int pos) : id(id), doc(doc), pos(pos) {}
	termContainer(string word, int doc, int pos) : word(word), doc(doc), pos(pos) {}
};

termContainer getContainer(string tuple);

bool comp(const termContainer &lhs, const termContainer &rhs);

#endif