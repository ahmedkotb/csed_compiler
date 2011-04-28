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

DFA::DFA(NFA* nfa) {

    start_state = new NFA_State();
    //nfa states must be numbered before conversion
    nfa->number_states();

    convert_NFA_to_DFA(nfa);
}

DFA::DFA(const DFA& orig) {
}

DFA::~DFA() {
}

vector<NFA_State *> * DFA::mov(NFA_State* current_state, INPUT_CHAR input_character) {
    return current_state->get_transitions(input_character);
}

NFA_State* DFA::get_start_state() {
    return this->start_state;
}

vector<int> DFA::get_IDS(vector<NFA_State*>* states) {

    vector<int> ids;
    for (unsigned int i = 0; i < states->size(); i++) {

        ids.push_back(states->at(i)->get_id());
    }

    return ids;
}

int DFA::hash(vector<int> ids) {
    int hash = 0;
    for (unsigned int i = 0; i < ids.size(); i++) {
        hash += ids[i];
        hash *= 37;
    }

    return hash;

}

NFA_State* DFA::create_DFA_state(vector<NFA_State*>* states) {
    NFA_State* state = new NFA_State();
    
    // STEPS
    // 1- (NOT DONE) if any of the NFA states is accepting , set DFA state as accepting
    // 2- (NOT DONE) set the accepting pattern of the new DFA state with the highest
    //     priority of the patterns of the NFA states


    return state;
}

void DFA::convert_NFA_to_DFA(NFA* nfa) {
    if(nfa->get_states_count()!=0)
    	get_DFA(nfa->get_start_state()->get_transitions(EPSILON), nfa);
}

void DFA::number_states() {
    //set used as visited flag for each state
    set<NFA_State *> visited;
    //bfs queue
    queue<NFA_State *> queue;
    visited.insert(this->start_state);
    queue.push(this->start_state);

    NFA_State * current_state;
    vector<INPUT_CHAR>* inputs;
    vector<NFA_State *>* states;
    int counter = 0;
    while (!queue.empty()) {
        current_state = queue.front();
        queue.pop();
        current_state->set_id(counter++);
        //add neighbors to the queue
        inputs = current_state->get_transitions_inputs();
        for (unsigned int i = 0; i < inputs->size(); ++i) {
            states = current_state->get_transitions(inputs->at(i));
            for (unsigned int j = 0; j < states->size(); ++j) {
                if (visited.find(states->at(j)) != visited.end()) continue;
                queue.push(states->at(j));
                visited.insert(states->at(j));
            }
            //delete the states vector that is created by the get_transitions method
            delete states;
        }
        //delete the inputs vector that is created by get_transitions_inputs() method
        delete inputs;
    }
}

void DFA::get_DFA(vector<NFA_State*>* states, NFA* nfa) {

    // vector holding the transitioned states for each input
    vector<vector<NFA_State*>*>* row_new_states = new vector<vector<NFA_State*>*>;

    // vector holding the ids of the states
    vector<int> ids = this->get_IDS(states);
    map <int, NFA_State*>::iterator iterator = states_IDS.find(this->hash(ids));

    NFA_State * current;
    if (iterator == states_IDS.end()) {

        current = create_DFA_state(states);
        states_IDS.insert(pair<int, NFA_State*>(this->hash(ids), current));

    } else {
        current = iterator->second;
    }

    if (states_IDS.size() == 1) {// only one DFA state was saved
        this->start_state = current;
    }

    // input alphabet
    set<INPUT_CHAR>* alphabet = nfa->get_alphabet();
    set<INPUT_CHAR>::iterator it;

    for (it = alphabet->begin(); it != alphabet->end(); it++) {
        // vector representing group of NFA states (DFA State)
        vector<NFA_State*>* cell = new vector<NFA_State*>;

        if ((char) *it == EPSILON)// empty transition
            continue;
        for (unsigned int i = 0; i < states->size(); i++) {
            // states resulting from the mov function
            vector<NFA_State*>* mov_states = this->mov(states->at(i), *it);
            for (unsigned int k = 0; k < mov_states->size(); k++) {
                // states resulting from the epsilon closure
                vector<NFA_State*>* epsilon_states = mov_states->at(k)->get_transitions(EPSILON);
                for (unsigned int l = 0; l < epsilon_states->size(); l++) {
                    cell->push_back(epsilon_states->at(l));
                }
            }
        }

        if (cell->size() != 0) { // new states appeared in the table and needs new row
            vector<int> ids = this->get_IDS(cell);
            iterator = states_IDS.find(this->hash(ids));

            NFA_State* going;
            if (iterator == states_IDS.end()) {
                going = create_DFA_state(states);
                // add state to current row
                row_new_states->push_back(cell);
                states_IDS.insert(pair<int, NFA_State*>(this->hash(ids), going));

            } else {
                going = iterator->second;
            }
            current->add_transition(*it, going);
        }
    }

    for (unsigned int i = 0; i < row_new_states->size(); i++) {// loop through all DFA states in a table row and repeat the algorithm
        this->get_DFA(row_new_states->at(i), nfa);
    }

    // delete the vector carrying DFA states
    delete row_new_states;
}

void DFA::debug() {
//    cout << "Number of States : " << this->states_count << endl;
//    cout << "Alphabet : ";
//    set<INPUT_CHAR>::iterator it;
//    for (it = alphabet->begin(); it != alphabet->end(); it++) {
//        if (*it == -1)
//            cout << "EPSILON , ";
//        else
//            cout << (char) (*it) << " , ";
//    }
//    cout << endl;
    cout << "States :" << endl;
    //give numbers to states
    this->number_states();
    //set used as visited flag for each state
    set<NFA_State *> visited;
    //bfs queue
    queue<NFA_State *> queue;
    visited.insert(this->start_state);
    queue.push(this->start_state);

    NFA_State * current_state;
    vector<INPUT_CHAR>* inputs;
    vector<NFA_State *>* states;

    while (!queue.empty()) {
        current_state = queue.front();
        queue.pop();
        cout << current_state-> get_description();
        //add neighbors to the queue
        inputs = current_state->get_transitions_inputs();
        for (unsigned int i = 0; i < inputs->size(); ++i) {
            states = current_state->get_transitions(inputs->at(i));
            for (unsigned int j = 0; j < states->size(); ++j) {
                if (visited.find(states->at(j)) != visited.end()) continue;
                queue.push(states->at(j));
                visited.insert(states->at(j));
            }
            //delete the states vector that is created by the get_transitions method
            delete states;
        }
        //delete the inputs vector that is created by get_transitions_inputs() method
        delete inputs;
    }
}
//int main() {
//
//
//    NFA_State* state1 = new NFA_State();
//    state1->set_id(1);
//
//    NFA_State* state2 = new NFA_State();
//    state2->set_id(2);
//
//    vector<NFA_State*>* states = new vector<NFA_State*>;
//
//    states->push_back(state1);
//    states->push_back(state2);
//
//
//    DFA d;
//    vector<int> ids = d.get_IDS(states);
//
//    for (int i = 0; i < ids.size(); i++)
//        cout << ids.at(i) << endl;
//
//    vector<int> ids2;
//    ids2.push_back(3);
//    ids2.push_back(2);
//
//    cout << d.hash(ids2) << endl;
//
//
//    return 0;
//}
