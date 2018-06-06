/***************************************************************************
 *   Copyright (C) 2012 by root   *
 *   root@localhost.localdomain   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef BASELOCK_H
#define BASELOCK_H

#include <stdio.h>

#if defined(WIN32)
#include <afx.h>
#endif
using namespace std;
/**

 */
class BaseLock
{
    public:
        void lock();
        void unlock();   
        BaseLock & operator = (const BaseLock& RightSides)  ;
        BaseLock(const BaseLock& C);  

    public:
        BaseLock();
        virtual ~BaseLock();

    public:

    #if defined(WIN32)
    HANDLE          m_mutex;

    #else
     pthread_mutex_t *m_mutex;
    #endif
 
};


#endif
