/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Baranyi, Botond
 *
 ******************************************************************************/

#include "Resources.hh"

namespace Resources {

static INTEGER current_number(0);

INTEGER ef__next__number()
{
  return ++current_number;
}

void ef__reset()
{
  current_number = 0;
}

BITSTRING ef__enc__uni(const Uni& x) {
  return int2bit(x.i(), 8);
}

}
