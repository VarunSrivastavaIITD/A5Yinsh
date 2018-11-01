#ifndef MOVE_H
#define MOVE_H

#include <list>
#include <utility>

using namespace std;

enum Mode { P,
            S };

class Move {
  public:
    // typedefs
    typedef std::pair<int, int> Coordinate;

    struct MarkerRow {
        Coordinate start;
        Coordinate end;
        Coordinate ring;
    };

    // member variables
    Mode mode;
    std::list<MarkerRow> initial_removal;
    std::list<MarkerRow> final_removal;
    Coordinate placing_pos;
    Coordinate initial_pos;
    Coordinate final_pos;
};
#endif