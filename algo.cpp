#include "algo.h"

vector<vector<int> > cascade_coverage(vector<node>& list, char* file_name, int n_cas)
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
    int schedule = 0;
    int t = 0;
    vector<int> rm_nodes;

    // for each level larger than 1
    for(int i = n_fu; i >= 2; i--)
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
                match = rec_search(list, m, comb, 0, i, match);

                // a full matched pattern found
                if(match.size() == i)
                {
                    node* head = &(list[match[0]]);
                    node* tail = &(list[match[match.size()-1]]);
                    head->super = i;
                    head->op_list = (operation*)malloc(sizeof(operation)*i);
                    head->op_list[0] = head->op;
                    //printf("time step %d:", t);
                    schedule += i; 
                    // kill the suc of the head
                    assert(head->sucs.size() == 1);
                    head->sucs.pop_back();

                    // reconnection, process middle & tail
                    for(int n = 1; n < match.size(); n++)
                    {
                        node* cur = &(list[match[n]]);
                        head->op_list[n] = cur->op;
                        cur->scheduled = true;  // update node status
                        // record the following nodes, rm in the next stage
                        rm_nodes.push_back(match[n]);
                        // except the super node
                        int pres_size = cur->pres.size();
                        int sucs_size = cur->sucs.size();
                        // the following node have mem ref
                        if(pres_size == 1 && cur->op != SHI)
                            cur->wait_mem++;

                        for(int h = 0; h < pres_size; h++) 
                        {
                            node* cur_pre = &(list[cur->pres[h]]);
                            // check is the pre is inside the cascaded
                            int in_cas = 0;
                            for(int o = 0; o < match.size(); o++)
                            {
                                if(match[o] == cur_pre->id) 
                                    in_cas = 1;
                            }
                            if( !in_cas )
                                cur_pre->reconnect(head->id, cur->id);
                        }
                        for(int h = 0; h < sucs_size; h++)
                        {
                            node* cur_suc = &(list[cur->sucs[h]]); 
                            int in_cas = 0;
                            for(int o = 0; o < match.size(); o++)
                            {
                                if(match[o] == cur_suc->id) 
                                    in_cas = 1;
                            }
                            if( !in_cas )
                            cur_suc->reconnected(head->id, cur->id);
                        }
                    }

                    // head inherits sucs of the tail
                }
                if(schedule == list.size())
                    break;
            
            }
        }
        fin.close();

    }
    // init 
    int die = rm_nodes.size();
    for(int i = 0; i < list.size(); i++)
        list[i].scheduled = false;

    for(int i = 0; i < rm_nodes.size(); i++)
        list[rm_nodes[i]].scheduled = true;

    vector<vector<int> > _asap = asap(list, die);
    vector<vector<int> > _alap = alap(list, die);

    int tmp_id;

    for(int i = 0; i < _asap.size(); i++)
    {
        // update for each node
        for(int j = 0; j < _asap[i].size(); j++) 
        {
            tmp_id = _asap[i][j];
            list[tmp_id].ts = i;
        }
    }

    for(int i = 0; i < _alap.size(); i++)
    {
        // update for each node
        for(int j = 0; j < _alap[i].size(); j++) 
        {
            tmp_id = _alap[i][j];
            // reverse the time step because we did backward analysis
            list[tmp_id].tl = _asap.size()-1-i;
        }
    }

    printf("debug: mobility: ");
    for(int i = 0; i < list.size(); i++)
    {
        if(!list[i].scheduled)
            printf("(%d %d) ", list[i].mob(), list[i].super);
    }
    printf("\n");


    vector<vector<int> > _lbs = scalar_lbs(list, n_cas, n_cas, n_cas);

    
    return _lbs;

   
}


vector<int> rec_search(const vector<node>& list, int id, operation* cas_fu, int cur, int end, vector<int> vec_in)
{
    // not match or already scheduled, return directly
    if(cas_fu[cur] != list[id].op || list[id].scheduled == true)
        return vec_in;

    // except the tail, we can't wb rf
    if(cur != end && list[id].sucs.size() > 1)
        return vec_in;

    vec_in.push_back(id);
    vector<int> vec_cpy = vec_in;

    // is it the end of cascade
    if(cur == end) 
    {
        return vec_cpy;
    }

    // hit, then find its succesors
    for(int i = 0; i < list[id].sucs.size(); i++) 
    {
        vec_cpy = rec_search(list, list[id].sucs[i], cas_fu, cur+1, end, vec_in);
        if(vec_cpy.size() > vec_in.size())
            break;
    }
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

    for(int i = 0; i < list.size(); i++)
    {
        if(list[i].scheduled) 
            schedule++;
    }

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

            // load operands from mem
            for(int i = 0; i < ptr->wait_mem; i++)
            {
                printf("<LD #%d>  ", reg_cnt);
                ptr->rf_mem.push_back(reg_cnt++);  // use the reg
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
            for(int i = 0; i < ptr->wait_mem; i++)
            {
                printf("<LD #%d>  ", reg_cnt);
                ptr->rf_mem.push_back(reg_cnt++);  // use the reg
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
            for(int i = 0; i < ptr->wait_mem; i++)
            {
                printf("<LD #%d>  ", reg_cnt);
                ptr->rf_mem.push_back(reg_cnt++);  // use the reg
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

            for(int i = 0; i < ptr->wait_mem; i++)
            {
                printf("<LD #%d>  ", reg_cnt);
                ptr->rf_mem.push_back(reg_cnt++);  // use the reg
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

vector<vector<int> > asap(vector<node> list, int die)
{
    printf("----ASAP----\n");
    vector<vector<int> > step;

    int schedule = 0;
    int t = 0;
    
    schedule += die;
    // schedule for each time step
    while(schedule != list.size()) 
    {
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

vector<vector<int> > alap(vector<node> list, int die)
{
    printf("----ALAP----\n");
    vector<vector<int> > step;

    int schedule = 0;
    int t = 0;
    schedule += die;

    vector<int> tmp;
    step.push_back(tmp);
    // iterate nodes for last time step
    printf("time step %d:", t);
    for(int i = 0; i < list.size(); i++)
    {
        // find nodes without followers as start point
        if( !list[i].sucs.size() && !list[i].scheduled)
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

            // find a pre
            for(int i = 0; i < ptr->pres.size(); i++)
            {
                if( !list[ptr->pres[i]].scheduled )
                {
                    step[t].push_back(ptr->pres[i]);
                    list[ptr->pres[i]].scheduled = true;
                    schedule ++;
                    printf("%d ", ptr->pres[i]);
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


