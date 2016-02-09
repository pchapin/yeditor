/*! \file    winexcept.hpp
    \brief   Helper classes for exception throwing Win32 programs.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

This file contains a number of classes that are useful in preventing resource leaks in Win32
programs that throw exceptions. Any resource that is acquired and that later needs releasing
should have a small class to control that process. An object should be created in such a way
that its initialization obtains the resource and its destruction releases the resource. Since
destructors are automatically executed both when an exception is thrown and when a block is
exited normally, this approach makes resource deallocation automatic.
*/

#ifndef WINEXCEPT_H
#define WINEXCEPT_H

#include <stdexcept>
#include <string>

#include <windows.h>

namespace spica {
    namespace Win32 {
  
        //
        // class API_Error
        //
        // This class is a general Windows exception class. It is intended only for reporting
        // errors in Win32 API functions. The constructor calls GetLastError() and so you should
        // only throw this exception after a function that sets GetLastError()'s return in a
        // meaningful way. This class is derived from the standard library's runtime_error class
        // to facilitate consistent error handling in applications. I'm not certain if
        // runtime_error is the best choice, but it should be at least reasonable most of the
        // time.
        //
        class API_Error : public std::runtime_error {
        public:
            // The message M is intended to make sense to the user.
            explicit API_Error(const std::string &M) throw() : std::runtime_error(M)
                { raw_error = GetLastError(); }

            // Returns the 16 bit error code associated with this error.
            DWORD error_code() const throw()
                { return raw_error & 0x0000FFFF; }

            // Returns the 12 bit facility code assciated with this error.
            DWORD facility_code() const throw()
                { return (raw_error & 0x0FFF0000) >> 16; }

            // Returns true if this is an application-defined error.
            bool application_defined() const throw()
                { return (raw_error & 0x20000000) ? true : false; }

            // Returns the severity of this error as reported by GetLastError().
            //   0 => Success, 1 => Informational, 2 => Warning, 3 => Error.
            int severity_level() const throw()
                { return (raw_error & 0xC0000000) >> 30; }

        private:
            DWORD  raw_error;
        };


        //
        // class Handle
        //
        // This class abstracts the concept of a handle and insures that the handle will get
        // properly closed even when an exception is thrown.
        //
        class Handle {
        public:
            Handle() :         the_handle(INVALID_HANDLE_VALUE) { }
            Handle(HANDLE h) : the_handle(h)                    { }
            operator HANDLE() const  { return the_handle; }
            void operator=(HANDLE h) { the_handle = h;    }

            ~Handle()
                { if (the_handle != INVALID_HANDLE_VALUE) CloseHandle(the_handle); }

        protected:
            // Give access to derived classes so that specialized versions of Handle can easily
            // be created for various kernel object types.
            //
            HANDLE the_handle;

        private:
            // Make copying illegal for now. To allow copying is trickier than it looks.
            Handle(const Handle &);
            Handle &operator=(const Handle &);
        };


        //
        // class Critical_Grabber
        //
        // This class simplifies the task of grabbing a CRITICAL_SECTION object. It insures that
        // the CRITICAL_SECTION will be properly released if an exception is thrown. This class
        // is obsolete. Applications should use Mutex_Sem objects from my semaphore library
        // instead. Such objects offer a greater degree of cross platform support than this
        // class.
        //
        class Critical_Grabber {
        public:
            Critical_Grabber(CRITICAL_SECTION *p) : cs(p) { EnterCriticalSection(cs); }
            ~Critical_Grabber() { LeaveCriticalSection(cs); }

        private:
            CRITICAL_SECTION *cs;

            // Make copying illegal for now. To allow copying is trickier than it looks.
            Critical_Grabber(const Critical_Grabber &);
            Critical_Grabber &operator=(const Critical_Grabber &);
        };


        //
        // class Paint_Context
        //
        // This class insures that device contexts obtained with BeginPaint() are properly
        // released with EndPaint() even when an exception is thrown.
        //
        class Paint_Context {
        public:
            Paint_Context(HWND Handle) : window_handle(Handle)
                { context_handle = BeginPaint(window_handle, &paint_info); }

            operator HDC() const
                { return context_handle; }

            ~Paint_Context()
                { EndPaint(window_handle, &paint_info); }

        private:
            PAINTSTRUCT paint_info;
            HWND        window_handle;
            HDC         context_handle;

            // Make copying illegal for now. To allow copying is trickier than it looks.
            Paint_Context(const Paint_Context &);
            Paint_Context &operator=(const Paint_Context &);
        };


        //
        // class Device_Context
        //
        // This class insures that device contexts obtained with GetDC() get properly released
        // with ReleaseDC() even when an exception is thrown.
        //
        class Device_Context {
        public:
            Device_Context(HWND Handle) : window_handle(Handle)
                { context_handle = GetDC(window_handle); }

            operator HDC() const
                { return context_handle; }

            ~Device_Context()
                { ReleaseDC(window_handle, context_handle); }

        private:
            HWND   window_handle;
            HDC    context_handle;
            
            // Make copying illegal for now. To allow copying is trickier than it looks.
            Device_Context(const Device_Context &);
            Device_Context &operator=(const Device_Context &);
        };

    }
}

#endif
