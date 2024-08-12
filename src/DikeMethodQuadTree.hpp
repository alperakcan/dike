
#if !defined(DIKE_METHOD_QUADTREE_HPP)
#define DIKE_METHOD_QUADTREE_HPP

#include "DikePath.hpp"
#include "DikeMethod.hpp"

class DikeMethodQuadTreePrivate;

class DikeMethodQuadTree:
        public DikeMethod {
public:
        DikeMethodQuadTree (void);
        virtual ~DikeMethodQuadTree (void);

        virtual int addTrack (DikePath *path);
        virtual int addRecord (DikePath *path);

        virtual double calculate (void);

private:
        DikeMethodQuadTreePrivate *_private;
};

#endif
