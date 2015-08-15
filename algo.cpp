#include "algo.h"



vector<vector<int> > lbs(vector<node> list, int add, int mul, int shi)
{
    assert(add > 0 && mul > 0 && shi > 0);
    vector<vector<int> > step;

    int schedule = 0;
    int t = 0;
    // schedule for each time step
    while(schedule != list.size()) 
    {
        vector<int> tmp;
        step.push_back(tmp);
        printf("time step %d:", t);

        // pair <mob, id>
        vector<int_pair> add_list;
        vector<int_pair> mul_list;
        vector<int_pair> shi_list;


        // check each node
        for(int i = 0; i < list.size(); i++)
        {
            node* ptr = &(list[i]);
            if( (!ptr->scheduled) && (!ptr->wait) )
            {
                // put the node to function list
                if(ptr->op == ADD || ptr->op == SUB)
                    add_list.push_back(int_pair(ptr->mob(), ptr->id));
                else if(ptr->op == MUL)
                    mul_list.push_back(int_pair(ptr->mob(), ptr->id));
                else if(ptr->op == SHI)
                    shi_list.push_back(int_pair(ptr->mob(), ptr->id));
            }
        }

        sort(add_list.begin(), add_list.end());
        sort(mul_list.begin(), mul_list.end());
        sort(shi_list.begin(), shi_list.end());

        // scheduling adder
        for(int i = 0; i < add_list.size(); i++)
        {
            if(i == add)
                break;
            int_pair fire = add_list[i];
            int id = fire.second;
            step[t].push_back(id); // push id
            schedule++;
            list[id].scheduled = true;
            printf("%d ", id);
        }

        // scheduling multiplier
        for(int i = 0; i < mul_list.size(); i++)
        {
            if(i == mul)
                break;
            int_pair fire = mul_list[i];
            int id = fire.second;
            step[t].push_back(id); // push id
            schedule++;
            list[id].scheduled = true;
            printf("%d ", id);
        }

        // scheduling shifter
        for(int i = 0; i < shi_list.size(); i++)
        {
            if(i == shi)
                break;
            int_pair fire = shi_list[i];
            int id = fire.second;
            step[t].push_back(id); // push id
            schedule++;
            list[id].scheduled = true;
            printf("%d ", id);
        }


        // trigger following nodes of scheduled nodes
        for(int i = 0; i < step[t].size(); i++)
        {
            // locate the node object by time step and id
            node* ptr = &(list[step[t][i]]);

            // iterate each id of its followers
            for(int j = 0; j < ptr->sucs.size(); j++) 
            {
                list[ptr->sucs[j]].wait--;
            }
        }
        // next time step
        printf("\n");
        t++;
    }
    return step;

}

vector<vector<int> > asap(vector<node> list)
{
    printf("----ASAP----\n");
    vector<vector<int> > step;

    int schedule = 0;
    int t = 0;

    // schedule for each time step
    while(schedule != list.size()) 
    {
        //if(t > 30)
         //   break;
        vector<int> tmp;
        step.push_back(tmp);
        printf("time step %d:", t);
        // check each node
        for(int i = 0; i < list.size(); i++)
        {
            node* ptr = &(list[i]);
            if( (!ptr->scheduled) && (!ptr->wait) )
            {
                // shedule the node to this time step by recording its id
                step[t].push_back(i);
                schedule++;
                ptr->scheduled = true;
                printf("%d ", i);
            }
        }

        // trigger following nodes of scheduled nodes
        for(int i = 0; i < step[t].size(); i++)
        {
            // locate the node object by time step and id
            node* ptr = &(list[step[t][i]]);

            // iterate each id of its followers
            for(int j = 0; j < ptr->sucs.size(); j++) 
            {
                list[ptr->sucs[j]].wait--;
            }
        }
        // next time step
        printf("\n");
        t++;
    }
    return step;
}

vector<vector<int> > alap(vector<node> list)
{
    printf("----ALAP----\n");
    vector<vector<int> > step;

    int schedule = 0;
    int t = 0;
    vector<int> tmp;
    step.push_back(tmp);
    // iterate nodes for last time step
    printf("time step %d:", t);
    for(int i = 0; i < list.size(); i++)
    {
        // find nodes without followers as start point
        if( !list[i].sucs.size() )
        {
            // schedule the node
            list[i].scheduled = true;
            schedule ++;
            step[t].push_back(i);
            printf("%d ", i);
        }
         
    }
    printf("\n");
    t++;

    while( schedule != list.size())
    {
        //if(t > 30)
         //   break;
        vector<int> tmp;
        step.push_back(tmp);
        printf("time step %d:", t);
        // iterate all nodes for this step
        for(int i = 0; i < step[t-1].size(); i++)
        {
            // find its l_pre & r_pre
            node* ptr = &(list[step[t-1][i]]);

            // find a pre_l
            if( ptr->pre_l >= 0 ) 
            {
                if( !list[ptr->pre_l].scheduled )
                {
                    step[t].push_back(ptr->pre_l);
                    list[ptr->pre_l].scheduled = true;
                    schedule ++;
                    printf("%d ", ptr->pre_l);
                }
            }

            // find a pre_r
            if( ptr->pre_r >= 0) 
            {
                if( !list[ptr->pre_r].scheduled )
                {
                    step[t].push_back(ptr->pre_r);
                    list[ptr->pre_r].scheduled = true;
                    schedule ++;
                    printf("%d ", ptr->pre_r);
                }
            }
        }
        printf("\n");
        t++;
    
    }
    return step;
}
