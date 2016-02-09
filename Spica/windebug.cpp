/*! \file    windebug.cpp
    \brief   Implementation of a Windows run-time debugging module.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

See windebug.hpp for more information. Note that for this module to work, several non-local
static objects need to be initialized first. This has implications about when functions from
this module can be called as the program starts up. If they are called during the construction
(or destruction) of non-local static objects in other modules, they might fail because they
might not be constructed yet. This is less than ideal since it would be desirable to display
debugging information for the constructors of non-local static objects. Currently that can't be
done reliably.
*/

#include <algorithm>
#include <cstring>
#include <deque>
#include <fstream>
#include <iomanip>
#include <string>

#include <windows.h>
#undef max
#undef min

#include "windebug.hpp"
#include "windebug.rh"
#include "winexcept.hpp"

#undef Tracer
  // Remove this macro so that we can see the function declaration normally. That is necessary
  // so that we can write the definition below!

namespace spica {
    namespace Win32 {

        extern LRESULT CALLBACK debug_callback(HWND, UINT, WPARAM, LPARAM);

        // This is the maximum number of debugging messages that will be saved. This is a
        // compile-time constant and not currently a configurable item.
        //
        const int MAX_MESSAGECOUNT = 250;

        //--------------------------------------------
        //           Internally Linked Data
        //--------------------------------------------

        // This deque contains the debugging messages. It can be updated even if the debugging
        // window is not around. Pretty cool, eh?
        //
        static std::deque<std::string> debug_messages;

        // This is the number of the next line to be displayed in the debug window. Numbering
        // each line makes it easier to notice changes in the window and easier to refer to
        // specific messages.
        //
        static int L_number = 1;

        // The window handle for the (one and only) debugging window.
        static HWND handle;

        // =true when the debugging window has been created.
        static bool window_exists = false;

        // The current size of the debugging window's client area in character positions and the
        // current size of a character cell in pixels. These values are only valid if the
        // debugging window exists. They are updated with every WM_SIZE message.
        //
        static int Y_size, X_size, char_width, char_height;

        // V_position is the index into the debug_messages deque of the first line that appears
        // at the top of the debugging window. H_position is the column index of the first
        // column of debugging messages to appear on the left edge of the debugging window.
        //
        static int V_position = 0, H_position = 0;

        // This is the file object where debug messages are logged. Note that the destructor
        // will close this file so that there is no need to worry about cleaning it up when the
        // program exits.
        //
        static std::ofstream log_file;
        
        // =true when debug messages are being logged to a file.
        static bool logging = false;

#if defined(eMULTITHREADED)
        // This object is used to synchronize access to the debug messages and associated
        // debugging window support information. This is necessary because debug messages could
        // be written by a different thread than that painting (or resizing) the window. The
        // information protected by this mutex includes:
        //
        //   1. debug_messages
        //   2. Y_size, X_size, char_width, char_height
        //   3. V_position, H_position
        //   4. log_file
        //
        static CRITICAL_SECTION debugMessages_mutex;

        // This object is used to synchronize access to the L_number variable. Without this a
        // second thread might end up reading the same value of L_number before the first thread
        // has updated it. This situation occurs in the Say() members of debugstream and
        // notifystream. This has to be a different mutex than the debugMessages_mutex because
        // the stream classes have no direct knowledge of the debug messages deque and
        // supporting information. In addition, two separate threads might be trying to Say()
        // debug information at the same time.
        //
        static CRITICAL_SECTION lineNumber_mutex;
        
        class Init_DebuggingCritical {
        private:
            CRITICAL_SECTION *cs;

        public:
            Init_DebuggingCritical(CRITICAL_SECTION *p) : cs(p)
            { InitializeCriticalSection(cs); }

            ~Init_DebuggingCritical()
            { DeleteCriticalSection(cs); }
        };

        // This object will insure that the debugMessages_mutex will be initialized even if no
        // debugging window is ever created. Threads can add messages to debug_messages before
        // there is a debugging window.
        //
        static Init_DebuggingCritical initializer_1(&debugMessages_mutex);

        // This object will insure that lineNumber_mutex will be initialized properly as well.
        static Init_DebuggingCritical initializer_2(&lineNumber_mutex);
#endif

        //---------------------------------------------------------
        //           Internally Linked Support Functions
        //---------------------------------------------------------

        //
        // display_debugText
        //
        // This function writes a line of text into the debugging window's list of text. This
        // function can be called even if the debugging window hasn't yet been created.
        //
        static void display_debugText(const std::string &line)
        {
#if defined(eMULTITHREADED)
            Critical_Grabber critical(&debugMessages_mutex);
#endif

            if (logging) log_file << line << std::endl;
     
            // Add this line to the list.
            debug_messages.push_back(line);

            // Check to see if we are to throw away a string in the list.
            while (debug_messages.size() > MAX_MESSAGECOUNT) {
                debug_messages.pop_front();
            }

            // If there is a debugging window, then make sure it knows about this change.
            if (window_exists) {

                // Adjust V_position so that the new line is in the window. This might force an
                // abrupt shift in the window if the user has been scrolling through old
                // messages.
                //
                if (static_cast<int>(debug_messages.size() - V_position) > Y_size)
                    V_position = debug_messages.size() - Y_size;

                InvalidateRect(handle, 0, FALSE);
            }
        }


        //
        // register_debugWindow
        //
        // This function registers the debug window class with Windows.
        //
        static void register_debugWindow(const char *class_name)
        {
            WNDCLASS the_class;
            static bool already_registered = false;
            
            if (!already_registered) {
                the_class.style         = CS_HREDRAW | CS_VREDRAW;
                the_class.lpfnWndProc   = debug_callback;
                the_class.cbClsExtra    = 0;
                the_class.cbWndExtra    = 0;
                the_class.hInstance     = GetModuleHandle(0);
                the_class.hIcon         =
                    LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(DEBUG_ICON));
                the_class.hCursor       = LoadCursor(0, IDC_ARROW);
                the_class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
                the_class.lpszMenuName  = MAKEINTRESOURCE(DEBUG_MENU);
                the_class.lpszClassName = class_name;

                if (!RegisterClass(&the_class))
                    throw API_Error("Cannot register debugging window class");

                already_registered = true;
            }
        }


        //
        // get_clientSize
        //
        // This function computes the size of the client area in character positions.
        //
        static void get_clientSize(
                                   HWND window_handle,
                                   int &X_size,
                                   int &Y_size,
                                   int &char_width,
                                   int &char_height
                                   )
        {
            HDC         context_handle;
            TEXTMETRIC  text_metrics;
            RECT        client_rectangle;
            int         raw_X, raw_Y;
            
            // Figure out the character height so we can figure out the height of the window in
            // rows.
            context_handle = GetDC(window_handle);

            // Set up the font.
            SelectObject(context_handle, GetStockObject(ANSI_FIXED_FONT));
            GetTextMetrics(context_handle, &text_metrics);
            char_width  = text_metrics.tmAveCharWidth;
            char_height = text_metrics.tmHeight + text_metrics.tmExternalLeading;

            ReleaseDC(window_handle, context_handle);

            // Learn the size of the client area.
            GetClientRect(window_handle, &client_rectangle);
            raw_X = client_rectangle.right - client_rectangle.left;
            raw_Y = client_rectangle.bottom - client_rectangle.top;
            X_size = raw_X/char_width;
            Y_size = raw_Y/char_height;
        }


        //
        // paint_function
        //
        // This function does the grunt work of painting the debug window.
        //
        static int paint_function(HWND window_handle)
        {
            Paint_Context painter(window_handle);
            int longest_line = 0;

#if defined(eMULTITHREADED)
            Critical_Grabber critical(&debugMessages_mutex);
#endif

            // Set up the font the drawing attributes.
            if (SelectObject(painter, GetStockObject(ANSI_FIXED_FONT)) == 0)
                throw API_Error("Can't select ANSI_FIXED_FONT in the debug window");

            if (SetTextColor(painter, GetSysColor(COLOR_WINDOWTEXT)) == CLR_INVALID)
                throw API_Error("Can't set the text color in the debug window");

            if (SetBkColor(painter, GetSysColor(COLOR_WINDOW)) == CLR_INVALID)
                throw API_Error("Can't set the background color in the debug window");

            // Create a string of spaces to use to erase parts of the screen.
            std::string spaces(X_size + 1, ' ');

            // Loop over all lines in the client window.
            int counter  = 0;
            while (counter < Y_size) {
                int char_counter = 0;

                // If there is text to display on this line, then do so.
                if (V_position + counter < static_cast<int>(debug_messages.size())) {
                    char_counter = debug_messages[V_position + counter].size();
                    if (char_counter > longest_line) longest_line = char_counter;
                    TextOut(painter, 0, counter * char_height,
                            debug_messages[V_position + counter].c_str(), char_counter);
                }

                // Erase the rest of this line.
                if (X_size + 1 - char_counter > 0) {
                    TextOut(
                            painter,
                            char_counter * char_width, counter * char_height,
                            spaces.c_str(),
                            X_size + 1 - char_counter
                            );
                }
                counter++;
            }

            // Now position the scroll boxes.
            SCROLLINFO scroll_info;
            memset(&scroll_info, 0, sizeof(scroll_info));
            scroll_info.cbSize = sizeof(scroll_info);
            scroll_info.fMask  = SIF_ALL;
            scroll_info.nMin   = 0;
            scroll_info.nMax   = debug_messages.size() - 1;
            scroll_info.nPage  = Y_size;
            scroll_info.nPos   = V_position;
            SetScrollInfo(window_handle, SB_VERT, &scroll_info, TRUE);

            memset(&scroll_info, 0, sizeof(scroll_info));
            scroll_info.cbSize = sizeof(scroll_info);
            scroll_info.fMask  = SIF_ALL;
            scroll_info.nMin   = 0;
            scroll_info.nMax   = longest_line - 1;
            scroll_info.nPage  = X_size;
            scroll_info.nPos   = H_position;
            SetScrollInfo(window_handle, SB_HORZ, &scroll_info, TRUE);
            
            return longest_line;
        }
        

        //
        // Vscroll_function
        //
        // This function handles vertical scrolling in the debugging window.
        //
        static void Vscroll_function(
                                     HWND   window_handle,
                                     WPARAM wParam,
                                     LPARAM lParam
                                     )
        {
#if defined(eMULTITHREADED)
            Critical_Grabber critical(&debugMessages_mutex);
#endif

            switch (LOWORD(wParam)) {
            case SB_LINEDOWN:
                V_position++;
                if (V_position > static_cast<int>(debug_messages.size()) - Y_size)
                    V_position =
                        std::max(0, static_cast<int>(debug_messages.size()) - Y_size);
                InvalidateRect(window_handle, 0, FALSE);
                break;

            case SB_LINEUP:
                V_position--;
                if (V_position < 0) V_position = 0;
                InvalidateRect(window_handle, 0, FALSE);
                break;

            case SB_PAGEDOWN:
                V_position += Y_size;
                if (V_position > static_cast<int>(debug_messages.size()) - Y_size)
                    V_position =
                        std::max(0, static_cast<int>(debug_messages.size()) - Y_size);
                InvalidateRect(window_handle, 0, FALSE);
                break;

            case SB_PAGEUP:
                V_position -= Y_size;
                if (V_position < 0) V_position = 0;
                InvalidateRect(window_handle, 0, FALSE);
                break;

            case SB_THUMBTRACK:
                V_position = HIWORD(wParam);
                InvalidateRect(window_handle, 0, FALSE);
                break;
            }
        }


        //
        // Hscroll_function
        //
        // This function handles horizontal scrolling in the debugging window.
        //
        static void Hscroll_function(
                                     HWND   window_handle,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     int    longest_line
                                     )
        {
#if defined(eMULTITHREADED)
            Critical_Grabber critical(&debugMessages_mutex);
#endif

            switch (LOWORD(wParam)) {
            case SB_LINEDOWN:
                H_position++;
                if (H_position > longest_line - X_size)
                    H_position = std::max(0, longest_line - X_size);
                InvalidateRect(window_handle, 0, FALSE);
                break;
                
            case SB_LINEUP:
                H_position--;
                if (H_position < 0) H_position = 0;
                InvalidateRect(window_handle, 0, FALSE);
                break;

            case SB_PAGEDOWN:
                H_position += X_size;
                if (H_position > longest_line - X_size)
                    H_position = std::max(0, longest_line - X_size);
                InvalidateRect(window_handle, 0, FALSE);
                break;

            case SB_PAGEUP:
                H_position -= X_size;
                if (H_position < 0) H_position = 0;
                InvalidateRect(window_handle, 0, FALSE);
                break;

            case SB_THUMBTRACK:
                H_position = HIWORD(wParam);
                InvalidateRect(window_handle, 0, FALSE);
                break;
            }
        }


        //
        // size_function
        //
        // This function handles the resizing of the debugging window. It might have to adjust
        // V_position so that the most text possible will fit into the window.
        //
        static void size_function(HWND window_handle)
        {
#if defined(eMULTITHREADED)
            Critical_Grabber critical(&debugMessages_mutex);
#endif

            // How big is the current window?
            get_clientSize(window_handle, X_size, Y_size, char_width, char_height);

            // If this resizing would cause a blank spot at the bottom of the window to appear,
            // then make a correction.
            //
            if (V_position + Y_size > debug_messages.size()) {
                V_position = debug_messages.size() - Y_size;
                if (V_position < 0) V_position = 0;
            }
        }


        //--------------------------------------
        //           Tracer Functions
        //--------------------------------------
        
        Tracer::Tracer(
                       // Parameter declarations.
                       int   trace_level,
                       const char *trace_message,
                       const char *file_name,
                       int   line_number) :

            // Initializers for the members.
            level  (trace_level),
            message(trace_message),
            file   (file_name),
            line   (line_number)

            // Function body.
        {
            std::ostringstream message_text;
            
            {
#if defined(eMULTITHREADED)
                Critical_Grabber critical(&lineNumber_mutex);
#endif
                
                message_text << std::setw(4) << L_number++
                             << " TP: (" << level << ") "
                             << message
                             << " FILE=" << file
                             << " LINE=" << line;
            }
            display_debugText(message_text.str().c_str());
        }

        //-------------------------------------------
        //           debugstream Functions
        //-------------------------------------------
        
        void debugstream::say(int level)
        {
            std::ostringstream message_text;

            {
#if defined(eMULTITHREADED)
                Critical_Grabber critical(&lineNumber_mutex);
#endif
      
                message_text << std::setw(4) << L_number++
                             << "   : (" << level << ") " << str();
            }
            display_debugText(message_text.str());
        }

        //--------------------------------------------
        //           notifystream Functions
        //--------------------------------------------

        //
        // notifystream::say
        //
        // This function tries to display the exception notification in the debug window as well
        // as on the screen. Is that going to be a problem for exception noti- fications
        // pertaining to out of memory issues? The ostringstream here may fail and throw an
        // exception that will end up being an exception inside the excep- tion handler.
        //
        void notifystream::say(HWND window_handle)
        {
            std::ostringstream message_text;
            
            {
#if defined(eMULTITHREADED)
                Critical_Grabber critical(&lineNumber_mutex);
#endif
      
                message_text << std::setw(4) << L_number++ << " EX: (0) " << str();
            }
            display_debugText(message_text.str());

            // Show the user as well and wait for a reaction.
            MessageBox(window_handle,
                       str().c_str(),
                       "Exception Notification",
                       MB_ICONEXCLAMATION
                       );
        }


        //--------------------------------------
        //           Public Functions
        //--------------------------------------

        //
        // create_debugWindow
        //
        // This function creates a debugging window. It does everything necessary to get the
        // window on screen.
        //
        void create_debugWindow()
        {
            // Don't make two debugging windows. If there already is one, just reuse it.

            if (window_exists) {
                if (SetFocus(handle) == 0)
                    throw API_Error("Unable to set focus to existing debugging window");
                return;
            }

            static const char *class_name = "spicaDebugging_Window";
            DWORD my_style =
                WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN;

            // Register the debug window class with windows.
            register_debugWindow(class_name);
            
            // Create a specific instance of the window class named by class_name.
            handle = CreateWindow(
                class_name,           // From which class should this window be?
                "Debugging",          // Text which appears on title bar and under icon.
                my_style,             // Window style.
                CW_USEDEFAULT,        // Horizontal position of window on screen.
                CW_USEDEFAULT,        // Vertical position of window on screen.
                CW_USEDEFAULT,        // Width of window.
                CW_USEDEFAULT,        // Height of window.
                0,                    // Handle of parent window.
                0,                    // Handle of menu or child window identifier.
                GetModuleHandle(0),   // Handle of creating program.
                0                     // Address of "window creation" data.
                                  );

            if (handle == 0)
                throw API_Error("Unable to create debugging window");

            // Display the window we just created.
            ShowWindow(handle, SW_SHOW);

            // Send WM_PAINT to the window procedure to force the window to draw into its client
            // area. (The ShowWindow() function only displays the borders. title bar and so
            // forth).
            //
            UpdateWindow(handle);

            window_exists = true;
        }


        //----------------------------------------
        //           Callback Functions
        //----------------------------------------

        //
        // debug_callback
        //
        // This function handles all messages that are sent to the debug window.
        //
        LRESULT CALLBACK debug_callback(
                                        HWND   window_handle,
                                        UINT   message,
                                        WPARAM wParam,
                                        LPARAM lParam
                                        )
        {
            static int longest_line = 0;
            // The longest line currently in the window. This is updated for each WM_PAINT.
    
            try {
                switch (message) {

                    // Part of the window client area needs to be redrawn.
                case WM_PAINT:
                    longest_line = paint_function(window_handle);
                    return 0;

                    // Trying to scroll vertically.
                case WM_VSCROLL:
                    Vscroll_function(window_handle, wParam, lParam);
                    return 0;

                    // Trying to scroll horizontally.
                case WM_HSCROLL:
                    Hscroll_function(window_handle, wParam, lParam, longest_line);
                    return 0;

                    // The window is being resized.
                case WM_SIZE:
                    size_function(window_handle);
                    return 0;

                    // The user pressed a key.
                case WM_KEYDOWN:
                    switch (wParam) {
                    case VK_HOME : break;
                    case VK_END  : break;
                    case VK_PRIOR:
                        SendMessage(handle, WM_VSCROLL, SB_PAGEUP, 0L);
                        break;
                    case VK_NEXT :
                        SendMessage(handle, WM_VSCROLL, SB_PAGEDOWN, 0L);
                        break;
                    case VK_UP   :
                        SendMessage(handle, WM_VSCROLL, SB_LINEUP, 0L);
                        break;
                    case VK_DOWN :
                        SendMessage(handle, WM_VSCROLL, SB_LINEDOWN, 0L);
                        break;
                    }
                    break;

                    // The user selects a menu item.
                case WM_COMMAND:
                    switch (wParam) {

                        // Save the current list of debug messages to a file.
                    case DEBUG_SAVEBUFFER: {
            
                        char file_name[260+1];
                        OPENFILENAME log_info;
                        memset(&log_info, 0, sizeof(log_info));
                        log_info.lStructSize  = sizeof(log_info);
                        log_info.hwndOwner    = window_handle;
                        log_info.lpstrFilter  = "Debug Log File(*.dbg)\0*.dbg\0";
                        log_info.nFilterIndex = 1;
                        log_info.lpstrFile    = file_name;
                        file_name[0] = '\0';
                        log_info.nMaxFile     = 261;
                        log_info.lpstrTitle   = "Specify Buffer File";
                        log_info.Flags        = OFN_LONGNAMES | OFN_PATHMUSTEXIST;
                        log_info.lpstrDefExt  = "dbg";

                        // We are ready to get the name of the file. (Finally!)
                        if (GetSaveFileName(&log_info) == TRUE) {

                            // Open the file and send all the current debug text to it.
                            std::ofstream output_file(file_name);
                            if (!output_file)
                                MessageBox(window_handle,
                                           "Unable to open file!", "Debug Error",
                                           MB_ICONEXCLAMATION
                                           );
                            else {
#if defined(eMULTITHREADED)
                                Critical_Grabber critical(&debugMessages_mutex);
#endif
                  
                                std::deque<std::string>::iterator p(debug_messages.begin());

                                while (p != debug_messages.end()) {
                                    output_file << *p << '\n';
                                    p++;
                                }
                            }
                        }
                    }
                        break;
                        
                        // Specify the name of a file for logging debug messages.
                    case DEBUG_STARTSAVE: {

                        // Turn off logging if it is already active.
                        log_file.close();
                        logging = false;

                        char file_name[260+1];
                        OPENFILENAME log_info;
                        memset(&log_info, 0, sizeof(log_info));
                        log_info.lStructSize  = sizeof(log_info);
                        log_info.hwndOwner    = window_handle;
                        log_info.lpstrFilter  = "Debug Log File(*.dbg)\0*.dbg\0";
                        log_info.nFilterIndex = 1;
                        log_info.lpstrFile    = file_name;
                        file_name[0] = '\0';
                        log_info.nMaxFile     = 261;
                        log_info.lpstrTitle   = "Specify Log File";
                        log_info.Flags        = OFN_LONGNAMES | OFN_PATHMUSTEXIST;
                        log_info.lpstrDefExt  = "dbg";

                        // We are ready to get the name of the file. (Finally!)
                        if (GetSaveFileName(&log_info) == TRUE) {
                            log_file.open(file_name);
                            if (!log_file)
                                MessageBox(window_handle,
                                           "Unable to open file!", "Debug Error",
                                           MB_ICONEXCLAMATION
                                           );
                            else
                                logging = true;
                        }
                    }
                        break;

                        // Stop logging debug messages.
                    case DEBUG_STOPSAVE:
                        log_file.close();
                        logging = false;
                        break;

                        // The following menu items are not yet implemented.
                    case DEBUG_CUT:
                    case DEBUG_CONFIGURE:
                    case DEBUG_COPY:
                    case DEBUG_EXIT:
                    case DEBUG_HELPTOPICS:
                    case DEBUG_ABOUT:
                        MessageBox(window_handle,
                                   "Not implemented", "Sorry", MB_ICONEXCLAMATION);
                        break;
                    }
                    break;

                    // If the user changes the system colors, redraw my window.
                case WM_SYSCOLORCHANGE:
                    InvalidateRect(window_handle, 0, TRUE);
                    return 0;

                    // The window is being closed.
                case WM_DESTROY:
                    window_exists = false;
                    return 0;
                }
            }
            catch (std::bad_alloc) {
                notifystream message;
                message << "Out of memory in debugging window";
                message.say(window_handle);
                return 0;
            }
            catch (API_Error We) {
                notifystream message;
                message << "Unexpected API error in debugging window\r"
                        << We.what() << "\r"
                        << "Error code = " << We.error_code();
                message.say(window_handle);
                return 0;
            }
            catch (...) {
                notifystream message;
                message << "Unknown exception in debugging window";
                message.say(window_handle);
                return 0;
            }
            
            // Pass messages we don't care about to Windows for default handling.
            return DefWindowProc(window_handle, message, wParam, lParam);
        }
        
    } // End of namespace scopes.
}
