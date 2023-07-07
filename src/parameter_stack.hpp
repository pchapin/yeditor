/*! \file    parameter_stack.hpp
 *  \brief   Interface to the parameter stack.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef PARAMETER_STACK_HPP
#define PARAMETER_STACK_HPP

#include "EditBuffer.hpp"
#include "EditList.hpp"
#include "mystack.hpp"

class Parameter {
private:
    EditList    input_data;     // Parameter data.
    const char *prompt_string;  // Points at prompt. Leading and trailing space added.

    int edit( EditBuffer & );

public:
    //! Constructor sets the prompt string.
    explicit Parameter( const char *prompt_string );

    int get( bool pop = true );   // Read a parameter. Return YES if no abort.
    std::string value( );         // Returns the most recent parameter.
};

extern Stack<EditBuffer> parameter_stack;

#endif
