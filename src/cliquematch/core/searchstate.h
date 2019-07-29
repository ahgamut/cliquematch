#ifndef SEARCHSTATE_H
#define SEARCHSTATE_H
#include <core/vertex.h>

struct SearchState {
    unsigned int start_at;
    graphBits cand, res;

    SearchState(const vertex&);
    SearchState(graphBits, graphBits);
};

#endif /* SEARCHSTATE_H */
