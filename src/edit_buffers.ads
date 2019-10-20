private with Ada.Strings.Unbounded;

package Edit_Buffers is
   type Edit_Buffer is private;


   -- Build an Edit_Buffer from an ordinary String.
   -- @param S The source string used to create the Edit_Buffer.
   -- @return An Edit_Buffer containing the same text as the source string with no additional
   -- trailing spaces. If the source string contains trailing spaces, the Edit_Buffer will also.
   function Make(S : String) return Edit_Buffer
     with Post =>
       Length(Make'Result) = S'Length and then
         (for all I in 1 .. Length(Make'Result) => Element(Make'Result, I) = S(S'First + (I - 1)));

   -- @param E The Edit_Buffer from which to extract an element.
   -- @param Position The position in the Edit_Buffer of the element to extract.
   -- @return The character in the given Edit_Buffer at the given position. If the position is
   -- off the end of the Edit_Buffer, a space character is returned. Thus, it is legal to call
   -- this function for any value of Position in the range Positive'First .. Positive'Last. Note
   -- that the given Edit_Buffer is never extended by this function.
   function Element(E : Edit_Buffer; Position : Positive) return Character
     with Post => (if Position > Length(E) then Element'Result = ' ');

   -- @return The number of characters currently in the given Edit_Buffer. This might include
   -- trailing spaces.
   function Length(E : Edit_Buffer) return Natural;

   -- @return The String representation of the given Edit_Buffer's contents. The returned String
   -- might include trialing spaces if the Edit_Buffer has them. The returned String always has
   -- a starting index of 1.
   function To_String(E : Edit_Buffer) return String
     with Post =>
       To_String'Result'Length = Length(E) and
         (for all I in 1 .. Length(E) => To_String'Result(1 + (I - 1)) = Element(E, I));

   -- Insert Letter into E at Position. The character previously at Position is moved "down."
   -- Thus this procedure is a form of insert-before. If Position is off the end of E, the
   -- Edit_Buffer is extended with spaces, as necessary.
   procedure Insert(E : in out Edit_Buffer; Letter : in Character; Position : in Positive)
     with Post =>
       Element(E, Position) = Letter and
       (if Position <= Length(E'Old) then Length(E) = Length(E'Old) + 1
                                     else Length(E) = Position);

   -- Replace the character in E at Position with Letter. If Position is off the end of E, the
   -- Edit_Buffer is extended with spaces, as necessary.
   procedure Replace(E : in out Edit_Buffer; Letter : in Character; Position : in Positive)
     with Post =>
       Element(E, Position) = Letter and
       (if Position <= Length(E'Old) then Length(E) = Length(E'Old)
                                     else Length(E) = Position);

   -- Erase the character in E at Position. The Edit_Buffer is then collapsed to close the gap.
   -- If Position is off the end of E, there is no effect.
   procedure Erase(E : in out Edit_Buffer; Position : in Positive);

   -- Erase the entire Edit_Buffer, E.
   procedure Erase(E : in out Edit_Buffer);

   -- Append Additional to the end of E.
   procedure Append(E : in out Edit_Buffer; Additional : in Character);

   -- Append Additional to the end of E.
   procedure Append(E : in out Edit_Buffer; Additional : in String);

   -- Append Additional to the end of E.
   procedure Append(E : in out Edit_Buffer; Additional : in Edit_Buffer);

   -- @return An Edit_Buffer that contains the text of E starting at Start and of size Length.
   -- If Start is off the end of E, the returned Edit_Buffer contains Length spaces. If Length
   -- extends beyond the end of E, the returned Edit_Buffer will contain spaces for those extra
   -- positions.
   function Subbuffer(E : Edit_Buffer; Start : Positive; Length : Natural) return Edit_Buffer;

   -- Truncate E starting at (and including) Position. If Position is off the end of E, there
   -- is no effect.
   procedure Trim(E : in out Edit_Buffer; Position : in Positive);

private

   -- An Edit Buffer is just a wrapper around Unbounded_String.
   type Edit_Buffer is
      record
         Workspace : Ada.Strings.Unbounded.Unbounded_String;
      end record;

end Edit_Buffers;
