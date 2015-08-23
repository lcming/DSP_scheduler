#ifndef _ALGO_H
#define _ALGO_H

#include "dfg.h"
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <map>

using namespace std;

typedef pair<int, int> int_pair;
typedef map<int, int> int_map;


typedef enum
{
    scalar,
    vliw,
    cascade
} archi;


int generate_combos(int n, int k);

vector<vector<int> > asap(vector<node> list);

vector<vector<int> > alap(vector<node> list);

vector<vector<int> > vliw_lbs(vector<node> list, int add, int mul, int shi);
vector<vector<int> > scalar_lbs(vector<node> list, int add, int mul, int shi);
vector<vector<int> > cascade_coverage(vector<node> list, char* file_name);

vector<int> rec_search(const vector<node>& list, int* node_status, int id, operation* cas_fu, int cur, int end, vector<int> vec_in);



#endif
