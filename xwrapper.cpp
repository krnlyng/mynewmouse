/*
   Mobile Mouse Linux Server 
   Copyright (C) 2011 Erik Lax <erik@datahack.se>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "xwrapper.hpp"

#include <stdexcept>
#include <stdlib.h>

#include <X11/extensions/XTest.h>

#include <iostream>

XMouseInterface::XMouseInterface(const std::string display)
{
	if ((m_display = XOpenDisplay(display.empty()?NULL:display.c_str())) == NULL)
	{
		// sorry for throwing in constructor...
		throw std::runtime_error("cannot open xdisplay");
	}
}

XMouseInterface::~XMouseInterface()
{
	if (m_display)
	{
		XCloseDisplay(m_display);
	}
}

void XMouseInterface::MouseLeft(MouseState state)
{
	XTestFakeButtonEvent(m_display, 1,
			state==BTN_DOWN?True:False,
			CurrentTime);
	XFlush(m_display);
}

void XMouseInterface::MouseRight(MouseState state)
{
	XTestFakeButtonEvent(m_display, 3,
			state==BTN_DOWN?True:False,
			CurrentTime);
	XFlush(m_display);
}

void XMouseInterface::MouseWheelY(int offset)
{
	for(int i = abs(offset); i > 0; i--)
	{
		XTestFakeButtonEvent(m_display, offset>0?5:4,
				True,
				CurrentTime);
		XTestFakeButtonEvent(m_display, offset>0?5:4,
				False,
				CurrentTime);
	}
	XFlush(m_display);
}

void XMouseInterface::MouseMove(int x, int y)
{
	XTestFakeRelativeMotionEvent(m_display, x, y, CurrentTime);
	XFlush(m_display);
}

XKeyboardInterface::XKeyboardInterface(const std::string display)
{
	if ((m_display = XOpenDisplay(display.empty()?NULL:display.c_str())) == NULL)
	{
		// sorry for throwing in constructor...
		throw std::runtime_error("cannot open xdisplay");
	}
}

XKeyboardInterface::~XKeyboardInterface()
{
	if (m_display)
	{
		XCloseDisplay(m_display);
	}
}

void XKeyboardInterface::SendKey(int keycode)
{
	std::list<int> keys;
	keys.push_back(keycode);
	SendKey(keys);
}

void XKeyboardInterface::SendKey(const std::list<int>& keycode)
{
	for(std::list<int>::const_iterator i = keycode.begin();
			i != keycode.end(); i++)
	{
		KeyCode key = XKeysymToKeycode(m_display, *i);
		if(key == NoSymbol) continue;

		XTestFakeKeyEvent(m_display, key, True, CurrentTime);
	}
	for(std::list<int>::const_reverse_iterator i = keycode.rbegin();
			i != keycode.rend(); i++)
	{
		KeyCode key = XKeysymToKeycode(m_display, *i);
		if(key == NoSymbol) continue;

		XTestFakeKeyEvent(m_display, key, False, CurrentTime);
	}
	XFlush(m_display);
}

void XKeyboardInterface::PressKey(int keycode)
{
	KeyCode key = XKeysymToKeycode(m_display, keycode);
	if(key == NoSymbol)
	{
		//std::cout << keycode << " is not a symbol." << std::endl;
		return;
	}

	XTestFakeKeyEvent(m_display, key, True, CurrentTime);
	XFlush(m_display);
}


void XKeyboardInterface::ReleaseKey(int keycode)
{
	KeyCode key = XKeysymToKeycode(m_display, keycode);
	if(key == NoSymbol)
	{
		//std::cout << keycode << " is not a symbol." << std::endl;
		return;
	}

	XTestFakeKeyEvent(m_display, key, False, CurrentTime);
	XFlush(m_display);
}


void XKeyboardInterface::PressShift()
{
	XTestFakeKeyEvent(m_display, XKeysymToKeycode(m_display, XK_Shift_L), True, CurrentTime);
}

void XKeyboardInterface::ReleaseShift()
{
	XTestFakeKeyEvent(m_display, XKeysymToKeycode(m_display, XK_Shift_L), False, CurrentTime);
}

void XKeyboardInterface::PressCtrl()
{
	XTestFakeKeyEvent(m_display, XKeysymToKeycode(m_display, XK_Control_L), True, CurrentTime);
}

void XKeyboardInterface::ReleaseCtrl()
{
	XTestFakeKeyEvent(m_display, XKeysymToKeycode(m_display, XK_Control_L), False, CurrentTime);
}

void XKeyboardInterface::PressAltL()
{
	XTestFakeKeyEvent(m_display, XKeysymToKeycode(m_display, XK_Alt_L), True, CurrentTime);
}

void XKeyboardInterface::ReleaseAltL()
{
	XTestFakeKeyEvent(m_display, XKeysymToKeycode(m_display, XK_Alt_L), False, CurrentTime);
}

void XKeyboardInterface::PressAltR()
{
	XTestFakeKeyEvent(m_display, XKeysymToKeycode(m_display, XK_Alt_R), True, CurrentTime);
}

void XKeyboardInterface::ReleaseAltR()
{
	XTestFakeKeyEvent(m_display, XKeysymToKeycode(m_display, XK_Alt_R), False, CurrentTime);
}

void XKeyboardInterface::PressMeta()
{
	XTestFakeKeyEvent(m_display, XKeysymToKeycode(m_display, XK_Home), True, CurrentTime);
}

void XKeyboardInterface::ReleaseMeta()
{
	XTestFakeKeyEvent(m_display, XKeysymToKeycode(m_display, XK_Home), False, CurrentTime);
}

