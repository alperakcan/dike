
#if !defined(DIKE_METHOD_BRUTE_HPP)
#define DIKE_METHOD_BRUTE_HPP

#include "DikePath.hpp"
#include "DikeMethod.hpp"

class DikeMethodBrutePrivate;

class DikeMethodBrute:
        public DikeMethod {
public:
        DikeMethodBrute (void);
        virtual ~DikeMethodBrute (void);

        virtual int addTrack (DikePath *path);
        virtual int addRecord (DikePath *path);

        virtual double calculate (void);

private:
        DikeMethodBrutePrivate *_private;
};

#endif
