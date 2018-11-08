#ifndef STATE_H
#define STATE_H

#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using namespace std;

enum Player { BLACK, WHITE };
enum Values { EMPTY, WHITE_RING, BLACK_RING, WHITE_MARKER, BLACK_MARKER };
enum class Mode_S { P, S };

class State {
public:
  Mode_S mode;
  Player player;
  unordered_map<pair<int, int>, Values, boost::hash<pair<int, int>>> board_map;
  unordered_set<pair<int, int>, boost::hash<pair<int, int>>> black_rings;
  unordered_set<pair<int, int>, boost::hash<pair<int, int>>> white_rings;
  unordered_set<pair<int, int>, boost::hash<pair<int, int>>> white_markers;
  unordered_set<pair<int, int>, boost::hash<pair<int, int>>> black_markers;

  State();
  static pair<int, int> hex_to_ycoord(pair<size_t, size_t>);
  static pair<size_t, size_t> ycoord_to_hex(pair<int, int>);

  decltype(white_rings) &get_player_rings();
  decltype(white_markers) &get_player_markers();
  decltype(white_rings) &get_other_player_rings();
  decltype(white_markers) &get_other_player_markers();

  const decltype(white_rings) &get_player_rings() const;
  const decltype(white_markers) &get_player_markers() const;
  const decltype(white_rings) &get_other_player_rings() const;
  const decltype(white_markers) &get_other_player_markers() const;
};

#endif