#include "move.h"
#include "state.h"
#include <boost/algorithm/string/join.hpp>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

typedef pair<int, int> Coordinate;

Move input_parse(string s) {
  Move result;
  int vecpos;
  int size;
  vector<string> vec;
  istringstream ss(s);

  copy(istream_iterator<string>(ss), istream_iterator<string>(),
       back_inserter(vec));
  size = vec.size();

  if (vec[0] == "P") {
    result.mode = Mode_M::P;
    Coordinate p1 = State::hex_to_ycoord(make_pair(stoi(vec[1]), stoi(vec[2])));
    result.placing_pos = p1;
    return result;
  }

  // else if (vec[0] == "S")
  // {
  //     result.mode = Mode::S;
  //     Coordinate p1 = State::hex_to_ycoord(make_pair(stoi(vec[1]),
  //     stoi(vec[2]))); Coordinate p2 =
  //     State::hex_to_ycoord(make_pair(stoi(vec[4]), stoi(vec[5])));
  //     result.initial_pos = p1;
  //     result.final_pos = p2;

  //     vecpos = 6;
  //     Move::MarkerRow m;

  //     while(vecpos != size)
  //     {
  //         Coordinate p1 = State::hex_to_ycoord(make_pair(stoi(vec[vecpos +
  //         1]), stoi(vec[vecpos + 2]))); Coordinate p2 =
  //         State::hex_to_ycoord(make_pair(stoi(vec[vecpos + 4]),
  //         stoi(vec[vecpos + 5]))); Coordinate p3 =
  //         State::hex_to_ycoord(make_pair(stoi(vec[vecpos + 7]),
  //         stoi(vec[vecpos + 8]))); m.start = p1; m.end = p2; m.ring = p3;
  //         result.final_removal.push_back(m);
  //         vecpos += 9;
  //     }

  //     return result;
  // }

  else {
    result.mode = Mode_M::S;

    vecpos = 0;
    Move::MarkerRow m;

    while (vec[vecpos] != "S") {
      Coordinate p1 = State::hex_to_ycoord(
          make_pair(stoi(vec[vecpos + 1]), stoi(vec[vecpos + 2])));
      Coordinate p2 = State::hex_to_ycoord(
          make_pair(stoi(vec[vecpos + 4]), stoi(vec[vecpos + 5])));
      Coordinate p3 = State::hex_to_ycoord(
          make_pair(stoi(vec[vecpos + 7]), stoi(vec[vecpos + 8])));
      m.start = p1;
      m.end = p2;
      m.ring = p3;
      result.initial_removal.push_back(m);
      vecpos += 9;
    }

    Coordinate p1 = State::hex_to_ycoord(
        make_pair(stoi(vec[vecpos + 1]), stoi(vec[vecpos + 2])));
    Coordinate p2 = State::hex_to_ycoord(
        make_pair(stoi(vec[vecpos + 4]), stoi(vec[vecpos + 5])));
    result.initial_pos = p1;
    result.final_pos = p2;
    vecpos += 6;

    while (vecpos != size) {
      Coordinate p1 = State::hex_to_ycoord(
          make_pair(stoi(vec[vecpos + 1]), stoi(vec[vecpos + 2])));
      Coordinate p2 = State::hex_to_ycoord(
          make_pair(stoi(vec[vecpos + 4]), stoi(vec[vecpos + 5])));
      Coordinate p3 = State::hex_to_ycoord(
          make_pair(stoi(vec[vecpos + 7]), stoi(vec[vecpos + 8])));
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

string output_parse(Move m) {
  string s;
  if (m.mode == Mode_M::P) {
    s = "P ";
    s += to_string(m.placing_pos.first);
    s += " ";
    s += to_string(m.placing_pos.second);
    return s;
  } else {
    while (m.initial_removal.size() != 0) {
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

    while (m.final_removal.size() != 0) {
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

    if (s[s.size() - 1] == ' ')
      s.pop_back();

    return s;
  }
  return s;
}

State remove_some_markers(State state, Coordinate start, Coordinate end) {}

State remove_toggle_combined(Coordinate p2, Coordinate p3, State state,
                             int both_exclusive) {
  State newstate = state;
  auto flip = [](decltype(newstate.black_markers) &s,
                 decltype(newstate.black_markers) &t,
                 const decltype(newstate.black_markers)::value_type &e) {
    auto search = s.find(e);
    if (search != s.end())
      s.erase(search);
    t.insert(e);
  };

  if (p2.first == p3.first) {
    if (p3.second > p2.second) {
      for (int i = p2.second + both_exclusive; i <= p3.second - both_exclusive;
           i++) {
        if (both_exclusive == 1) {
          auto search = newstate.board_map.find(make_pair(p2.first, i));
          if (search != newstate.board_map.end()) {
            if (newstate.board_map[make_pair(p2.first, i)] == WHITE_MARKER) {
              newstate.board_map[make_pair(p2.first, i)] = BLACK_MARKER;
              flip(newstate.white_markers, newstate.black_markers,
                   make_pair(p2.first, i));
            } else if (newstate.board_map[make_pair(p2.first, i)] ==
                       BLACK_MARKER) {
              newstate.board_map[make_pair(p2.first, i)] = WHITE_MARKER;
              flip(newstate.black_markers, newstate.white_markers,
                   make_pair(p2.first, i));
            }
          }
        } else if (both_exclusive == 0) {
        }
      }
    }

    else {
      for (int i = p2.second - both_exclusive; i >= p3.second + both_exclusive;
           i--) {
        if (both_exclusive == 1) {
          auto search = newstate.board_map.find(make_pair(p2.first, i));
          if (search != newstate.board_map.end()) {
            if (newstate.board_map[make_pair(p2.first, i)] == WHITE_MARKER) {
              newstate.board_map[make_pair(p2.first, i)] = BLACK_MARKER;
              flip(newstate.white_markers, newstate.black_markers,
                   make_pair(p2.first, i));
            } else if (newstate.board_map[make_pair(p2.first, i)] ==
                       BLACK_MARKER) {
              newstate.board_map[make_pair(p2.first, i)] = WHITE_MARKER;
              flip(newstate.black_markers, newstate.white_markers,
                   make_pair(p2.first, i));
            }
          }
        } else if (both_exclusive == 0) {
        }
      }
    }
  }

  else if (p2.second == p3.second) {
    if (p3.first > p2.first) {
      for (int i = p2.first + both_exclusive; i <= p3.first - both_exclusive;
           i++) {
        if (both_exclusive == 1) {
          auto search = newstate.board_map.find(make_pair(i, p2.second));
          if (search != newstate.board_map.end()) {
            if (newstate.board_map[make_pair(i, p2.second)] == WHITE_MARKER) {
              newstate.board_map[make_pair(i, p2.second)] = BLACK_MARKER;
              flip(newstate.white_markers, newstate.black_markers,
                   make_pair(i, p2.second));
            } else if (newstate.board_map[make_pair(i, p2.second)] ==
                       BLACK_MARKER) {
              newstate.board_map[make_pair(i, p2.second)] = WHITE_MARKER;
              flip(newstate.black_markers, newstate.white_markers,
                   make_pair(i, p2.second));
            }
          }
        } else if (both_exclusive == 0) {
        }
      }
    } else {
      for (int i = p2.first - both_exclusive; i >= p3.first + both_exclusive;
           i--) {
        if (both_exclusive == 1) {
          auto search = newstate.board_map.find(make_pair(i, p2.second));
          if (search != newstate.board_map.end()) {
            if (newstate.board_map[make_pair(i, p2.second)] == WHITE_MARKER) {
              newstate.board_map[make_pair(i, p2.second)] = BLACK_MARKER;
              flip(newstate.white_markers, newstate.black_markers,
                   make_pair(i, p2.second));
            } else if (newstate.board_map[make_pair(i, p2.second)] ==
                       BLACK_MARKER) {
              newstate.board_map[make_pair(i, p2.second)] = WHITE_MARKER;
              flip(newstate.black_markers, newstate.white_markers,
                   make_pair(i, p2.second));
            }
          }
        } else if (both_exclusive == 0) {
        }
      }
    }
  }

  else {
    if (p3.first > p2.first) {
      for (int i = p2.first + both_exclusive, j = p2.second + both_exclusive;
           i <= p3.first - both_exclusive; i++, j++) {
        if (both_exclusive == 1) {
          auto search = newstate.board_map.find(make_pair(i, j));
          if (search != newstate.board_map.end()) {
            if (newstate.board_map[make_pair(i, j)] == WHITE_MARKER) {
              newstate.board_map[make_pair(i, j)] = BLACK_MARKER;
              flip(newstate.white_markers, newstate.black_markers,
                   make_pair(i, j));
            } else if (newstate.board_map[make_pair(i, j)] == BLACK_MARKER) {
              newstate.board_map[make_pair(i, j)] = WHITE_MARKER;
              flip(newstate.black_markers, newstate.white_markers,
                   make_pair(i, j));
            }
          }
        } else if (both_exclusive == 0) {
        }
      }
    } else {
      for (int i = p2.first - both_exclusive, j = p2.second - both_exclusive;
           i >= p3.first + both_exclusive; i--, j--) {
        if (both_exclusive == 1) {
          auto search = newstate.board_map.find(make_pair(i, j));
          if (search != newstate.board_map.end()) {
            if (newstate.board_map[make_pair(i, j)] == WHITE_MARKER) {
              newstate.board_map[make_pair(i, j)] = BLACK_MARKER;
              flip(newstate.white_markers, newstate.black_markers,
                   make_pair(i, j));
            } else if (newstate.board_map[make_pair(i, j)] == BLACK_MARKER) {
              newstate.board_map[make_pair(i, j)] = WHITE_MARKER;
              flip(newstate.black_markers, newstate.white_markers,
                   make_pair(i, j));
            }
          }
        } else if (both_exclusive == 0) {
        }
      }
    }
  }
  return newstate;
}

State perform_move(State state, Move move, int M) {
  State newstate = state;

  auto delete_from_set =
      [](decltype(newstate.black_markers) &s,
         const decltype(newstate.black_markers)::value_type &e) {
        auto search = s.find(e);
        if (search != s.end())
          s.erase(search);
      };

  auto update_from_map =
      [](decltype(newstate.board_map) &s,
         const decltype(newstate.board_map)::key_type &e,
         const decltype(newstate.board_map)::mapped_type &f) {
        auto search = s.find(e);
        if (search != s.end())
          s[e] = f;
      };

  decltype(state.board_map)::mapped_type ring;
  decltype(state.board_map)::mapped_type marker;
  auto rings = std::ref(newstate.white_rings);
  auto otherrings = std::ref(newstate.black_rings);
  auto markers = std::ref(newstate.white_markers);
  auto othermarkers = std::ref(newstate.black_markers);

  if (state.player == WHITE) {
    marker = WHITE_MARKER;
    ring = WHITE_RING;
    markers = std::ref(newstate.white_markers);
    othermarkers = std::ref(newstate.black_markers);
    rings = std::ref(newstate.white_rings);
    otherrings = std::ref(newstate.black_rings);
  } else {
    marker = BLACK_MARKER;
    ring = BLACK_RING;
    markers = std::ref(newstate.black_markers);
    othermarkers = std::ref(newstate.white_markers);
    rings = std::ref(newstate.black_rings);
    otherrings = std::ref(newstate.white_rings);
  }

  if (move.mode == Mode_M::P) {
    // check if board Coordinate exists
    update_from_map(newstate.board_map, move.placing_pos, ring);
    rings.get().insert(move.placing_pos);
    if (rings.get().size() == M && otherrings.get().size() == M) {
      newstate.mode = Mode_S::S;
    }
    return newstate;
  } else {
    while (move.initial_removal.size() != 0) {
      newstate = remove_toggle_combined(move.initial_removal.front().start,
                                         move.initial_removal.front().end,
                                         newstate, 0);
      delete_from_set(rings.get(), move.initial_removal.front().ring);
      move.initial_removal.pop_front();
    }

    update_from_map(newstate.board_map, move.initial_pos, marker);
    update_from_map(newstate.board_map, move.final_pos, ring);
    delete_from_set(rings.get(), move.initial_pos);
    rings.get().insert(move.final_pos);
    markers.get().insert(move.initial_pos);
    newstate =
        remove_toggle_combined(move.initial_pos, move.final_pos, newstate, 1);

    while (move.final_removal.size() != 0) {
      newstate =
          remove_toggle_combined(move.final_removal.front().start,
                                  move.final_removal.front().end, newstate, 0);
      delete_from_set(rings.get(), move.final_removal.front().ring);
      move.final_removal.pop_front();
    }

    return newstate;
  }
  return newstate;
}

int main() {
  string s = "RS 1 2 RE 3 4 X 5 6 RS 1 2 RE 3 4 X 5 6 S 1 2 M 3 4 RS 1 2 RE 3 "
             "4 X 5 6 RS 1 2 RE 3 4 X 5 6";
  Move m = input_parse(s);
  string t = output_parse(m);
  cout << s << endl;
  cout << t << endl;
  if (t == s) {
    cout << "i/p o/p done";
  }
  return 0;
}