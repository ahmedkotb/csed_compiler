/* 
 * File:   DFA.cpp
 * Author: Ahmed El-Sharkasy
 * 
 * Created on 23 أبريل, 2011, 09:49 م
 */

#include "DFA.h"

DFA::DFA() {


    start_state = new NFA_State();
}

DFA::DFA(const DFA& orig) {
}

DFA::~DFA() {
}

vector<NFA_State *> * DFA:: mov(NFA_State* current_state, char input_character){
    return current_state->get_transitions(input_character);
}

NFA_State* DFA::get_start_state(){
    return this->start_state;
}

vector<int> DFA::get_IDS(vector<NFA_State*>* states){

    vector<int> ids;
    for (int i = 0; i < states->size(); i++){

        ids.push_back(states->at(i)->get_id());
    }

    return ids;
}

int DFA::hash(vector<int> ids){
    int hash =0;
for(int i =0 ;i < ids.size();i++){
    hash += ids[i];
    hash *=37;
}

    return hash;

}

NFA_State* DFA::create_DFA_state(NFA_State* states){
    NFA_State* state = new NFA_State();
    // not complete

}

DFA* DFA::convert_NFA_to_DFA(NFA* nfa){}
int main(){


    NFA_State* state1= new NFA_State();
    state1->set_id(1);

    NFA_State* state2= new NFA_State();
    state2->set_id(2);

    vector<NFA_State*>* states = new vector<NFA_State*>;

    states->push_back(state1);
    states->push_back(state2);


    DFA d;
    vector<int> ids=d.get_IDS(states);

    for(int i=0 ; i<ids.size();i++)
        cout << ids.at(i) <<endl;

    vector<int> ids2;
    ids2.push_back(3);
    ids2.push_back(2);

    cout<< d.hash(ids2)<<endl;

    return 0;
}
