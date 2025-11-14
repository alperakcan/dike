
#if !defined(DIKE_METHOD_BRUTEFORCE_HPP)
#define DIKE_METHOD_BRUTEFORCE_HPP

#include "DikePath.hpp"
#include "DikeCalculateMethod.hpp"

class DikeCalculateMethodBruteForcePrivate;

class DikeCalculateMethodBruteForce:
        public DikeCalculateMethod {
public:
        DikeCalculateMethodBruteForce (const DikeCalculateMethodOptions &options);
        virtual ~DikeCalculateMethodBruteForce (void);

        virtual int addTrack (DikePath *path);
        virtual int addRecord (DikePath *path);

        virtual std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>>  calculate (void);

private:
        DikeCalculateMethodBruteForcePrivate *_private;
};

#endif
