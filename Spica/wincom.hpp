/*! \file    wincom.hpp
    \brief   Interface to a serial port class for Win32
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef WINCOM_H
#define WINCOM_H

#include "environ.hpp"

#include <string>
#include "winexcept.hpp"
#include "WorkQueue.hpp"

#if !defined(eMULTITHREADED)
#error WinCom requires multiple threads and multithread support not active!
#endif

namespace spica {
    namespace Win32 {

        //
        // class Com_Port
        //
        // Objects of type Com_Port represent a serial port on a Win32 machine. This class is a
        // bit rough. It is only a first cut.
        //
        class Com_Port : public Handle {

            friend unsigned int __stdcall comPort_reader(void *Port_Object);

        private:
            const char   *name;             // The "file" name for this port.
            DCB           oldComm_state;    // Old settings of the port.
            DCB           newComm_state;    // Our settings.
            COMMTIMEOUTS  oldComm_timeouts; // Old timeouts on the port.
            COMMTIMEOUTS  newComm_timeouts; // Our timeouts.
            bool          port_set;         // =true if the port initialized.
            bool          outer_usage;      // =true if constructing or destroying.
            bool          testing_mode;     // =true when we simulate port input.
            void        (*read_processor)(char); // Function that handles input.
            HANDLE        helperThread_handle;   // Handle of reading thread.
            HANDLE        read_event;       // Event object used during reads.
            HANDLE        write_event;      // Event object used during writes.
            WorkQueue<std::string> input_buffer; // Used to hold simulated input.

        public:
            Com_Port(bool testing = false);
           ~Com_Port();

            // Set the parameters of the port and specifies the function that will process each
            // character as it arrives. (This is done by a helper thread). This class configures
            // the port to use hardware (RTS/CTS) flow control. It also configures "appropriate"
            // timeout intervals for both reading and writing. Future versions of this class
            // might allow the user to specify more of these things.
            //
            void set(const char *given_name, int baud, void (*read)(char));

            // Allows the caller to define a line of simulated input. This is only useful if the
            // port was constructed in testing mode. In that case, an attempt to read the port
            // will return the simulated input.
            //
            void set_Input(const std::string &input)
                { input_buffer.push(input); }

            // Start the thread that processes incoming characters.
            void start_reading();

            // Send data out this port. Data arriving at the port is handled by the read
            // processor function.
            //
            void write(const char *outgoing);

            // Kill the thread that is processing incoming characters.
            void stop_reading();

            // Put the port into the state it was in before it was constructed.
            void unset();
        };

    }  // End of namespace scopes.
}

#endif

