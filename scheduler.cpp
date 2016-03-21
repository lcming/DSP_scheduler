
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
    // processing arguments
    if(argc < 3)
    {
        printf("usage: <input file> <archi>\n");
        exit(1);
    }

    printf("log: read input file %s\n", argv[1]);
    int _archi = atoi(argv[2]);
    int num_add, num_mul, num_shi, n_cas;

    if( _archi == 2) // only for cascade
    {
        if(argc < 5)
        {
            printf("usage: <input file> <archi> <cascade order file> <# of cascade>\n");
            exit(1);
        }
        char* file_name = argv[3];
        n_cas = atoi(argv[4]);
    }
    else            // VLIW or scalar must specify # of each FU type
    {
        if(argc < 6)
        {
            printf("usage: <input file> <archi> <# of adder> <# of multiplier> <# of shifter>\n");
            exit(1);
        }
        num_add = atoi(argv[3]); 
        num_mul = atoi(argv[4]); 
        num_shi = atoi(argv[5]);
        printf("log: %d adders, %d multipliers, %d shifters\n", num_add, num_mul, num_shi);
   
    }
    

    // read file to construct DFG
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
        if(name_op == "ADD" || name_op == "add" || name_op == "SUB" || name_op == "sub")
            node_list[id].init(id, ADD, &node_list);
        else if (name_op == "MUL" || name_op == "mul")
            node_list[id].init(id, MUL, &node_list);
        else if (name_op == "SHI" || name_op == "shi")
            node_list[id].init(id, SHI, &node_list);
        else
            printf("fatal!, unknown op name %s\n", name_op.c_str());
        //printf("%d  %s\n", id, name_op.c_str());
    }
    
    // read data flow
    int from, to;
    while(fin >> from >> to)
    {
        node_list[from].connect(to);
    }

    // find ts(asap) & tl(alap) for each node
    vector<vector<int> > _asap = asap(node_list, 0);
    vector<vector<int> > _alap = alap(node_list, 0);

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

    // start profiling
    if (_archi == vliw) 
    {
        vector<vector<int> > _lbs = vliw_lbs(node_list, num_add, num_mul, num_shi);
        printf("vliw_total %s %d\n", argv[1], _lbs.size());
        float opc = (float)(num_op) / _lbs.size();
        printf("opc = %f\n", opc);
    }
    else if(_archi == scalar)
    {
        vector<vector<int> > _lbs = scalar_lbs(node_list, num_add, num_mul, num_shi);
        printf("scalar_total %s %d\n", argv[1], _lbs.size());
    }
    else if (_archi == cascade)
    {
        vector<vector<int> > _lbs = cascade_coverage(node_list, argv[3], n_cas);
        printf("cascade_total %s %d\n", argv[1], _lbs.size());
        float opc = (float)(num_op) / _lbs.size();
        printf("opc = %f\n", opc);
    }
    else
    {
        printf("error: the archi %d is undefined\n", _archi);
    }

    return 0;
}
