
package body Edit_Buffers is

   function Make(S : String) return Edit_Buffer is
      Result : Edit_Buffer;
   begin
      return Result;
   end Make;


   function Element(E : Edit_Buffer; Position : Positive) return Character is
   begin
      return ' ';
   end Element;


   function Length(E : Edit_Buffer) return Natural is
   begin
      return 0;
   end Length;


   function To_String(E : Edit_Buffer) return String is
   begin
      return "";
   end To_String;


   procedure Insert(E : in out Edit_Buffer; Letter : in Character; Position : in Positive) is
   begin
      null;
   end Insert;


   procedure Replace(E : in out Edit_Buffer; Letter : in Character; Position : in Positive) is
   begin
      null;
   end Replace;


   procedure Erase(E : in out Edit_Buffer; Position : in Positive) is
   begin
      null;
   end Erase;


   procedure Erase(E : in out Edit_Buffer) is
   begin
      null;
   end Erase;


   procedure Append(E : in out Edit_Buffer; Additional : in Character) is
   begin
      null;
   end Append;


   procedure Append(E : in out Edit_Buffer; Additional : in String) is
   begin
      null;
   end Append;


   procedure Append(E : in out Edit_Buffer; Additional : in Edit_Buffer) is
   begin
      null;
   end Append;


   function Subbuffer(E : Edit_Buffer; Start : Positive; Length : Natural) return Edit_Buffer is
      Result : Edit_Buffer;
   begin
      return Result;
   end Subbuffer;


   procedure Trim(E : in out Edit_Buffer; Position : in Positive) is
   begin
      null;
   end Trim;

end Edit_Buffers;
