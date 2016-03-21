
#include "dfg.h"

using namespace std;


node::node()
{}
node::~node()
{}

void node::init(int _id, operation _op, vector<node>* _node_list)
{
    id = _id;
    op = _op;
    super = -1;
    rf_id = -1;
    step = -1;
    tl = -1;
    ts = -1;
    scheduled = false;
    node_list = _node_list;
    wait = 0;
    if(op == SHI)
        wait_mem = 1;
    else
        wait_mem = 2;
}

int node::mob()
{
    //assert(tl >= ts);
    return (tl - ts);
}


void node::set_src_regs(int src_id, int src_rf)
{
    wait --;
    // find the index of pre and fill rf_pre
    for(int i = 0; i < pres.size(); i++)
    {
        if(src_id == pres[i]) 
        {
           rf_pres[i] = src_rf;
           break;
        }
    }
}

void node::display(int level)
{
    /*
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
    */
}

void node::connect(int target)
{
    sucs.push_back(target);
    (*node_list)[target].pres.push_back(id);
    (*node_list)[target].rf_pres.push_back(-1);
    (*node_list)[target].wait++;
    (*node_list)[target].wait_mem--;
}
/*
void node::connected(int id)
{
    pres.push_back(id);
    rf_pres.push_back(-1);
    wait++;
    wait_mem--;
}
*/
void node::reconnected(int id_new, int id_old)
{
    // replace the old pre

    for(int i = 0; i < pres.size(); i++)
    {
       if(pres[i] == id_old)  
       {
            pres[i] = id_new;
            printf("reconnected: %d from %d to %d\n", id, id_old, id_new);
            break;
       }
    }
    (*node_list)[id_new].sucs.push_back(id);
    /*
    printf("debug: add %d to %d's sucs\n", id, id_new);
    
    printf("debug: id:%d, pres:", id);
    for(int i = 0; i < pres.size(); i++)
        printf("%d ", pres[i]);
    printf("\n");
    */


}

void node::reconnect(int id_new, int id_old)
{
    // replace the old suc
    // since the reconnect check applied to all cascaded nodes, 
    // we should never reconnect to the node itself
    for(int i = 0; i < sucs.size(); i++)
    {
        if(sucs[i] == id_old) 
        {
            sucs[i] = id_new;
            printf("reconnect: %d from %d to %d\n", id, id_old, id_new);
            break;
        }
    }

    // add more pres on the super node
    (*node_list)[id_new].pres.push_back(id);
    (*node_list)[id_new].rf_pres.push_back(-1);
    (*node_list)[id_new].wait++;
    
    /*
    printf("debug: id:%d, sucs:", id);
    for(int i = 0; i < sucs.size(); i++)
        printf("%d ", sucs[i]);
    printf("\n");

    */
    
}

void node::show_inst()
{
    if(super < 0)
    {
        switch(op)
        {
            case ADD: 
                printf("<ADD #%d ", rf_id);
                break;
            case MUL:
                printf("<MUL #%d ", rf_id);
                break;
            case SHI:
                printf("<SHI #%d", rf_id);
                break;
            default:
                printf("show_inst() error: undefined operation");
        }
        for(int i = 0; i < rf_pres.size(); i++)
            printf("#%d ", rf_pres[i]);
        for(int i = 0; i < rf_mem.size(); i++)
            printf("#%d ", rf_mem[i]);

        printf(">");
    }
    else
    {
        printf("<");
        for(int i = 0; i < super; i++)
        {
            switch(op_list[i])
            {
                case ADD: 
                    printf("ADD ");
                    break;
                case MUL:
                    printf("MUL ");
                    break;
                case SHI:
                    printf("SHI ");
                    break;
                default:
                    printf("show_inst() error: undefined operation");
            }
 
        }
        printf("#%d ", rf_id);

        for(int i = 0; i < rf_pres.size(); i++)
            printf("#%d ", rf_pres[i]);
        for(int i = 0; i < rf_mem.size(); i++)
            printf("#%d ", rf_mem[i]);

        printf(">");
    }

}



void display(vector<node>& node_list, int size, int level)
{
    for(int i = 0; i < size; i++)
        node_list[i].display(level);
}

