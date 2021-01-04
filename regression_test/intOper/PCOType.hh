/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Kovacs, Ferenc
 *   Lovassy, Arpad
 *   Raduly, Csaba
 *
 ******************************************************************************/
// This Test Port skeleton header file was generated by the
// TTCN-3 Compiler of the TTCN-3 Test Executor version 1.7.pre4 build 5
// for  (eferkov@E0017083EF5F5) on Wed Aug 20 21:06:45 2008

// You may modify this file. Add your attributes and prototypes of your
// member functions here.

#ifndef PCOType_HH
#define PCOType_HH

#include "TintOper.hh"

#ifndef OLD_NAMES
namespace TintOper {
#endif

class PCOType : public PCOType_BASE {
public:
	PCOType(const char *par_port_name = NULL);
	~PCOType();

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
	void user_map(const char *system_port);
	void user_unmap(const char *system_port);

	void user_start();
	void user_stop();

	void outgoing_send(const INTEGER& send_par);
	void outgoing_send(const eint1& send_par);
	void outgoing_send(const rint1& send_par);
};

#ifndef OLD_NAMES
} /* end of namespace */
#endif

#endif
