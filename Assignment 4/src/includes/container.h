#ifndef CONTAINER_H
#define CONTAINER_H

#include <bits/stdc++.h>

using namespace std;

struct termContainer {
	int id;
	int doc;
	int pos;

	termContainer(int id, int doc, int pos) : id(id), doc(doc), pos(pos) {}
};

bool comp(const termContainer &lhs, const termContainer &rhs);

termContainer getContainer(string tuple);

#endif