/*! \file    key_test.cpp
    \brief   This file contains test code for the keyboard handling library.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include "check.hpp"
#include "scr.hpp"

using namespace scr;

struct KeyInfo {
    int   code;
    const char *name;
};

static KeyInfo key_data[] = {
  { K_F1,     "F1"            },
  { K_F2,     "F2"            },
  { K_F3,     "F3"            },
  { K_F4,     "F4"            },
  { K_F5,     "F5"            },
  { K_F6,     "F6"            },
  { K_F7,     "F7"            },
  { K_F8,     "F8"            },
  { K_F9,     "F9"            },
  { K_F10,    "F10"           },

  { K_SF1,    "Shift + F1"    },
  { K_SF2,    "Shift + F2"    },
  { K_SF3,    "Shift + F3"    },
  { K_SF4,    "Shift + F4"    },
  { K_SF5,    "Shift + F5"    },
  { K_SF6,    "Shift + F6"    },
  { K_SF7,    "Shift + F7"    },
  { K_SF8,    "Shift + F8"    },
  { K_SF9,    "Shift + F9"    },
  { K_SF10,   "Shift + F10"   },

  { K_CF1,    "Ctrl + F1"     },
  { K_CF2,    "Ctrl + F2"     },
  { K_CF3,    "Ctrl + F3"     },
  { K_CF4,    "Ctrl + F4"     },
  { K_CF5,    "Ctrl + F5"     },
  { K_CF6,    "Ctrl + F6"     },
  { K_CF7,    "Ctrl + F7"     },
  { K_CF8,    "Ctrl + F8"     },
  { K_CF9,    "Ctrl + F9"     },
  { K_CF10,   "Ctrl + F10"    },

  { K_AF1,    "Alt + F1"      },
  { K_AF2,    "Alt + F2"      },
  { K_AF3,    "Alt + F3"      },
  { K_AF4,    "Alt + F4"      },
  { K_AF5,    "Alt + F5"      },
  { K_AF6,    "Alt + F6"      },
  { K_AF7,    "Alt + F7"      },
  { K_AF8,    "Alt + F8"      },
  { K_AF9,    "Alt + F9"      },
  { K_AF10,   "Alt + F10"     },

  { K_HOME,   "Home"          },
  { K_END,    "End"           },
  { K_PGUP,   "PgUp"          },
  { K_PGDN,   "PgDn"          },
  { K_LEFT,   "Left"          },
  { K_RIGHT,  "Right"         },
  { K_UP,     "Up"            },
  { K_DOWN,   "Down"          },
  { K_INS,    "Insert"        },
  { K_DEL,    "Delete"        },

  { K_CHOME,  "Ctrl + Home"   },
  { K_CEND,   "Ctrl + End"    },
  { K_CPGUP,  "Ctrl + PgUp"   },
  { K_CPGDN,  "Ctrl + PgDn"   },
  { K_CLEFT,  "Ctrl + Left"   },
  { K_CRIGHT, "Ctrl + Right"  },
  { K_CUP,    "Ctrl + Up"     },
  { K_CDOWN,  "Ctrl + Down"   },
  { K_CINS,   "Ctrl + Insert" },
  { K_CDEL,   "Ctrl + Delete" },

  { K_ALTA,   "Alt + A"       },
  { K_ALTB,   "Alt + B"       },
  { K_ALTC,   "Alt + C"       },
  { K_ALTD,   "Alt + D"       },
  { K_ALTE,   "Alt + E"       },
  { K_ALTF,   "Alt + F"       },
  { K_ALTG,   "Alt + G"       },
  { K_ALTH,   "Alt + H"       },
  { K_ALTI,   "Alt + I"       },
  { K_ALTJ,   "Alt + J"       },
  { K_ALTK,   "Alt + K"       },
  { K_ALTL,   "Alt + L"       },
  { K_ALTM,   "Alt + M"       },
  { K_ALTN,   "Alt + N"       },
  { K_ALTO,   "Alt + O"       },
  { K_ALTP,   "Alt + P"       },
  { K_ALTQ,   "Alt + Q"       },
  { K_ALTR,   "Alt + R"       },
  { K_ALTS,   "Alt + S"       },
  { K_ALTT,   "Alt + T"       },
  { K_ALTU,   "Alt + U"       },
  { K_ALTV,   "Alt + V"       },
  { K_ALTW,   "Alt + W"       },
  { K_ALTX,   "Alt + X"       },
  { K_ALTY,   "Alt + Y"       },
  { K_ALTZ,   "Alt + Z"       },

  { K_ALT0,   "Alt + 0"       },
  { K_ALT1,   "Alt + 1"       },
  { K_ALT2,   "Alt + 2"       },
  { K_ALT3,   "Alt + 3"       },
  { K_ALT4,   "Alt + 4"       },
  { K_ALT5,   "Alt + 5"       },
  { K_ALT6,   "Alt + 6"       },
  { K_ALT7,   "Alt + 7"       },
  { K_ALT8,   "Alt + 8"       },
  { K_ALT9,   "Alt + 9"       },

  { 0,        "null"          }
};

static const char *lookup_name( int code )
{
    KeyInfo *p = key_data;
    while( p->code != 0 ) {
        if( p->code == code ) return( p->name );
        ++p;
    }
    return( "Key description not found" );
}


void key_test( )
{
    int key_code;
    int key_count = 0;

    CLEAR_SCREEN;
    HOME_CURSOR;

    refresh( );
    while( ( key_code = key( ) ) != K_ESC ) {
        ++key_count;
        clear( 1, 1, 80, 1, WHITE|REV_BLACK );
        if( key_code >= XF ) {
            print_text(1, 1, 80, "Count = %d, Code = XF + %d, Key = %s", key_count, key_code - XF, lookup_name( key_code ) );
        }
        else {
            print_text( 1, 1, 80, "Count = %d, Code = %d, Key = %c", key_count, key_code, key_code );
        }
        refresh( );
    }
}

