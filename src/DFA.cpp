/* 
 * File:   DFA.cpp
 * Author: Ahmed El-Sharkasy
 * 
 * Created on 23 أبريل, 2011, 09:49 م
 */

#include "DFA.h"
#include <iomanip>

#define ALPHA(x,k) (*x)->get_dfa_transition(*k)
#define ALPHA_NUM(x,k) (*x)->get_dfa_transition(*k)->get_id()
#define DISTINCT(i,j) ((i) < (j) ? distinct())

DFA::DFA() {
    start_state = new NFA_State();
}

DFA::DFA(NFA* nfa) {

	//initialize state count and alphabet
	this->states_count = 0;
	this->alphabet = new set<INPUT_CHAR>();

	//make a copy of nfa alphabet except for EPSILON
    set<INPUT_CHAR>::iterator it;
    set<INPUT_CHAR> * nfa_alphabet = nfa->get_alphabet();
    for (it = nfa_alphabet->begin(); it != nfa_alphabet->end(); it++){
    	if (*it != EPSILON)
    		alphabet->insert(*it);
    }

    //nfa states must be numbered before conversion
    nfa->number_states();

    convert_NFA_to_DFA(nfa);
}

DFA::~DFA() {
	delete alphabet;
}

set<NFA_State *> * DFA::mov(NFA_State* current_state, INPUT_CHAR input_character) {
    return current_state->get_transitions(input_character);
}

NFA_State* DFA::get_start_state() {
    return this->start_state;
}

vector<int> * DFA::get_IDS(set<NFA_State*>* states) {
    vector<int> * ids = new vector<int>();
    set<NFA_State *>::iterator it;
	for (it = states->begin() ;it != states->end() ;++it){
		ids->push_back((*it)->get_id());
	}
    return ids;
}

long DFA::hash(vector<int>* ids) {
	//sort the vector to obtain the same hash
	sort(ids->begin(),ids->end());
    long hash = 0;
    for (unsigned int i = 0; i < ids->size(); i++) {
        hash += ids->at(i);
        hash *= 37;
    }
    return hash;
}

NFA_State* DFA::create_DFA_state(set<NFA_State*>* states) {
    NFA_State* state = new NFA_State();
    set<NFA_State*>::iterator it;
    //set accepting token of new DFA state to the accepting token of highest priority
    int min = 1 << 30;
    string acc_pattern = "";
    for (it=states->begin() ; it != states->end() ; ++it){
    	if ((*it)->is_accepting_state() && (*it)->get_token_id() < min ){
    		min = (*it)->get_token_id();
    		acc_pattern = (*it)->get_accepting_pattern();
    	}
    }
    if (min != 1<<30){
    	state->set_token_id(min);
    	state->set_accepting_pattern(acc_pattern);
    }
    return state;
}

void DFA::convert_NFA_to_DFA(NFA* nfa) {
	set <NFA_State *> * start_states = nfa->get_start_state()->get_transitions(EPSILON);
    assert (nfa->get_states_count()!=0);
    get_DFA(start_states, nfa);
    delete start_states;
}

void DFA::number_states() {
	delete get_numbered_states();
}

set<NFA_State*> * DFA::get_numbered_states(){
    //set used as visited flag for each state
    set<NFA_State *> *visited = new set<NFA_State *>();
    //bfs queue
    queue<NFA_State *> queue;
    visited->insert(this->start_state);
    queue.push(this->start_state);

    NFA_State * current_state;
    vector<INPUT_CHAR>* inputs;
    set<NFA_State *>* states;
    set<NFA_State *>::iterator it;
    int counter = 0;
    while (!queue.empty()) {
        current_state = queue.front();
        queue.pop();
        current_state->set_id(counter++);
        //add neighbors to the queue
        inputs = current_state->get_transitions_inputs();
        for (unsigned int i = 0; i < inputs->size(); ++i) {
            states = current_state->get_transitions(inputs->at(i));
            for (it = states->begin() ;it != states->end();++it){
    			if (visited->find(*it) == visited->end()){
    				//a new state was found
    				visited->insert(*it);
    				queue.push(*it);
    			}
    		}
            //delete the states set that is created by the get_transitions method
            delete states;
        }
        //delete the inputs vector that is created by get_transitions_inputs() method
        delete inputs;
    }
    return visited;
}

void DFA::get_DFA(set<NFA_State*>* states, NFA* nfa) {

    // vector holding the transitioned states for each input
    vector<set<NFA_State*>*>* row_new_states = new vector<set<NFA_State*>*>();

    // vector holding the ids of the states
    vector<int>* ids = this->get_IDS(states);
    long hash = this->hash(ids);
    delete ids;
    map <long, NFA_State*>::iterator iterator = states_IDS.find(hash);

    NFA_State * current;
    if (iterator == states_IDS.end()) {
        current = create_DFA_state(states);
        ++this->states_count;
        states_IDS.insert(pair<long, NFA_State*>(hash, current));

    } else {
        current = iterator->second;
    }

    // only one DFA state was saved
    if (states_IDS.size() == 1) {
        this->start_state = current;
    }

    // input alphabet
    set<INPUT_CHAR>* alphabet = nfa->get_alphabet();
    set<INPUT_CHAR>::iterator it;

    for (it = alphabet->begin(); it != alphabet->end(); it++) {
        // set representing group of NFA states (DFA State)
        set<NFA_State*>* cell = new set<NFA_State*>;

        if ((char) *it == EPSILON)// empty transition
            continue;

        for (set<NFA_State*>::iterator si = states->begin() ; si != states->end() ; ++si){
            // states resulting from the mov function
            set<NFA_State*>* mov_states = this->mov(*(si), *it);
            //TODO : this step can be optimized by pre-calculation of epsilon closure of all states
            for (set<NFA_State*>::iterator sj = mov_states->begin() ; sj != mov_states->end() ; ++sj){
            	// states resulting from the epsilon closure
            	set<NFA_State*>* epsilon_states = (*sj)->get_transitions(EPSILON);
            	for (set<NFA_State*>::iterator sk = epsilon_states->begin() ; sk != epsilon_states->end() ; ++sk){
            		cell->insert(*(sk));
            	}
            }
        }

        // new states appeared in the table and needs new row
        if (cell->size() != 0) {
            vector<int> *ids = this->get_IDS(cell);
            long hash = this->hash(ids);
            iterator = states_IDS.find(hash);

            NFA_State* going;
            if (iterator == states_IDS.end()) {
                going = create_DFA_state(cell);
                ++this->states_count;
                // add state to current row
                row_new_states->push_back(cell);
                states_IDS.insert(pair<long, NFA_State*>(hash, going));
            } else {
                going = iterator->second;
            }
            current->add_transition(*it, going);
            delete ids;
        }
    }

    // loop through all DFA states in a table row and repeat the algorithm
    for (unsigned int i = 0; i < row_new_states->size(); i++) {
        this->get_DFA(row_new_states->at(i), nfa);
    }

    // delete the vector carrying DFA states
    delete row_new_states;
}

int DFA::get_states_count(){
	return this->states_count;
}

set<INPUT_CHAR>* DFA::get_alphabet(){
	return this->alphabet;
}


void DFA::minimize(){

	//steps
	//1-number states and get all of them in set
	//2-partition states on input (EPSILON)
	//3-partition states using alphabet
	//4-repeat step 3 till nothing changes
	//5-merge non distinct states

	// 1
	set<NFA_State *> * states = this->get_numbered_states();
	assert (states->size() == (unsigned int)this->get_states_count());
	bool change;

	//creating the 2d distinct flags array
	bool ** distinct = new bool*[states->size()];
	for (unsigned int i = 0; i < states->size(); ++i){
		distinct[i] = new bool[states->size()];
		for (unsigned int j = 0; j < states->size(); ++j) {
			distinct[i][j] = false;
		}
	}

	// 2 partition states on EPSILON input according to their token_id
	for (set<NFA_State * >::iterator si = states->begin() ; si != states->end() ; ++si){
		set<NFA_State * >::iterator sj = si;
		++sj;
		for (;sj != states->end() ; ++sj){
			//
			distinct[(*si)->get_id()][(*sj)->get_id()] = (*si)->get_token_id() != (*sj)->get_token_id();
		}
	}

	//3
	change = true;
	while (change){
		change = false;
		for (set<NFA_State * >::iterator si = states->begin() ; si != states->end() ; ++si){
			set<NFA_State * >::iterator sj = si;
			++sj;
			for (;sj != states->end() ; ++sj){
				//two equivalent states
				if (!distinct[(*si)->get_id()][(*sj)->get_id()]){
					set<INPUT_CHAR>::iterator k;
					for (k = alphabet->begin() ; k != alphabet->end() ; ++k){
						//similar states (both go to dead state on this input)
						if (ALPHA(si,k) == NULL && ALPHA(sj,k) == NULL) continue;
						//different states
						if (ALPHA(si,k) == NULL || ALPHA(sj,k) == NULL || distinct[ALPHA(si,k)->get_id()][ALPHA(sj,k)->get_id()]){
							distinct[(*si)->get_id()][(*sj)->get_id()] = true;
							change = true;
							break;
						}
					}
				}
			}
		}
	}
	//done flag .. used to optimize in merging
	//if merged state A and state B , state B is marked as done
	//so if states B and C found to be similar , we don't merge states B and C .. as state B will not be found
	bool * done = new bool[states->size()];
	for (unsigned int i = 0; i < states->size(); ++i)
		done[i] = false;

	//5
	for (set<NFA_State * >::iterator si = states->begin() ; si != states->end() ; ++si){
		set<NFA_State * >::iterator sj = si;
		++sj;
		for (;sj != states->end() ; ++sj){
			if (done[(*sj)->get_id()]) continue;
			if (!distinct[(*si)->get_id()][(*sj)->get_id()]){
				merge_states(*si,*sj,states);
				done[(*sj)->get_id()] = true;
			}
		}
	}
	//freeing memory
	for (unsigned int i = 0; i < states->size(); ++i)
		delete distinct[i];
	delete distinct;
	delete done;
	delete states;
	states = this->get_numbered_states();
	this->states_count = states->size();
	delete states;
}

void DFA::merge_states(NFA_State * state_a,NFA_State * state_b,set<NFA_State *>* states){
	//Steps
	for (set<NFA_State * >::iterator si = states->begin() ; si != states->end() ; ++si){
		if ((*si) == state_b) continue;
		set<INPUT_CHAR>::iterator k;
		for (k = alphabet->begin() ; k != alphabet->end() ; ++k){
			if (ALPHA(si,k) == state_b)
				(*si)->replace_dfa_transition(*k , state_a);
		}
	}
}
void DFA::debug() {
    cout << "Number of States : " << this->states_count << endl;
    cout << "Alphabet : ";
    set<INPUT_CHAR>::iterator it;
    for (it = alphabet->begin(); it != alphabet->end(); it++) {
    	cout << (char) (*it) << " , ";
    }
    cout << endl;
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
    set<NFA_State *>* states;

    while (!queue.empty()) {
        current_state = queue.front();
        queue.pop();
        cout << current_state-> get_description();
        //add neighbors to the queue
        inputs = current_state->get_transitions_inputs();
        for (unsigned int i = 0; i < inputs->size(); ++i) {
            states = current_state->get_transitions(inputs->at(i));
            for (set<NFA_State *>::iterator it = states->begin() ;it != states->end();++it){
    			if (visited.find(*it) == visited.end()){
    				//a new state was found
    				visited.insert(*it);
    				queue.push(*it);
    			}
    		}
            //delete the states vector that is created by the get_transitions method
            delete states;
        }
        //delete the inputs vector that is created by get_transitions_inputs() method
        delete inputs;
    }
}

void DFA::write_transition_table(string file_name,vector<string>* tokens){
	//number the states first
	set<NFA_State *> * states = this->get_numbered_states();
	ofstream myfile;
	myfile.open(file_name.c_str());
	myfile << setw(9) << left << "  State"  << "|" << setw(12) << left << "    Token";
    for (set<INPUT_CHAR>::iterator it = alphabet->begin(); it != alphabet->end(); it++) {
			string input;
			if (*it == '\n')
				myfile <<  "|   " <<setw(4) << left << "ENTER";
			else if (*it == '\t')
				myfile <<  "|   " <<setw(4) << left << "TAB";
			else if (*it == ' ')
				myfile <<  "|   " <<setw(4) << left << "SPACE";
			else
				myfile <<  "|   " <<setw(4) << left << (char) (*it);
    }
    myfile << "|\n";
    for (set<NFA_State*>::iterator it = states->begin(); it != states->end() ; ++it){
    	if ((*it)->get_token_id() > -1)
    		myfile << "    " << setw(5) << left << (*it)->get_id() << "|    " << setw(8) << left <<tokens->at((*it)->get_token_id());
    	else
    		myfile << "    " << setw(5) << left << (*it)->get_id() << "|" << setw(12) << left << "     NONE";

    	for (set<INPUT_CHAR>::iterator k = alphabet->begin(); k != alphabet->end(); k++) {
    		if (ALPHA(it,k))
      			myfile << "|   " << setw(4) << left << ALPHA_NUM(it,k);
    		else
    			myfile << "|   -   ";
    	}
        myfile << "|\n";
    }

	myfile.close();
	delete states;
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
