-- Foot profile

local find_access_tag = require("lib/access").find_access_tag

-- Begin of globals
barrier_whitelist = { [""] = true, ["cycle_barrier"] = true, ["bollard"] = true, ["entrance"] = true, ["cattle_grid"] = true, ["border_control"] = true, ["toll_booth"] = true, ["sally_port"] = true, ["gate"] = true, ["no"] = true, ["block"] = true, ["lift_gate"] = true}
access_tag_whitelist = { ["yes"] = true, ["foot"] = true, ["permissive"] = true, ["designated"] = true  }
access_tag_blacklist = { ["no"] = true, ["private"] = true, ["agricultural"] = true, ["forestry"] = true }
access_tag_restricted = { ["destination"] = true, ["delivery"] = true }
access_tags_hierachy = { "foot", "access" }
service_tag_restricted = { ["parking_aisle"] = true }
ignore_in_grid = { ["ferry"] = true }
restriction_exception_tags = { "foot" }

walking_speed = 5

-- http://wiki.openstreetmap.org/wiki/Speed_limits
maxspeed_table_default = {
  ["urban"] = 50,
  ["rural"] = 90,
  ["trunk"] = 110,
  ["motorway"] = 130
}

-- List only exceptions
maxspeed_table = {
  ["ch:rural"] = 80,
  ["ch:trunk"] = 100,
  ["ch:motorway"] = 120,
  ["de:living_street"] = 7,
  ["ru:living_street"] = 20,
  ["ru:urban"] = 60,
  ["ua:urban"] = 60,
  ["at:rural"] = 100,
  ["de:rural"] = 100,
  ["at:trunk"] = 100,
  ["cz:trunk"] = 0,
  ["ro:trunk"] = 100,
  ["cz:motorway"] = 0,
  ["de:motorway"] = 0,
  ["ru:motorway"] = 110,
  ["gb:nsl_single"] = (60*1609)/1000,
  ["gb:nsl_dual"] = (70*1609)/1000,
  ["gb:motorway"] = (70*1609)/1000,
  ["uk:nsl_single"] = (60*1609)/1000,
  ["uk:nsl_dual"] = (70*1609)/1000,
  ["uk:motorway"] = (70*1609)/1000
}
--[===[
        safeHighwayTags.add("footway");
        safeHighwayTags.add("path");
        safeHighwayTags.add("steps");
        safeHighwayTags.add("pedestrian");
        safeHighwayTags.add("living_street");
        safeHighwayTags.add("track");
        safeHighwayTags.add("residential");
        safeHighwayTags.add("service");

        avoidHighwayTags.add("trunk");
        avoidHighwayTags.add("trunk_link");
        avoidHighwayTags.add("primary");
        avoidHighwayTags.add("primary_link");
        avoidHighwayTags.add("secondary");
        avoidHighwayTags.add("secondary_link");
        avoidHighwayTags.add("tertiary");
        avoidHighwayTags.add("tertiary_link");

        // for now no explicit avoiding #257
        //avoidHighwayTags.add("cycleway");
        allowedHighwayTags.addAll(safeHighwayTags);
        allowedHighwayTags.addAll(avoidHighwayTags);
        allowedHighwayTags.add("cycleway");
        allowedHighwayTags.add("unclassified");
        allowedHighwayTags.add("road");
--]===]

--[=[
void collect( OSMWay way, TreeMap<Double, Integer> weightToPrioMap )
  {
    String highway = way.getTag("highway");
if (way.hasTag("foot", "designated"))
weightToPrioMap.put(100d, PREFER.getValue());

double maxSpeed = getMaxSpeed(way);
if (safeHighwayTags.contains(highway) || maxSpeed > 0 && maxSpeed <= 20)
  {
    weightToPrioMap.put(40d, PREFER.getValue());
if (way.hasTag("tunnel", intendedValues))
  {
if (way.hasTag("sidewalk", "no"))
weightToPrioMap.put(40d, REACH_DEST.getValue());
else
  weightToPrioMap.put(40d, UNCHANGED.getValue());
  }
  } else if (maxSpeed > 50 || avoidHighwayTags.contains(highway))
  {
if (way.hasTag("sidewalk", "no"))
weightToPrioMap.put(45d, WORST.getValue());
else
  weightToPrioMap.put(45d, REACH_DEST.getValue());
  }

  if (way.hasTag("bicycle", "official") || way.hasTag("bicycle", "designated"))
  weightToPrioMap.put(44d, AVOID_IF_POSSIBLE.getValue());
  }
--]=]

speeds = {
  ["footway"]=walking_speed,
  ["path"]=walking_speed,
  ["steps"]=walking_speed,
  ["pedestrian"]=walking_speed,
  ["living_street"]=walking_speed,
  ["track"]=walking_speed,
  ["residential"]=walking_speed,
  ["service"]=walking_speed,
  ["cycleway"]=walking_speed*0.9,
  ["unclassified"]=walking_speed*0.9,
  ["road"]=walking_speed*0.9,
  ["trunk"]=walking_speed*0.7,
  ["trunk_link"]=walking_speed*0.7,
  ["primary"]=walking_speed*0.7,
  ["secondary"]=walking_speed*0.7,
  ["tertiary"]=walking_speed*0.7,
}

speed_profile = {
  ["motorway"] = 90,
  ["motorway_link"] = 45,
  ["trunk"] = 85,
  ["trunk_link"] = 40,
  ["primary"] = 65,
  ["primary_link"] = 30,
  ["secondary"] = 55,
  ["secondary_link"] = 25,
  ["tertiary"] = 40,
  ["tertiary_link"] = 20,
  ["unclassified"] = 25,
  ["residential"] = 25,
  ["living_street"] = 10,
  ["service"] = 15,
  --  ["track"] = 5,
  ["ferry"] = 5,
  ["movable"] = 5,
  ["shuttle_train"] = 10,
  ["default"] = 10
}

route_speeds = {
  ["ferry"] = 5
}

platform_speeds = {
  ["platform"] = walking_speed
}

amenity_speeds = {
  ["parking"] = walking_speed,
  ["parking_entrance"] = walking_speed
}

man_made_speeds = {
  ["pier"] = walking_speed
}

surface_speeds = {
  ["fine_gravel"] =   walking_speed*0.75,
  ["gravel"] =        walking_speed*0.75,
  ["pebblestone"] =   walking_speed*0.75,
  ["mud"] =           walking_speed*0.5,
  ["sand"] =          walking_speed*0.5
}

leisure_speeds = {
  ["track"] = walking_speed
}

traffic_signal_penalty   = 2
u_turn_penalty           = 2
use_turn_restrictions    = false
local fallback_names     = true

--modes
local mode_normal = 1
local mode_ferry = 2

function get_exceptions(vector)
  for i,v in ipairs(restriction_exception_tags) do
    vector:Add(v)
  end
end

local function parse_maxspeed(source)
  if not source then
    return 0
  end
  local n = tonumber(source:match("%d*"))
  if n then
    if string.match(source, "mph") or string.match(source, "mp/h") then
      n = (n*1609)/1000
    end
  else
    -- parse maxspeed like FR:urban
    source = string.lower(source)
    n = maxspeed_table[source]
    if not n then
      local highway_type = string.match(source, "%a%a:(%a+)")
      n = maxspeed_table_default[highway_type]
      if not n then
        n = 0
      end
    end
  end
  return n
end

function node_function (node, result)
  local barrier = node:get_value_by_key("barrier")
  local access = find_access_tag(node, access_tags_hierachy)
  local traffic_signal = node:get_value_by_key("highway")

  -- flag node if it carries a traffic light
  if traffic_signal and traffic_signal == "traffic_signals" then
    result.traffic_lights = true
  end

  -- parse access and barrier tags
  if access and access ~= "" then
    if access_tag_blacklist[access] then
      result.barrier = true
    else
      result.barrier = false
    end
  elseif barrier and barrier ~= "" then
    if barrier_whitelist[barrier] then
      result.barrier = false
    else
      result.barrier = true
    end
  end

  return 1
end

function way_function (way, result)
  -- initial routability check, filters out buildings, boundaries, etc
  local highway = way:get_value_by_key("highway")
  local leisure = way:get_value_by_key("leisure")
  local route = way:get_value_by_key("route")
  local man_made = way:get_value_by_key("man_made")
  local railway = way:get_value_by_key("railway")
  local amenity = way:get_value_by_key("amenity")
  local public_transport = way:get_value_by_key("public_transport")
  local tunnel = way:get_value_by_key("tunnel")
  local sidewalk = way:get_value_by_key("sidewalk")
  local bicycle =  way:get_value_by_key("bicycle")

  if (not highway or highway == '') and
    (not leisure or leisure == '') and
    (not route or route == '') and
    (not railway or railway=='') and
    (not amenity or amenity=='') and
    (not man_made or man_made=='') and
    (not public_transport or public_transport=='')
    then
    return
  end

  local max_speed = parse_maxspeed( way:get_value_by_key("maxspeed") )

  --if (max_speed and max_speed ~= '' and highway and (highway == "secondary" or highway == "primary" )) then
  --  print(max_speed)
  --  return
  --end
  -- don't route on ways that are still under construction
  if highway=='construction' then
      return
  end

  -- access
  local access = find_access_tag(way, access_tags_hierachy)
  if access_tag_blacklist[access] then
    return
  end

  local name = way:get_value_by_key("name")
  local ref = way:get_value_by_key("ref")
  local junction = way:get_value_by_key("junction")
  local onewayClass = way:get_value_by_key("oneway:foot")
  local duration  = way:get_value_by_key("duration")
  local service  = way:get_value_by_key("service")
  local area = way:get_value_by_key("area")
  local foot = way:get_value_by_key("foot")
  local surface = way:get_value_by_key("surface")

   -- name
  if ref and "" ~= ref and name and "" ~= name then
    result.name = name .. ' / ' .. ref
    elseif ref and "" ~= ref then
      result.name = ref
  elseif name and "" ~= name then
    result.name = name
  elseif highway and fallback_names then
    result.name = "{highway:"..highway.."}"  -- if no name exists, use way type
                                            -- this encoding scheme is excepted to be a temporary solution
  end

    -- roundabouts
  if "roundabout" == junction then
    result.roundabout = true
  end

    -- speed
  if route_speeds[route] then
    -- ferries (doesn't cover routes tagged using relations)
    result.ignore_in_grid = true
  if duration and durationIsValid(duration) then
    result.duration = math.max( 1, parseDuration(duration) )
  else
    result.forward_speed = route_speeds[route]
    result.backward_speed = route_speeds[route]
  end
    result.forward_mode = mode_ferry
    result.backward_mode = mode_ferry
  elseif railway and platform_speeds[railway] then
    -- railway platforms (old tagging scheme)
    result.forward_speed = platform_speeds[railway]
    result.backward_speed = platform_speeds[railway]
  elseif platform_speeds[public_transport] then
    -- public_transport platforms (new tagging platform)
    result.forward_speed = platform_speeds[public_transport]
    result.backward_speed = platform_speeds[public_transport]
  elseif amenity and amenity_speeds[amenity] then
    -- parking areas
    result.forward_speed = amenity_speeds[amenity]
    result.backward_speed = amenity_speeds[amenity]
  elseif leisure and leisure_speeds[leisure] then
    -- running tracks
    result.forward_speed = leisure_speeds[leisure]
    result.backward_speed = leisure_speeds[leisure]
  elseif speeds[highway] then
    -- regular ways
    result.forward_speed = speeds[highway]
    result.backward_speed = speeds[highway]
  elseif access and access_tag_whitelist[access] then
      -- unknown way, but valid access tag
    result.forward_speed = walking_speed
    result.backward_speed = walking_speed
  end

  -- oneway
  if onewayClass == "yes" or onewayClass == "1" or onewayClass == "true" then
    result.backward_mode = 0
  elseif onewayClass == "no" or onewayClass == "0" or onewayClass == "false" then
    -- nothing to do
  elseif onewayClass == "-1" then
    result.forward_mode = 0
  end

  -- surfaces
  if surface then
    surface_speed = surface_speeds[surface]
    if surface_speed then
      result.forward_speed = math.min(result.forward_speed, surface_speed)
      result.backward_speed  = math.min(result.backward_speed, surface_speed)
    end
  end


  if max_speed > 0 and max_speed < 20 then
    result.forward_speed = result.backward_speed * 1.1
    result.backward_speed  = result.backward_speed * 1.1
  elseif tunnel and tunnel == "yes" and sidewalk and sidewalk == "no" then
    result.forward_speed = result.backward_speed * 0.8
    result.backward_speed  = result.backward_speed * 0.8

  elseif max_speed > 50 then
    if sidewalk and sidewalk == "no" then
      result.forward_speed = result.backward_speed * 0.6
      result.backward_speed  = result.backward_speed * 0.6
    else
      result.forward_speed = result.backward_speed * 0.8
      result.backward_speed  = result.backward_speed * 0.8
    end
  elseif bicycle and (bicycle == "official" or bicycle == "designated") then
    result.forward_speed = result.backward_speed * 0.8
    result.backward_speed  = result.backward_speed * 0.8
  end
end
