@routing  @guidance
Feature: New-Name Instructions

    Background:
        Given the profile "testbot"
        Given a grid size of 10 meters

    Scenario: Undisturbed name Change
        Given the node map
            | a |   | b |   | c |

        And the ways
            | nodes  |
            | ab     |
            | bc     |

       When I route I should get
            | waypoints | route      | turns                             |
            | a,c       | ab, bc, bc | depart, new-name-straight, arrive |


    Scenario: Undisturbed Name Change with unannounced Turn Right
        Given the node map
            | a |   | b |   |   |
            |   |   |   |   | c |

        And the ways
            | nodes  |
            | ab     |
            | bc     |

       When I route I should get
            | waypoints | route      | turns                             |
            | a,c       | ab, bc, bc | depart, new-name-straight, arrive |

    Scenario: Undisturbed Name Change with unannounced Turn Left
        Given the node map
            |   |   |   |   | c |
            | a |   | b |   |   |

        And the ways
            | nodes  |
            | ab     |
            | bc     |

       When I route I should get
            | waypoints | route      | turns                             |
            | a,c       | ab, bc, bc | depart, new-name-straight, arrive |

    Scenario: Disturbed Name Change with Turn
        Given the node map
            | a |   | b |   |   |
            |   | d |   |   | c |

        And the ways
            | nodes  |
            | ab     |
            | bc     |
            | db     |

       When I route I should get
            | waypoints | route      | turns                                 |
            | a,c       | ab, bc, bc | depart, new-name-slight-right, arrive |

    Scenario: Undisturbed Name Change with announced Turn Left
        Given the node map
            |   |   | c |
            | a |   | b |

        And the ways
            | nodes  |
            | ab     |
            | bc     |

       When I route I should get
            | waypoints | route      | turns                         |
            | a,c       | ab, bc, bc | depart, new-name-left, arrive |

    Scenario: Undisturbed Name Change with announced Turn Sharp Left
        Given the node map
            | c |   |   |
            | a |   | b |

        And the ways
            | nodes  |
            | ab     |
            | bc     |

       When I route I should get
            | waypoints | route      | turns                               |
            | a,c       | ab, bc, bc | depart, new-name-sharp-left, arrive |

    Scenario: Undisturbed Name Change with announced Turn Right
        Given the node map
            | a |   | b |
            |   |   | c |

        And the ways
            | nodes  |
            | ab     |
            | bc     |

       When I route I should get
            | waypoints | route      | turns                          |
            | a,c       | ab, bc, bc | depart, new-name-right, arrive |

    Scenario: Undisturbed Name Change with announced Turn Sharp Right
        Given the node map
            | a |   | b |
            | c |   |   |

        And the ways
            | nodes  |
            | ab     |
            | bc     |

       When I route I should get
            | waypoints | route      | turns                                |
            | a,c       | ab, bc, bc | depart, new-name-sharp-right, arrive |


    Scenario: Disturbed Name Change with minor road class
        Given the node map
            | a |   | b |   | d |
            |   |   |   |   | c |

        And the ways
            | nodes  | highway     |
            | ab     | residential |
            | bc     | residential |
            | bd     | service     |

       When I route I should get
            | waypoints | route      | turns                                 |
            | a,c       | ab, bc, bc | depart, new-name-slight-right, arrive |
