#ifndef ORIGINAL_EDGE_DATA_HPP
#define ORIGINAL_EDGE_DATA_HPP

#include "extractor/travel_mode.hpp"
#include "extractor/turn_instructions.hpp"
#include "util/typedefs.hpp"

#include <limits>

namespace osrm
{
namespace extractor
{

struct OriginalEdgeData
{
    explicit OriginalEdgeData(NodeID via_node,
                              unsigned name_id,
                              TurnInstruction turn_instruction,
                              TravelMode travel_mode)
        : via_node(via_node), name_id(name_id), turn_instruction(turn_instruction),
          travel_mode(travel_mode)
    {
    }

    OriginalEdgeData()
        : via_node(std::numeric_limits<unsigned>::max()),
          name_id(std::numeric_limits<unsigned>::max()), turn_instruction(TurnInstruction::NoTurn),
          travel_mode(TRAVEL_MODE_INACCESSIBLE)
    {
    }

    NodeID via_node;
    unsigned name_id;
    TurnInstruction turn_instruction;
    TravelMode travel_mode;
};
}
}

#endif // ORIGINAL_EDGE_DATA_HPP
