#include "move.h"
#include "state.h"
#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <boost/bimap.hpp>
#include <chrono>
#include <functional>
#include <iostream>
#include <sstream>

#include <string>
#include <vector>

const Coordinate DUMMY_COORDINATE(10, 10);
const int WINNING_RINGS = 3;

typedef boost::bimap<pair<int, int>, pair<int, int>> bm_type;
typedef bm_type::value_type bm_value_type;

template <typename Iterator>
bool next_combination(const Iterator first, Iterator k, const Iterator last) {
    if ((first == last) || (first == k) || (last == k))
        return false;
    Iterator i1 = first;
    Iterator i2 = last;
    ++i1;
    if (last == i1)
        return false;
    i1 = last;
    --i1;
    i1 = k;
    --i2;
    while (first != i1) {
        if (*--i1 < *i2) {
            Iterator j = k;
            while (!(*i1 < *j))
                ++j;
            std::iter_swap(i1, j);
            ++i1;
            ++j;
            i2 = k;
            std::rotate(i1, j, last);
            while (last != j) {
                ++j;
                ++i2;
            }
            std::rotate(k, i2, last);
            return true;
        }
    }
    std::rotate(first, k, last);
    return false;
}

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

State remove_toggle_combined(Coordinate p2, Coordinate p3, State state, int both_exclusive) {
    State newstate = state;

    auto flip = [](decltype(newstate.black_markers) &s, decltype(newstate.black_markers) &t, const decltype(newstate.black_markers)::value_type &e) {
        auto search = s.find(e);
        if (search != s.end())
            s.erase(search);
        t.insert(e);
    };

    auto delete_from_set = [](decltype(newstate.black_markers) &s, const decltype(newstate.black_markers)::value_type &e) {
        auto search = s.find(e);
        if (search != s.end())
            s.erase(search);
    };

    if (p2.first == p3.first) {
        if (p3.second > p2.second) {
            for (int i = p2.second + both_exclusive; i <= p3.second - both_exclusive; i++) {
                auto search = newstate.board_map.find(make_pair(p2.first, i));
                if (search != newstate.board_map.end()) {
                    if (newstate.board_map[make_pair(p2.first, i)] == WHITE_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[make_pair(p2.first, i)] = BLACK_MARKER;
                            flip(newstate.white_markers, newstate.black_markers, make_pair(p2.first, i));
                        } else if (both_exclusive == 0) {
                            newstate.board_map[make_pair(p2.first, i)] = EMPTY;
                            delete_from_set(newstate.white_markers, make_pair(p2.first, i));
                        }
                    } else if (newstate.board_map[make_pair(p2.first, i)] == BLACK_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[make_pair(p2.first, i)] = WHITE_MARKER;
                            flip(newstate.black_markers, newstate.white_markers, make_pair(p2.first, i));
                        } else if (both_exclusive == 0) {
                            newstate.board_map[make_pair(p2.first, i)] = EMPTY;
                            delete_from_set(newstate.black_markers, make_pair(p2.first, i));
                        }
                    }
                }
            }
        } else {
            for (int i = p2.second - both_exclusive; i >= p3.second + both_exclusive; i--) {
                auto search = newstate.board_map.find(make_pair(p2.first, i));
                if (search != newstate.board_map.end()) {
                    if (newstate.board_map[make_pair(p2.first, i)] == WHITE_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[make_pair(p2.first, i)] = BLACK_MARKER;
                            flip(newstate.white_markers, newstate.black_markers, make_pair(p2.first, i));
                        } else if (both_exclusive == 0) {
                            newstate.board_map[make_pair(p2.first, i)] = EMPTY;
                            delete_from_set(newstate.white_markers, make_pair(p2.first, i));
                        }
                    } else if (newstate.board_map[make_pair(p2.first, i)] == BLACK_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[make_pair(p2.first, i)] = WHITE_MARKER;
                            flip(newstate.black_markers, newstate.white_markers, make_pair(p2.first, i));
                        } else if (both_exclusive == 0) {
                            newstate.board_map[make_pair(p2.first, i)] = EMPTY;
                            delete_from_set(newstate.black_markers, make_pair(p2.first, i));
                        }
                    }
                }
            }
        }
    }

    else if (p2.second == p3.second) {
        if (p3.first > p2.first) {
            for (int i = p2.first + both_exclusive; i <= p3.first - both_exclusive; i++) {

                auto search = newstate.board_map.find(make_pair(i, p2.second));
                if (search != newstate.board_map.end()) {
                    if (newstate.board_map[make_pair(i, p2.second)] == WHITE_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[make_pair(i, p2.second)] = BLACK_MARKER;
                            flip(newstate.white_markers, newstate.black_markers, make_pair(i, p2.second));
                        } else if (both_exclusive == 0) {
                            newstate.board_map[make_pair(i, p2.second)] = EMPTY;
                            delete_from_set(newstate.white_markers, make_pair(i, p2.second));
                        }
                    } else if (newstate.board_map[make_pair(i, p2.second)] == BLACK_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[make_pair(i, p2.second)] = WHITE_MARKER;
                            flip(newstate.black_markers, newstate.white_markers, make_pair(i, p2.second));
                        } else if (both_exclusive == 0) {
                            newstate.board_map[make_pair(i, p2.second)] = EMPTY;
                            delete_from_set(newstate.black_markers, make_pair(i, p2.second));
                        }
                    }
                }
            }
        } else {
            for (int i = p2.first - both_exclusive; i >= p3.first + both_exclusive; i--) {
                auto search = newstate.board_map.find(make_pair(i, p2.second));
                if (search != newstate.board_map.end()) {
                    if (newstate.board_map[make_pair(i, p2.second)] == WHITE_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[make_pair(i, p2.second)] = BLACK_MARKER;
                            flip(newstate.white_markers, newstate.black_markers, make_pair(i, p2.second));
                        } else if (both_exclusive == 0) {
                            newstate.board_map[make_pair(i, p2.second)] = EMPTY;
                            delete_from_set(newstate.white_markers, make_pair(i, p2.second));
                        }
                    } else if (newstate.board_map[make_pair(i, p2.second)] == BLACK_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[make_pair(i, p2.second)] = WHITE_MARKER;
                            flip(newstate.black_markers, newstate.white_markers, make_pair(i, p2.second));
                        } else if (both_exclusive == 0) {
                            newstate.board_map[make_pair(i, p2.second)] = EMPTY;
                            delete_from_set(newstate.black_markers, make_pair(i, p2.second));
                        }
                    }
                }
            }
        }
    }

    //check for difference
    else {
        if (p3.first > p2.first) {
            for (int i = p2.first + both_exclusive, j = p2.second + both_exclusive; i <= p3.first - both_exclusive; i++, j++) {
                auto search = newstate.board_map.find(make_pair(i, j));
                if (search != newstate.board_map.end()) {
                    if (newstate.board_map[make_pair(i, j)] == WHITE_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[make_pair(i, j)] = BLACK_MARKER;
                            flip(newstate.white_markers, newstate.black_markers, make_pair(i, j));
                        } else if (both_exclusive == 0) {
                            newstate.board_map[make_pair(i, j)] = EMPTY;
                            delete_from_set(newstate.white_markers, make_pair(i, j));
                        }
                    } else if (newstate.board_map[make_pair(i, j)] == BLACK_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[make_pair(i, j)] = WHITE_MARKER;
                            flip(newstate.black_markers, newstate.white_markers, make_pair(i, j));
                        } else if (both_exclusive == 0) {
                            newstate.board_map[make_pair(i, j)] = EMPTY;
                            delete_from_set(newstate.black_markers, make_pair(i, j));
                        }
                    }
                }
            }
        } else {
            for (int i = p2.first - both_exclusive, j = p2.second - both_exclusive; i >= p3.first + both_exclusive; i--, j--) {
                auto search = newstate.board_map.find(make_pair(i, j));
                if (search != newstate.board_map.end()) {
                    if (newstate.board_map[make_pair(i, j)] == WHITE_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[make_pair(i, j)] = BLACK_MARKER;
                            flip(newstate.white_markers, newstate.black_markers, make_pair(i, j));
                        } else if (both_exclusive == 0) {
                            newstate.board_map[make_pair(i, j)] = EMPTY;
                            delete_from_set(newstate.white_markers, make_pair(i, j));
                        }
                    } else if (newstate.board_map[make_pair(i, j)] == BLACK_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[make_pair(i, j)] = WHITE_MARKER;
                            flip(newstate.black_markers, newstate.white_markers, make_pair(i, j));
                        } else if (both_exclusive == 0) {
                            newstate.board_map[make_pair(i, j)] = EMPTY;
                            delete_from_set(newstate.black_markers, make_pair(i, j));
                        }
                    }
                }
            }
        }
    }
    return newstate;
}

Move minimax(State state, int depth, size_t K, size_t M) {
    vector<Move> moves;
    State newstate;
    moves = get_all_moves(state, K, M);
    long max_score, score;
    Move max_move;
    for (auto it1 = moves.begin(); it1 != moves.end(); it1++) {
        newstate = perform_move(newstate, *it1, M);
        score = minimax_util(newstate, depth - 1, K, M);
        if (score > max_score) {
            max_score = score;
            max_move = *it1;
        }
    }
    return max_move;
}

long minimax_util(State state, int depth, size_t K, size_t M) {
    vector<Move> moves;
    State newstate;
    moves = get_all_moves(state, K, M);
    long ans;
    long max_score, score, min_score;
    // Move max_move, min_move;
    max_score = numeric_limits<long>::min();
    min_score = numeric_limits<long>::max();
    if (depth == 0) {
        return (heuristic(state));
    } else if (state.player == WHITE) {
        for (auto it1 = moves.begin(); it1 != moves.end(); it1++) {
            newstate = perform_move(newstate, *it1, M);
            score = minimax_util(newstate, depth - 1, K, M);
            if (score > max_score) {
                max_score = score;
                // max_move = *it1;
            }
        }
        return max_score;
    } else if (state.player == BLACK) {
        for (auto it1 = moves.begin(); it1 != moves.end(); it1++) {
            newstate = perform_move(newstate, *it1, M);
            score = minimax_util(newstate, depth - 1, K, M);
            ;
            if (score < min_score) {
                min_score = score;
                // min_move = *it1;
            }
        }
        return min_score;
    }
    return ans;
}

State perform_move(const State state, Move move, size_t M) {
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

    if (state.player == WHITE)
        newstate.player = BLACK;
    else
        newstate.player = WHITE;

    if (move.mode == Mode_M::P) {
        //check if board Coordinate exists
        update_from_map(newstate.board_map, move.placing_pos, ring);
        rings.get().insert(move.placing_pos);
        if (rings.get().size() == M && otherrings.get().size() == M) {
            newstate.mode = Mode_S::S;
        }
        return newstate;
    } else {
        while (move.initial_removal.size() != 0) {
            newstate = remove_toggle_combined(move.initial_removal.front().start, move.initial_removal.front().end, newstate, 0);
            delete_from_set(rings.get(), move.initial_removal.front().ring);
            update_from_map(newstate.board_map, move.initial_removal.front().ring, EMPTY);
            move.initial_removal.pop_front();
        }

        if (move.initial_pos != DUMMY_COORDINATE) {
            update_from_map(newstate.board_map, move.initial_pos, marker);
            update_from_map(newstate.board_map, move.final_pos, ring);
            delete_from_set(rings.get(), move.initial_pos);
            rings.get().insert(move.final_pos);
            markers.get().insert(move.initial_pos);
            newstate = remove_toggle_combined(move.initial_pos, move.final_pos, newstate, 1);
        }

        while (move.final_removal.size() != 0) {
            newstate = remove_toggle_combined(move.final_removal.front().start, move.final_removal.front().end, newstate, 0);
            delete_from_set(rings.get(), move.final_removal.front().ring);
            update_from_map(newstate.board_map, move.final_removal.front().ring, EMPTY);
            move.final_removal.pop_front();
        }

        return newstate;
    }
    return newstate;
}

std::deque<Coordinate> check_consecutive_markers(const State &state, size_t K = 5) {
    std::map<pair<int, int>, std::tuple<int, int, int>> marker_lines;
    const auto markers = state.get_player_markers();
    auto bmap = state.board_map;
    auto marker = Player::WHITE ? state.player == Player::WHITE : Player::BLACK;

    for (auto it = markers.cbegin(); it != markers.cend(); ++it)
        marker_lines[*it] = std::make_tuple(1, 1, 1);

    for (const auto &coordinate : markers) {

        if (get<0>(marker_lines[coordinate]) == 1) {
            std::deque<Coordinate> line;
            line.push_back(coordinate);
            for (auto y = coordinate.second + 1; bmap.find(std::make_pair(coordinate.first, y)) != bmap.end(); ++y) {
                if (bmap[std::make_pair(coordinate.first, y)] != marker)
                    break;
                else {
                    std::get<0>(marker_lines[std::make_pair(coordinate.first, y)]) = 0;
                    line.emplace_back(coordinate.first, y);
                }
            }
            for (auto y = coordinate.second - 1; bmap.find(std::make_pair(coordinate.first, y)) != bmap.end(); --y) {
                if (bmap[std::make_pair(coordinate.first, y)] != marker)
                    break;
                else {
                    get<0>(marker_lines[std::make_pair(coordinate.first, y)]) = 0;
                    line.emplace_front(coordinate.first, y);
                }
            }
            std::get<0>(marker_lines[std::make_pair(coordinate.first, coordinate.second)]) = 0;

            if (line.size() >= K)
                return line;
        }

        if (get<1>(marker_lines[coordinate]) == 1) {
            std::deque<Coordinate> line;
            line.push_back(coordinate);
            for (auto x = coordinate.first + 1; bmap.find(make_pair(x, coordinate.second)) != bmap.end(); ++x) {
                if (bmap[make_pair(x, coordinate.second)] != marker)
                    break;
                else {
                    std::get<1>(marker_lines[make_pair(x, coordinate.second)]) = 0;
                    line.emplace_back(x, coordinate.second);
                }
            }

            for (auto x = coordinate.first - 1; bmap.find(make_pair(x, coordinate.second)) != bmap.end(); --x) {
                if (bmap[make_pair(x, coordinate.second)] != marker)
                    break;
                else {
                    std::get<1>(marker_lines[make_pair(x, coordinate.second)]) = 0;
                    line.emplace_front(x, coordinate.second);
                }
            }
            std::get<1>(marker_lines[make_pair(coordinate.first, coordinate.second)]) = 0;
            if (line.size() >= K)
                return line;
        }

        if (get<2>(marker_lines[coordinate]) == 1) {
            std::deque<Coordinate> line;
            line.push_back(coordinate);
            for (auto x = coordinate.first + 1, y = coordinate.second + 1; bmap.find(make_pair(x, y)) != bmap.end(); ++x, ++y) {
                if (bmap[make_pair(x, y)] != marker)
                    break;
                else {
                    get<2>(marker_lines[make_pair(x, y)]) = 0;
                    line.emplace_back(x, y);
                }
            }

            for (auto x = coordinate.first - 1, y = coordinate.second - 1; bmap.find(make_pair(x, y)) != bmap.end(); --x, --y) {
                if (bmap[make_pair(x, y)] != marker)
                    break;
                else {
                    get<2>(marker_lines[make_pair(x, y)]) = 0;
                    line.emplace_front(x, y);
                }
            }
            get<2>(marker_lines[make_pair(coordinate.first, coordinate.second)]) = 0;
            if (line.size() >= K)
                return line;
        }
    }
    return std::deque<Coordinate>();
}

bool add_move(decltype(State::board_map) &bmap, const pair<int, int> p, vector<Move> &vec, const pair<int, int> coordinate, bool &found_marker, decltype(Move::initial_removal) mrow) {
    Move m;
    switch (bmap[p]) {
    case EMPTY:
        m.initial_removal = mrow;
        m.initial_pos = coordinate;
        m.final_pos = p;
        vec.push_back(m);
        if (!found_marker) {
            return false;
            break;
        } else {
            return true;
        }
    case WHITE_RING:
    case BLACK_RING:
        return true;
    case WHITE_MARKER:
    case BLACK_MARKER:
        found_marker = true;
        return false;
    }
    return false;
}

std::vector<Move> generate_pmode_moves(const State &state) {
    std::vector<Move> moves;
    Move move;
    for (auto it = state.board_map.begin(); it != state.board_map.end(); ++it) {
        if (it->second == EMPTY) {
            move.mode = Mode_M::P;
            move.placing_pos = it->first;
            moves.push_back(move);
        }
    }
    return moves;
}

std::vector<Move> get_all_moves(const State &state, size_t K, size_t M) {
    std::vector<Move> moves;

    if (state.mode == Mode_S::P) {
        moves = generate_pmode_moves(state);
        return moves;
    } else {
        auto line = check_consecutive_markers(state, K);
        auto rings = state.get_player_rings();

        Move temp_move;
        while (!line.empty()) {
            auto front = line.front();
            auto back = line.back();
            temp_move.initial_removal.emplace_back(front, back, DUMMY_COORDINATE);
            auto newstate = remove_toggle_combined(front, back, state, 0);
            line = check_consecutive_markers(newstate, K);
        }

        if (temp_move.initial_removal.size() != 0) {
            auto remove_rings = std::min(temp_move.initial_removal.size(), rings.size());
            bool early_terminate = false;

            if (rings.size() - remove_rings <= (M - WINNING_RINGS)) {
                remove_rings = rings.size() + WINNING_RINGS - M;
                early_terminate = true;
            }

            std::vector<Coordinate> vec(rings.begin(), rings.end());

            std::vector<Coordinate> ring_vector(vec.begin(), vec.begin() + remove_rings);
            do {
                Move move;
                auto it = temp_move.initial_removal.begin();
                for (size_t i = 0; i != remove_rings && it != temp_move.initial_removal.end(); ++i, ++it) {
                    move.initial_removal.emplace_back(it->start, it->end, ring_vector[i]);
                }

                moves.push_back(move);

            } while (next_combination(vec.begin(), vec.begin() + remove_rings, vec.end()));

            if (early_terminate) {
                for (auto &m : moves)
                    m.initial_pos = DUMMY_COORDINATE;
                return moves;
            }
        }
    }

    decltype(moves) temp_moves;

    if (moves.empty()) {
        moves.push_back(Move());
    }

    for (const auto &m : moves) {
        bool found_marker = false;
        State newstate = state;

        Move temp_move;
        if (!m.initial_removal.empty()) {
            temp_move.initial_removal = m.initial_removal;
            temp_move.initial_pos = DUMMY_COORDINATE;
            newstate = perform_move(newstate, temp_move, M);
        }

        auto newrings = newstate.get_player_rings();
        auto newbmap = newstate.board_map;

        for (const auto &coordinate : newrings) {
            for (auto y = coordinate.second + 1; newbmap.find(make_pair(coordinate.first, y)) != newbmap.end(); ++y) {
                if (add_move(newbmap, make_pair(coordinate.first, y), temp_moves, coordinate, found_marker, m.initial_removal))
                    break;
            }

            found_marker = false;
            for (auto y = coordinate.second - 1; newbmap.find(make_pair(coordinate.first, y)) != newbmap.end(); --y) {
                if (add_move(newbmap, make_pair(coordinate.first, y), temp_moves, coordinate, found_marker, m.initial_removal))
                    break;
            }
            found_marker = false;

            for (auto x = coordinate.first + 1; newbmap.find(make_pair(x, coordinate.second)) != newbmap.end(); ++x) {
                if (add_move(newbmap, make_pair(x, coordinate.second), temp_moves, coordinate, found_marker, m.initial_removal))
                    break;
            }
            found_marker = false;

            for (auto x = coordinate.first - 1; newbmap.find(make_pair(x, coordinate.second)) != newbmap.end(); --x) {
                if (add_move(newbmap, make_pair(x, coordinate.second), temp_moves, coordinate, found_marker, m.initial_removal))
                    break;
            }
            found_marker = false;

            for (auto x = coordinate.first + 1, y = coordinate.second + 1; newbmap.find(make_pair(x, y)) != newbmap.end(); ++x, ++y) {
                if (add_move(newbmap, make_pair(x, y), temp_moves, coordinate, found_marker, m.initial_removal))
                    break;
            }
            found_marker = false;

            for (auto x = coordinate.first - 1, y = coordinate.second - 1; newbmap.find(make_pair(x, y)) != newbmap.end(); --x, --y) {
                if (add_move(newbmap, make_pair(x, y), temp_moves, coordinate, found_marker, m.initial_removal))
                    break;
            }
        }
    }

    moves = temp_moves;

    temp_moves.clear();

    for (const auto &m : moves) {
        State newstate = perform_move(state, m, M);

        auto line = check_consecutive_markers(newstate, K);
        auto rings = newstate.get_player_rings();

        Move temp_move;
        while (!line.empty()) {
            auto front = line.front();
            auto back = line.back();
            temp_move.final_removal.emplace_back(front, back, DUMMY_COORDINATE);
            auto tempstate = remove_toggle_combined(front, back, newstate, 0);
            line = check_consecutive_markers(tempstate, K);
        }

        if (temp_move.final_removal.size() != 0) {
            auto remove_rings = std::min(temp_move.final_removal.size(), rings.size());

            if (rings.size() - remove_rings <= (M - WINNING_RINGS)) {
                remove_rings = rings.size() + WINNING_RINGS - M;
            }

            std::vector<Coordinate> vec(rings.begin(), rings.end());

            std::vector<Coordinate> ring_vector(vec.begin(), vec.begin() + remove_rings);
            do {
                Move move;
                auto it = temp_move.final_removal.begin();
                for (size_t i = 0; i != remove_rings && it != temp_move.final_removal.end(); ++i, ++it) {
                    move.final_removal.emplace_back(it->start, it->end, ring_vector[i]);
                }

                temp_moves.push_back(move);

            } while (next_combination(vec.begin(), vec.begin() + remove_rings, vec.end()));
        }
    }

    moves = temp_moves;

    return moves;
}

bm_type get_board_map(size_t N) {
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

// TODO: Fix for different rings to be removed
// TODO: Fix 
bool is_game_over(const State & state)  {
    if (state.mode != P) {
        return state.black_rings.size() == 2 || state.white_rings.size() == 2;
    }
    return false;
}

int main2() {
    // ifstream fin("input5.txt");
    auto &fin = cin;
    State state;
    state.mode = Mode_S::P;
    size_t N, M, K;
    int player_id, time_limit_in_seconds;
    string input_move;
    int depth = 3;
    auto begin = chrono::high_resolution_clock::now();
    auto current = chrono::high_resolution_clock::now();

    fin >> player_id >> N >> time_limit_in_seconds;
    fin.clear();
    fin.ignore(1000, '\n');

    if (player_id == 2) {
        //other person is moving first
        auto seconds = chrono::duration_cast<chrono::seconds>(current - begin).count();

        while (getline(fin, input_move) && (!is_game_over(state)) && (seconds < time_limit_in_seconds)) {
            auto move = input_parse(input_move);
            state = perform_move(state, move, M);

            auto input_remove_string = game_board.check_for_consecutive_markers();
            // print_map(game_board.state.board_map);
            if (game_board.is_game_over()) {
                cout << input_remove_string << endl;
                break;
            }

            auto ply = game_board.bestply(depth);
            game_board.state = perform_proper_ply(game_board.state, player, ply);
            auto output_remove_string = game_board.check_for_consecutive_markers();

            auto output = game_board.output_parse(ply);

            if (input_remove_string.size() > 0)
                output = input_remove_string + " " + output;
            if (output_remove_string.size() > 0)
                output = output + " " + output_remove_string;

            cout << output << endl;

            // cout << endl
            //      << "OUTPUT STATE" << endl;
            // print_map(game_board.state.board_map);

            current = chrono::high_resolution_clock::now();
            seconds = chrono::duration_cast<chrono::seconds>(current - begin).count();
        }
    } else if (player_id == 1) {
        auto seconds = chrono::duration_cast<chrono::seconds>(current - begin).count();

        auto ply = game_board.bestply(depth);
        game_board.state = perform_proper_ply(game_board.state, player, ply);
        auto output = game_board.output_parse(ply);
        cout << output << endl;

        while (getline(fin, input_move) && (!game_board.is_game_over()) && (seconds < time_limit_in_seconds)) {
            auto strx = game_board.remove_other_markers(input_move);
            game_board.input_parse(strx, BLACK);
            auto input_remove_string = game_board.check_for_consecutive_markers();

            if (game_board.is_game_over()) {
                cout << input_remove_string << endl;
                break;
            }

            auto ply = game_board.bestply(depth);
            game_board.state = perform_proper_ply(game_board.state, player, ply);
            auto output = game_board.output_parse(ply);
            auto output_remove_string = game_board.check_for_consecutive_markers();
            if (input_remove_string.size() > 0)
                output = input_remove_string + " " + output;
            if (output_remove_string.size() > 0)
                output = output + " " + output_remove_string;
            cout << output << endl;

            current = chrono::high_resolution_clock::now();
            seconds = chrono::duration_cast<chrono::seconds>(current - begin).count();
        }
    }
    return 0;
}