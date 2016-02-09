/*! \file    graph_tests.cpp
    \brief   Exercise Spica's C++ graph handling.
    \author  Peter C. Chapin <pcc482719@gmail.com>
*/

#include <iostream>
#include <spica.hpp>
#include "u_tests.hpp"
#include "UnitTestManager.hpp"

using namespace spica;

bool Graph_tests( )
{
    Graph<double> my_graph;

    UNIT_CHECK( my_graph.num_verticies( ) == 0 );
    UNIT_CHECK( my_graph.num_edges( )     == 0 );

    my_graph.create_edge( 0, 1, 1.23 );
    my_graph.create_edge( 1, 2, 4.56 );
    my_graph.create_edge( 0, 2, 7.89 );
    my_graph.create_edge( 3, 3, 0.12 );
    my_graph.create_vertex( 4 );

    UNIT_CHECK( my_graph.num_verticies( ) == 5 );
    UNIT_CHECK( my_graph.num_edges( )     == 4 );

    Graph<double>::EdgeInfo expected_results[] = {
        { 1, 1.23 },
        { 2, 7.89 },
        { 2, 4.56 },
        { 3, 0.12 },
        { 0, 0.00 }
    };
    Graph<double>::EdgeInfo *expected = expected_results;

    for(Graph<double>::count_t i = 0; i < my_graph.num_verticies( ); ++i ) {
        Graph<double>::edge_iterator p1, p2;
        p1 = my_graph.ebegin( i );
        p2 = my_graph.eend( i );
        while( p1 != p2 ) {
            UNIT_CHECK( p1->remote_vertex == expected->remote_vertex ||
                        p1->edge_weight == expected->edge_weight );
            ++p1;
            ++expected;
        }
    }
    return true;
}

