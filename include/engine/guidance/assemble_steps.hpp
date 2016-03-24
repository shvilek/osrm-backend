#ifndef ENGINE_GUIDANCE_ASSEMBLE_STEPS_HPP_
#define ENGINE_GUIDANCE_ASSEMBLE_STEPS_HPP_

#include "engine/guidance/route_step.hpp"
#include "engine/guidance/step_maneuver.hpp"
#include "engine/guidance/leg_geometry.hpp"
#include "engine/guidance/toolkit.hpp"
#include "extractor/guidance/turn_instruction.hpp"
#include "engine/internal_route_result.hpp"
#include "engine/phantom_node.hpp"
#include "util/coordinate_calculation.hpp"
#include "util/coordinate.hpp"
#include "util/bearing.hpp"
#include "extractor/travel_mode.hpp"

#include <vector>
#include <boost/optional.hpp>

namespace osrm
{
namespace engine
{
namespace guidance
{
namespace detail
{
StepManeuver stepManeuverFromGeometry(extractor::guidance::TurnInstruction instruction,
                                      const LegGeometry &leg_geometry,
                                      const std::size_t segment_index);

StepManeuver stepManeuverFromGeometry(extractor::guidance::TurnInstruction instruction,
                                      const WaypointType waypoint_type,
                                      const LegGeometry &leg_geometry);

} // ns detail

template <typename DataFacadeT>
std::vector<RouteStep> assembleSteps(const DataFacadeT &facade,
                                     const std::vector<PathData> &leg_data,
                                     const LegGeometry &leg_geometry,
                                     const PhantomNode &source_node,
                                     const PhantomNode &target_node,
                                     const bool source_traversed_in_reverse,
                                     const bool target_traversed_in_reverse)
{
    const double constexpr ZERO_DURATION = 0., ZERO_DISTANCE = 0.;
    const constexpr char* NO_ADDITIONAL_NAME = "";
    const EdgeWeight source_duration =
        source_traversed_in_reverse ? source_node.reverse_weight : source_node.forward_weight;
    const auto source_mode = source_traversed_in_reverse ? source_node.backward_travel_mode
                                                         : source_node.forward_travel_mode;

    const EdgeWeight target_duration =
        target_traversed_in_reverse ? target_node.reverse_weight : target_node.forward_weight;
    const auto target_mode = target_traversed_in_reverse ? target_node.backward_travel_mode
                                                         : target_node.forward_travel_mode;

    const auto number_of_segments = leg_geometry.GetNumberOfSegments();

    std::vector<RouteStep> steps;
    steps.reserve(number_of_segments);

    std::size_t segment_index = 0;
    BOOST_ASSERT(leg_geometry.locations.size() >= 2);

    // We report the relative position of source/target to the road only within a range that is
    // sufficiently different but not full of the path
    const constexpr double MINIMAL_RELATIVE_DISTANCE = 5., MAXIMAL_RELATIVE_DISTANCE = 300.;
    const auto distance_to_start = util::coordinate_calculation::haversineDistance(
        source_node.input_location, leg_geometry.locations[0]);
    const auto initial_modifier =
        distance_to_start >= MINIMAL_RELATIVE_DISTANCE &&
                distance_to_start <= MAXIMAL_RELATIVE_DISTANCE
            ? angleToDirectionModifier(util::coordinate_calculation::computeAngle(
                  source_node.input_location, leg_geometry.locations[0], leg_geometry.locations[1]))
            : extractor::guidance::DirectionModifier::UTurn;

    if (leg_data.size() > 0)
    {

        StepManeuver maneuver = detail::stepManeuverFromGeometry(
            extractor::guidance::TurnInstruction{extractor::guidance::TurnType::NoTurn,
                                                 initial_modifier},
            WaypointType::Depart, leg_geometry);
        maneuver.location = source_node.location;

        // PathData saves the information we need of the segment _before_ the turn,
        // but a RouteStep is with regard to the segment after the turn.
        // We need to skip the first segment because it is already covered by the
        // initial start of a route
        int segment_duration = 0;
        for (const auto &path_point : leg_data)
        {
            segment_duration += path_point.duration_until_turn;

            // all changes to this check have to be matched with assemble_geometry
            if (path_point.turn_instruction.type != extractor::guidance::TurnType::NoTurn)
            {
                BOOST_ASSERT(segment_duration >= 0);
                const auto name = facade.GetNameForID(path_point.name_id);
                const auto distance = leg_geometry.segment_distances[segment_index];
                steps.push_back(RouteStep{path_point.name_id,
                                          name,
                                          NO_ADDITIONAL_NAME,
                                          segment_duration / 10.0,
                                          distance,
                                          path_point.travel_mode,
                                          maneuver,
                                          leg_geometry.FrontIndex(segment_index),
                                          leg_geometry.BackIndex(segment_index) + 1});
                maneuver = detail::stepManeuverFromGeometry(path_point.turn_instruction,
                                                            leg_geometry, segment_index);
                segment_index++;
                segment_duration = 0;
            }
        }
        const auto distance = leg_geometry.segment_distances[segment_index];
        const int duration = segment_duration + target_duration;
        BOOST_ASSERT(duration >= 0);
        steps.push_back(RouteStep{target_node.name_id,
                                  facade.GetNameForID(target_node.name_id),
                                  NO_ADDITIONAL_NAME,
                                  duration / 10.,
                                  distance,
                                  target_mode,
                                  maneuver,
                                  leg_geometry.FrontIndex(segment_index),
                                  leg_geometry.BackIndex(segment_index) + 1});
    }
    // In this case the source + target are on the same edge segment
    else
    {
        BOOST_ASSERT(source_node.fwd_segment_position == target_node.fwd_segment_position);
        //     s     t
        // u-------------v
        // |---| source_duration
        // |---------| target_duration

        StepManeuver maneuver = detail::stepManeuverFromGeometry(
            extractor::guidance::TurnInstruction{extractor::guidance::TurnType::NoTurn,
                                                 initial_modifier},
            WaypointType::Depart, leg_geometry);
        int duration = target_duration - source_duration;
        BOOST_ASSERT(duration >= 0);

        steps.push_back(RouteStep{source_node.name_id,
                                  facade.GetNameForID(source_node.name_id),
                                  NO_ADDITIONAL_NAME,
                                  duration / 10.,
                                  leg_geometry.segment_distances[segment_index],
                                  source_mode,
                                  std::move(maneuver),
                                  leg_geometry.FrontIndex(segment_index),
                                  leg_geometry.BackIndex(segment_index) + 1});
    }

    BOOST_ASSERT(segment_index == number_of_segments - 1);
    const auto distance_from_end = util::coordinate_calculation::haversineDistance(
        target_node.input_location, leg_geometry.locations.back());
    const auto final_modifier =
        distance_from_end >= MINIMAL_RELATIVE_DISTANCE &&
                distance_from_end <= MAXIMAL_RELATIVE_DISTANCE
            ? angleToDirectionModifier(util::coordinate_calculation::computeAngle(
                  leg_geometry.locations[leg_geometry.locations.size() - 2],
                  leg_geometry.locations[leg_geometry.locations.size() - 1],
                  target_node.input_location))
            : extractor::guidance::DirectionModifier::UTurn;
    // This step has length zero, the only reason we need it is the target location
    auto final_maneuver = detail::stepManeuverFromGeometry(
        extractor::guidance::TurnInstruction{extractor::guidance::TurnType::NoTurn, final_modifier},
        WaypointType::Arrive, leg_geometry);
    steps.push_back(RouteStep{target_node.name_id,
                              facade.GetNameForID(target_node.name_id),
                              NO_ADDITIONAL_NAME,
                              ZERO_DURATION,
                              ZERO_DISTANCE,
                              target_mode,
                              final_maneuver,
                              leg_geometry.locations.size(),
                              leg_geometry.locations.size()});

    return steps;
}

} // namespace guidance
} // namespace engine
} // namespace osrm

#endif // ENGINE_GUIDANCE_SEGMENT_LIST_HPP_
