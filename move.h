#ifndef MOVE_H
#define MOVE_H

#include <list>
#include <utility>

using namespace std;

enum class Mode_M { P,
            S };

class Move {
  public:
    // typedefs
    typedef pair<int, int> Coordinate;

    struct MarkerRow {
        Coordinate start;
        Coordinate end;
        Coordinate ring;
    };

    // member variables
    Mode_M mode;
    list<MarkerRow> initial_removal;
    list<MarkerRow> final_removal;
    Coordinate placing_pos;
    Coordinate initial_pos;
    Coordinate final_pos;
};
#endif