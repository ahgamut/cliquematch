#ifndef ISOGRAPH_H
#define ISOGRAPH_H

#include <templates/ext_template.hpp>

class Isograph : public SlowTemplate<pygraph, pygraph>
{
   public:
    bool build_edges(pygraph&, pygraph&);
};

#endif /* ISOGRAPH_H */
