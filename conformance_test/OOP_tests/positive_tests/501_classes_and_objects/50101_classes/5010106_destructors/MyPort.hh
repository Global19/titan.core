// This Test Port skeleton header file was generated by the
// TTCN-3 Compiler of the TTCN-3 Test Executor version 7/CAX 105 7730 R2A
// for Global Warning (james00@GlobalWarning1) on Tue Nov 10 10:16:29 2020

// Copyright (c) 2000-2021 Ericsson Telecom AB

// You may modify this file. Add your attributes and prototypes of your
// member functions here.

#ifndef MyPort_HH
#define MyPort_HH

#include "Sem_5010106_Destructors_001.hh"

namespace Sem__5010106__Destructors__001 {

class MyPort : public MyPort_BASE {
public:
	MyPort(const char *par_port_name = NULL);
	~MyPort();

	void set_parameter(const char *parameter_name,
		const char *parameter_value);

private:
	/* void Handle_Fd_Event(int fd, boolean is_readable,
		boolean is_writable, boolean is_error); */
	void Handle_Fd_Event_Error(int fd);
	void Handle_Fd_Event_Writable(int fd);
	void Handle_Fd_Event_Readable(int fd);
	/* void Handle_Timeout(double time_since_last_call); */
protected:
	void user_map(const char *system_port, Map_Params& params);
	void user_unmap(const char *system_port, Map_Params& params);

	void user_start();
	void user_stop();

	void outgoing_send(const INTEGER& send_par);
};

} /* end of namespace */

#endif
