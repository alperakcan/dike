
#if !defined(DIKE_METHOD_QUADTREE_HPP)
#define DIKE_METHOD_QUADTREE_HPP

#include "DikePath.hpp"
#include "DikeCalculateMethod.hpp"

class DikeCalculateMethodQuadTreePrivate;

class DikeCalculateMethodQuadTree:
        public DikeCalculateMethod {
public:
        DikeCalculateMethodQuadTree (const DikeCalculateMethodOptions &options);
        virtual ~DikeCalculateMethodQuadTree (void);

        virtual int addTrack (DikePath *path);
        virtual int addRecord (DikePath *path);

        virtual std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>>  calculate (void);

private:
        DikeCalculateMethodQuadTreePrivate *_private;
};

#endif
