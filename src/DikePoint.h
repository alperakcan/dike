
#include <cstdint>

class DikePoint {
public:
        DikePoint (void);
        DikePoint (int32_t lat, int32_t lon);
        DikePoint (int32_t lat, int32_t lon, int32_t ele);
        DikePoint (int32_t lat, int32_t lon, int32_t ele, int32_t tim);
        ~DikePoint (void);

        int32_t lat (void) const;
        int32_t lon (void) const;
        int32_t ele (void) const;
        int32_t tim (void) const;

        void setLat (int32_t lat);
        void setLon (int32_t lon);
        void setEle (int32_t ele);
        void setTim (int32_t tim);

private:
        int32_t _lat;
        int32_t _lon;
        int32_t _ele;
        int32_t _tim;
};
