#ifndef STATE_H
#define STATE_H

#include <unordered_map>
#include <utility>
#include <unordered_set>
#include <boost/functional/hash.hpp>

using namespace std;

enum Player { BLACK, WHITE };
enum Values { EMPTY, WHITE_RING, BLACK_RING, WHITE_MARKER, BLACK_MARKER };
enum Mode { P, S };

class State
{
public:
  Mode mode;
  Player player;
  unordered_map<pair<int, int>, Values, boost::hash<pair<int, int>>> board_map;
  unordered_set<pair<int, int>, boost::hash<pair<int, int>>> black_rings;
  unordered_set<pair<int, int>, boost::hash<pair<int, int>>> white_rings;
  unordered_set<pair<int, int>, boost::hash<pair<int, int>>> white_markers;
  unordered_set<pair<int, int>, boost::hash<pair<int, int>>> black_markers;

  State();
  static pair<int, int> hex_to_ycoord(pair<size_t, size_t>);
  static pair<size_t, size_t> ycoord_to_hex(pair<int, int>);
};

#endif