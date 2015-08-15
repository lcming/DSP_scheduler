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
    SUB,
    MUL,
    SHI
};

class node
{
    public:
        operation op;      // op type
        int id;
        int step;          // fianl scheduled step
        int tl;            // time of alap
        int ts;            // time of asap
        bool scheduled;
        node* node_list;
        int wait;          // # of operands it waits for
        int pre_l, pre_r;  // left & right predecessor IDs
        vector<int> sucs;  // successors IDs

        node();
        ~node();
        void init(int _id, operation _op);
        void connect(int target);   // connet to a successor
        void connected(int source); // connet by a predecessor
        void display(int level);    // show status
        int  mob();


};

void display(vector<node>& node_list, int size, int level);

#endif
