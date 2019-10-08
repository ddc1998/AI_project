#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#define SIZE12 12

string str, word;
int cnt_varis =0;//how many words place here
int state_i=0;
int flag_end =0;
int node_cnt=0;

fstream fin,f_3000;
struct _3000_len_voca_value{
    int val;
    string vocabulary;
    int len;
}words[2932]={0};

struct variables{
    int coor_x;
    int coor_y;
    int len;
    char ori;   
}vari[SIZE12]; 

struct category_by_length{
    int vals[1000];
    int cnt_vals;
}cat[15];//cat[單字長度]

struct domain{
    int vals[1000];
    int used;//index to vals[""]
    int cnt_vals;//domain 有多大
};

struct assignment{
    int assign[SIZE12];
}ass;

struct states{
    int vari;
    int value;                      //指到的3000單中哪一個(index)
    struct domain state_domain[20]; //state_domain[此層state中 第幾號variable]
}state[SIZE12];                         //state[到第幾層的state]

struct crossing{
    int crossing_cnt;           //總共有幾個crossing點
    int with_who[20];           //和哪些variable crossing
    int coor_x[20], coor_y[20]; //和哪些variables crossing的座標
    int seq[20];                //在crossing時 撞到自己的第幾位[]literal
    int seq_ee[20];             //在crossing時 撞到對方的第幾位[]literal
}cro_vari[SIZE12];                  //cro_vari[varible] -> 看第幾個varible和別人croosing的狀況

void initial(){
    cnt_varis=0;
    state_i=0;
    flag_end =0;
    node_cnt=0;
    for(int i=0;i<SIZE12;i++){
        vari[i].coor_x=0;
        vari[i].coor_y=0;
        vari[i].len=0;
        vari[i].ori=0;
        ass.assign[i]=0;
        state[i].vari=0;
        state[i].value=0;
        cro_vari[i].crossing_cnt = 0;
        for(int j=0;j<20;j++){
            cro_vari[i].with_who[j]=0;
            cro_vari[i].coor_x[j]=0;
            cro_vari[i].coor_y[j]=0;
            cro_vari[i].seq[j]=0;
            cro_vari[i].seq_ee[j]=0;
            state[i].state_domain[j].used=0;
            state[i].state_domain[j].cnt_vals=0;
        }
    }
}

void crossing_detect(){
    //crossing_detect
    for(int i=0;i<cnt_varis;i++){
        for(int j=0;j<cnt_varis;j++){
            if(vari[i].ori==65 && vari[j].ori!=65){//65:A
                if ((vari[i].coor_x<=vari[j].coor_x)&&(vari[i].coor_y>=vari[j].coor_y)
                &&  (vari[i].len>=(vari[j].coor_x-vari[i].coor_x+1))
                &&  (vari[j].len>=(vari[i].coor_y-vari[j].coor_y+1)) ){                    
                    cro_vari[i].with_who[cro_vari[i].crossing_cnt] = j;
                    cro_vari[i].coor_x[cro_vari[i].crossing_cnt]=vari[j].coor_x;
                    cro_vari[i].coor_y[cro_vari[i].crossing_cnt]=vari[i].coor_y;
                    cro_vari[i].seq[cro_vari[i].crossing_cnt]=vari[j].coor_x-vari[i].coor_x;
                    cro_vari[i].seq_ee[cro_vari[i].crossing_cnt]=vari[i].coor_y-vari[j].coor_y;
                    cro_vari[i].crossing_cnt++;

                    cro_vari[j].with_who[cro_vari[j].crossing_cnt] = i;
                    cro_vari[j].coor_x[cro_vari[j].crossing_cnt] = vari[j].coor_x;
                    cro_vari[j].coor_y[cro_vari[j].crossing_cnt] = vari[i].coor_y;
                    cro_vari[j].seq[cro_vari[j].crossing_cnt] = vari[i].coor_y-vari[j].coor_y;
                    cro_vari[j].seq_ee[cro_vari[j].crossing_cnt] = vari[j].coor_x-vari[i].coor_x;
                    cro_vari[j].crossing_cnt++;
                    
                }
            }  
        }
    }
}


void setup_root(){
    int the_min_domain_cnt = 1000, the_min_domain_vari=0;
    //set the root node domain
    for(int i=0;i<cnt_varis;i++){
        copy(cat[vari[i].len].vals, cat[vari[i].len].vals + cat[vari[i].len].cnt_vals, state[0].state_domain[i].vals);
        state[0].state_domain[i].cnt_vals=cat[vari[i].len].cnt_vals;
        state[0].state_domain[i].used=0;
        if(the_min_domain_cnt>state[0].state_domain[i].cnt_vals){
            the_min_domain_cnt = state[0].state_domain[i].cnt_vals;
            the_min_domain_vari = i;
        }
    }
    //set the root node
    state[0].vari=the_min_domain_vari;
    ass.assign[the_min_domain_vari] = 1; 
    state[0].value=state[0].state_domain[the_min_domain_vari].vals[state[0].state_domain[state[0].vari].used];
}

void copy_to_child_domain(int now_state){
    //cout<<"copy_to_child_domain...."<<endl;
    for(int i=0; i<cnt_varis; i++){
        copy(state[now_state].state_domain[i].vals,
             state[now_state].state_domain[i].vals+state[now_state].state_domain[i].cnt_vals, 
             state[now_state+1].state_domain[i].vals);
        state[now_state+1].state_domain[i].cnt_vals=state[now_state].state_domain[i].cnt_vals;
        //cout<<state[now_state].state_domain[i].cnt_vals<<"   "<<state[now_state].state_domain[i].vals[0]<<"   "<<state[now_state].state_domain[i].vals[1]<<endl;
        //cout<<state[now_state+1].state_domain[i].cnt_vals<<"   "<<state[now_state+1].state_domain[i].vals[0]<<"   "<<state[now_state+1].state_domain[i].vals[1]<<endl<<endl;
    }
}

int vari_to_value_currently(int vari){
    //cout<<"vari_to_value_currently: state_i="<<state_i<<", vari="<<vari<<endl;
    for(int i=0; i<=state_i; i++){
        //cout<<state[i].vari<<"  ";
        if(state[i].vari == vari) return state[i].value;
    }
    //cout<<endl;
    return 1000;
}

int take_seq(int a, int b){
    for(int i=0; i<cnt_varis; i++){
        if(cro_vari[a].with_who[i] == b){
            return cro_vari[a].seq[i];
        }
    }
    return 1000;
}

int trim_domain(int child_state, int anc_vari, int child_vari){
    //cout<<"trim_domain:" << child_state <<" "<< anc_vari <<" "<< child_vari<<endl;
    int index=0;
    int origin_domain_size = state[child_state].state_domain[child_vari].cnt_vals;
    int trimmed_domain_size = 0;
    int childXanc_child_seq = take_seq(child_vari, anc_vari);
    int childXanc_anc_seq = take_seq(anc_vari, child_vari);
        if(childXanc_child_seq == 1000) return 1;//no crossing, stay the same
        
    int tmp = vari_to_value_currently(anc_vari);
        if(tmp==1000) cout<<"Something wrong here! anc_vari="<<anc_vari<<endl;
    char matching_literal = words[tmp].vocabulary[childXanc_anc_seq];
        //cout<<"anc_vari_voco = "<<words[tmp].vocabulary<<"  matching literal = "<<matching_literal<<endl;
    for(int i=0;i<origin_domain_size;i++){
        index=state[child_state].state_domain[child_vari].vals[i];
        if(words[index].vocabulary[childXanc_child_seq]==matching_literal){
            state[child_state].state_domain[child_vari].vals[trimmed_domain_size]=index;
            //cout<<"the saved one num= "<<trimmed_domain_size<<"  voca= "<<words[index].vocabulary<<endl;
            trimmed_domain_size++;
        }
    }
    state[child_state].state_domain[child_vari].cnt_vals=trimmed_domain_size;
    //cout<<"origin_domain_size="<<origin_domain_size<<", trim_domain_size="<<trimmed_domain_size<<endl;
    if(trimmed_domain_size) return 1;
    else return 0;
}

int cnt_assigned(){
    int cnt_ass=0;
    for(int i=0; i<cnt_varis;i++){
        if(ass.assign[i]==1) cnt_ass++;
    }
    return cnt_ass;
}

int forward_checking(int state_i){
    int assign_cnt = cnt_assigned();
    int unassign_cnt = cnt_varis-assign_cnt;
    //if(unassign_cnt!=0){
        int child[unassign_cnt] = {0};
        int anc[assign_cnt] = {0};
    //}
    //else{
    //    int child[1] = {0};
    //    int anc[4] = {0};
    //}
    int tmp1=0,tmp2=0;
    for(int i=0;i<cnt_varis;i++){
        if(ass.assign[i]==1){
            anc[tmp1]=i;
            tmp1++;
        }
        else{
            child[tmp2]=i;
            tmp2++;
        }
    }
    //cout<<"AC_forward_checking: (assigned)"<<assign_cnt<<endl;
    int remain_domain =1;
    for(int i=0; i<assign_cnt;i++){
        for(int j=0; j<unassign_cnt;j++){
            remain_domain = trim_domain(state_i+1, anc[i], child[j]);
            if(remain_domain==0) return 0;
        }
    }
    //cout<<"AC_good_finish"<<endl;
    return 1;
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
    int index = state[new_state].state_domain[the_min_domain_vari].used;
    state[new_state].value=state[new_state].state_domain[the_min_domain_vari].vals[index];
    //cout <<"the new node is "<< state[new_state].vari<<", it's value="<<state[new_state].value<<endl;
    //cout << "the new state=" << new_state << ", variable=" << state[new_state].vari << ", value" << state[new_state].value << ", the word is " << words[state[new_state].value].vocabulary<<endl;
}

int go_back_the_last_state(){
    //cout << "non_permit, go back to last domain" << endl <<"state_i="<<state_i;
    copy_to_child_domain(state_i-1);
    /*for(int i=0;i<state[state_i-1].state_domain[4].cnt_vals;i++){
        cout<<i<<" "<<words[  state[state_i-1].state_domain[4].vals[i]  ].vocabulary<<endl;
    }*/
    //system("PAUSE");
    ass.assign[state[state_i].vari] = 0;
    state[state_i].state_domain[state[state_i].vari].used = 0;
    int index = state[state_i].state_domain[state[state_i].vari].used;
    //cout <<"state="<< state_i <<" vari="<< state[state_i].vari<<" used="<<index<< " word="<<words[state[state_i].value].vocabulary<<endl;
    state_i--;
    index = state[state_i].state_domain[state[state_i].vari].used;
    state[state_i].value = state[state_i].state_domain[state[state_i].vari].vals[index];
    //cout <<"state="<< state_i <<" vari="<< state[state_i].vari<<" used="<<index<< " word="<<words[state[state_i].value].vocabulary<<endl;
    if ((state[state_i].state_domain[state[state_i].vari].used + 1) == state[state_i].state_domain[state[state_i].vari].cnt_vals){
        return 1000;
    }
    state[state_i].state_domain[state[state_i].vari].used++;
    index = state[state_i].state_domain[state[state_i].vari].used;
    state[state_i].value = state[state_i].state_domain[state[state_i].vari].vals[index];
    //cout <<"state="<< state_i <<" vari="<< state[state_i].vari<<" used="<<index<< " word="<<words[state[state_i].value].vocabulary<<endl;    
    return 1;
}

void print_puzzle(){
    for(int i=0;i<cnt_varis;i++){
        cout<<"state:"<<i<<" vari:"<<state[i].vari<<" value:"<<state[i].value<<" voca="<<words[state[i].value].vocabulary<<endl;
    }
    cout<<"node_cnt="<<node_cnt<<endl;
}


int build_node(){
    node_cnt++;
    int permit=forward_checking(state_i);
    //cout<<"After AC3_forward_checking, state= "<<state_i<<endl;
    if(permit){
        state_i++;
        //cout<<"permit, state_i="<<state_i<<endl;
        update_state(state_i);
        if((state_i+1)==cnt_varis){
            //cout<<"done"<<endl;
            flag_end=1;
            return 0;
        }
        copy_to_child_domain(state_i);
        build_node();
        if(flag_end==1) return 0;
    }
    else{       
        if((state[state_i].state_domain[state[state_i].vari].used+1)==state[state_i].state_domain[state[state_i].vari].cnt_vals){
            int still_full_used = go_back_the_last_state();
            while(still_full_used==1000){
                still_full_used=go_back_the_last_state();
            }                        
            build_node();
            if(flag_end==1) return 0;
        }
        //cout<<"non_permit, state_i="<<state_i<<", same state, vari="<<state[state_i].vari<<endl;;
        copy_to_child_domain(state_i);
        state[state_i].state_domain[state[state_i].vari].used++;
        int index2 = state[state_i].state_domain[state[state_i].vari].used;
        state[state_i].value = state[state_i].state_domain[state[state_i].vari].vals[index2];
        //cout<<"the num of value in domain="<<index2<<" value="<<state[state_i].value<<endl;
        build_node();
        if(flag_end==1) return 0;
    }
}



int main(){
    f_3000.open("English words 3000.txt", ios::in);
    int num = 0, max_len =0;
    for(int i=0; i<15; i++){
        cat[i].cnt_vals=0;
    }
    while (num<2932){
        getline(f_3000, word);
        words[num].vocabulary = word;
        words[num].val = num;
        words[num].len=word.length();
        int len_tmp = word.length();
        cat[len_tmp].cnt_vals++;
        int cnt_tmp = cat[len_tmp].cnt_vals;
        cat[len_tmp].vals[cnt_tmp-1] = num;
        num++;
    }
    fin.open("puzzle.txt", ios::in);
    //getline(fin,str);
    //getline(fin, str);
    //getline(fin, str);
    //cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;
    //cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;
    while(getline(fin,str)){
        initial();
        int tmp, flag_A=0, flag_D=0;
        for(tmp=0; (tmp)<str.size();tmp+=10){    
            vari[tmp/10].coor_x = str[(tmp)]-48;
            vari[tmp/10].coor_y = str[(tmp)+2]-48;
            vari[tmp/10].len = str[(tmp)+4]-48;
            vari[tmp/10].ori=str[(tmp)+6];
        }
        cnt_varis = tmp/10;
        crossing_detect();
        setup_root();
        copy_to_child_domain(state_i);
        build_node();
        print_puzzle();
        //break;
        cout<<endl;
    }
    return 0;
} 