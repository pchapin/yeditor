pragma SPARK_Mode(On);

package Screen is

   -- Largest supported screen as measured in character cells positions.
   Maximum_Screen_Size : constant := 2048;

   -- Types for describing a location on the screen.
   type Row_Type is range 1 .. Maximum_Screen_Size;
   type Column_Type is range 1 .. Maximum_Screen_Size;

   -- Types for describing the dimensions of a rectangular screen region.
   type Height_Type is range 1 .. Maximum_Screen_Size;
   type Width_Type is range 1 .. Maximum_Screen_Size;

   -- Encapsulates location information for a spot on the screen.
   type Point_Type is
      record
         Row : Row_Type := 1;
         Column : Column_Type := 1;
      end record;

   -- Encapsulates size information for a rectangular region.
   type Region_Type is
      record
         Width : Width_Type := 1;
         Height : Height_Type := 1;
      end record;

   -- Colors and special text effects.
   type Foreground_Color_Type is
     (Black, Blue, Green, Cyan, Red, Magenta, Brown, White);

   type Background_Color_Type is
     (Black, Blue, Green, Cyan, Red, Magenta, Brown, White);

   type Effect_Type is
     (None, Bright, Blink);

   -- Encapsulates color information for a character cell.
   type Attribute_Type is
      record
         Foreground_Color : Foreground_Color_Type := White;
         Background_Color : Background_Color_Type := Black;
         Effect : Effect_Type := None;
      end record;

   -- Encapsulates information about a character cell.
   type Character_Cell is
      record
         Ch : Character := ' ';
         Color : Attribute_Type;
      end record;

   type Character_Cell_Array is array(Natural range <>) of Character_Cell;
   type Character_Array is array(Natural range <>) of Character;

   -- Character_Arrays represent raw text off the screen with rows concatenated and trailing
   -- spaces on rows preserved so each row consumes exactly the same amount of space in the
   -- array. Furthermore, Character_Arrays (and Characater_Cell_Arrays) can use zero-based
   -- indexing to facilitate coordinate computations. In contrast, Strings are the normal Ada
   -- string type and are used abstractly to represent meaningful sequences of characters. The
   -- following functions do conversions.

   function To_String(Buffer : Character_Array) return String
     with
       Post =>
         To_String'Result'First = 1 and then
         To_String'Result'Length = Buffer'Length and then
         (for all I in Buffer'Range => Buffer(I) = To_String'Result((I - Buffer'First) + 1));

   function To_Character_Array(S : String) return Character_Array
     with
       Post =>
         To_Character_Array'Result'First = 0 and then
         To_Character_Array'Result'Length = S'Length and then
         (for all I in S'Range => S(I) = To_Character_Array'Result((I - S'First) + 0));

   -- Query the current size of the entire screen.
   function Number_Of_Rows return Height_Type;
   function Number_Of_Columns return Width_Type;

   -- Used in the contracts below to verify that points and regions are entirely on screen.
   function In_Bounds(P : Point_Type) return Boolean is
     (Height_Type(P.Row) <= Number_Of_Rows and Width_Type(P.Column) <= Number_Of_Columns)
     with Ghost;

   -- Note that the truth of this In_Bounds implies the truth of the previous one.
   function In_Bounds(P : Point_Type; R : Region_Type) return Boolean is
     (Height_Type(P.Row) + (R.Height - 1) <= Number_Of_Rows and Width_Type(P.Column) + (R.Width - 1) <= Number_Of_Columns)
     with Ghost;

   -- Initialize the screen. Use only this package after this point.
   procedure Initialize;

   -- Clean up the screen. Do not use this package after this point.
   procedure Finalize;

   procedure Read
     (Upper_Left_Corner : in  Point_Type;
      Region            : in  Region_Type;
      Buffer            : out Character_Cell_Array)
     with
       Pre =>
         In_Bounds(Upper_Left_Corner, Region) and
         Buffer'Length >= Natural(Region.Width) * Natural(Region.Height),
       Post =>
         (for all I in Buffer'First + Natural(Region.Width)*Natural(Region.Height) .. Buffer'Last =>
            Buffer(I) = Buffer'Old(I));

   procedure Read_Text
     (Upper_Left_Corner : in  Point_Type;
      Region            : in  Region_Type;
      Buffer            : out Character_Array)
     with
       Pre =>
         In_Bounds(Upper_Left_Corner, Region) and
         Buffer'Length >= Natural(Region.Width) * Natural(Region.Height),
       Post =>
         (for all I in Buffer'First + Natural(Region.Width)*Natural(Region.Height) .. Buffer'Last =>
            Buffer(I) = Buffer'Old(I));

   procedure Write
     (Upper_Left_Corner : in Point_Type;
      Region            : in Region_Type;
      Buffer            : in Character_Cell_Array)
     with
       Pre =>
         In_Bounds(Upper_Left_Corner, Region) and
         Buffer'Length >= Natural(Region.Width) * Natural(Region.Height);

   procedure Write_Text
     (Upper_Left_Corner : in Point_Type;
      Region            : in Region_Type;
      Buffer            : in Character_Array)
     with
       Pre =>
          In_Bounds(Upper_Left_Corner, Region) and
          Buffer'Length >= Natural(Region.Width) * Natural(Region.Height);

   -- If Count is large, the extra character cells are padded with spaces using the given color.
   procedure Print
     (Starting_At : in Point_Type;
      Count       : in Width_Type;
      Color       : in Attribute_Type;
      Text        : in String)
     with
       Pre =>
         In_Bounds(Starting_At) and
         Width_Type(Starting_At.Column) + (Count - 1) <= Number_Of_Columns and
         Text'Length <= Count;

   -- Like Print except the color attributes already on screen are used.
   procedure Print_Text
     (Starting_At : in Point_Type;
      Count       : in Width_Type;
      Text        : in String)
     with
       Pre =>
         In_Bounds(Starting_At) and
         Width_Type(Starting_At.Column) + (Count - 1) <= Number_Of_Columns and
         Text'Length <= Count;

   -- Replaces given region with spaces using the given color attribute.
   procedure Clear
     (Upper_Left_Corner : in Point_Type;
      Region            : in Region_Type;
      Attribute         : in Attribute_Type)
     with Pre => In_Bounds(Upper_Left_Corner, Region);

   -- Changes color of given region; the text is not affected.
   procedure Set_Color
     (Upper_Left_Corner : in Point_Type;
      Region            : in Region_Type;
      Attribute         : in Attribute_Type)
     with Pre => In_Bounds(Upper_Left_Corner, Region);

   -- Clears the entire screen to spaces using the given color attribute.
   procedure Clear_Screen(Attribute : in Attribute_Type := (White, Black, None));

   type Direction_Type is (Up, Down);

   -- Scrolls the given region up/down the given number of rows.
   -- The "fresh" space is filled with spaces using the given color attribute.
   procedure Scroll
     (Direction         : in Direction_Type;
      Upper_Left_Corner : in Point_Type;
      Region            : in Region_Type;
      Count             : in Height_Type;
      Attribute         : in Attribute_Type)
     with
       Pre =>
         In_Bounds(Upper_Left_Corner, Region) and
         Count <= Region.Height;

   procedure Set_Cursor_Position(Position : in Point_Type)
     with Pre => In_Bounds(Position);

   procedure Get_Cursor_Position(Position : out Point_Type);

   -- Updates screen so previous changes take effect.
   -- The refresh is done "intelligently" using the minimum number of changes.
   procedure Refresh;

   -- Updates screen so previous changes take effect.
   -- The redraw rewrites every character cell on the screen.
   procedure Redraw;

   -- Temporarily disables screen handling (for example, when shelling out of a program).
   procedure Off;

   -- Restores screen handling (for example, when returning from a shell).
   procedure On;

   -- Special characters.
   type Special_Key_Type is
   (K_F01,  K_F02,  K_F03,  K_F04,  K_F05,  K_F06,
    K_F07,  K_F08,  K_F09,  K_F10,  K_F11,  K_F12,
    K_SF01, K_SF02, K_SF03, K_SF04, K_SF05, K_SF06,
    K_SF07, K_SF08, K_SF09, K_SF10, K_SF11, K_SF12,
    K_CF01, K_CF02, K_CF03, K_CF04, K_CF05, K_CF06,
    K_CF07, K_CF08, K_CF09, K_CF10, K_CF11, K_CF12,
    K_AF01, K_AF02, K_AF03, K_AF04, K_AF05, K_AF06,
    K_AF07, K_AF08, K_AF09, K_AF10, K_AF11, K_AF12,

    K_HOME,  K_END,   K_PGUP,  K_PGDN,  K_LEFT,  K_RIGHT,  K_UP,  K_DOWN,  K_INS,  K_DEL,
    K_CHOME, K_CEND,  K_CPGUP, K_CPGDN, K_CLEFT, K_CRIGHT, K_CUP, K_CDOWN, K_CINS, K_CDEL,

    K_ALTL, K_ALTM, K_ALTN, K_ALTO, K_ALTP, K_ALTQ, K_ALTR, K_ALTS, K_ALTT, K_ALTU, K_ALTV,
    K_ALTW, K_ALTX, K_ALTY, K_ALTZ,

    K_ALT1, K_ALT2, K_ALT3, K_ALT4, K_ALT5, K_ALT6, K_ALT7, K_ALT8, K_ALT9, K_ALT0,
    K_ALTDASH, K_ALTEQU);

   -- Nice names for certain ASCII characters.
   K_CTRLA : constant Character := Character'Val( 1);
   K_CTRLB : constant Character := Character'Val( 2);
   K_CTRLC : constant Character := Character'Val( 3);
   K_CTRLD : constant Character := Character'Val( 4);
   K_CTRLE : constant Character := Character'Val( 5);
   K_CTRLF : constant Character := Character'Val( 6);
   K_CTRLG : constant Character := Character'Val( 7);
   K_CTRLH : constant Character := Character'Val( 8);
   K_CTRLI : constant Character := Character'Val( 9);
   K_CTRLJ : constant Character := Character'Val(10);
   K_CTRLK : constant Character := Character'Val(11);
   K_CTRLL : constant Character := Character'Val(12);
   K_CTRLM : constant Character := Character'Val(13);
   K_CTRLN : constant Character := Character'Val(14);
   K_CTRLO : constant Character := Character'Val(15);
   K_CTRLP : constant Character := Character'Val(16);
   K_CTRLQ : constant Character := Character'Val(17);
   K_CTRLR : constant Character := Character'Val(18);
   K_CTRLS : constant Character := Character'Val(19);
   K_CTRLT : constant Character := Character'Val(20);
   K_CTRLU : constant Character := Character'Val(21);
   K_CTRLV : constant Character := Character'Val(22);
   K_CTRLW : constant Character := Character'Val(23);
   K_CTRLX : constant Character := Character'Val(24);
   K_CTRLY : constant Character := Character'Val(25);
   K_CTRLZ : constant Character := Character'Val(26);
   K_ESC   : constant Character := Character'Val(27);
   K_SPACE : constant Character := ' ';
   K_TAB   : constant Character := K_CTRLI;
   K_BACKSPACE : constant Character := K_CTRLH;
   K_CR    : constant Character := K_CTRLM;
   K_LF    : constant Character := K_CTRLJ;


   type Key_Kind_Type is (ASCII, Special);

   type Key_Type(Kind : Key_Kind_Type := ASCII) is
      record
         case Kind is
            when ASCII =>
               Key : Character;
            when Special =>
               Special_Key : Special_Key_Type;
         end case;
      end record;


   -- Waits for the user to type a key. The character typed is not echoed.
   procedure Key(Keystroke : out Key_Type);

end Screen;
