#include "heuristic.h"
#include "move.h"
#include "state.h"
#include <algorithm>
#include <boost/function.hpp>
#include <boost/functional/hash.hpp>
#include <boost/range/combine.hpp>
#include <functional>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

long value_heuristic(State state, int M)
{
    auto rings = state.get_player_rings();
    if (state.mode == Mode_S::P)
        return 0;
    if (M - rings.size() == 3)
        return 100000 * static_cast<long>(M - rings.size());
    return 10000 * static_cast<long>(M - rings.size());
}

long marker_heuristic(const State& state)
{
    auto markers = state.get_player_markers();
    return 10 * static_cast<long>(markers.size());
}

long ring_moves_heuristic(State& state)
{

    auto add_ply = [](decltype(State::board_map)& bmap, const pair<int, int> p, long& count) -> bool {
        switch (bmap[p]) {
        case EMPTY:
            ++count;
            break;
        case WHITE_RING:
        case BLACK_RING:
            return true;
        case WHITE_MARKER:
        case BLACK_MARKER:
            return false;
            break;
        }
        return false;
    };

    auto bmap = state.board_map;
    auto rings = state.get_player_rings();
    long count = 0;
    for (const auto& coordinate : rings) {
        for (auto y = coordinate.second + 1; bmap.find(make_pair(coordinate.first, y)) != bmap.end(); ++y) {
            if (add_ply(bmap, make_pair(coordinate.first, y), count))
                break;
        }

        for (auto y = coordinate.second - 1; bmap.find(make_pair(coordinate.first, y)) != bmap.end(); --y) {
            if (add_ply(bmap, make_pair(coordinate.first, y), count))
                break;
        }

        for (auto x = coordinate.first + 1; bmap.find(make_pair(x, coordinate.second)) != bmap.end(); ++x) {
            if (add_ply(bmap, make_pair(x, coordinate.second), count))
                break;
        }

        for (auto x = coordinate.first - 1; bmap.find(make_pair(x, coordinate.second)) != bmap.end(); --x) {
            if (add_ply(bmap, make_pair(x, coordinate.second), count))
                break;
        }

        for (auto x = coordinate.first + 1, y = coordinate.second + 1; bmap.find(make_pair(x, y)) != bmap.end(); ++x, ++y) {
            if (add_ply(bmap, make_pair(x, y), count))
                break;
        }

        for (auto x = coordinate.first - 1, y = coordinate.second - 1; bmap.find(make_pair(x, y)) != bmap.end(); --x, --y) {
            if (add_ply(bmap, make_pair(x, y), count))
                break;
        }
    }
    return count;
}

long ring_connected_heuristic(State& state)
{
    std::vector<Coordinate> coords;
    for (const auto& m : state.board_map)
        coords.push_back(m.first);

    auto rings = state.get_player_rings();

    long count = 0;
    for (const auto& r : rings)
        count += std::count_if(coords.begin(), coords.end(), [&r](const Coordinate& x) {
            return r.first == x.first || r.second == x.second || (r.second - r.first) == (x.second - x.first);
        });

    // subtract current position
    return count - 1;
}

long ring_controlled_heuristic(State& state)
{

    auto add_ply = [](decltype(State::board_map)& bmap, const pair<int, int> p, long& count) -> bool {
        switch (bmap[p]) {
        case EMPTY:
            break;
        case WHITE_RING:
        case BLACK_RING:
            return true;
        case WHITE_MARKER:
        case BLACK_MARKER:
            ++count;
            break;
        }
        return false;
    };

    auto bmap = state.board_map;
    auto rings = state.get_player_rings();
    long count = 0;
    for (const auto& coordinate : rings) {
        for (auto y = coordinate.second + 1; bmap.find(make_pair(coordinate.first, y)) != bmap.end(); ++y) {
            if (add_ply(bmap, make_pair(coordinate.first, y), count))
                break;
        }

        for (auto y = coordinate.second - 1; bmap.find(make_pair(coordinate.first, y)) != bmap.end(); --y) {
            if (add_ply(bmap, make_pair(coordinate.first, y), count))
                break;
        }

        for (auto x = coordinate.first + 1; bmap.find(make_pair(x, coordinate.second)) != bmap.end(); ++x) {
            if (add_ply(bmap, make_pair(x, coordinate.second), count))
                break;
        }

        for (auto x = coordinate.first - 1; bmap.find(make_pair(x, coordinate.second)) != bmap.end(); --x) {
            if (add_ply(bmap, make_pair(x, coordinate.second), count))
                break;
        }

        for (auto x = coordinate.first + 1, y = coordinate.second + 1; bmap.find(make_pair(x, y)) != bmap.end(); ++x, ++y) {
            if (add_ply(bmap, make_pair(x, y), count))
                break;
        }

        for (auto x = coordinate.first - 1, y = coordinate.second - 1; bmap.find(make_pair(x, y)) != bmap.end(); --x, --y) {
            if (add_ply(bmap, make_pair(x, y), count))
                break;
        }
    }
    return count;
}

long ring_fuse_heuristic(State& state, long w1, long w2)
{

    auto add_ply = [](decltype(State::board_map)& bmap, const pair<int, int> p, long& connect_count, long& control_count) -> bool {
        switch (bmap[p]) {
        case EMPTY:
            ++control_count;
            break;
        case WHITE_RING:
        case BLACK_RING:
            return true;
        case WHITE_MARKER:
        case BLACK_MARKER:
            ++connect_count;
            break;
        }
        return false;
    };

    auto bmap = state.board_map;
    auto rings = state.get_player_rings();
    long connect_count = 0;
    long control_count = 0;
    for (const auto& coordinate : rings) {
        for (auto y = coordinate.second + 1; bmap.find(make_pair(coordinate.first, y)) != bmap.end(); ++y) {
            if (add_ply(bmap, make_pair(coordinate.first, y), connect_count, control_count))
                break;
        }

        for (auto y = coordinate.second - 1; bmap.find(make_pair(coordinate.first, y)) != bmap.end(); --y) {
            if (add_ply(bmap, make_pair(coordinate.first, y), connect_count, control_count))
                break;
        }

        for (auto x = coordinate.first + 1; bmap.find(make_pair(x, coordinate.second)) != bmap.end(); ++x) {
            if (add_ply(bmap, make_pair(x, coordinate.second), connect_count, control_count))
                break;
        }

        for (auto x = coordinate.first - 1; bmap.find(make_pair(x, coordinate.second)) != bmap.end(); --x) {
            if (add_ply(bmap, make_pair(x, coordinate.second), connect_count, control_count))
                break;
        }

        for (auto x = coordinate.first + 1, y = coordinate.second + 1; bmap.find(make_pair(x, y)) != bmap.end(); ++x, ++y) {
            if (add_ply(bmap, make_pair(x, y), connect_count, control_count))
                break;
        }

        for (auto x = coordinate.first - 1, y = coordinate.second - 1; bmap.find(make_pair(x, y)) != bmap.end(); --x, --y) {
            if (add_ply(bmap, make_pair(x, y), connect_count, control_count))
                break;
        }
    }
    return w1 * connect_count + w2 * control_count;
}

long ring_heuristic(const std::array<long, 3>& weights, State& state)
{

    long result = 0;
    std::vector<long (*)(State & state)> heuristics({ ring_moves_heuristic, ring_connected_heuristic, ring_controlled_heuristic });

    assert(weights.size() == heuristics.size());

    for (int i = 0; i != weights.size(); ++i) {
        if (weights[i] != 0)
            result += weights[i] * heuristics[i](state);
    }

    return result;
}

long consecutive_markers_heuristic(const State& state, int M)
{
    std::map<pair<int, int>, std::tuple<int, int, int>> marker_lines;
    const auto markers = state.get_player_markers();
    auto bmap = state.board_map;
    auto marker = state.player == Player::WHITE ? WHITE_MARKER : BLACK_MARKER;

    for (auto it = markers.cbegin(); it != markers.cend(); ++it) {
        if (abs(it->first) == M || abs(it->second) == M || (abs(it->second - it->first) == M))
            continue;
        marker_lines[*it] = std::make_tuple(1, 1, 1);
    }

    long max = 0;
    for (const auto& coordinate : markers) {

        if (get<0>(marker_lines[coordinate]) == 1) {
            long cnt = 1;
            for (auto y = coordinate.second + 1; bmap.find(std::make_pair(coordinate.first, y)) != bmap.end(); ++y) {
                if (bmap[std::make_pair(coordinate.first, y)] != marker)
                    break;
                else {
                    std::get<0>(marker_lines[std::make_pair(coordinate.first, y)]) = 0;
                    ++cnt;
                }
            }
            for (auto y = coordinate.second - 1; bmap.find(std::make_pair(coordinate.first, y)) != bmap.end(); --y) {
                if (bmap[std::make_pair(coordinate.first, y)] != marker)
                    break;
                else {
                    get<0>(marker_lines[std::make_pair(coordinate.first, y)]) = 0;
                    ++cnt;
                }
            }
            std::get<0>(marker_lines[coordinate]) = 0;
            if (max < cnt)
                max = cnt;
        }

        if (get<1>(marker_lines[coordinate]) == 1) {
            long cnt = 1;

            for (auto x = coordinate.first + 1; bmap.find(make_pair(x, coordinate.second)) != bmap.end(); ++x) {
                if (bmap[make_pair(x, coordinate.second)] != marker)
                    break;
                else {
                    std::get<1>(marker_lines[make_pair(x, coordinate.second)]) = 0;
                    ++cnt;
                }
            }

            for (auto x = coordinate.first - 1; bmap.find(make_pair(x, coordinate.second)) != bmap.end(); --x) {
                if (bmap[make_pair(x, coordinate.second)] != marker)
                    break;
                else {
                    std::get<1>(marker_lines[make_pair(x, coordinate.second)]) = 0;
                    ++cnt;
                }
            }
            std::get<1>(marker_lines[coordinate]) = 0;
            if (max < cnt)
                max = cnt;
        }

        if (get<2>(marker_lines[coordinate]) == 1) {
            long cnt = 1;
            for (auto x = coordinate.first + 1, y = coordinate.second + 1; bmap.find(make_pair(x, y)) != bmap.end(); ++x, ++y) {
                if (bmap[make_pair(x, y)] != marker)
                    break;
                else {
                    get<2>(marker_lines[make_pair(x, y)]) = 0;
                    ++cnt;
                }
            }

            for (auto x = coordinate.first - 1, y = coordinate.second - 1; bmap.find(make_pair(x, y)) != bmap.end(); --x, --y) {
                if (bmap[make_pair(x, y)] != marker)
                    break;
                else {
                    get<2>(marker_lines[make_pair(x, y)]) = 0;
                    ++cnt;
                }
            }
            get<2>(marker_lines[coordinate]) = 0;
            if (max < cnt)
                max = cnt;
        }
    }

    return max * 100;
}

long heuristic(State state, int M)
{
    auto player = state.player;
    auto my_valh = value_heuristic(state, M);
    auto my_markerh = marker_heuristic(state);
    // auto my_ringh = 1 * ring_moves_heuristic(state) + ring_fuse_heuristic(state, 1, 1);
    auto my_ringh = ring_connected_heuristic(state);

    auto my_consmarkerh = consecutive_markers_heuristic(state, M);

    Player other_player;
    if (player == WHITE)
        other_player = BLACK;
    else
        other_player = WHITE;

    state.player = other_player;

    auto other_valh = value_heuristic(state, M);
    auto other_markerh = marker_heuristic(state);
    auto other_ringh = ring_connected_heuristic(state);
    // auto other_ringh = 1 * ring_moves_heuristic(state) + ring_fuse_heuristic(state, 1, 1);
    auto other_consmarkerh = consecutive_markers_heuristic(state, M);

    auto valh = my_valh - other_valh;
    auto markerh = my_markerh - other_markerh;
    auto ringh = my_ringh - other_ringh;

    auto consh = my_consmarkerh - other_consmarkerh;

    return valh + markerh + ringh + consh;
}