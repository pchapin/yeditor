/*! \file    ListEntity.cpp
 *  \brief   Implementation of the clac numeric type ListEntity.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include <memory>
#include "Entities.hpp"

ListEntity::~ListEntity( )
{
    std::list< Entity * >::iterator p;
    for( p = value.begin( ); p != value.end( ); ++p ) {
        delete *p;
    }
}

EntityType ListEntity::my_type( ) const
{
    return LIST;
}

std::string ListEntity::display( ) const
{
    std::string workspace = "{ ";
    std::list< Entity * >::const_iterator p;

    for( p = value.begin( ); p != value.end( ); ++p ) {
        workspace.append( ( *p )->display( ) );
        workspace.append( " " );
    }

    workspace.append( "}" );
    return workspace;
}


Entity *ListEntity::duplicate( ) const
{
    std::list< Entity * > new_value;
    std::list< Entity * >::const_iterator p;

    for( p = value.begin( ); p != value.end( ); ++p ) {

        // If (*p)->duplicate() throws, new_value will be destroyed but the already duplicated
        // objects won't be. This must be fixed eventually.
        // 
        new_value.push_back( ( *p )->duplicate( ) );
    }

    // This involves copying the list (of pointers) which is time consuming. There is also a
    // problem here if creating the new ListEntity throws.
    //
    return new ListEntity( new_value );
}


Entity *ListEntity::plus( const Entity *R ) const
{
    const ListEntity *right = dynamic_cast< const ListEntity * >( R );
    std::list< Entity * >::const_iterator p;

    std::auto_ptr< ListEntity > new_list( new ListEntity( value ) );
    for( p = right->value.begin( ); p != right->value.end( ); ++p ) {
        new_list->value.push_back( *p );
    }

    return new_list.release( );
}
