
#include <cmath>
#include "DikeProjectionMercator.hpp"

#include <math.h>

struct bound {
        double minlon;
        double maxlon;
        double minlat;
        double maxlat;
};
static struct bound g_bound;

static const double g_earth_mean_radius     = 6378137;
static const double g_earth_bound_lon_east  = +180.00;
static const double g_earth_bound_lon_west  = -180.00;
static const double g_earth_bound_lat_north = +83.00;
static const double g_earth_bound_lat_south = -83.00;

static inline double mercator_lon_to_x (double lon)
{
	return lon * g_earth_mean_radius * M_PI / 180.00;
}

static inline double mercator_lat_to_y (double lat)
{
	return log(tan(M_PI_4 + lat * M_PI / 360.00)) * g_earth_mean_radius;
}

static inline double mercator_x_to_lon (double x)
{
	return x / g_earth_mean_radius / M_PI * 180.00;
}

static inline double mercator_y_to_lat (double y)
{
	return atan(exp(y / g_earth_mean_radius)) / M_PI * 360.00 - 90.00;
}

DikeProjectionMercator::DikeProjectionMercator (void)
{
	g_bound.minlon = mercator_lon_to_x(g_earth_bound_lon_west);
	g_bound.maxlon = mercator_lon_to_x(g_earth_bound_lon_east);
	g_bound.minlat = mercator_lat_to_y(g_earth_bound_lat_south);
	g_bound.maxlat = mercator_lat_to_y(g_earth_bound_lat_north);
	g_bound.minlon = g_bound.minlon + g_bound.maxlon;
	g_bound.maxlon = g_bound.maxlon + g_bound.maxlon;
	g_bound.minlat = g_bound.minlat + g_bound.maxlat;
	g_bound.maxlat = g_bound.maxlat + g_bound.maxlat;
}

DikeProjectionMercator::~DikeProjectionMercator (void)
{
}

void DikeProjectionMercator::forward (double lon, double lat, double &x, double &y)
{
	if (lon > g_earth_bound_lon_east) {
		lon = g_earth_bound_lon_east;
	}
	if (lon < g_earth_bound_lon_west) {
		lon = g_earth_bound_lon_west;
	}
	if (lat > g_earth_bound_lat_north) {
		lat = g_earth_bound_lat_north;
	}
	if (lat < g_earth_bound_lat_south) {
		lat = g_earth_bound_lat_south;
	}
        x = mercator_lon_to_x(lon);
        y = mercator_lat_to_y(lat);
}

void DikeProjectionMercator::inverse (double x, double y, double &lon, double &lat)
{
        lon = mercator_x_to_lon(x);
        lat = mercator_y_to_lat(y);
	if (lon > g_earth_bound_lon_east) {
		lon = g_earth_bound_lon_east;
	}
	if (lon < g_earth_bound_lon_west) {
		lon = g_earth_bound_lon_west;
	}
	if (lat > g_earth_bound_lat_north) {
		lat = g_earth_bound_lat_north;
	}
	if (lat < g_earth_bound_lat_south) {
		lat = g_earth_bound_lat_south;
	}
}
