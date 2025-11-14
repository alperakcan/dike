
#if !defined(DIKE_METHOD_QUADTREE_HPP)
#define DIKE_METHOD_QUADTREE_HPP

#include "DikePath.hpp"
#include "DikeMethod.hpp"

class DikeMethodQuadTreePrivate;

class DikeMethodQuadTree:
        public DikeMethod {
public:
        DikeMethodQuadTree (const DikeMethodOptions &options);
        virtual ~DikeMethodQuadTree (void);

        virtual int addTrack (DikePath *path);
        virtual int addRecord (DikePath *path);

        virtual std::tuple<int, int, int, double, double>  calculate (void);

private:
        DikeMethodQuadTreePrivate *_private;
};

#endif
