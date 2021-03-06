/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Lovassy, Arpad
 *
 ******************************************************************************/
// This Test Port skeleton header file was generated by the
// TTCN-3 Compiler of the TTCN-3 Test Executor version 1.8.pre3 build 4
// for Attila Balasko (ethbaat@ehubuux110) on Wed Jun  2 15:51:57 2010


// You may modify this file. Add your attributes and prototypes of your
// member functions here.

#ifndef PCO__PT_HH
#define PCO__PT_HH

#include "Titan_LogTestDefinitions.hh"

namespace Titan__LogTestDefinitions {

class PCO__PT : public PCO__PT_BASE {
public:
	PCO__PT(const char *par_port_name = NULL);
	~PCO__PT();

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

	void outgoing_send(const CHARSTRING& send_par);
};

} /* end of namespace */

#endif
