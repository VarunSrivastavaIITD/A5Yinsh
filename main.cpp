#include "heuristic.h"
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

State perform_move(const State& state, Move move, size_t M);
std::vector<Move> get_all_moves(const State& state, size_t K, size_t M);
long minimax_util(State state, int depth, size_t K, size_t M);
long negascoututil(State state, int depth, long alpha, long beta, int K, int M);

const Coordinate DUMMY_COORDINATE(10, 10);
const int L = 3;

typedef boost::bimap<pair<int, int>, pair<int, int>> bm_type;
typedef bm_type::value_type bm_value_type;

bm_type bm;

template <typename Iterator>
bool next_combination(const Iterator first, Iterator k, const Iterator last)
{
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

pair<int, int> hex_to_ycoord(pair<size_t, size_t> p)
{
    return bm.left.at(p);
}

pair<size_t, size_t> ycoord_to_hex(pair<int, int> p)
{
    return bm.right.at(p);
}

Move input_parse(string s)
{
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
        Coordinate p1 = hex_to_ycoord(make_pair(stoi(vec[1]), stoi(vec[2])));
        result.placing_pos = p1;
        return result;
    }

    else {
        result.mode = Mode_M::S;

        vecpos = 0;
        Move::MarkerRow m;

        while (vec[vecpos] != "S" && vecpos != size) {
            Coordinate p1 = hex_to_ycoord(
                make_pair(stoi(vec[vecpos + 1]), stoi(vec[vecpos + 2])));
            Coordinate p2 = hex_to_ycoord(
                make_pair(stoi(vec[vecpos + 4]), stoi(vec[vecpos + 5])));
            Coordinate p3 = hex_to_ycoord(
                make_pair(stoi(vec[vecpos + 7]), stoi(vec[vecpos + 8])));
            m.start = p1;
            m.end = p2;
            m.ring = p3;
            result.initial_removal.push_back(m);
            vecpos += 9;
        }

        if (vecpos == size) {
            result.initial_pos = DUMMY_COORDINATE;
            result.final_pos = DUMMY_COORDINATE;
            return result;
        }
        Coordinate p1 = hex_to_ycoord(
            make_pair(stoi(vec[vecpos + 1]), stoi(vec[vecpos + 2])));
        Coordinate p2 = hex_to_ycoord(
            make_pair(stoi(vec[vecpos + 4]), stoi(vec[vecpos + 5])));
        result.initial_pos = p1;
        result.final_pos = p2;
        vecpos += 6;

        while (vecpos != size) {
            Coordinate p1 = hex_to_ycoord(
                make_pair(stoi(vec[vecpos + 1]), stoi(vec[vecpos + 2])));
            Coordinate p2 = hex_to_ycoord(
                make_pair(stoi(vec[vecpos + 4]), stoi(vec[vecpos + 5])));
            Coordinate p3 = hex_to_ycoord(
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

string output_parse(Move m)
{
    string s;
    if (m.mode == Mode_M::P) {
        Coordinate p1 = ycoord_to_hex(m.placing_pos);
        s = "P ";
        s += to_string(p1.first);
        s += " ";
        s += to_string(p1.second);
        return s;
    } else {
        while (m.initial_removal.size() != 0) {
            s += "RS ";
            Coordinate p1 = ycoord_to_hex(m.initial_removal.front().start);
            s += to_string(p1.first);
            s += " ";
            s += to_string(p1.second);
            s += " ";
            s += "RE ";
            Coordinate p2 = ycoord_to_hex(m.initial_removal.front().end);
            s += to_string(p2.first);
            s += " ";
            s += to_string(p2.second);
            s += " ";
            s += "X ";
            Coordinate p3 = ycoord_to_hex(m.initial_removal.front().ring);
            s += to_string(p3.first);
            s += " ";
            s += to_string(p3.second);
            s += " ";
            m.initial_removal.pop_front();
        }

        if (m.initial_pos == DUMMY_COORDINATE || m.final_pos == DUMMY_COORDINATE) {
            if (s[s.size() - 1] == ' ')
                s.pop_back();

            return s;
        }

        s += "S ";
        Coordinate p1 = ycoord_to_hex(m.initial_pos);
        s += to_string(p1.first);
        s += " ";
        s += to_string(p1.second);
        s += " ";
        s += "M ";
        Coordinate p2 = ycoord_to_hex(m.final_pos);
        s += to_string(p2.first);
        s += " ";
        s += to_string(p2.second);
        s += " ";

        while (m.final_removal.size() != 0) {
            s += "RS ";
            Coordinate p1 = ycoord_to_hex(m.final_removal.front().start);
            s += to_string(p1.first);
            s += " ";
            s += to_string(p1.second);
            s += " ";
            s += "RE ";
            Coordinate p2 = ycoord_to_hex(m.final_removal.front().end);
            s += to_string(p2.first);
            s += " ";
            s += to_string(p2.second);
            s += " ";
            s += "X ";
            Coordinate p3 = ycoord_to_hex(m.final_removal.front().ring);
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

State remove_toggle_combined(Coordinate p2, Coordinate p3, const State& state, int both_exclusive)
{
    State newstate = state;

    auto flip = [](decltype(newstate.black_markers)& s, decltype(newstate.black_markers)& t, const decltype(newstate.black_markers)::value_type& e) {
        auto search = s.find(e);
        if (search != s.end())
            s.erase(search);
        t.insert(e);
    };

    auto delete_from_set = [](decltype(newstate.black_markers)& s, const decltype(newstate.black_markers)::value_type& e) {
        auto search = s.find(e);
        if (search != s.end())
            s.erase(search);
    };

    if (p2.first == p3.first) {
        if (p3.second > p2.second) {

            for (int i = p2.second + both_exclusive; i <= p3.second - both_exclusive; i++) {
                auto p = make_pair(p2.first, i);

                auto search = newstate.board_map.find(p);
                if (search != newstate.board_map.end()) {
                    if (newstate.board_map[p] == WHITE_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[p] = BLACK_MARKER;
                            flip(newstate.white_markers, newstate.black_markers, p);
                        } else if (both_exclusive == 0) {
                            newstate.board_map[p] = EMPTY;
                            delete_from_set(newstate.white_markers, p);
                        }
                    } else if (newstate.board_map[p] == BLACK_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[p] = WHITE_MARKER;
                            flip(newstate.black_markers, newstate.white_markers, p);
                        } else if (both_exclusive == 0) {
                            newstate.board_map[p] = EMPTY;
                            delete_from_set(newstate.black_markers, p);
                        }
                    }
                }
            }
        } else {
            for (int i = p2.second - both_exclusive; i >= p3.second + both_exclusive; i--) {
                auto p = make_pair(p2.first, i);

                auto search = newstate.board_map.find(p);
                if (search != newstate.board_map.end()) {
                    if (newstate.board_map[p] == WHITE_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[p] = BLACK_MARKER;
                            flip(newstate.white_markers, newstate.black_markers, p);
                        } else if (both_exclusive == 0) {
                            newstate.board_map[p] = EMPTY;
                            delete_from_set(newstate.white_markers, p);
                        }
                    } else if (newstate.board_map[p] == BLACK_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[p] = WHITE_MARKER;
                            flip(newstate.black_markers, newstate.white_markers, p);
                        } else if (both_exclusive == 0) {
                            newstate.board_map[p] = EMPTY;
                            delete_from_set(newstate.black_markers, p);
                        }
                    }
                }
            }
        }
    }

    else if (p2.second == p3.second) {
        if (p3.first > p2.first) {
            for (int i = p2.first + both_exclusive; i <= p3.first - both_exclusive; i++) {
                auto p = make_pair(i, p2.second);
                auto search = newstate.board_map.find(p);
                if (search != newstate.board_map.end()) {
                    if (newstate.board_map[p] == WHITE_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[p] = BLACK_MARKER;
                            flip(newstate.white_markers, newstate.black_markers, p);
                        } else if (both_exclusive == 0) {
                            newstate.board_map[p] = EMPTY;
                            delete_from_set(newstate.white_markers, p);
                        }
                    } else if (newstate.board_map[p] == BLACK_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[p] = WHITE_MARKER;
                            flip(newstate.black_markers, newstate.white_markers, p);
                        } else if (both_exclusive == 0) {
                            newstate.board_map[p] = EMPTY;
                            delete_from_set(newstate.black_markers, p);
                        }
                    }
                }
            }
        } else {
            for (int i = p2.first - both_exclusive; i >= p3.first + both_exclusive; i--) {
                auto p = make_pair(i, p2.second);
                auto search = newstate.board_map.find(p);
                if (search != newstate.board_map.end()) {
                    if (newstate.board_map[p] == WHITE_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[p] = BLACK_MARKER;
                            flip(newstate.white_markers, newstate.black_markers, p);
                        } else if (both_exclusive == 0) {
                            newstate.board_map[p] = EMPTY;
                            delete_from_set(newstate.white_markers, p);
                        }
                    } else if (newstate.board_map[p] == BLACK_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[p] = WHITE_MARKER;
                            flip(newstate.black_markers, newstate.white_markers, p);
                        } else if (both_exclusive == 0) {
                            newstate.board_map[p] = EMPTY;
                            delete_from_set(newstate.black_markers, p);
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
                auto p = make_pair(i, j);
                auto search = newstate.board_map.find(p);
                if (search != newstate.board_map.end()) {
                    if (newstate.board_map[p] == WHITE_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[p] = BLACK_MARKER;
                            flip(newstate.white_markers, newstate.black_markers, p);
                        } else if (both_exclusive == 0) {
                            newstate.board_map[p] = EMPTY;
                            delete_from_set(newstate.white_markers, p);
                        }
                    } else if (newstate.board_map[p] == BLACK_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[p] = WHITE_MARKER;
                            flip(newstate.black_markers, newstate.white_markers, p);
                        } else if (both_exclusive == 0) {
                            newstate.board_map[p] = EMPTY;
                            delete_from_set(newstate.black_markers, p);
                        }
                    }
                }
            }
        } else {
            for (int i = p2.first - both_exclusive, j = p2.second - both_exclusive; i >= p3.first + both_exclusive; i--, j--) {
                auto p = make_pair(i, j);

                auto search = newstate.board_map.find(p);
                if (search != newstate.board_map.end()) {
                    if (newstate.board_map[p] == WHITE_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[p] = BLACK_MARKER;
                            flip(newstate.white_markers, newstate.black_markers, p);
                        } else if (both_exclusive == 0) {
                            newstate.board_map[p] = EMPTY;
                            delete_from_set(newstate.white_markers, p);
                        }
                    } else if (newstate.board_map[p] == BLACK_MARKER) {
                        if (both_exclusive == 1) {
                            newstate.board_map[p] = WHITE_MARKER;
                            flip(newstate.black_markers, newstate.white_markers, p);
                        } else if (both_exclusive == 0) {
                            newstate.board_map[p] = EMPTY;
                            delete_from_set(newstate.black_markers, p);
                        }
                    }
                }
            }
        }
    }
    return newstate;
}

Move minimax(State state, int depth, size_t K, size_t M)
{
    vector<Move> moves;
    State newstate = state;
    moves = get_all_moves(state, K, M);
    long max_score = numeric_limits<long>::min(), score;
    Move max_move;
    for (auto it1 = moves.begin(); it1 != moves.end(); it1++) {
        newstate = perform_move(state, *it1, M);
        score = minimax_util(newstate, depth - 1, K, M);
        if (score > max_score) {
            max_score = score;
            max_move = *it1;
        }
    }
    return max_move;
}

long minimax_util(State state, int depth, size_t K, size_t M)
{
    vector<Move> moves;
    State newstate = state;
    moves = get_all_moves(state, K, M);
    long ans = numeric_limits<long>::min();
    long max_score, score, min_score;
    // Move max_move, min_move;
    max_score = numeric_limits<long>::min();
    min_score = numeric_limits<long>::max();
    if (depth == 0) {
        state.player = state.player == Player::WHITE ? Player::BLACK : Player::WHITE;
        ans = (heuristic(state, M));
    } else if (state.player == WHITE) {
        for (auto it1 = moves.begin(); it1 != moves.end(); it1++) {
            newstate = perform_move(state, *it1, M);
            score = minimax_util(newstate, depth - 1, K, M);
            if (score > max_score) {
                max_score = score;
                // max_move = *it1;
            }
        }
        ans = max_score;
    } else if (state.player == BLACK) {
        for (auto it1 = moves.begin(); it1 != moves.end(); it1++) {
            newstate = perform_move(state, *it1, M);
            score = minimax_util(newstate, depth - 1, K, M);
            if (score < min_score) {
                min_score = score;
                // min_move = *it1;
            }
        }
        ans = min_score;
    }
    return ans;
}

State perform_move(const State& state, Move move, size_t M)
{
    State newstate = state;

    auto delete_from_set =
        [](decltype(newstate.black_markers)& s,
            const decltype(newstate.black_markers)::value_type& e) {
            auto search = s.find(e);
            if (search != s.end())
                s.erase(search);
        };

    auto update_from_map =
        [](decltype(newstate.board_map)& s,
            const decltype(newstate.board_map)::key_type& e,
            const decltype(newstate.board_map)::mapped_type& f) {
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

std::deque<Coordinate> check_consecutive_markers(const State& state, size_t K = 5)
{
    std::map<pair<int, int>, std::tuple<int, int, int>> marker_lines;
    const auto markers = state.get_player_markers();
    auto bmap = state.board_map;
    auto marker = state.player == Player::WHITE ? WHITE_MARKER : BLACK_MARKER;

    for (auto it = markers.cbegin(); it != markers.cend(); ++it)
        marker_lines[*it] = std::make_tuple(1, 1, 1);

    for (const auto& coordinate : markers) {

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
            std::get<0>(marker_lines[coordinate]) = 0;

            if (line.size() >= K)
                return decltype(line)(line.begin(), line.begin() + K);
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
            std::get<1>(marker_lines[coordinate]) = 0;
            if (line.size() >= K)
                return decltype(line)(line.begin(), line.begin() + K);
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
            get<2>(marker_lines[coordinate]) = 0;
            if (line.size() >= K)
                return decltype(line)(line.begin(), line.begin() + K);
        }
    }
    return std::deque<Coordinate>();
}

bool add_move(decltype(State::board_map)& bmap, const pair<int, int> p, vector<Move>& vec, const pair<int, int> coordinate, bool& found_marker, decltype(Move::initial_removal) mrow)
{
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

std::vector<Move> generate_pmode_moves(const State& state)
{
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

std::vector<Move> get_all_moves(const State& state, size_t K, size_t M)
{
    std::vector<Move> moves;

    if (state.mode == Mode_S::P) {
        moves = generate_pmode_moves(state);
        return moves;
    } else {

        // Initial RS Phase
        auto line = check_consecutive_markers(state, K);
        auto rings = state.get_player_rings();

        Move temp_move;
        State newstate = state;
        while (!line.empty()) {
            auto front = line.front();
            auto back = line.back();
            temp_move.initial_removal.emplace_back(front, back, DUMMY_COORDINATE);
            newstate = remove_toggle_combined(front, back, newstate, 0);
            line = check_consecutive_markers(newstate, K);
        }

        if (temp_move.initial_removal.size() != 0) {
            auto remove_rings = std::min(temp_move.initial_removal.size(), rings.size());
            bool early_terminate = false;

            if (rings.size() - remove_rings <= (M - L)) {
                remove_rings = rings.size() + L - M;
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
                for (auto& m : moves)
                    m.initial_pos = DUMMY_COORDINATE;
                return moves;
            }
        }
    }

    decltype(moves) temp_moves;

    if (moves.empty()) {
        moves.push_back(Move());
    }

    // S phase
    for (const auto& m : moves) {
        bool found_marker = false;
        State newstate = state;

        Move temp_move;
        if (!m.initial_removal.empty()) {
            temp_move.initial_removal = m.initial_removal;
            temp_move.initial_pos = DUMMY_COORDINATE;
            newstate = perform_move(newstate, temp_move, M);
            newstate.player = state.player;
        }

        auto newrings = newstate.get_player_rings();
        auto newbmap = newstate.board_map;

        for (const auto& coordinate : newrings) {
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

    // Final RS Phase
    for (const auto& m : moves) {
        State newstate = perform_move(state, m, M);
        newstate.player = state.player;

        auto line = check_consecutive_markers(newstate, K);
        auto rings = newstate.get_player_rings();

        Move temp_move = m;
        State tempstate = newstate;
        while (!line.empty()) {
            auto front = line.front();
            auto back = line.back();
            temp_move.final_removal.emplace_back(front, back, DUMMY_COORDINATE);
            tempstate = remove_toggle_combined(front, back, tempstate, 0);
            line = check_consecutive_markers(tempstate, K);
        }

        if (temp_move.final_removal.size() != 0) {
            auto remove_rings = std::min(temp_move.final_removal.size(), rings.size());

            if (rings.size() - remove_rings <= (M - L)) {
                remove_rings = rings.size() + L - M;
            }

            std::vector<Coordinate> vec(rings.begin(), rings.end());

            std::vector<Coordinate> ring_vector(vec.begin(), vec.begin() + remove_rings);
            do {
                Move move = m;

                auto it = temp_move.final_removal.begin();
                for (size_t i = 0; i != remove_rings && it != temp_move.final_removal.end(); ++i, ++it) {
                    move.final_removal.emplace_back(it->start, it->end, ring_vector[i]);
                }

                temp_moves.push_back(move);

            } while (next_combination(vec.begin(), vec.begin() + remove_rings, vec.end()));
        } else {
            temp_moves.push_back(m);
        }
    }

    moves = temp_moves;

    return moves;
}

void get_board_map(size_t N)
{
    if (N == 5) {
        bm.insert(bm_value_type(make_pair(0, 0), make_pair(0, 0)));
        // hex 1
        bm.insert(bm_value_type(make_pair(1, 0), make_pair(0, 1)));
        bm.insert(bm_value_type(make_pair(1, 1), make_pair(1, 1)));
        bm.insert(bm_value_type(make_pair(1, 2), make_pair(1, 0)));
        bm.insert(bm_value_type(make_pair(1, 3), make_pair(0, -1)));
        bm.insert(bm_value_type(make_pair(1, 4), make_pair(-1, -1)));
        bm.insert(bm_value_type(make_pair(1, 5), make_pair(-1, 0)));
        // hex 2
        bm.insert(bm_value_type(make_pair(2, 0), make_pair(0, 2)));
        bm.insert(bm_value_type(make_pair(2, 1), make_pair(1, 2)));
        bm.insert(bm_value_type(make_pair(2, 2), make_pair(2, 2)));
        bm.insert(bm_value_type(make_pair(2, 3), make_pair(2, 1)));
        bm.insert(bm_value_type(make_pair(2, 4), make_pair(2, 0)));
        bm.insert(bm_value_type(make_pair(2, 5), make_pair(1, -1)));
        bm.insert(bm_value_type(make_pair(2, 6), make_pair(0, -2)));
        bm.insert(bm_value_type(make_pair(2, 7), make_pair(-1, -2)));
        bm.insert(bm_value_type(make_pair(2, 8), make_pair(-2, -2)));
        bm.insert(bm_value_type(make_pair(2, 9), make_pair(-2, -1)));
        bm.insert(bm_value_type(make_pair(2, 10), make_pair(-2, 0)));
        bm.insert(bm_value_type(make_pair(2, 11), make_pair(-1, 1)));
        // hex 3
        bm.insert(bm_value_type(make_pair(3, 0), make_pair(0, 3)));
        bm.insert(bm_value_type(make_pair(3, 1), make_pair(1, 3)));
        bm.insert(bm_value_type(make_pair(3, 2), make_pair(2, 3)));
        bm.insert(bm_value_type(make_pair(3, 3), make_pair(3, 3)));
        bm.insert(bm_value_type(make_pair(3, 4), make_pair(3, 2)));
        bm.insert(bm_value_type(make_pair(3, 5), make_pair(3, 1)));
        bm.insert(bm_value_type(make_pair(3, 6), make_pair(3, 0)));
        bm.insert(bm_value_type(make_pair(3, 7), make_pair(2, -1)));
        bm.insert(bm_value_type(make_pair(3, 8), make_pair(1, -2)));
        bm.insert(bm_value_type(make_pair(3, 9), make_pair(0, -3)));
        bm.insert(bm_value_type(make_pair(3, 10), make_pair(-1, -3)));
        bm.insert(bm_value_type(make_pair(3, 11), make_pair(-2, -3)));
        bm.insert(bm_value_type(make_pair(3, 12), make_pair(-3, -3)));
        bm.insert(bm_value_type(make_pair(3, 13), make_pair(-3, -2)));
        bm.insert(bm_value_type(make_pair(3, 14), make_pair(-3, -1)));
        bm.insert(bm_value_type(make_pair(3, 15), make_pair(-3, 0)));
        bm.insert(bm_value_type(make_pair(3, 16), make_pair(-2, 1)));
        bm.insert(bm_value_type(make_pair(3, 17), make_pair(-1, 2)));
        // hex 4
        bm.insert(bm_value_type(make_pair(4, 0), make_pair(0, 4)));
        bm.insert(bm_value_type(make_pair(4, 1), make_pair(1, 4)));
        bm.insert(bm_value_type(make_pair(4, 2), make_pair(2, 4)));
        bm.insert(bm_value_type(make_pair(4, 3), make_pair(3, 4)));
        bm.insert(bm_value_type(make_pair(4, 4), make_pair(4, 4)));
        bm.insert(bm_value_type(make_pair(4, 5), make_pair(4, 3)));
        bm.insert(bm_value_type(make_pair(4, 6), make_pair(4, 2)));
        bm.insert(bm_value_type(make_pair(4, 7), make_pair(4, 1)));
        bm.insert(bm_value_type(make_pair(4, 8), make_pair(4, 0)));
        bm.insert(bm_value_type(make_pair(4, 9), make_pair(3, -1)));
        bm.insert(bm_value_type(make_pair(4, 10), make_pair(2, -2)));
        bm.insert(bm_value_type(make_pair(4, 11), make_pair(1, -3)));
        bm.insert(bm_value_type(make_pair(4, 12), make_pair(0, -4)));
        bm.insert(bm_value_type(make_pair(4, 13), make_pair(-1, -4)));
        bm.insert(bm_value_type(make_pair(4, 14), make_pair(-2, -4)));
        bm.insert(bm_value_type(make_pair(4, 15), make_pair(-3, -4)));
        bm.insert(bm_value_type(make_pair(4, 16), make_pair(-4, -4)));
        bm.insert(bm_value_type(make_pair(4, 17), make_pair(-4, -3)));
        bm.insert(bm_value_type(make_pair(4, 18), make_pair(-4, -2)));
        bm.insert(bm_value_type(make_pair(4, 19), make_pair(-4, -1)));
        bm.insert(bm_value_type(make_pair(4, 20), make_pair(-4, 0)));
        bm.insert(bm_value_type(make_pair(4, 21), make_pair(-3, 1)));
        bm.insert(bm_value_type(make_pair(4, 22), make_pair(-2, 2)));
        bm.insert(bm_value_type(make_pair(4, 23), make_pair(-1, 3)));
        // hex 5
        bm.insert(bm_value_type(make_pair(5, 1), make_pair(1, 5)));
        bm.insert(bm_value_type(make_pair(5, 2), make_pair(2, 5)));
        bm.insert(bm_value_type(make_pair(5, 3), make_pair(3, 5)));
        bm.insert(bm_value_type(make_pair(5, 4), make_pair(4, 5)));
        bm.insert(bm_value_type(make_pair(5, 6), make_pair(5, 4)));
        bm.insert(bm_value_type(make_pair(5, 7), make_pair(5, 3)));
        bm.insert(bm_value_type(make_pair(5, 8), make_pair(5, 2)));
        bm.insert(bm_value_type(make_pair(5, 9), make_pair(5, 1)));
        bm.insert(bm_value_type(make_pair(5, 11), make_pair(4, -1)));
        bm.insert(bm_value_type(make_pair(5, 12), make_pair(3, -2)));
        bm.insert(bm_value_type(make_pair(5, 13), make_pair(2, -3)));
        bm.insert(bm_value_type(make_pair(5, 14), make_pair(1, -4)));
        bm.insert(bm_value_type(make_pair(5, 16), make_pair(-1, -5)));
        bm.insert(bm_value_type(make_pair(5, 17), make_pair(-2, -5)));
        bm.insert(bm_value_type(make_pair(5, 18), make_pair(-3, -5)));
        bm.insert(bm_value_type(make_pair(5, 19), make_pair(-4, -5)));
        bm.insert(bm_value_type(make_pair(5, 21), make_pair(-5, -4)));
        bm.insert(bm_value_type(make_pair(5, 22), make_pair(-5, -3)));
        bm.insert(bm_value_type(make_pair(5, 23), make_pair(-5, -2)));
        bm.insert(bm_value_type(make_pair(5, 24), make_pair(-5, -1)));
        bm.insert(bm_value_type(make_pair(5, 26), make_pair(-4, 1)));
        bm.insert(bm_value_type(make_pair(5, 27), make_pair(-3, 2)));
        bm.insert(bm_value_type(make_pair(5, 28), make_pair(-2, 3)));
        bm.insert(bm_value_type(make_pair(5, 29), make_pair(-1, 4)));
    }

    else {
        bm.insert(bm_value_type(make_pair(0, 0), make_pair(0, 0)));
        // hex 1
        bm.insert(bm_value_type(make_pair(1, 0), make_pair(0, 1)));
        bm.insert(bm_value_type(make_pair(1, 1), make_pair(1, 1)));
        bm.insert(bm_value_type(make_pair(1, 2), make_pair(1, 0)));
        bm.insert(bm_value_type(make_pair(1, 3), make_pair(0, -1)));
        bm.insert(bm_value_type(make_pair(1, 4), make_pair(-1, -1)));
        bm.insert(bm_value_type(make_pair(1, 5), make_pair(-1, 0)));
        // hex 2
        bm.insert(bm_value_type(make_pair(2, 0), make_pair(0, 2)));
        bm.insert(bm_value_type(make_pair(2, 1), make_pair(1, 2)));
        bm.insert(bm_value_type(make_pair(2, 2), make_pair(2, 2)));
        bm.insert(bm_value_type(make_pair(2, 3), make_pair(2, 1)));
        bm.insert(bm_value_type(make_pair(2, 4), make_pair(2, 0)));
        bm.insert(bm_value_type(make_pair(2, 5), make_pair(1, -1)));
        bm.insert(bm_value_type(make_pair(2, 6), make_pair(0, -2)));
        bm.insert(bm_value_type(make_pair(2, 7), make_pair(-1, -2)));
        bm.insert(bm_value_type(make_pair(2, 8), make_pair(-2, -2)));
        bm.insert(bm_value_type(make_pair(2, 9), make_pair(-2, -1)));
        bm.insert(bm_value_type(make_pair(2, 10), make_pair(-2, 0)));
        bm.insert(bm_value_type(make_pair(2, 11), make_pair(-1, 1)));
        // hex 3
        bm.insert(bm_value_type(make_pair(3, 0), make_pair(0, 3)));
        bm.insert(bm_value_type(make_pair(3, 1), make_pair(1, 3)));
        bm.insert(bm_value_type(make_pair(3, 2), make_pair(2, 3)));
        bm.insert(bm_value_type(make_pair(3, 3), make_pair(3, 3)));
        bm.insert(bm_value_type(make_pair(3, 4), make_pair(3, 2)));
        bm.insert(bm_value_type(make_pair(3, 5), make_pair(3, 1)));
        bm.insert(bm_value_type(make_pair(3, 6), make_pair(3, 0)));
        bm.insert(bm_value_type(make_pair(3, 7), make_pair(2, -1)));
        bm.insert(bm_value_type(make_pair(3, 8), make_pair(1, -2)));
        bm.insert(bm_value_type(make_pair(3, 9), make_pair(0, -3)));
        bm.insert(bm_value_type(make_pair(3, 10), make_pair(-1, -3)));
        bm.insert(bm_value_type(make_pair(3, 11), make_pair(-2, -3)));
        bm.insert(bm_value_type(make_pair(3, 12), make_pair(-3, -3)));
        bm.insert(bm_value_type(make_pair(3, 13), make_pair(-3, -2)));
        bm.insert(bm_value_type(make_pair(3, 14), make_pair(-3, -1)));
        bm.insert(bm_value_type(make_pair(3, 15), make_pair(-3, 0)));
        bm.insert(bm_value_type(make_pair(3, 16), make_pair(-2, 1)));
        bm.insert(bm_value_type(make_pair(3, 17), make_pair(-1, 2)));
        // hex 4
        bm.insert(bm_value_type(make_pair(4, 0), make_pair(0, 4)));
        bm.insert(bm_value_type(make_pair(4, 1), make_pair(1, 4)));
        bm.insert(bm_value_type(make_pair(4, 2), make_pair(2, 4)));
        bm.insert(bm_value_type(make_pair(4, 3), make_pair(3, 4)));
        bm.insert(bm_value_type(make_pair(4, 4), make_pair(4, 4)));
        bm.insert(bm_value_type(make_pair(4, 5), make_pair(4, 3)));
        bm.insert(bm_value_type(make_pair(4, 6), make_pair(4, 2)));
        bm.insert(bm_value_type(make_pair(4, 7), make_pair(4, 1)));
        bm.insert(bm_value_type(make_pair(4, 8), make_pair(4, 0)));
        bm.insert(bm_value_type(make_pair(4, 9), make_pair(3, -1)));
        bm.insert(bm_value_type(make_pair(4, 10), make_pair(2, -2)));
        bm.insert(bm_value_type(make_pair(4, 11), make_pair(1, -3)));
        bm.insert(bm_value_type(make_pair(4, 12), make_pair(0, -4)));
        bm.insert(bm_value_type(make_pair(4, 13), make_pair(-1, -4)));
        bm.insert(bm_value_type(make_pair(4, 14), make_pair(-2, -4)));
        bm.insert(bm_value_type(make_pair(4, 15), make_pair(-3, -4)));
        bm.insert(bm_value_type(make_pair(4, 16), make_pair(-4, -4)));
        bm.insert(bm_value_type(make_pair(4, 17), make_pair(-4, -3)));
        bm.insert(bm_value_type(make_pair(4, 18), make_pair(-4, -2)));
        bm.insert(bm_value_type(make_pair(4, 19), make_pair(-4, -1)));
        bm.insert(bm_value_type(make_pair(4, 20), make_pair(-4, 0)));
        bm.insert(bm_value_type(make_pair(4, 21), make_pair(-3, 1)));
        bm.insert(bm_value_type(make_pair(4, 22), make_pair(-2, 2)));
        bm.insert(bm_value_type(make_pair(4, 23), make_pair(-1, 3)));

        // hex 5
        bm.insert(bm_value_type(make_pair(5, 0), make_pair(0, 5)));
        bm.insert(bm_value_type(make_pair(5, 1), make_pair(1, 5)));
        bm.insert(bm_value_type(make_pair(5, 2), make_pair(2, 5)));
        bm.insert(bm_value_type(make_pair(5, 3), make_pair(3, 5)));
        bm.insert(bm_value_type(make_pair(5, 4), make_pair(4, 5)));
        bm.insert(bm_value_type(make_pair(5, 5), make_pair(5, 5)));
        bm.insert(bm_value_type(make_pair(5, 6), make_pair(5, 4)));
        bm.insert(bm_value_type(make_pair(5, 7), make_pair(5, 3)));
        bm.insert(bm_value_type(make_pair(5, 8), make_pair(5, 2)));
        bm.insert(bm_value_type(make_pair(5, 9), make_pair(5, 1)));
        bm.insert(bm_value_type(make_pair(5, 10), make_pair(5, 0)));
        bm.insert(bm_value_type(make_pair(5, 11), make_pair(4, -1)));
        bm.insert(bm_value_type(make_pair(5, 12), make_pair(3, -2)));
        bm.insert(bm_value_type(make_pair(5, 13), make_pair(2, -3)));
        bm.insert(bm_value_type(make_pair(5, 14), make_pair(1, -4)));
        bm.insert(bm_value_type(make_pair(5, 15), make_pair(0, -5)));
        bm.insert(bm_value_type(make_pair(5, 16), make_pair(-1, -5)));
        bm.insert(bm_value_type(make_pair(5, 17), make_pair(-2, -5)));
        bm.insert(bm_value_type(make_pair(5, 18), make_pair(-3, -5)));
        bm.insert(bm_value_type(make_pair(5, 19), make_pair(-4, -5)));
        bm.insert(bm_value_type(make_pair(5, 20), make_pair(-5, -5)));
        bm.insert(bm_value_type(make_pair(5, 21), make_pair(-5, -4)));
        bm.insert(bm_value_type(make_pair(5, 22), make_pair(-5, -3)));
        bm.insert(bm_value_type(make_pair(5, 23), make_pair(-5, -2)));
        bm.insert(bm_value_type(make_pair(5, 24), make_pair(-5, -1)));
        bm.insert(bm_value_type(make_pair(5, 25), make_pair(-5, 0)));
        bm.insert(bm_value_type(make_pair(5, 26), make_pair(-4, 1)));
        bm.insert(bm_value_type(make_pair(5, 27), make_pair(-3, 2)));
        bm.insert(bm_value_type(make_pair(5, 28), make_pair(-2, 3)));
        bm.insert(bm_value_type(make_pair(5, 29), make_pair(-1, 4)));

        //hex6
        //hex6
        // bm.insert(bm_value_type(make_pair(6, 0), make_pair(0, 6)));
        bm.insert(bm_value_type(make_pair(6, 1), make_pair(1, 6)));
        bm.insert(bm_value_type(make_pair(6, 2), make_pair(2, 6)));
        bm.insert(bm_value_type(make_pair(6, 3), make_pair(3, 6)));
        bm.insert(bm_value_type(make_pair(6, 4), make_pair(4, 6)));
        bm.insert(bm_value_type(make_pair(6, 5), make_pair(5, 6)));
        // bm.insert(bm_value_type(make_pair(6, 6), make_pair(6, 6)));
        bm.insert(bm_value_type(make_pair(6, 7), make_pair(6, 5)));
        bm.insert(bm_value_type(make_pair(6, 8), make_pair(6, 4)));
        bm.insert(bm_value_type(make_pair(6, 9), make_pair(6, 3)));
        bm.insert(bm_value_type(make_pair(6, 10), make_pair(6, 2)));
        bm.insert(bm_value_type(make_pair(6, 11), make_pair(6, 1)));
        // bm.insert(bm_value_type(make_pair(6, 12), make_pair(6, 0)));
        bm.insert(bm_value_type(make_pair(6, 13), make_pair(5, -1)));
        bm.insert(bm_value_type(make_pair(6, 14), make_pair(4, -2)));
        bm.insert(bm_value_type(make_pair(6, 15), make_pair(3, -3)));
        bm.insert(bm_value_type(make_pair(6, 16), make_pair(2, -4)));
        bm.insert(bm_value_type(make_pair(6, 17), make_pair(1, -5)));
        // bm.insert(bm_value_type(make_pair(6, 18), make_pair(0, -6)));
        bm.insert(bm_value_type(make_pair(6, 19), make_pair(-1, -6)));
        bm.insert(bm_value_type(make_pair(6, 20), make_pair(-2, -6)));
        bm.insert(bm_value_type(make_pair(6, 21), make_pair(-3, -6)));
        bm.insert(bm_value_type(make_pair(6, 22), make_pair(-4, -6)));
        bm.insert(bm_value_type(make_pair(6, 23), make_pair(-5, -6)));

        // bm.insert(bm_value_type(make_pair(6, 24), make_pair(-6, -6)));
        bm.insert(bm_value_type(make_pair(6, 25), make_pair(-6, -5)));
        bm.insert(bm_value_type(make_pair(6, 26), make_pair(-6, -4)));
        bm.insert(bm_value_type(make_pair(6, 27), make_pair(-6, -3)));
        bm.insert(bm_value_type(make_pair(6, 28), make_pair(-6, -2)));
        bm.insert(bm_value_type(make_pair(6, 29), make_pair(-6, -1)));
        // bm.insert(bm_value_type(make_pair(6, 30), make_pair(-6, 0)));

        bm.insert(bm_value_type(make_pair(6, 31), make_pair(-5, 1)));
        bm.insert(bm_value_type(make_pair(6, 32), make_pair(-4, 2)));
        bm.insert(bm_value_type(make_pair(6, 33), make_pair(-3, 3)));
        bm.insert(bm_value_type(make_pair(6, 34), make_pair(-2, 4)));
        bm.insert(bm_value_type(make_pair(6, 35), make_pair(-1, 5)));
    }
}

bool is_game_over(const State& state, const size_t M)
{
    if (state.mode != Mode_S::P) {
        const size_t L = 3;
        return state.black_rings.size() == (M - L) || state.white_rings.size() == (M - L);
    }
    return false;
}

Move negascout(State state, int depth, int K, int M)
{
    long bestvalue, value, beta2, alpha, beta;
    vector<Move> moves;
    Move bestmove;
    State newstate = state;
    bestvalue = numeric_limits<long>::min();
    alpha = numeric_limits<long>::min();
    beta = numeric_limits<long>::max();
    beta2 = beta;
    moves = get_all_moves(state, K, M);
    for (auto it1 = moves.begin(); it1 != moves.end(); it1++) {
        newstate = perform_move(state, *it1, M);
        value = (negascoututil(newstate, depth - 1, -(beta2), -(alpha), K, M));

        if ((value > alpha) && (value < beta) && (it1 != moves.begin()))
            value = (negascoututil(newstate, depth - 1, -(beta), -(alpha), K, M));

        if (value > bestvalue) {
            bestvalue = value;
            bestmove = *it1;
        }

        if (value > alpha)
            alpha = value;

        if (alpha >= beta)
            break;

        beta2 = alpha + 1;
    }
    return bestmove;
}

long negascoututil(State state, int depth, long alpha, long beta, int K, int M)
{
    long bestvalue, value, beta2;
    vector<Move> moves;
    State newstate = state;
    long color = state.player == Player::WHITE ? 1 : -1;
    if ((depth == 0) || (is_game_over(state, M))) {
        // state.player = state.player == Player::WHITE ? Player::BLACK : Player::WHITE;
        state.player = Player::WHITE;
        return heuristic(state, M);
    }
    bestvalue = numeric_limits<long>::min();
    beta2 = beta;
    moves = get_all_moves(state, K, M);
    for (auto it1 = moves.begin(); it1 != moves.end(); it1++) {
        newstate = perform_move(state, *it1, M);
        value = color * (negascoututil(newstate, depth - 1, -(beta2), -(alpha), K, M));
        if ((value > alpha) && (value < beta) && (it1 != moves.begin()))
            value = color * (negascoututil(newstate, depth - 1, -(beta), -(alpha), K, M));

        if (value > bestvalue)
            bestvalue = value;

        if (value > alpha)
            alpha = value;

        if (alpha >= beta)
            break;

        beta2 = alpha + 1;
    }
    return bestvalue;
}

long alphabeta(State state, size_t depth, long alpha, long beta, size_t K, size_t M)
{
    if ((depth == 0) || (is_game_over(state, M))) {
        state.player = Player::WHITE;
        return heuristic(state, M);
    }
    auto moves = get_all_moves(state, K, M);
    // int i, swapped;
    // Move temp;

    if (state.player == Player::WHITE) {
        long value = numeric_limits<long>::min();

        // swapped = 0;
        // for (i = 0; i < moves.size(); i++) {
        //     if (((!moves[i].initial_removal.empty()) || (!moves[i].final_removal.empty())) && swapped != i) {
        //         temp = moves[swapped];
        //         moves[swapped] = moves[i];
        //         moves[i] = temp;
        //         swapped++;
        //     }
        // }

        for (auto it = moves.begin(); it != moves.end(); it++) {
            auto newstate = perform_move(state, *it, M);
            value = std::max(value, alphabeta(newstate, depth - 1, alpha, beta, K, M));
            alpha = std::max(alpha, value);
            if (alpha >= beta)
                break;
        }
        return value;
    } else {
        long value = numeric_limits<long>::max();

        // swapped = moves.size() - 1;
        // for (i = 0; i < moves.size(); i++) {
        //     if (((!moves[i].initial_removal.empty()) || (!moves[i].final_removal.empty())) && swapped != i) {
        //         temp = moves[swapped];
        //         moves[swapped] = moves[i];
        //         moves[i] = temp;
        //         swapped--;
        //     }
        // }

        for (auto it = moves.begin(); it != moves.end(); it++) {
            auto newstate = perform_move(state, *it, M);
            value = std::min(value, alphabeta(newstate, depth - 1, alpha, beta, K, M));
            beta = std::min(beta, value);
            if (alpha >= beta)
                break;
        }
        return value;
    }
}

Move best(State state, size_t depth, size_t K, size_t M)
{
    if (state.mode == Mode_S::P)
        depth = 1;
    Move bestmove;
    State newstate = state;
    auto bestvalue = numeric_limits<long>::min();
    auto alpha = numeric_limits<long>::min();
    auto beta = numeric_limits<long>::max();
    auto moves = get_all_moves(state, K, M);
    // std::vector<std::pair<long, Move>> est_values;

    // for (auto m : moves) {
    //     est_values.emplace_back(heuristic(perform_move(state, m, M), M), m);
    // }

    // std::sort(est_values.begin(), est_values.end(), [&](std::pair<long, Move> m1, std::pair<long, Move> m2) {return  m1.first > m2.first; });
    // int i, swapped = 0;
    // Move temp;
    // for (i = 0; i < moves.size(); i++) {
    //     if (((!moves[i].initial_removal.empty()) || (!moves[i].final_removal.empty())) && swapped != i) {
    //         temp = moves[swapped];
    //         moves[swapped] = moves[i];
    //         moves[i] = temp;
    //         swapped++;
    //     }
    // }

    for (auto it1 = moves.begin(); it1 != moves.end(); it1++) {
        newstate = perform_move(state, *it1, M);
        auto value = alphabeta(newstate, depth - 1, alpha, beta, K, M);

        if (value > bestvalue) {
            bestvalue = value;
            bestmove = *it1;
        }
    }
    return bestmove;
}

int main()
{
    auto& fin = cin;
    size_t N, M, K;
    int player_id, time_limit_in_seconds;
    string input_move;
    string output_move;
    int depth = 2;

    auto begin = chrono::high_resolution_clock::now();
    auto current = chrono::high_resolution_clock::now();

    // TODO: K
    fin >> player_id >> N >> time_limit_in_seconds >> K;
    M = N;

    if (N == 5)
        depth = 3;
    else
        depth = 2;

    fin.clear();
    fin.ignore(1000, '\n');
    get_board_map(N);

    State state(N);
    state.mode = Mode_S::P;

    if (player_id == 2) {
        state.player = BLACK;
        //other person is moving first
        auto seconds = chrono::duration_cast<chrono::seconds>(current - begin).count();

        while (getline(fin, input_move) && (!is_game_over(state, N)) && (seconds < time_limit_in_seconds)) {
            auto move = input_parse(input_move);
            state = perform_move(state, move, M);

            auto best_move = best(state, depth, K, M);
            state = perform_move(state, best_move, M);
            output_move = output_parse(best_move);
            cout << output_move << endl;

            current = chrono::high_resolution_clock::now();
            seconds = chrono::duration_cast<chrono::seconds>(current - begin).count();
        }
    } else if (player_id == 1) {
        state.player = WHITE;
        auto seconds = chrono::duration_cast<chrono::seconds>(current - begin).count();

        auto best_move = best(state, depth, K, M);
        state = perform_move(state, best_move, M);
        output_move = output_parse(best_move);
        cout << output_move << endl;

        while (getline(fin, input_move) && (!is_game_over(state, N)) && (seconds < time_limit_in_seconds)) {

            auto move = input_parse(input_move);
            state = perform_move(state, move, M);

            auto best_move = best(state, depth, K, M);
            state = perform_move(state, best_move, M);
            output_move = output_parse(best_move);
            cout << output_move << endl;

            current = chrono::high_resolution_clock::now();
            seconds = chrono::duration_cast<chrono::seconds>(current - begin).count();
        }
    }
    return 0;
}