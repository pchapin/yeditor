/*! \file    command_table.cpp
 *  \brief   Dispatch table for the commands.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This file contains declarations of the macro word dispatch table. The entries in this table
 * associate a macro command (as from get_word()) with a command function. The command functions
 * implement the desired commands, in many cases by simply calling an appropriate method for the
 * active YEditFile object. In many other cases, the command functions do significant
 * preliminary work.
 *
 * There is a design trade-off in deciding how much work the methods of YEditFile should do and
 * how much should be done by the command functions. As a rule of thumb, YEditFile methods
 * should do work that is generic and appropriate for a large number of editing tasks. Certainly
 * the command functions must do any work that requires knowledge of many YEditFile objects,
 * since YEditFile objects do not know about each other. However, the balance of work between
 * command functions and YEditFile methods is partly a matter of preference; it will always be a
 * matter of adjustment.
 */

#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "command.hpp"
#include "command_table.hpp"
#include "parameter_stack.hpp"
#include "support.hpp"

struct DispatchTableEntry {
    const char *macro_word;
    bool ( *command_function )( );
};


static DispatchTableEntry command_table[] = {
    { "add_text",           add_text_command           },
    { "background_color",   background_color_command   },
    { "backspace",          backspace_command          },
    { "block_off",          block_off_command          },
    { "copy",               copy_block_command         },
    { "cursor_down",        CP_down_command            },
    { "cursor_left",        CP_left_command            },
    { "cursor_right",       CP_right_command           },
    { "cursor_up",          CP_up_command              },
    { "cut",                delete_block_command       },
    { "define_key",         define_key_command         },
    { "delete",             delete_command             },
    { "delete_to_eol",      delete_EOL_command         },
    { "delete_to_sol",      delete_SOL_command         },
    { "editor_info",        editor_info_command        },
    { "end_of_file",        goto_file_end_command      },
    { "end_of_line",        goto_line_end_command      },
    { "error_message",      error_message_command      },
    { "execute_file",       execute_file_command       },
    { "execute_macro",      execute_macro_command      },
    { "exit",               exit_command               },
    { "external_command",   external_command_command   },
    { "external_filter",    filter_command             },
    { "filelist_info",      filelist_info_command      },
    { "file_info",          file_info_command          },
    { "file_insert",        file_insert_command        },
    { "find_file",          find_file_command          },
    { "foreground_color",   foreground_color_command   },
    { "goto_column",        goto_column_command        },
    { "goto_line",          goto_line_command          },
    { "help",               help_command               },
    { "input",              input_command              },
    { "insert_file",        insert_file_command        },
    { "kill_file",          kill_file_command          },
    { "legal_info",         legal_info_command         },
    { "new_line",           new_line_command           },
    { "next_file",          next_file_command          },
    { "next_procedure",     next_procedure_command     },
    { "page_down",          page_down_command          },
    { "page_up",            page_up_command            },
    { "paste",              paste_block_command        },
    { "previous_file",      previous_file_command      },
    { "previous_procedure", previous_procedure_command },
    { "quit",               quit_command               },
    { "redirect_from",      redirect_from_command      },
    { "redirect_to",        redirect_to_command        },
    { "reformat_paragraph", reformat_command           },
    { "refresh_file",       refresh_file_command       },
    { "remove_file",        remove_file_command        },
    { "rename_file",        rename_file_command        },
    { "restricted_mode",    restricted_mode_command    },
    { "save_file",          save_file_command          },
    { "search_first",       search_first_command       },
    { "search_next",        search_next_command        },
    { "search_replace",     search_and_replace_command },
    { "set_mark",           set_bookmark_command       },
    { "set_tab",            set_tab_command            },
    { "start_of_line",      goto_line_start_command    },
    { "tab",                tab_command                },
    { "toggle_block",       toggle_block_command       },
    { "toggle_mark",        toggle_bookmark_command    },
    { "toggle_replace",     insert_command             },
    { "top_of_file",        goto_file_start_command    },
    { "word_left",          skip_left_command          },
    { "word_right",         skip_right_command         },
    { "yexit",              yexit_command              },

    // Parameter stack commands for the macro language.
    { "drop",               drop_command               },
    { "dup",                dup_command                },
    { "xchg",               xchg_command               },

    // Experimental for the moment.
    { "getch",              getch_command              },

    // Special marker at end.
    { NULL,                 NULL                  }
};


//! Scan the command table looking for index of the specified macro word.
/*!
 * Returns -1 if the word cannot be found. This function does a simple linear search of the
 * table.
 */
static int scan_table( const std::string &word )
{
    // Start at the beginning.
    int index = 0;

    // While there is more to the table, return the current index if this is right entry.
    while( command_table[index].macro_word != NULL ) {
        if( std::strcmp( command_table[index].macro_word, word.c_str( ) ) == 0 ) return index;
        index++;
    }

    // Got here? Must never have seen a match.
    return -1;
  }


//! Performs actions corresponding to the specified word of macro text.
/*!
 * If the word is a quoted string, it pushes the word onto the parameter stack. Otherwise it
 * looks up the word in the dispatch table and executes the appropriate function.
 */
void handle_word( const EditBuffer &word )
{
    int table_index;

    // Search the dispatch table.
    if( ( table_index = scan_table( word.to_string( ) ) ) != -1 ) {
        // TODO: Do something with the bool return value from the command function!
        command_table[table_index].command_function( );
    }

    // Otherwise, we don't know what it is. Treat it like a string.
    else {
        parameter_stack.push( word );
    }
}
