#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "state.h"
#include <array>

long value_heuristic(State, int);
long marker_heuristic(const State&);
long ring_moves_heuristic(State&);
long ring_connected_heuristic(State&);
long ring_controlled_heuristic(State&);
long ring_fuse_heuristic(State&, long, long);
long ring_heuristic(const std::array<long, 3>&, State&);
long heuristic(State&, int);
#endif