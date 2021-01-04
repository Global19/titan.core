// This Test Port skeleton source file was generated by the
// TTCN-3 Compiler of the TTCN-3 Test Executor version CRL 113 200/6 R5A
// for ebotbar (ebotbar@ebotbarVB) on Thu Dec  6 16:01:48 2018

// Copyright (c) 2000-2021 Ericsson Telecom AB

// You may modify this file. Complete the body of empty functions and
// add your member functions here.

#include "PortB.hh"

namespace B {

PortB::PortB(const char *par_port_name)
	: PortB_BASE(par_port_name)
{

}

PortB::~PortB()
{

}

void PortB::set_parameter(const char * /*parameter_name*/,
	const char * /*parameter_value*/)
{

}

/*void PortB::Handle_Fd_Event(int fd, boolean is_readable,
	boolean is_writable, boolean is_error) {}*/

void PortB::Handle_Fd_Event_Error(int /*fd*/)
{

}

void PortB::Handle_Fd_Event_Writable(int /*fd*/)
{

}

void PortB::Handle_Fd_Event_Readable(int /*fd*/)
{

}

/*void PortB::Handle_Timeout(double time_since_last_call) {}*/

void PortB::user_map(const char * /*system_port*/)
{

}

void PortB::user_unmap(const char * /*system_port*/)
{

}

void PortB::user_start()
{

}

void PortB::user_stop()
{

}

void PortB::outgoing_send(const OCTETSTRING& send_par)
{
	incoming_message(oct2str(send_par));
}

} /* end of namespace */

