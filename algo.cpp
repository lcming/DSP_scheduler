#include "algo.h"

vector<vector<int> > cascade_coverage(vector<node> list, char* file_name)
{
    ifstream fin2(file_name);
    int n_fu, tmp;
    fin2 >> n_fu;
    operation* cas_fu = (operation*)malloc(sizeof(operation)*n_fu);
    for(int i = 0; i < n_fu; i++)
    {
        fin2>>tmp;
        switch(tmp)
        {
            case 0:
                cas_fu[i] = ADD;
                break;

            case 1:
                cas_fu[i] = MUL;
                break;
                
            case 2:
                cas_fu[i] = SHI;
                break;

            default:
                printf("error: undefined FU %d\n", tmp);
        }
    }
    vector<vector<int> > step;

    // a list to record node scheduling status, initialized as 0
    int* node_status = (int*)calloc(sizeof(int), list.size());
    int schedule = 0;
    int t = 0;

    // for each level
    for(int i = n_fu; i >= 1; i--)
    {
        printf("level %d\n", i);
        int cnt = generate_combos(n_fu, i);
        operation* comb = (operation*)malloc(sizeof(operation)*i);
        fstream fin("comb.txt");
        int pick;
        for(int k = 0; k < cnt; k++)
        {
            for(int j = 0; j < i; j++)
            {
                fin >> pick;
                // pick up an index, then get the fu
                comb[j] = cas_fu[pick];
                //printf("%d ", comb[j]);
            }
            //printf("\n");
            // coverage for this combination, check for each node
            for(int m = 0; m < list.size(); m++)
            {
                // try to find a pattern mached
                vector<int> match;
                match = rec_search(list, node_status, m, comb, 0, i, match);

                // a full matched pattern found
                if(match.size() == i)
                {
                    printf("time step %d:", t);
                    vector<int> tmp;
                    schedule += i; 
                    for(int k = 0; k < match.size(); k++)
                    {
                        node_status[match[k]] = 1;  // update node status
                        tmp.push_back(match[k]);
                        printf("%d ", match[k]);
                    }
                    printf("\n");
                    step.push_back(tmp);
                    t++;
                }
                if(schedule == list.size())
                    break;
            
            }
        }
        fin.close();

    }

    return step;

   
}


vector<int> rec_search(const vector<node>& list, int* node_status, int id, operation* cas_fu, int cur, int end, vector<int> vec_in)
{
    // not match or already scheduled, return directly
    if(cas_fu[cur] != list[id].op || node_status[id] == 1)
        return vec_in;

    vec_in.push_back(id);
    vector<int> vec_cpy = vec_in;
    printf("debug: hit %d type: %d\n", id, list[id].op);

    // is it the end of cascade
    if(cur == end) 
    {
        printf("debug: return size: %d\n", vec_cpy.size());
        return vec_cpy;
    }

    // hit, then find its succesors
    for(int i = 0; i < list[id].sucs.size(); i++) 
    {
        printf("debug: call size: %d\n", vec_in.size());
        vec_cpy = rec_search(list, node_status, list[id].sucs[i], cas_fu, cur+1, end, vec_in);
        if(vec_cpy.size() > vec_in.size())
            break;
    }
    printf("debug: return size: %d\n", vec_cpy.size());
    return vec_cpy;
}



vector<vector<int> > scalar_lbs(vector<node> list, int add, int mul, int shi)
{
    assert(add > 0 && mul > 0 && shi > 0);
    assert(add == mul && shi == mul);
    vector<vector<int> > step;

    int schedule = 0;
    int t = 0;
    int reg_cnt = 0;
    // schedule for each time step
    while(schedule != list.size()) 
    {
        vector<int> tmp;
        step.push_back(tmp);
        printf("time step %d:", t);

        // pair <mob, id>
        vector<int_pair> fu_list;


        // check each node
        for(int i = 0; i < list.size(); i++)
        {
            node* ptr = &(list[i]);
            if( (!ptr->scheduled) && (!ptr->wait) )
            {
               // put the node to function list
                fu_list.push_back(int_pair(ptr->mob(), ptr->id));
            }
        }

        sort(fu_list.begin(), fu_list.end());

        // scheduling fu
        for(int i = 0; i < fu_list.size(); i++)
        {
            if(i == add) // full
                break;
            int_pair fire = fu_list[i];
            int id = fire.second;
            node* ptr = &(list[id]);
            step[t].push_back(id); // push id
            schedule++;
            list[id].scheduled = true;
            //printf("%d ", id);

            // source operand not exsit, must be loaded from mem
            if(ptr->pre_l == -1)    // load operand from mem
            {
                printf("<LD #%d>  ", reg_cnt);
                ptr->rf_pre_l = reg_cnt++;  // use the reg
            }

            // considering two operands case, so check the right
            if(ptr->op != SHI && ptr->pre_r == -1)
            {
                printf("<LD #%d>  ", reg_cnt);
                ptr->rf_pre_r = reg_cnt++;  // use the reg
            }
            ptr->rf_id = reg_cnt++;
            ptr->show_inst();
        }


        // trigger following nodes of scheduled nodes
        for(int i = 0; i < step[t].size(); i++)
        {
            // locate the node object by time step and id
            node* ptr = &(list[step[t][i]]);

            // iterate each id of its followers
            for(int j = 0; j < ptr->sucs.size(); j++) 
            {
                list[ptr->sucs[j]].set_src_regs(ptr->id, ptr->rf_id);
            }
        }
        // next time step
        printf("\n");
        t++;
    }
    return step;


}

vector<vector<int> > vliw_lbs(vector<node> list, int add, int mul, int shi)
{
    assert(add > 0 && mul > 0 && shi > 0);
    vector<vector<int> > step;

    int schedule = 0;
    int t = 0;
    int reg_cnt = 0;
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
                if(ptr->op == ADD)
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
            node* ptr = &(list[id]);
            step[t].push_back(id); // push id
            schedule++;
            ptr->scheduled = true;
            //printf("%d ", id);

            // source operand not exsit, must be loaded from mem
            if(ptr->pre_l == -1)    // load operand from mem
            {
                printf("<LD #%d>  ", reg_cnt);
                ptr->rf_pre_l = reg_cnt++;  // use the reg
            }

            // considering two operands case, so check the right
            if(ptr->op != SHI && ptr->pre_r == -1)
            {
                printf("<LD #%d>  ", reg_cnt);
                ptr->rf_pre_r = reg_cnt++;  // use the reg
            }
            ptr->rf_id = reg_cnt++;
            ptr->show_inst();

        }

        // scheduling multiplier
        for(int i = 0; i < mul_list.size(); i++)
        {
            if(i == mul)
                break;
            int_pair fire = mul_list[i];
            int id = fire.second;
            node* ptr = &(list[id]);
            step[t].push_back(id); // push id
            schedule++;
            ptr->scheduled = true;
            //printf("%d ", id);

            // source operand not exsit, must be loaded from mem
            if(ptr->pre_l == -1)    // load operand from mem
            {
                printf("<LD #%d>  ", reg_cnt);
                ptr->rf_pre_l = reg_cnt++;  // use the reg
            }

            // considering two operands case, so check the right
            if(ptr->op != SHI && ptr->pre_r == -1)
            {
                printf("<LD #%d>  ", reg_cnt);
                ptr->rf_pre_r = reg_cnt++;  // use the reg
            }
            ptr->rf_id = reg_cnt++;
            ptr->show_inst();

        }

        // scheduling shifter
        for(int i = 0; i < shi_list.size(); i++)
        {
            if(i == shi)
                break;
            int_pair fire = shi_list[i];
            int id = fire.second;
            node* ptr = &(list[id]);
            step[t].push_back(id); // push id
            schedule++;
            ptr->scheduled = true;
            //printf("%d ", id);

            // source operand not exsit, must be loaded from mem
            if(ptr->pre_l == -1)    // load operand from mem
            {
                printf("<LD #%d>  ", reg_cnt);
                ptr->rf_pre_l = reg_cnt++;  // use the reg
            }

            // considering two operands case, so check the right
            if(ptr->op != SHI && ptr->pre_r == -1)
            {
                printf("<LD #%d>  ", reg_cnt);
                ptr->rf_pre_r = reg_cnt++;  // use the reg
            }
            ptr->rf_id = reg_cnt++;
            ptr->show_inst();

        }


        // trigger following nodes of scheduled nodes
        for(int i = 0; i < step[t].size(); i++)
        {
            // locate the node object by time step and id
            node* ptr = &(list[step[t][i]]);

            // iterate each id of its followers
            for(int j = 0; j < ptr->sucs.size(); j++) 
            {
                list[ptr->sucs[j]].set_src_regs(ptr->id, ptr->rf_id);
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

int generate_combos(int n, int k) {
    ofstream fout("comb.txt");
    int com[100];
    for (int i = 0; i < k; i++) com[i] = i;
    while (com[k - 1] < n) {
        for (int i = 0; i < k; i++)
            fout << com[i] << " ";
        //fout << endl;

        int t = k - 1;
        while (t != 0 && com[t] == n - k + t) t--;
        com[t]++;
        for (int i = t + 1; i < k; i++) com[i] = com[i - 1] + 1;
    }
    fout.close();

    int ret = 1;
    int tmp = n;
    for(int i = 0; i < k; i++)
    {
        ret *= tmp;
        tmp--;
    }
    for(int i = k; i >=1; i--)
    {
        ret /= i; 
    }
    return ret;
}


