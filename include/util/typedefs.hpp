#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include "util/strong_typedef.hpp"

#include <limits>
#include <cstddef>

// OpenStreetMap node ids are higher than 2^32
OSRM_STRONG_TYPEDEF(uint64_t, OSMNodeID)
OSRM_STRONG_TYPEDEF(uint32_t, OSMWayID)

static const OSMNodeID SPECIAL_OSM_NODEID = OSMNodeID(std::numeric_limits<std::uint64_t>::max());
static const OSMWayID SPECIAL_OSM_WAYID = OSMWayID(std::numeric_limits<std::uint32_t>::max());

static const OSMNodeID MAX_OSM_NODEID = OSMNodeID(std::numeric_limits<std::uint64_t>::max());
static const OSMNodeID MIN_OSM_NODEID = OSMNodeID(std::numeric_limits<std::uint64_t>::min());
static const OSMWayID MAX_OSM_WAYID = OSMWayID(std::numeric_limits<std::uint32_t>::max());
static const OSMWayID MIN_OSM_WAYID = OSMWayID(std::numeric_limits<std::uint32_t>::min());

using OSMNodeID_weak = std::uint64_t;
using OSMEdgeID_weak = std::uint64_t;

using NodeID = unsigned int;
using EdgeID = unsigned int;
using EdgeWeight = int;

static const NodeID SPECIAL_NODEID = std::numeric_limits<unsigned>::max();
static const EdgeID SPECIAL_EDGEID = std::numeric_limits<unsigned>::max();
static const unsigned INVALID_NAMEID = std::numeric_limits<unsigned>::max();
static const unsigned INVALID_COMPONENTID = 0;
static const EdgeWeight INVALID_EDGE_WEIGHT = std::numeric_limits<int>::max();
static const EdgeWeight MIN_INVALID_EDGE_WEIGHT = 0;
#endif /* TYPEDEFS_H */
