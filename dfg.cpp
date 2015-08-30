
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
    super = -1;
    rf_id = -1;
    pre_l = -1;
    pre_r = -1;
    rf_pre_l = -1;
    rf_pre_r = -1;
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


void node::set_src_regs(int src_id, int src_rf)
{
    wait --;
    if(src_id == pre_l) // is a left parent
    {
        rf_pre_l = src_rf; 
    }
    else if(src_id == pre_r) // is a right parent
    {
        rf_pre_r = src_rf; 
    }
    else
    {
        printf("set_suc_reg matching parents fail! <left right this> <%d %d %d>\n", pre_l, pre_r, src_rf);
    }
}

void node::display(int level)
{
    char* opname;
    switch(op)
    {
        case ADD: 
            opname = "ADD";
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

void node::show_inst()
{
    switch(op)
    {
        case ADD: 
            printf("<ADD #%d #%d #%d>  ", rf_id, rf_pre_l, rf_pre_r);
            break;
        case MUL:
            printf("<MUL #%d #%d #%d>  ", rf_id, rf_pre_l, rf_pre_r);
            break;
        case SHI:
            printf("<SHI #%d #%d>  ", rf_id, rf_pre_l);
            break;
        default:
            printf("show_inst() error: undefined operation");
    }

}



void display(vector<node>& node_list, int size, int level)
{
    for(int i = 0; i < size; i++)
        node_list[i].display(level);
}

