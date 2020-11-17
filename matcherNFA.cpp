#include <iostream>
#include <vector>
#include "matcherNFA.h"


//NonDeterministicFiniteAutomation
std::vector<State> NonDeterministicFiniteAutomation::change_state(State &state, char in_symbol)
{
    std::vector<State> next_states;
    for (auto id: state.transitions){
        if (this->states[id].symbol == '\0' || this->states[id].symbol & in_symbol){
            next_states.push_back(states[id]);
        }
    }
    return next_states;
}

std::vector<char> NonDeterministicFiniteAutomation::convert_pattern(std::vector<int> pattern){
    std::vector<char> fragments;
    fragments.push_back(BLANK); //implicit conversion to char
    fragments.push_back('*');
    for (int p: pattern){
        for (int i{0}; i<p; i ++){
            fragments.push_back(BOX);
            fragments.push_back('.');
        }
        fragments.push_back(BLANK);
        fragments.push_back('+');
        fragments.push_back('.');
    }
    // the final blank is implicitly checked
    fragments.pop_back(); // remove .
    fragments.pop_back(); // remove +
    fragments.pop_back(); // remove BLANK 
    return fragments;
}



void NonDeterministicFiniteAutomation::compile(std::vector<int> pattern_)
{   
    /* convert a numerical sequence such as (a, b, ..) to the following regex ([23]*)([13]){a}([b]+)....([13]){z}([12]*)
    e.g. (1,5,3) -> ([23]*)([13]){1}([12]+)([13]){5}([12]+)([13]){3}([12]+)([23]*) */

    //reset 
    this->pattern = pattern_;
    num_states = 0;
    is_compiled = false;
    this->states = {};

    // add start and end to state vector
    State start {'\0', num_states++, 's'};
    states.push_back(start);
    State end {'\0', num_states++}; //continuously update the end
    states.push_back(end);
    
    // match an empty pattern
    if (pattern.size() == 0){
        states[1].is_end = true;
        states[1].symbol = BLANK; //
        states[0].transitions.push_back(1);
        is_compiled = true;
        return;
    }

    //general case
    std::stack<int> st;  // state ids to modify
    st.push(0);  // add start to the stack
    st.push(1);  // add end to the stack
    int next_, state, prev_;  // always work with these 3 states. Underscore because next and prev are std keywords
    std::vector<char> fragments = convert_pattern(pattern);
    for (char sym: fragments){
        State new_end;  // only used for "default"
        switch (sym){
            case '+':  // one or more
                next_ = st.top();   st.pop();
                state = st.top();  
                this->states[state].qualifier = '+';
                //this->states[state].transitions.insert(this->states[state].transitions.begin(), next_); //normal catenation
                this->states[state].transitions.push_back(state); //loop back on itself  
                st.push(next_);
                break;
            case '*': //zero or more.
                next_ = st.top();   st.pop();
                state = st.top();   st.pop();
                prev_ = st.top();
                this->states[state].transitions.insert(this->states[state].transitions.begin(), next_); //normal catenation
                this->states[state].transitions.push_back(state); //loop back on itself  
                this->states[state].qualifier = '*';
                this->states[prev_].transitions.insert(this->states[prev_].transitions.begin(), state); //normal catenation
               // this->states[prev_].transitions.insert(this->states[prev_].transitions.begin(), next_); //option to skip this state
                st.push(next_); //state is not added to the stack
                break;
            case '.': // catenation
                next_ = st.top();   st.pop();
                state = st.top();   st.pop();
                prev_ = st.top();   // prev is kept on the stack as the start (incase of multiple consecutive *)
                this->states[prev_].transitions.insert(this->states[prev_].transitions.begin(), state);
                st.push(state);
                st.push(next_); 
                break;
            default:
                next_ = st.top();
                this->states[next_].symbol = sym;
                new_end.id = num_states++;
                st.push(new_end.id);
                this->states.push_back(new_end);
        }
    }
    this->states.pop_back(); // remove unnecessary end
    states.back().is_end = true;
    is_compiled = true;
}   


std::string line_to_string(std::vector<int>& line, std::vector<int>& pattern){
    std::string out = "";
    // first append the line
    for (int & cell: line){
        out = out.append(std::to_string(cell));
    }
    out.append("-");
    // then append the pattern. Each run should take up at least 3 spaces (for uniqueness)
    int n_zero = 3; 
    for (int & p: pattern){
        std::string p_str = std::to_string(p);
        if (n_zero - p_str.length() > 0){
            p_str = std::string(n_zero - p_str.length(), '0') + p_str; //# prepend with zeros
        }
        out.append(p_str);
    }
    return out;
}


Match NonDeterministicFiniteAutomation::find_match(std::vector<int>& line){
    std::string hash_string = line_to_string(line, this->pattern); // can't hash a vector, so hash the string instead
    Match result;
    if (this->cache->exists(hash_string)){
        result = this->cache->get(hash_string);
        ++(cache->hits);
    }
    else {
        result = this->find_match_(line);
        this->cache->put(hash_string, result);
        ++(this->cache->misses);
    }
    return result;
}

std::vector<int> extract_keys(std::unordered_map<int, std::vector<int> > const& input_map) {
  std::vector<int> keys;
  for (auto const& element : input_map) {
    keys.push_back(element.first);
  }
  return keys;
}

bool is_finished(std::vector<int>& array, int idx){
    for (int i = idx+1; i < array.size(); ++i){
        if (array[i] == BOX){
            return false;
        }
    }
    return true;
}

Match NonDeterministicFiniteAutomation::find_match_(std::vector<int>& array){
    if (!this->is_compiled){
        throw "The NFA was not compiled!";
    }
    if (pattern.empty() && array.empty()){
        return Match{{}, {}, true};
    }

    int idx = -1; // index in the array
    std::unordered_map<int, std::vector<int>> matches; //key, value pair is state_id, match
    std::unordered_map<int, std::vector<int>> new_matches; //key, value pair is state_id, match

    std::vector<int> empty_vec;
    empty_vec.reserve(array.size());
    matches.insert(std::pair<int, std::vector<int>>(0, empty_vec)); 
    while (idx < (int)array.size() - 1 && !matches.empty()){
        ++idx;    
        std::unordered_map<int, std::vector<int>>::iterator it;
        for (it = matches.begin(); it != matches.end(); ++it){
            int state_id = it->first; 
            std::vector<int>* match = &it->second;
            State* state = &states[state_id];
            for (int next_id: state->transitions){
                // is this a valid transition?
                if (this->states[next_id].symbol == '\0' || this->states[next_id].symbol & array[idx]){
                    State* next_state = &states[next_id];
                    if (next_state->is_end){
                        if (is_finished(array, idx)){
                            Match m{*match, this->pattern, true};
                            std::vector<int>* match_final = &m.match;
                            match_final->push_back(next_state->symbol);
                            // add final set of blanks
                            std::vector<int> trailing_zeros(array.size() - (idx+1), BLANK);
                            match_final->insert( match_final->end(), trailing_zeros.begin(), trailing_zeros.end() );                       
                            return m;
                        } 
                    }
                    // keep only the earliest repeated states in the stack 
                    else if ((next_state->id == state->id) || (new_matches.find(next_state->id) == new_matches.end())){
                        new_matches[next_state->id] = *match; //overwrite newer entries for repeats
                        new_matches[next_state->id].push_back(next_state->symbol);
                    }
                } // else skip this transition
            } // move to the next active state
        }
        matches.swap(new_matches);
        new_matches.clear();
    }
    return Match {{}, this->pattern, false};  // no match was found
}
