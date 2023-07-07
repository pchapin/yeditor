/*! \file    command.hpp
 *  \brief   Prototypes of all the command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This file contains the prototypes of all the command functions. One and only one of these
 * functions is associated with each macro word.
 *
 * Notice that all command functions return bool. The idea is that a command function returns
 * true if it works and false if it does not.
*/

#ifndef COMMAND_HPP
#define COMMAND_HPP

extern bool add_text_command( );
extern bool background_color_command( );
extern bool backspace_command( );
extern bool block_off_command( );
extern bool copy_block_command( );
extern bool CP_down_command( );
extern bool CP_left_command( );
extern bool CP_right_command( );
extern bool CP_up_command( );
extern bool define_key_command( );
extern bool delete_command( );
extern bool delete_EOL_command( );
extern bool delete_SOL_command( );
extern bool delete_block_command( );
extern bool editor_info_command( );
extern bool error_message_command( );
extern bool execute_file_command( );
extern bool execute_macro_command( );
extern bool exit_command( );
extern bool external_command_command( );
extern bool filelist_info_command( );
extern bool file_info_command( );
extern bool file_insert_command( );
extern bool filter_command( );
extern bool find_file_command( );
extern bool foreground_color_command( );
extern bool goto_column_command( );
extern bool goto_file_end_command( );
extern bool goto_file_start_command( );
extern bool goto_line_command( );
extern bool goto_line_end_command( );
extern bool goto_line_start_command( );
extern bool help_command( );
extern bool input_command( );
extern bool insert_command( );
extern bool insert_file_command( );
extern bool kill_file_command( );
extern bool legal_info_command( );
extern bool new_line_command( );
extern bool next_file_command( );
extern bool next_procedure_command( );
extern bool page_down_command( );
extern bool page_up_command( );
extern bool pan_left_command( );
extern bool pan_right_command( );
extern bool paste_block_command( );
extern bool previous_file_command( );
extern bool previous_procedure_command( );
extern bool quit_command( );
extern bool redirect_from_command( );
extern bool redirect_to_command( );
extern bool reformat_command( );
extern bool refresh_file_command( );
extern bool remove_file_command( );
extern bool rename_file_command( );
extern bool restricted_mode_command( );
extern bool save_file_command( );
extern bool search_and_replace_command( );
extern bool search_first_command( );
extern bool search_next_command( );
extern bool set_bookmark_command( );
extern bool set_tab_command( );
extern bool skip_left_command( );
extern bool skip_right_command( );
extern bool tab_command( );
extern bool toggle_block_command( );
extern bool toggle_bookmark_command( );
extern bool yexit_command( );

// Experimental commands and "draft" commands.

extern bool drop_command( );
extern bool dup_command( );
extern bool xchg_command( );
extern bool getch_command( );

#endif
