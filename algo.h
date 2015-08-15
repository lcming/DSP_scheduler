#ifndef _ALGO_H
#define _ALGO_H

#include "dfg.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <map>

using namespace std;

typedef pair<int, int> int_pair;
typedef map<int, int> int_map;


vector<vector<int> > asap(vector<node> list);

vector<vector<int> > alap(vector<node> list);

vector<vector<int> > lbs(vector<node> list, int add, int mul, int shi);



#endif
