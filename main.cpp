#include "move.h"
#include "state.h"
#include <boost/algorithm/string/join.hpp>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Move input_parse(string s) {
    Move result;
    istringstream ss(s);
    vector<string> vec;
    size_t vecpos = 0;
    copy(istream_iterator<string>(ss), istream_iterator<string>(), back_inserter(vec));

    if (vec[vecpos] == "P") {
        result.mode = Mode::P;
        pair<int, int> p1 = State::hex_to_ycoord(make_pair(stoi(vec[1]), stoi(vec[2])));
        result.placing_pos = p1;
    }

    else if (vec[vecpos] == "RS") {
        
    }
}
