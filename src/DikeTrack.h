
#include <cstdint>
#include <string>

class DikeTrackPoint {
public:
        DikeTrackPoint (void);
        DikeTrackPoint (int32_t lat, int32_t lon);
        DikeTrackPoint (int32_t lat, int32_t lon, int32_t elevation);
        ~DikeTrackPoint (void);

        int32_t lat (void) const;
        int32_t lon (void) const;
        int32_t ele (void) const;

        void setLat (int32_t lat);
        void setLon (int32_t lon);
        void setEle (int32_t ele);

private:
        int32_t _lat;
        int32_t _lon;
        int32_t _ele;
};

enum DikeTrackPathCommand {
        DikeTrackPathCommandMoveTo,
        DikeTrackPathCommandTo,
};

class DikeTrackPath {
public:
        DikeTrackPath (void);
        DikeTrackPath (const char *name);
        DikeTrackPath (const std::string &name);
        ~DikeTrackPath (void);

        void moveTo (int32_t lat, int32_t lon);
        void moveTo (int32_t lat, int32_t lon, int32_t elevation);
        void moveTo (const DikeTrackPoint &point);

        void lineTo (int32_t lat, int32_t lon);
        void lineTo (int32_t lat, int32_t lon, int32_t elevation);
        void lineTo (const DikeTrackPoint &point);

private:
        std::string _name;
};

class DikeTrack {
public:
        DikeTrack (void);
        ~DikeTrack (void);
};
