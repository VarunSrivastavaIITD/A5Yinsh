#include "state.h"

decltype(State::white_markers) &State::get_player_markers() {
    switch (player) {
    case WHITE:
        return white_markers;
    default:
        return black_markers;
    }
}

decltype(State::white_rings) &State::get_other_player_rings() {
    switch (player) {
    case WHITE:
        return black_rings;
    default:
        return white_rings;
    }
}

decltype(State::white_markers) &State::get_other_player_markers() {
    switch (player) {
    case WHITE:
        return black_markers;
    default:
        return white_markers;
    }
}

decltype(State::white_rings) &State::get_player_rings() {
    switch (player) {
    case WHITE:
        return white_rings;
    default:
        return black_rings;
    }
}

const decltype(State::white_markers) &State::get_player_markers() const {
    switch (player) {
    case WHITE:
        return white_markers;
    default:
        return black_markers;
    }
}

const decltype(State::white_rings) &State::get_other_player_rings() const {
    switch (player) {
    case WHITE:
        return black_rings;
    default:
        return white_rings;
    }
}

const decltype(State::white_markers) &State::get_other_player_markers() const {
    switch (player) {
    case WHITE:
        return black_markers;
    default:
        return white_markers;
    }
}

const decltype(State::white_rings) &State::get_player_rings() const {
    switch (player) {
    case WHITE:
        return white_rings;
    default:
        return black_rings;
    }
}