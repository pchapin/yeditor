
Y Server
========

The Y server is an unimplemented idea for supporting concurrent editing sessions in Y. The note
below is taking from the Y mailing list, dated December 1, 1997. The author of the note is Peter
Chapin.

Message Transcript
------------------

Hello again. This is part II of my musings from the Thanksgiving weekend. In this letter I will
give you an overview of my ideas regarding remote storage of Y objects.

Y was originally designed to support the classes at VTC. One feature that would be quite nice to
implement would be some way for users to collaborate on a single file. That is: two different
invocations of Y should be able to edit the same file at the same time. This file might be in a
shared network directory or it might not be. I could imagine one Y user "exporting" a file so
that another Y user could edit it. There are some file locking issues that would have to be
solved here. I'm imagining the locks being done on a line by line basis so that two users could
edit different lines at the same time but not the same line at the same time.

A general solution to this would entail constructing a "Y server" that holds the shared objects
and that Y would communicate with over the network. Inside Y itself there would be a "server
interface" object that the program uses to send and retrieve other objects from the server. That
server interface object might talk to a server over the network or talk to a server-like module
contained in the program that in turn would access a file in a shared directory or objects in
shared memory. All the code that uses the server interface object would not need to know the
precise location of the shared data or the manner in which it is actually accessed.

Actually I see the server as being a generic object server. It would do business in general
binary images without any real knowledge of the object's format. In this way an object server
could service applications other than Y using objects other than the ones Y is interested in
using. I'm imagining, for example, running an object server on twilight and providing a library
that will allow developers to save objects on that server using TCP over the Internet. The
server would run full time and service any and all applications that used the library. Obviously
there are some non-trivial security issues that need to be thought about.

In Y, the central data structure that is used by each file is a list of `Edit_Buffer` objects.
An `Edit_Buffer` is a simple string class with a few features that are very Y-like. For example,
you can insert characters off the end of an `Edit_Buffer` and it extends itself with spaces. The
list class, called `Edit_List`, is very old. It should be replaced with something more modern.
I'm imagining replacing it with the code necessary to allow the list and all the `Edit_Buffers`
on it to be stored on a remote object server.

Actually this doesn't sound like it would be all that hard to implement in a simple way. Doing
it "right" would be more involved. Yet I think there is a surprisingly satisfactory solution
available without too much work. For the moment, forget about security. Now imagine:

    class ServerInterface {
      public
        typedef long object_ID;

        virtual int write( object_ID, void *, int );
          // Writes an array of bytes to the server under the given ID.

        virtual int read( object_ID, void *, int );
          // Reads a raw object from the server.

        virtual object_ID Get_NextID( );
          // Returns a unique ID number from the server.
    };

There would be derived classes for the various server types: TCP over the network, shared file,
shared memory, etc. Now consider:

    class Remotable {

      public:
        Remotable( ServerInterface & );
          // Gets a new object ID number from the server.

        virtual void write_object( );
          // Writes the object to the server.

        virtual void read_object( );
          // Gets an object from the server.

        // There would also need to be locking functions and corresponding
        //   functions in the ServerInterface class.
};

Now the punch line:

    class RemotableString : public Remotable, public String {
      // etc...
    };

The idea here is to take an existing class -- something that knows nothing about the object
server -- and graft onto it a subobject of type `Remotable`. The resulting derived object would
support the normal operations of the host class (String in this case) as well as the operations
`read_object( )` and `write_object( )`. Those functions would have to convert the host type to a
raw stream of binary bytes to send to the server and vice-versa. This would have to be done
without access to the private section of the host type (`RemotableString` has no access to the
private section of `String`), but I don't think that would be a problem for any type that has a
full set of access functions.

Thus we have something like:

    RemotableString my_string( server, "Hello" );

    // ...
    // Do String operations on my_string as normal.
    // ...

    my_string.write_object( );
      // Send it to the server.

Now consider:

    list<RemotableString> text;

In this case, the program (Y, for example) could walk down the list sending or fetching lines
from the server as necessary. Keep in mind that class `Remotable` would also have some object
locking facilities too so that would be how the line locking would be implemented.

Actually in the case of Y, the entire list will need to be remotable too. This is because of the
need to synchronize line insertions and deletions. Thus:

    class RemotableList : public Remotable, public list<RemotableString> {
      // It looks nasty, but I think it could work.
    };

There are basically three parts to this idea:

1. An object server that takes in objects as raw binary data and stashes them away somehow.
   There could be several different server implementations. One possibility would be a server
   that runs on a remote host and accepts TCP connections. Another possibility would be a module
   of code that resides in the application itself and manages shared memory. In any case, the
   server would preside over the objects in some sort of shared space.

2. A server interface class that gives the application a uniform way of communicating with the
   various server types.

3. Class `Remotable` that can be "attached" to an unsuspecting class via multiple inheritance to
   create a class that acts like the original but that also supports remote storage and
   retrieval.

It would be very nice if you could get automatic fetching and caching of remotable objects
without having to explicitly ask for it. If that were true then remotable objects could be given
to (library) functions that know nothing about remote object servers. As the functions operate
on the remotable objects they would automatically be fetched, locked, and sent back to the
object server. This would be much cooler than the "manual" access I described above and it would
allow remotable objects to be used in a natural way with existing libraries.

I think it may be possible to achieve this goal by using the STL model. Imagine a remotable list
that was hand created -- not just a merger of some class `Remotable` with the template list.
Such a beast could define iterators that would act like smart pointers. Each access of a list
element via its iterator would first fetch the object from the server. For example:

    RemotableList<String>           text;
    RemotableList<String>::iterator p;

    for( p = text.begin(); p != text.end( ); p++ ) {
      p->append( "Hello!" );
    }

The overloaded `operator->()` in class `RemotableList<String>::iterator` would first fetch the
String from the server and lock it before returning a reference to the String itself. Then when
`operator++()` is applied to `p`, the resulting modified `String` might be sent back to the
server and unlocked. Manipulation of objects in the list would be automatic. The users of the
iterators would have no idea that the objects actually reside in a remote location. This would
allow all of the STL algorithms to be applied to a collection of remotable objects.

Thoughts?

Peter
