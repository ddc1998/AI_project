int AC3_forward_checking(int state_i){//剪掉不符合交叉點的domain component
    //1.找交叉點 2.找與之交叉的vari 3.check domain
    //state[state_i].vari
    int flag[12] = {0};
    int ka_vari = state[state_i].vari;
    int ka_value = state[state_i].value;
    int ee_vari[cro_vari[ka_vari].crossing_cnt];
    int tmp[cro_vari[ka_vari].crossing_cnt]={0};
    char matching_literal[cro_vari[ka_vari].crossing_cnt]={0};
    
    //ee_vari[]等待與甲variable配對的crossing 乙variable們
    //cout<<"matching_literal  X  ee_vari  X  ka_vari's seq"<<endl;
    for(int i=0; i<cro_vari[ka_vari].crossing_cnt; i++){
        ee_vari[i]=cro_vari[ka_vari].with_who[i];
        tmp[i]=cro_vari[ka_vari].seq[i];
        matching_literal[i]=words[ka_vari].vocabulary[tmp[i]];
        //cout<<matching_literal[i]<<"   "<<ee_vari[i]<<"   "<<tmp[i]<<endl;
    }
    //匹配ka(甲) & ee(乙) domain
    for (int i=0; i<cro_vari[ka_vari].crossing_cnt; i++){        
        int tmp2 = cro_vari[ka_vari].seq_ee[i];
        for(int j=0; j<state[state_i].state_domain[ee_vari[i]].cnt_vals;j++){
            int index = state[state_i+1].state_domain[ee_vari[i]].vals[j];
            if(words[index].vocabulary[tmp2]==matching_literal[i]){
                flag[i] = 1;    
                //cout<<"matching: "<<matching_literal[i]<<"  with the voca in ee: "<<words[index].vocabulary<<"  at ee's seq= "<<tmp2<<endl;
                break;            
            }
        }        
    }
    int _AC3 = 1;
    for(int i=0;i<cro_vari[ka_vari].crossing_cnt;i++){
        _AC3=_AC3&&flag[i];
    }
    //cout<<ass.assign[ka_vari]<<endl;
    //cout<<"permit= "<<_AC3<<endl;
    if(_AC3==1){
        for(int i=0; i<cro_vari[ka_vari].crossing_cnt; i++){
            //cout<<"cnt_varis="<<i<<"  assign= "<<ass.assign[i]<<endl;
            if(ass.assign[i]==0){
                //cout<<state_i+1<<"  "<<matching_literal[i]<<"  "<<ee_vari[i]<<"  "<<cro_vari[ka_vari].seq_ee[i]<<endl;
                forward_checking(state_i+1,matching_literal[i],ee_vari[i], cro_vari[ka_vari].seq_ee[i]);
            }           
        }
    }
    
    return _AC3;
}

void update_state(int new_state){
    //set the child node domain -> forward_checking trim時已做完 

    //find the unassigned variable who has min domain as the child node's variable
    int the_min_domain_cnt = 1000, the_min_domain_vari = 0;
    for(int i=0;i<cnt_varis;i++){
        if((ass.assign[i]==0)&&(the_min_domain_cnt>state[new_state].state_domain[i].cnt_vals)){
            the_min_domain_cnt = state[new_state].state_domain[i].cnt_vals;
            the_min_domain_vari = i;
        }
        //cout<<domain[i].cnt_vals<<"   "<<domain[i].vals[0]<<"   "<<domain[i].vals[1]<<endl;   
    }
    //set the root node
    state[new_state].vari=the_min_domain_vari;
    ass.assign[the_min_domain_vari] = 1; 
    state[new_state].value=state[new_state].state_domain[the_min_domain_vari].vals[0];
    //cout <<"the new node is "<< state[new_state].vari<<", it's value="<<state[new_state].value<<endl;
}