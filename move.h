#ifndef MOVE_H
#define MOVE_H

#include <list>
#include <utility>

using namespace std;

enum class Mode_M { P,
                    S };

typedef pair<int, int> Coordinate;

class Move {
  public:
    // typedefs

    struct MarkerRow {
        Coordinate start;
        Coordinate end;
        Coordinate ring;

        MarkerRow() = default;
        MarkerRow(Coordinate, Coordinate, Coordinate);
    };

    // member variables
    Mode_M mode;
    list<MarkerRow> initial_removal;
    list<MarkerRow> final_removal;
    Coordinate placing_pos;
    Coordinate initial_pos;
    Coordinate final_pos;

    Move();
    Move(Coordinate, Coordinate);
};
#endif