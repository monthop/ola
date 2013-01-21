/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * ACNPort.h
 * Contains the UDP port for ACN
 * Copyright (C) 2011 Simon Newton
 */

#ifndef PLUGINS_E131_E131_ACNPORT_H_
#define PLUGINS_E131_E131_ACNPORT_H_

#include <stdint.h>

namespace ola {
namespace plugin {
namespace e131 {

const uint16_t ACN_PORT = 5568;
const uint16_t E133_PORT = 5569;

}  // e131
}  // plugin
}  // ola
#endif  // PLUGINS_E131_E131_ACNPORT_H_
