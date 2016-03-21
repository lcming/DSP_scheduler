#ifndef _DFG_H
#define _DFG_H

#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

enum operation
{
    ADD,
    MUL,
    SHI
};

class node
{
    public:
        operation op;      // op type
        operation* op_list; // only used by super node
        int super;
        int id;
        int step;          // fianl scheduled step
        int tl;            // time of alap
        int ts;            // time of asap
        bool scheduled;
        vector<node>* node_list;
        int wait_mem;
        int wait;          // # of operands it waits for
        int rf_id;
        vector<int> sucs;  // successors IDs
        vector<int> pres;  // predecessor, only used in super mode
        vector<int> rf_pres;  // predecessor, only used in super mode
        vector<int> rf_mem;

        node();
        ~node();
        void init(int _id, operation _op, vector<node>* _node_list);
        void connect(int target);   // connet to a successor
        void connected(int source); // connet by a predecessor
        void reconnect(int id_new, int id_old);
        void reconnected(int id_new, int id_old);
        void display(int level);    // show status
        void set_src_regs(int src_id, int src_rf);
        void show_inst();
        int  mob();


};

void display(vector<node>& node_list, int size, int level);

#endif
