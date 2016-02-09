/*! \file    Graph.hpp
    \brief   Interface to a graph template.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iterator>
#include <vector>

namespace spica {

    //! Weighted, directed graphs.
    /*!
     * This template contains an adjacancy list representation of a weighted, directed graph.
     * The type used for weights is a template parameter. If an unweighted graph is needed, use
     * a dummy type, such as int, for the weights and otherwise ignore them. If an undirected
     * graph is needed, you will have to insert two directed edges to simulate a single
     * undirected edge.
     *
     * Verticies are numbered from 0 to some maximum value. All vertex numbers in that range are
     * assumed to correspond to actual verticies in the graph; the vertex numbering can't have
     * any "holes." Both verticies and edges are counted with an unsigned integral type named
     * 'count_t.'
     */
    template< typename Weight >
    class Graph {
    public:
        typedef unsigned int count_t;
        static const count_t nil = static_cast< count_t >( -1 );

        //! Stores information about a single edge in the graph.
        /*!
         * This structure characterizes a graph edge. The source vertex for the edge is
         * implicit. The destination vertex and weight on the edge are defined here. Note that
         * in an unweighted graph, the edge's weight should be ignored.
         */
        struct EdgeInfo {
            count_t  remote_vertex;
            Weight   edge_weight;
        };

    private:
        std::vector< std::vector< EdgeInfo > > adjacency;

    public:

        //! Provides a way to visit all the edges leaving a vertex.
        /*!
         * Edge iterators are models of forward iterators. They allow you to iterate over all
         * the edges leaving a particular vertex and examine the edge information for those
         * edges.
         */
        class edge_iterator :
            public std::iterator< std::forward_iterator_tag, EdgeInfo > {
        public:
            edge_iterator( ) : current( ) { }
            edge_iterator( typename std::vector< EdgeInfo >::iterator p )
                : current( p ) { }

            EdgeInfo      &operator*( )  { return( *current ); }
            EdgeInfo      *operator->( ) { return( &*current ); }
            edge_iterator &operator++( ) { ++current; return( *this ); }
            edge_iterator  operator++( int )
            {
                typename std::vector< EdgeInfo >::iterator temp( current );
                ++current;
                return( edge_iterator( temp ) );
            }

            bool operator==( const edge_iterator &other )
                { return( current == other.current ); }

            bool operator!=( const edge_iterator &other )
                { return( current != other.current ); }

        private:
            typename std::vector< EdgeInfo >::iterator current;
        };

        //! Return the number of verticies in the graph.
        count_t num_verticies( ) const;

        //! Return the number of directed edges in the graph.
        count_t num_edges( ) const;

        //! Creates a new vertex.
        /*!
         *  \param v_number The vertex number to create. If the vertex already exists, there is
         *  no effect. Creating a particular vertex, v, also creates verticies 0..v-1 if they
         *  don't already exist.
         *
         *  \exception std::bad_alloc if memory exhausted.
         */
        void create_vertex( count_t v_number );

        //! Creates a new edge
        /*!
         * If either v1 or v2 do not exist, they are created as if by calling create_vertex.
         *
         * \param v1 The source vertex for the edge.
         * \param v2 The destination vertex for the edge.
         * \param w  The weight on the edge.
         *
         * \exception std::bad_alloc if memory exhausted.
         */
        void create_edge( count_t v1, count_t v2, Weight w );

        //! Start an edge iteration.
        /*!
         * The edges of a vertex are return in an unspecified order. Specific- ally it is not
         * gaurenteed that the edges will be returned in the same order in which they were
         * created using create_edge.
         *
         * \param v_number The vertex for whose outgoing edges we wish to examine.
         *
         * \return An edge_iterator that points at the first edge leaving vertex v_number. If
         * there is no such edge, eend(v_number) is returned.
         *
         */
        edge_iterator ebegin( count_t v_number );

        //! Stop an edge iteration.
        /*!
         * \param v_number The vertex for whose outgoing edges we wish to examine.
         *
         * \return An edge_iterator that points just past the last edge leaving vertex v_number.
         */
        edge_iterator eend( count_t v_number );
    };


    template< typename Weight >
    inline
    typename Graph< Weight >::count_t Graph< Weight >::num_verticies( ) const
    {
        return adjacency.size( );
    }


    template< typename Weight >
    typename Graph< Weight >::count_t Graph< Weight >::num_edges( ) const
    {
        count_t total = 0;
        for( count_t i = 0; i < adjacency.size( ); ++i ) {
            total += adjacency[i].size( );
        }
        return( total );
    }


    template< typename Weight >
    void Graph< Weight >::create_vertex( count_t v_number )
    {
        if( v_number < adjacency.size( ) ) return;
        adjacency.resize( v_number + 1 );
    }


    template< typename Weight >
    void Graph< Weight >::create_edge( count_t v1, count_t v2, Weight w )
    {
        create_vertex( v1 );
        create_vertex( v2 );
        EdgeInfo new_edge = { v2, w };
        adjacency[v1].push_back( new_edge );
    }


    template< typename Weight >
    inline
    typename Graph< Weight >::edge_iterator Graph< Weight >::ebegin( count_t v_number )
    {
        return( edge_iterator( adjacency[v_number].begin( ) ) );
    }


    template< typename Weight >
    inline
    typename Graph< Weight >::edge_iterator Graph< Weight >::eend( count_t v_number )
    {
        return( edge_iterator( adjacency[v_number].end( ) ) );
    }
}

#endif
