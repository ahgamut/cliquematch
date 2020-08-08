#ifndef ISOGRAPH_H
#define ISOGRAPH_H

#include <templates/ext_template.hpp>

namespace cliquematch
{
namespace ext
{
    class IsoGraph : public SlowTemplate<pygraph, pygraph>
    {
       public:
        bool build_edges(pygraph&, pygraph&);
    };
}  // namespace ext
}  // namespace cliquematch
#endif /* ISOGRAPH_H */
