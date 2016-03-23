@routing  @guidance
Feature: Basic Roundabout

    Background:
        Given the profile "testbot"
        Given a grid size of 10 meters

    Scenario: Ramp Exit Right
        Given the node map
            | a | b | c | d | e |
            |   |   |   | f | g |

        And the ways
            | nodes  | highway       |
            | abcde  | motorway      |
            | bfg    | motorway_link |

       When I route I should get
            | waypoints | route           | turns                             |
            | a,e       | abcde, abcde    | depart, arrive                    |
            | a,g       | abcde, bfg, bfg | depart, ramp-slight-right, arrive |

