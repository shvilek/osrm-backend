#include "util/coordinate_calculation.hpp"

#include "util/string_util.hpp"
#include "util/trigonometry_table.hpp"

#include <boost/assert.hpp>

#include <cmath>

#include <limits>

namespace osrm
{
namespace util
{

namespace coordinate_calculation
{

double euclideanDistance(const Coordinate coordinate_1, const Coordinate coordinate_2)
{
    const double x1 = static_cast<double>(toFloating(coordinate_1.lon));
    const double y1 = mercator::latToY(toFloating(coordinate_1.lat));
    const double x2 = static_cast<double>(toFloating(coordinate_2.lon));
    const double y2 = mercator::latToY(toFloating(coordinate_2.lat));
    const double dx = x1 - x2;
    const double dy = y1 - y2;

    return std::sqrt(dx * dx + dy * dy);
}

double haversineDistance(const Coordinate coordinate_1, const Coordinate coordinate_2)
{
    auto lon1 = static_cast<int>(coordinate_1.lon);
    auto lat1 = static_cast<int>(coordinate_1.lat);
    auto lon2 = static_cast<int>(coordinate_2.lon);
    auto lat2 = static_cast<int>(coordinate_2.lat);
    BOOST_ASSERT(lon1 != std::numeric_limits<int>::min());
    BOOST_ASSERT(lat1 != std::numeric_limits<int>::min());
    BOOST_ASSERT(lon2 != std::numeric_limits<int>::min());
    BOOST_ASSERT(lat2 != std::numeric_limits<int>::min());
    const double lt1 = lat1 / COORDINATE_PRECISION;
    const double ln1 = lon1 / COORDINATE_PRECISION;
    const double lt2 = lat2 / COORDINATE_PRECISION;
    const double ln2 = lon2 / COORDINATE_PRECISION;
    const double dlat1 = lt1 * DEGREE_TO_RAD;

    const double dlong1 = ln1 * DEGREE_TO_RAD;
    const double dlat2 = lt2 * DEGREE_TO_RAD;
    const double dlong2 = ln2 * DEGREE_TO_RAD;

    const double dlong = dlong1 - dlong2;
    const double dlat = dlat1 - dlat2;

    const double aharv = std::pow(std::sin(dlat / 2.0), 2.0) +
                         std::cos(dlat1) * std::cos(dlat2) * std::pow(std::sin(dlong / 2.), 2);
    const double charv = 2. * std::atan2(std::sqrt(aharv), std::sqrt(1.0 - aharv));
    return EARTH_RADIUS * charv;
}

double greatCircleDistance(const Coordinate coordinate_1, const Coordinate coordinate_2)
{
    auto lon1 = static_cast<int>(coordinate_1.lon);
    auto lat1 = static_cast<int>(coordinate_1.lat);
    auto lon2 = static_cast<int>(coordinate_2.lon);
    auto lat2 = static_cast<int>(coordinate_2.lat);
    BOOST_ASSERT(lat1 != std::numeric_limits<int>::min());
    BOOST_ASSERT(lon1 != std::numeric_limits<int>::min());
    BOOST_ASSERT(lat2 != std::numeric_limits<int>::min());
    BOOST_ASSERT(lon2 != std::numeric_limits<int>::min());

    const double float_lat1 = (lat1 / COORDINATE_PRECISION) * DEGREE_TO_RAD;
    const double float_lon1 = (lon1 / COORDINATE_PRECISION) * DEGREE_TO_RAD;
    const double float_lat2 = (lat2 / COORDINATE_PRECISION) * DEGREE_TO_RAD;
    const double float_lon2 = (lon2 / COORDINATE_PRECISION) * DEGREE_TO_RAD;

    const double x_value = (float_lon2 - float_lon1) * std::cos((float_lat1 + float_lat2) / 2.0);
    const double y_value = float_lat2 - float_lat1;
    return std::hypot(x_value, y_value) * EARTH_RADIUS;
}

double perpendicularDistance(const Coordinate source_coordinate,
                             const Coordinate target_coordinate,
                             const Coordinate query_location)
{
    double ratio;
    Coordinate nearest_location;

    return perpendicularDistance(source_coordinate, target_coordinate, query_location,
                                 nearest_location, ratio);
}

double perpendicularDistance(const Coordinate segment_source,
                             const Coordinate segment_target,
                             const Coordinate query_location,
                             Coordinate &nearest_location,
                             double &ratio)
{
    using namespace coordinate_calculation;

    return perpendicularDistanceFromProjectedCoordinate(
        segment_source, segment_target, query_location,
        {static_cast<double>(toFloating(query_location.lon)),
         mercator::latToY(toFloating(query_location.lat))},
        nearest_location, ratio);
}

double perpendicularDistanceFromProjectedCoordinate(
    const Coordinate source_coordinate,
    const Coordinate target_coordinate,
    const Coordinate query_location,
    const std::pair<double, double> projected_xy_coordinate)
{
    double ratio;
    Coordinate nearest_location;

    return perpendicularDistanceFromProjectedCoordinate(source_coordinate, target_coordinate,
                                                        query_location, projected_xy_coordinate,
                                                        nearest_location, ratio);
}

double perpendicularDistanceFromProjectedCoordinate(
    const Coordinate segment_source,
    const Coordinate segment_target,
    const Coordinate query_location,
    const std::pair<double, double> projected_xy_coordinate,
    Coordinate &nearest_location,
    double &ratio)
{
    using namespace coordinate_calculation;

    BOOST_ASSERT(query_location.IsValid());

    // initialize values
    const double x = projected_xy_coordinate.second;
    const double y = projected_xy_coordinate.first;
    const double a = mercator::latToY(toFloating(segment_source.lat));
    const double b = static_cast<double>(toFloating(segment_source.lon));
    const double c = mercator::latToY(toFloating(segment_target.lat));
    const double d = static_cast<double>(toFloating(segment_target.lon));
    double p, q /*,mX*/, new_y;
    if (std::abs(a - c) > std::numeric_limits<double>::epsilon())
    {
        const double m = (d - b) / (c - a); // slope
        // Projection of (x,y) on line joining (a,b) and (c,d)
        p = ((x + (m * y)) + (m * m * a - m * b)) / (1.0 + m * m);
        q = b + m * (p - a);
    }
    else
    {
        p = c;
        q = y;
    }
    new_y = (d * p - c * q) / (a * d - b * c);

    // discretize the result to coordinate precision. it's a hack!
    if (std::abs(new_y) < (1.0 / COORDINATE_PRECISION))
    {
        new_y = 0.0;
    }

    // compute ratio
    ratio = static_cast<double>((p - new_y * a) /
                                c); // These values are actually n/m+n and m/m+n , we need
    // not calculate the explicit values of m an n as we
    // are just interested in the ratio
    if (std::isnan(ratio))
    {
        ratio = (segment_target == query_location ? 1.0 : 0.0);
    }
    else if (std::abs(ratio) <= std::numeric_limits<double>::epsilon())
    {
        ratio = 0.0;
    }
    else if (std::abs(ratio - 1.0) <= std::numeric_limits<double>::epsilon())
    {
        ratio = 1.0;
    }

    // compute nearest location
    BOOST_ASSERT(!std::isnan(ratio));
    if (ratio <= 0.0)
    {
        nearest_location = segment_source;
    }
    else if (ratio >= 1.0)
    {
        nearest_location = segment_target;
    }
    else
    {
        // point lies in between
        nearest_location.lon = toFixed(FloatLongitude(q));
        nearest_location.lat = toFixed(FloatLatitude(mercator::yToLat(p)));
    }
    BOOST_ASSERT(nearest_location.IsValid());

    const double approximate_distance = greatCircleDistance(query_location, nearest_location);
    BOOST_ASSERT(0.0 <= approximate_distance);
    return approximate_distance;
}

double degToRad(const double degree)
{
    using namespace boost::math::constants;
    return degree * (pi<double>() / 180.0);
}

double radToDeg(const double radian)
{
    using namespace boost::math::constants;
    return radian * (180.0 * (1. / pi<double>()));
}

double bearing(const Coordinate first_coordinate, const Coordinate second_coordinate)
{
    const double lon_diff =
        static_cast<double>(toFloating(second_coordinate.lon - first_coordinate.lon));
    const double lon_delta = degToRad(lon_diff);
    const double lat1 = degToRad(static_cast<double>(toFloating(first_coordinate.lat)));
    const double lat2 = degToRad(static_cast<double>(toFloating(second_coordinate.lat)));
    const double y = std::sin(lon_delta) * std::cos(lat2);
    const double x =
        std::cos(lat1) * std::sin(lat2) - std::sin(lat1) * std::cos(lat2) * std::cos(lon_delta);
    double result = radToDeg(std::atan2(y, x));
    while (result < 0.0)
    {
        result += 360.0;
    }

    while (result >= 360.0)
    {
        result -= 360.0;
    }
    return result;
}

double computeAngle(const Coordinate first, const Coordinate second, const Coordinate third)
{
    using namespace boost::math::constants;
    using namespace coordinate_calculation;

    const double v1x = static_cast<double>(toFloating(first.lon - second.lon));
    const double v1y =
        mercator::latToY(toFloating(first.lat)) - mercator::latToY(toFloating(second.lat));
    const double v2x = static_cast<double>(toFloating(third.lon - second.lon));
    const double v2y =
        mercator::latToY(toFloating(third.lat)) - mercator::latToY(toFloating(second.lat));

    double angle = (atan2_lookup(v2y, v2x) - atan2_lookup(v1y, v1x)) * 180. / pi<double>();

    while (angle < 0.)
    {
        angle += 360.;
    }

    return angle;
}

boost::optional<Coordinate>
circleCenter(const Coordinate C1, const Coordinate C2, const Coordinate C3)
{
    // require three distinct points
    if (C1 == C2 || C2 == C3 || C1 == C3)
        return boost::none;

    // define line through c1, c2 and c2,c3
    const double C2C1_lat = static_cast<double>(toFloating(C2.lat - C1.lat)); // yDelta_a
    const double C2C1_lon = static_cast<double>(toFloating(C2.lon - C1.lon)); // xDelta_a
    const double C3C2_lat = static_cast<double>(toFloating(C3.lat - C2.lat)); // yDelta_b
    const double C3C2_lon = static_cast<double>(toFloating(C3.lon - C2.lon)); // xDelta_b

    // check for collinear points in X-Direction
    if (std::abs(C2C1_lon) < std::numeric_limits<double>::epsilon() &&
        std::abs(C3C2_lon) < std::numeric_limits<double>::epsilon())
    {
        return boost::none;
    }
    else if (std::abs(C2C1_lon) < std::numeric_limits<double>::epsilon())
    {
        // vertical line C2C1
        // due to c1.lon == c2.lon && c1.lon != c3.lon we can rearrange this way
        BOOST_ASSERT(std::abs(static_cast<double>(toFloating(C3.lon - C1.lon))) >=
                         std::numeric_limits<double>::epsilon() &&
                     std::abs(static_cast<double>(toFloating(C2.lon - C3.lon))) >=
                         std::numeric_limits<double>::epsilon());
        return circleCenter(C1, C3, C2);
    }
    else if (std::abs(C3C2_lon) < std::numeric_limits<double>::epsilon())
    {
        // vertical line C3C2
        // due to c2.lon == c3.lon && c1.lon != c3.lon we can rearrange this way
        // after rearrangement both deltas will be zero
        BOOST_ASSERT(std::abs(static_cast<double>(toFloating(C1.lon - C2.lon))) >=
                         std::numeric_limits<double>::epsilon() &&
                     std::abs(static_cast<double>(toFloating(C3.lon - C1.lon))) >=
                         std::numeric_limits<double>::epsilon());
        return circleCenter(C2, C1, C3);
    }
    else
    { // valid slope values for both lines, calculate the center as intersection of the lines
        const double C2C1_slope = C2C1_lat / C2C1_lon;
        const double C3C2_slope = C3C2_lat / C3C2_lon;

        //can this ever happen?
        if (std::abs(C2C1_slope - C3C2_slope) < std::numeric_limits<double>::epsilon())
            return boost::none;

        const double C1_y = static_cast<double>(toFloating(C1.lat));
        const double C1_x = static_cast<double>(toFloating(C1.lon));
        const double C2_y = static_cast<double>(toFloating(C2.lat));
        const double C2_x = static_cast<double>(toFloating(C2.lon));
        const double C3_y = static_cast<double>(toFloating(C3.lat));
        const double C3_x = static_cast<double>(toFloating(C3.lon));

        const double lon = (C2C1_slope * C3C2_slope * (C1_y - C3_y) + C3C2_slope * (C1_x + C2_x) -
                            C2C1_slope * (C2_x + C3_x)) /
                           (2 * (C3C2_slope - C2C1_slope));
        const double lat = (0.5 * (C1_x + C2_x) - lon) / C2C1_slope + 0.5 * (C1_y + C2_y);
        return Coordinate(FloatLongitude(lon), FloatLatitude(lat));
    }
}

double circleRadius(const Coordinate C1, const Coordinate C2, const Coordinate C3)
{
    // a circle by three points requires thee distinct points
    auto center = circleCenter(C1, C2, C3);
    if (center)
        return haversineDistance(C1, *center);
    else
        return std::numeric_limits<double>::infinity();
}

Coordinate interpolateLinear(double factor, const Coordinate from, const Coordinate to)
{
    BOOST_ASSERT(0 <= factor && factor <= 1.0);
    return {
        from.lon +
            toFixed(FloatLongitude(factor * static_cast<double>(toFloating(to.lon - from.lon)))),
        from.lat +
            toFixed(FloatLatitude(factor * static_cast<double>(toFloating(to.lat - from.lat))))};
}

namespace mercator
{
FloatLatitude yToLat(const double y)
{
    const double normalized_lat = RAD_TO_DEGREE * 2. * std::atan(std::exp(y * DEGREE_TO_RAD));

    return FloatLatitude(normalized_lat - 90.);
}

double latToY(const FloatLatitude latitude)
{
    const double normalized_lat = 90. + static_cast<double>(latitude);

    return RAD_TO_DEGREE * std::log(std::tan(normalized_lat * DEGREE_TO_RAD * 0.5));
}

FloatLatitude clamp(const FloatLatitude lat)
{
    return std::max(std::min(lat, FloatLatitude(detail::MAX_LATITUDE)),
                    FloatLatitude(-detail::MAX_LATITUDE));
}

FloatLongitude clamp(const FloatLongitude lon)
{
    return std::max(std::min(lon, FloatLongitude(detail::MAX_LONGITUDE)),
                    FloatLongitude(-detail::MAX_LONGITUDE));
}

inline void pixelToDegree(const double shift, double &x, double &y)
{
    const double b = shift / 2.0;
    x = (x - b) / shift * 360.0;
    // FIXME needs to be simplified
    const double g = (y - b) / -(shift / (2 * M_PI)) / DEGREE_TO_RAD;
    static_assert(DEGREE_TO_RAD / (2 * M_PI) - 1 / 360. < 0.0001, "");
    y = static_cast<double>(util::coordinate_calculation::mercator::yToLat(g));
}

double degreeToPixel(FloatLongitude lon, unsigned zoom)
{
    const double shift = (1u << zoom) * TILE_SIZE;
    const double b = shift / 2.0;
    const double x = b * (1 + static_cast<double>(lon) / 180.0);
    return x;
}

double degreeToPixel(FloatLatitude lat, unsigned zoom)
{
    const double shift = (1u << zoom) * TILE_SIZE;
    const double b = shift / 2.0;
    const double y = b * (1. - latToY(lat) / 180.);
    return y;
}

// Converts a WMS tile coordinate (z,x,y) into a wsg84 bounding box
void xyzToWSG84(
    const int x, const int y, const int z, double &minx, double &miny, double &maxx, double &maxy)
{
    using util::coordinate_calculation::mercator::TILE_SIZE;

    minx = x * TILE_SIZE;
    miny = (y + 1.0) * TILE_SIZE;
    maxx = (x + 1.0) * TILE_SIZE;
    maxy = y * TILE_SIZE;
    // 2^z * TILE_SIZE
    const double shift = (1u << static_cast<unsigned>(z)) * TILE_SIZE;
    pixelToDegree(shift, minx, miny);
    pixelToDegree(shift, maxx, maxy);
}

// Converts a WMS tile coordinate (z,x,y) into a mercator bounding box
void xyzToMercator(
    const int x, const int y, const int z, double &minx, double &miny, double &maxx, double &maxy)
{
    using namespace util::coordinate_calculation::mercator;

    xyzToWSG84(x, y, z, minx, miny, maxx, maxy);

    minx = static_cast<double>(clamp(util::FloatLongitude(minx))) * DEGREE_TO_PX;
    miny = latToY(clamp(util::FloatLatitude(miny))) * DEGREE_TO_PX;
    maxx = static_cast<double>(clamp(util::FloatLongitude(maxx))) * DEGREE_TO_PX;
    maxy = latToY(clamp(util::FloatLatitude(maxy))) * DEGREE_TO_PX;
}

} // ns mercato // ns mercatorr
} // ns coordinate_calculation
} // ns util
} // ns osrm
