
#include "dfg.h"
#include "algo.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;


int main(int argc, char* argv[])
{
    if(argc < 5)
    {
        printf("usage: <input file> <# of adder> <# of multiplier> <# of shifter>\n");
        exit(1);
    }
    printf("log: read input file %s\n", argv[1]);
    int num_add = atoi(argv[2]); 
    int num_mul = atoi(argv[3]); 
    int num_shi = atoi(argv[4]);
    int _archi = atoi(argv[5]);
    
    printf("log: %d adders, %d multipliers, %d shifters\n", num_add, num_mul, num_shi);

    ifstream fin;
    fin.open(argv[1]);
    if(fin.fail())
    {
        printf("invalid input file '%s'\n", argv[1]);
        exit(1);
    }
    int num_op;
    int id;
    string name_op;
    fin >> num_op;
    printf("number of operation = %d\n", num_op);
    vector<node> node_list;
    node_list.resize(num_op);

    // read operations
    for(int i = 0; i < num_op; i++)
    {
        fin >> id >> name_op; 
        //assert( id == i);
        if(name_op == "ADD" || name_op == "add")
            node_list[id].init(id, ADD);
        else if (name_op == "SUB" || name_op == "sub")
            node_list[id].init(id, SUB);
        else if (name_op == "MUL" || name_op == "mul")
            node_list[id].init(id, MUL);
        else if (name_op == "SHI" || name_op == "shi")
            node_list[id].init(id, SHI);
        else
            printf("fatal!, unknown op name %s\n", name_op.c_str());
        //printf("%d  %s\n", id, name_op.c_str());
    }
    
    // read data flow
    int from, to;
    while(fin >> from >> to)
    {
        node_list[from].connect(to);
        node_list[to].connected(from);
    }

    // find ts(asap) & tl(alap) for each node
    vector<vector<int> > _asap = asap(node_list);
    vector<vector<int> > _alap = alap(node_list);

    // update ts for asap
    // iterate each time step
    for(int i = 0; i < _asap.size(); i++)
    {
        // update for each node
        for(int j = 0; j < _asap[i].size(); j++) 
        {
            id = _asap[i][j];
            node_list[id].ts = i;
        }
    }


    // update tl for asap
    // iterate each time step
    for(int i = 0; i < _alap.size(); i++)
    {
        // update for each node
        for(int j = 0; j < _alap[i].size(); j++) 
        {
            id = _alap[i][j];
            // reverse the time step because we did backward analysis
            node_list[id].tl = _asap.size()-1-i;
        }
    }

    printf("debug: mobility: ");
    for(int i = 0; i < node_list.size(); i++)
        printf("%d ", node_list[i].mob());
    printf("\n");
    if (_archi == vliw) 
    {
        vector<vector<int> > _lbs = vliw_lbs(node_list, num_add, num_mul, num_shi);
        printf("vliw_total %s %d\n", argv[1], _lbs.size()+1);
    }
    else if(_archi == scalar)
    {
        vector<vector<int> > _lbs = scalar_lbs(node_list, num_add, num_mul, num_shi);
        printf("scalar_total %s %d\n", argv[1], _lbs.size()+1);
    }

    //display(node_list, num_op, 2);
    return 0;
}
