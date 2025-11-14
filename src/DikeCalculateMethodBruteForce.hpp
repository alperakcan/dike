
#if !defined(DIKE_METHOD_BRUTEFORCE_HPP)
#define DIKE_METHOD_BRUTEFORCE_HPP

#include "DikePath.hpp"
#include "DikeMethod.hpp"

class DikeMethodBruteForcePrivate;

class DikeMethodBruteForce:
        public DikeMethod {
public:
        DikeMethodBruteForce (const DikeMethodOptions &options);
        virtual ~DikeMethodBruteForce (void);

        virtual int addTrack (DikePath *path);
        virtual int addRecord (DikePath *path);

        virtual std::tuple<int, int, int, double, double>  calculate (void);

private:
        DikeMethodBruteForcePrivate *_private;
};

#endif
