#include "move.h"

Move::MarkerRow::MarkerRow(Coordinate s, Coordinate e, Coordinate r) {
    start = s;
    end = e;
    ring = r;
}

Move::Move(Coordinate c1, Coordinate c2) {
    initial_pos = c1;
    final_pos = c2;
    mode = Mode_M::S;
}

Move::Move() {
    mode = Mode_M::S;
}