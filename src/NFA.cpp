/*
 * NFA.cpp
 *
 *  Created on: Apr 22, 2011
 *      Author: ahmedkotb
 */

#include "NFA.h"

//creates empty nfa

NFA::NFA() {
    alphabet = new set<INPUT_CHAR > ();
    combined_NFA = false;
    states_count = 0;
}

//creates two states with one transition from the first to the second

NFA::NFA(INPUT_CHAR input_char) {

    alphabet = new set<INPUT_CHAR > ();
    combined_NFA = false;

    start_state = new NFA_State();
    final_state = new NFA_State();
    start_state->add_transition(input_char, final_state);

    //add the character to the NFA alphabet
    alphabet->insert(input_char);
    //set the accepting pattern of the final state
    stringstream ss;
    ss << (char) input_char;
    final_state->set_accepting_pattern(ss.str());
    states_count = 2;
}

//creates a linked list like graph of nodes from the given sequence

NFA::NFA(string sequence) {

    alphabet = new set<INPUT_CHAR > ();
    combined_NFA = false;

    start_state = new NFA_State();
    NFA_State * ptr = start_state;
    for (unsigned int i = 0; i < sequence.length(); ++i) {
        NFA_State * new_state = new NFA_State();
        ptr->add_transition(sequence[i], new_state);
        //update the alphabet of the NFA
        alphabet->insert(sequence[i]);
        //update accepting pattern
        new_state->set_accepting_pattern(sequence.substr(0, i + 1));
        //advance
        ptr = new_state;
    }
    final_state = ptr;
    states_count = sequence.length() + 1;
}

NFA::NFA(INPUT_CHAR start, INPUT_CHAR end) {

    //start char must be <= end
    assert(start <= end);

    alphabet = new set<INPUT_CHAR > ();
    combined_NFA = false;

    start_state = new NFA_State();
    final_state = new NFA_State();

    stringstream acc_pattern_stream;
    for (INPUT_CHAR i = start; i <= end; ++i) {
        start_state->add_transition(i, final_state);
        //generate accepting pattern
        if (i == end)
            acc_pattern_stream << (char) i;
        else
            acc_pattern_stream << (char) i << " | ";
        //update the alphabet
        alphabet->insert(i);
    }

    //set the accepting pattern of the final string to the OR of all characters
    final_state->set_accepting_pattern(acc_pattern_stream.str());
    states_count = 2;
}

void NFA::apply_plus_closure() {
    //plus closure can't be applied to combined NFA
    assert(combined_NFA == false);

    NFA_State* new_start_state = new NFA_State();
    NFA_State* new_final_state = new NFA_State();

    //attach the two new states
    new_start_state->add_transition(EPSILON, this->start_state);
    this->final_state->add_transition(EPSILON, new_final_state);

    //the repeating edge (that allows for more than one occurence of pattern)
    this->final_state->add_transition(EPSILON, new_start_state);

    //modifty starting and final state of this nfa
    this->start_state = new_start_state;
    this->final_state = new_final_state;

    //modify accepting pattern of final state
    this->final_state->set_accepting_pattern("( " + this->final_state->get_accepting_pattern() + " )+");

    //add EPSILON to alphabet
    this->alphabet->insert(EPSILON);
    states_count += 2;
}

void NFA::apply_star_closure() {
    this->apply_plus_closure();
    //add the zero edge (that enables zero occurence of pattern)
    this->start_state->add_transition(EPSILON, this->final_state);
    //replace the + of the plus closure with *
    unsigned int index = this->final_state->get_accepting_pattern().rfind('+');
    // a + must be found as we have called apply_plus_closure()
    assert(index != string::npos);
    this->final_state->set_accepting_pattern(this->final_state->get_accepting_pattern().replace(index, 1, "*"));
}

//joins the given NFA with this NFA
//TWO types of join exist NFA_JOIN_OR , NFA_JOIN_CONCATENATION

void NFA::join(NFA* nfa, int join_type) {
    //join can't be applied to combined NFA
    assert(combined_NFA == false);

    //check for null pointer
    assert(nfa != 0);
    //check for invalid join_type
    assert(join_type == NFA_JOIN_OR || join_type == NFA_JOIN_CONCATENATE);

    if (join_type == NFA_JOIN_OR)
        join_or(nfa);
    else
        join_concatenate(nfa);

    //add EPSILON to alphabet as Both ORing and Concatenation add epsilon transitions
    this->alphabet->insert(EPSILON);
}

//make a bfs traversal of the 2nd graph and pre-appends the given pattern to each of the states accepting patterns

void NFA::concatenate_pattern(NFA * nfa, string pattern) {
    //set used as visited flag for each state
    set<NFA_State *> visited;
    //bfs queue
    queue<NFA_State *> queue;
    visited.insert(nfa->start_state);
    queue.push(nfa->start_state);
    NFA_State * current_state;
    vector<INPUT_CHAR>* inputs;
    vector<NFA_State *>* states;
    while (!queue.empty()) {
        current_state = queue.front();
        queue.pop();
        //set accepting pattern
        current_state->set_accepting_pattern(pattern + current_state->get_accepting_pattern());

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

void NFA::number_states() {
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

void NFA::debug() {
    cout << "NFA Type : " << (combined_NFA ? "Combined NFA" : "Normal") << endl;
    cout << "Number of States : " << this->states_count << endl;
    cout << "Alphabet : ";
    set<INPUT_CHAR>::iterator it;
    for (it = alphabet->begin(); it != alphabet->end(); it++) {
        if (*it == -1)
            cout << "EPSILON , ";
        else
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

void NFA::join_concatenate(NFA* nfa) {
    //TODO revise this , and check for copy problem
    //STEPS
    //1 - copy nfa .. or not ?
    //2 - join final state in this nfa with the start state in the given nfa with epsilon transition
    //3 - union alphabet
    //4 - modify states count
    //5 - Accepting pattern :
    //  --- there is a problem here . all the acc_pattern of the states of the 2nd graph will need to be changed
    //  --- Suggested solution , traverse the 2nd graph and edit the accepting pattern
    //  --- but since accepting_pattern is not needed for the program to work correctly (used only for debugging)
    //  --- we can enable it just during the coding phase and disable it when we finish so that join_concatenate
    //  --- remains o(1) instead of o(n + e) where n is the number of states and e is number of edges in the 2nd nfa
    //6 - change the final state of this nfa to the final state of the other nfa
    //----------------------------------------------------

    //1
    // this implementation doesn't copy the second nfa
    //2
    this->final_state->add_transition(EPSILON, nfa->start_state);
    //3
    set<INPUT_CHAR>::iterator it;
    for (it = nfa->alphabet->begin(); it != nfa->alphabet->end(); it++)
        this->alphabet->insert(*it);
    //4
    this->states_count += nfa->get_states_count();
    //5 fix the accepting pattern
    //disable the following code in the project final phase
    concatenate_pattern(nfa, this->final_state->get_accepting_pattern());
    //6
    this->final_state = nfa->final_state;
}

void NFA::join_or(NFA* nfa) {
    //TODO revise this , and check for copy problem
    //STEPS
    //1 - copy nfa .. or not ?
    //2 - add 2 new states (start and end)
    //3 - add epsilon transitions from new start state to the start state of each nfa
    //	  add epsilon transitions from end states of each nfa to the new end state
    //4 - union alphabet
    //5 - modify accepting pattern of total final state
    //6 - modify states count
    //7 - modify start and end state of this nfa
    //----------------------------------------------------
    //1
    // this implementation doesn't copy the second nfa
    //2
    NFA_State *new_start_state = new NFA_State();
    NFA_State *new_final_state = new NFA_State();
    //3
    new_start_state->add_transition(EPSILON, this->start_state);
    new_start_state->add_transition(EPSILON, nfa->start_state);
    nfa->final_state->add_transition(EPSILON, new_final_state);
    this->final_state->add_transition(EPSILON, new_final_state);
    //4
    set<INPUT_CHAR>::iterator it;
    for (it = nfa->alphabet->begin(); it != nfa->alphabet->end(); it++)
        this->alphabet->insert(*it);
    //5
    new_final_state->set_accepting_pattern("( " + this->final_state->get_accepting_pattern() + " | " + nfa->final_state->get_accepting_pattern() + " )");
    //6
    this->states_count += nfa->get_states_count() + 2;
    //7
    this->start_state = new_start_state;
    this->final_state = new_final_state;
}

set<INPUT_CHAR>* NFA::get_alphabet() {
    set<INPUT_CHAR> * alphabet_copy = new set<INPUT_CHAR > ();
    set<INPUT_CHAR>::iterator it;
    for (it = alphabet->begin(); it != alphabet->end(); it++)
        alphabet_copy->insert(*it);
    return alphabet_copy;
}

int NFA::get_states_count() {
    return this->states_count;
}

bool NFA::is_combined_NFA() {
    return combined_NFA;
}

void NFA::finalize_NFA(int token_id) {
    this->final_state->set_token_id(token_id);
}

NFA* NFA::create_combined_NFA(vector<NFA*> * single_nfa_vector) {
    //Steps
    //1-create a new start state and initialize its variable (set the combined state boolean to true)
    //2-Combine
    //2.1-add epsilon transitions to start state of each nfa
    //2.2-set combined nfa alphabet to union alphabet of each nfa
    //2.3-set combined nfa state_count to sum of state_count of each nfa

    //1
    NFA* new_combined_NFA = new NFA();
    new_combined_NFA->combined_NFA = true;
    new_combined_NFA->start_state = new NFA_State();
    new_combined_NFA->final_state = NULL;
    new_combined_NFA->states_count = 1;
    //2.0
    for (unsigned int i = 0; i < single_nfa_vector->size(); ++i) {
        //2.1
        new_combined_NFA->start_state->add_transition(EPSILON, single_nfa_vector->at(i)->start_state);
        //2.2
        set<INPUT_CHAR>::iterator it;
        for (it = single_nfa_vector->at(i)->alphabet->begin(); it != single_nfa_vector->at(i)->alphabet->end(); it++)
            new_combined_NFA->alphabet->insert(*it);
        //2.3
        new_combined_NFA->states_count += single_nfa_vector->at(i)->states_count;
    }

    return new_combined_NFA;
}

NFA_State* NFA::get_start_state() {
    return this->start_state;
}


NFA::~NFA() {
    // TODO Implement the destructor
    delete alphabet;
    //must make decision about the copy problem
    //delete start_state;
    //delete final_state;
}
