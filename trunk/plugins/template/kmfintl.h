// **************************************************************************
//   modified kde gettext implementation to read mo from QIODevice
//
//   libintl.cpp -- gettext related functions from glibc-2.0.5
//   Copyright (C) 1995 Software Foundation, Inc.
//
//   gettext.c -- implementation of gettext(3) function
//   Copyright (C) 1995 Software Foundation, Inc.
//
//   Copyright (C) 2004-2006 by Petri Damsten
//   petri.damsten@iki.fi
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the
//   Free Software Foundation, Inc.,
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
// **************************************************************************

#ifndef KMFINTL_H
#define KMFINTL_H

class QIODevice;
struct loaded_domain;

struct kmf_loaded_l10nfile {
    int decided;

    const void *data;

    kmf_loaded_l10nfile() : decided(0), data(0){}
};

void kmf_nl_load_domain(QIODevice *device, int size, struct kmf_loaded_l10nfile *domain_file);
char*kmf_nl_find_msg(const struct kmf_loaded_l10nfile *domain_file, const char *msgid);
void kmf_nl_unload_domain(struct loaded_domain *domain);

#endif // KMFINTL_H
