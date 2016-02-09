/*! \file    actions.hpp
    \brief   This file contains prototypes of functions that implement the action words.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include "cstack.hpp"

extern void do_bin         ( Stack & );
extern void do_clear       ( Stack & );
extern void do_dec         ( Stack & );
extern void do_deg         ( Stack & );
extern void do_drop        ( Stack & );
extern void do_dropn       ( Stack & );
extern void do_dup         ( Stack & );
extern void do_dupn        ( Stack & );
extern void do_fix         ( Stack & );
extern void do_eng         ( Stack & );
extern void do_eval        ( Stack & );
extern void do_grad        ( Stack & );
extern void do_hex         ( Stack & );
extern void do_oct         ( Stack & );
extern void do_polar       ( Stack & );
extern void do_purge       ( Stack & );
extern void do_rad         ( Stack & );
extern void do_read        ( Stack & );
extern void do_rec         ( Stack & );
extern void do_roll_down   ( Stack & );
extern void do_roll_up     ( Stack & );
extern void do_rot         ( Stack & );
extern void do_run         ( Stack & );
extern void do_sci         ( Stack & );
extern void do_store       ( Stack & );
extern void do_stws        ( Stack & );
extern void do_swap        ( Stack & );
// extern void do_sys         ( Stack & );
extern void do_write       ( Stack & );

extern void do_shift_left  ( Stack & );
extern void do_shift_right ( Stack & );
// extern void do_ashift_right( Stack & );

extern void do_off       ( Stack & );

#endif
