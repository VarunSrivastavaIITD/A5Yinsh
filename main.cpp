#include "move.h"
#include "state.h"
#include <boost/algorithm/string/join.hpp>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

typedef pair<int, int> Coordinate;

Move input_parse(string s) 
{
    Move result;
    int vecpos;
    int size;
    vector<string> vec;
    istringstream ss(s);

    copy(istream_iterator<string>(ss), istream_iterator<string>(), back_inserter(vec));
    size = vec.size();

    if (vec[0] == "P") 
    {
        result.mode = Mode_M::P;
        Coordinate p1 = State::hex_to_ycoord(make_pair(stoi(vec[1]), stoi(vec[2])));
        result.placing_pos = p1;
        return result;
    }

    // else if (vec[0] == "S")
    // {
    //     result.mode = Mode::S;
    //     Coordinate p1 = State::hex_to_ycoord(make_pair(stoi(vec[1]), stoi(vec[2])));
    //     Coordinate p2 = State::hex_to_ycoord(make_pair(stoi(vec[4]), stoi(vec[5])));
    //     result.initial_pos = p1;
    //     result.final_pos = p2;

    //     vecpos = 6;
    //     Move::MarkerRow m;

    //     while(vecpos != size)
    //     {
    //         Coordinate p1 = State::hex_to_ycoord(make_pair(stoi(vec[vecpos + 1]), stoi(vec[vecpos + 2])));
    //         Coordinate p2 = State::hex_to_ycoord(make_pair(stoi(vec[vecpos + 4]), stoi(vec[vecpos + 5])));
    //         Coordinate p3 = State::hex_to_ycoord(make_pair(stoi(vec[vecpos + 7]), stoi(vec[vecpos + 8])));
    //         m.start = p1;
    //         m.end = p2;
    //         m.ring = p3;
    //         result.final_removal.push_back(m);
    //         vecpos += 9;
    //     }

    //     return result;
    // }

    else
    {
        result.mode = Mode_M::S;

        vecpos = 0;
        Move::MarkerRow m;

        while(vec[vecpos] != "S")
        {
            Coordinate p1 = State::hex_to_ycoord(make_pair(stoi(vec[vecpos + 1]), stoi(vec[vecpos + 2])));
            Coordinate p2 = State::hex_to_ycoord(make_pair(stoi(vec[vecpos + 4]), stoi(vec[vecpos + 5])));
            Coordinate p3 = State::hex_to_ycoord(make_pair(stoi(vec[vecpos + 7]), stoi(vec[vecpos + 8])));
            m.start = p1;
            m.end = p2;
            m.ring = p3;
            result.initial_removal.push_back(m);
            vecpos += 9;
        } 

        Coordinate p1 = State::hex_to_ycoord(make_pair(stoi(vec[vecpos + 1]), stoi(vec[vecpos + 2])));
        Coordinate p2 = State::hex_to_ycoord(make_pair(stoi(vec[vecpos + 4]), stoi(vec[vecpos + 5])));
        result.initial_pos = p1;
        result.final_pos = p2;
        vecpos += 6;

        while(vecpos != size)
        {
            Coordinate p1 = State::hex_to_ycoord(make_pair(stoi(vec[vecpos + 1]), stoi(vec[vecpos + 2])));
            Coordinate p2 = State::hex_to_ycoord(make_pair(stoi(vec[vecpos + 4]), stoi(vec[vecpos + 5])));
            Coordinate p3 = State::hex_to_ycoord(make_pair(stoi(vec[vecpos + 7]), stoi(vec[vecpos + 8])));
            m.start = p1;
            m.end = p2;
            m.ring = p3;
            result.final_removal.push_back(m);
            vecpos += 9;
        }

        return result;
    }

    return result;
}

string output_parse(Move m)
{
    string s;
    if(m.mode == Mode_M::P)
    {
        s = "P ";
        s += to_string(m.placing_pos.first);
        s += " ";
        s += to_string(m.placing_pos.second);
        return s;
    }
    else
    {
        while(m.initial_removal.size()!=0)
        {
            s += "RS ";
            Coordinate p1 = State::ycoord_to_hex(m.initial_removal.front().start);
            s += to_string(p1.first);
            s += " ";
            s += to_string(p1.second);
            s += " ";
            s += "RE ";
            Coordinate p2 = State::ycoord_to_hex(m.initial_removal.front().end);
            s += to_string(p2.first);
            s += " ";
            s += to_string(p2.second);
            s += " ";
            s += "X ";
            Coordinate p3 = State::ycoord_to_hex(m.initial_removal.front().ring);
            s += to_string(p3.first);
            s += " ";
            s += to_string(p3.second);
            s += " ";
            m.initial_removal.pop_front();
        }  

        s += "S ";
        Coordinate p1 = State::ycoord_to_hex(m.initial_pos);
        s += to_string(p1.first);
        s += " ";
        s += to_string(p1.second);
        s += " ";
        s += "M ";
        Coordinate p2 = State::ycoord_to_hex(m.final_pos);
        s += to_string(p2.first);
        s += " ";
        s += to_string(p2.second);
        s += " ";
        
        while(m.final_removal.size()!=0)
        {
            s += "RS ";
            Coordinate p1 = State::ycoord_to_hex(m.final_removal.front().start);
            s += to_string(p1.first);
            s += " ";
            s += to_string(p1.second);
            s += " ";
            s += "RE ";
            Coordinate p2 = State::ycoord_to_hex(m.final_removal.front().end);
            s += to_string(p2.first);
            s += " ";
            s += to_string(p2.second);
            s += " ";
            s += "X ";
            Coordinate p3 = State::ycoord_to_hex(m.final_removal.front().ring);
            s += to_string(p3.first);
            s += " ";
            s += to_string(p3.second);
            s += " ";
            m.final_removal.pop_front();
        }  

        if(s[s.size()-1] == ' ')
            s.pop_back();
        
        return s;
    }
    return s;
}