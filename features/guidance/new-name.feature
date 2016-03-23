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
            | waypoints | route      | turns                              |
            | a,c       | ab, bc, bc | depart, new-name-straight, arrive  |


    Scenario: Undisturbed Name Change with unannounced Turn Right
        Given the node map
            | a |   | b |   |   |
            |   |   |   |   | c |

        And the ways
            | nodes  |
            | ab     |
            | bc     |

       When I route I should get
            | waypoints | route      | turns                              |
            | a,c       | ab, bc, bc | depart, new-name-straight, arrive  |

    Scenario: Undisturbed Name Change with unannounced Turn Left
        Given the node map
            |   |   |   |   | c |
            | a |   | b |   |   |

        And the ways
            | nodes  |
            | ab     |
            | bc     |

       When I route I should get
            | waypoints | route      | turns                              |
            | a,c       | ab, bc, bc | depart, new-name-straight, arrive  |

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
            | waypoints | route      | turns                                  |
            | a,c       | ab, bc, bc | depart, new-name-slight-right, arrive  |
