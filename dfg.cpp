
#include "dfg.h"

using namespace std;


node::node()
{}
node::~node()
{}

void node::init(int _id, operation _op)
{
    id = _id;
    op = _op;
    pre_l = -1;
    pre_r = -1;
    step = -1;
    tl = -1;
    ts = -1;
    scheduled = false;
    wait = 0;
}

int node::mob()
{
    //assert(tl >= ts);
    return (tl - ts);
}

void node::display(int level)
{
    char* opname;
    switch(op)
    {
        case ADD: 
            opname = "ADD";
            break;
        case SUB:
            opname = "SUB";
            break;
        case MUL:
            opname = "MUL";
            break;
        case SHI:
            opname = "SHI";
        break;
    }
    if(op == SHI)
        printf("node<%d, %s>  <== %d\n", id, opname, pre_l);
    else
        printf("node<%d, %s>  <== %d, %d\n", id, opname, pre_l, pre_r);

    if(level > 1)
    {
        printf("==> ");
        for(int i = 0; i < sucs.size(); i++)    
            printf("%d, ", sucs[i]);
        printf("\n");
    }
}

void node::connect(int id)
{
   sucs.push_back(id);
}

void node::connected(int id)
{
    if(pre_l == -1)  // has no pre now
        pre_l = id;
    else             // already has one
        pre_r = id;
    wait++;
}


void display(vector<node>& node_list, int size, int level)
{
    for(int i = 0; i < size; i++)
        node_list[i].display(level);
}

