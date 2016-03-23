@routing  @guidance
Feature: Basic Roundabout

    Background:
        Given the profile "testbot"
        Given a grid size of 10 meters

    Scenario: Fork Same Road Class
        Given the node map
            |   |   |   |   | c |
            | a |   | b |   |   |
            |   |   |   |   | d |

        And the ways
            | nodes  | highway |
            | ab     | primary |
            | bc     | primary |
            | bd     | primary |


       When I route I should get
            | waypoints | route      | turns                      |
            | a,d       | ab, bc, bc | depart, fork-left, arrive  |
            | a,d       | ab, bd, bd | depart, fork-right, arrive |

    Scenario: Do not fork on link type
        Given the node map
            |   |   |   |   | c |
            | a |   | b |   |   |
            |   |   |   |   | d |

        And the ways
            | nodes  | highway      |
            | abc    | primary      |
            | bd     | primary_link |


       When I route I should get
            | waypoints | route   | turns                             |
            | a,d       | abc     | depart, arrive                    |
            | a,d       | abc, bd | depart, turn-slight-right, arrive |

    Scenario: Fork in presence of other roads
        Given the node map
            |   |   |   |   | c |
            | a |   | b |   |   |
            |   | e |   |   | d |

        And the ways
            | nodes  | highway |
            | ab     | primary |
            | bc     | primary |
            | bd     | primary |
            | eb     | primary |

       When I route I should get
            | waypoints | route      | turns                      |
            | a,d       | ab, bc, bc | depart, fork-left, arrive  |
            | a,d       | ab, bd, bd | depart, fork-right, arrive |

    Scenario: Fork Turning Slight Left
        Given the node map
            |   |   |   |   | c |
            |   |   |   |   |   |
            | a |   | b |   | d |

        And the ways
            | nodes  | highway |
            | ab     | primary |
            | bc     | primary |
            | bd     | primary |


       When I route I should get
            | waypoints | route      | turns                      |
            | a,d       | ab, bc, bc | depart, fork-left, arrive  |
            | a,d       | ab, bd, bd | depart, fork-right, arrive |

    Scenario: Fork Turning Slight Right
        Given the node map
            | a |   | b |   | c |
            |   |   |   |   |   |
            |   |   |   |   | d |

        And the ways
            | nodes  | highway |
            | ab     | primary |
            | bc     | primary |
            | bd     | primary |


       When I route I should get
            | waypoints | route      | turns                      |
            | a,d       | ab, bc, bc | depart, fork-left, arrive  |
            | a,d       | ab, bd, bd | depart, fork-right, arrive |
