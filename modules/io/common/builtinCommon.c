/*
 * Copyright (c) 2019-2020  Moddable Tech, Inc.
 *
 *   This file is part of the Moddable SDK Runtime.
 *
 *   The Moddable SDK Runtime is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   The Moddable SDK Runtime is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with the Moddable SDK Runtime.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "xsmc.h"
#include "mc.xs.h"			// for xsID_ values

#include "xsHost.h"
#include "builtinCommon.h"


#ifdef __ets__
	static uint32_t gDigitalAvailable[kPinBanks] = {
		(1 <<  0) |
		(1 <<  1) |
		(1 <<  2) |
		(1 <<  3) |
		(1 <<  4) |
		(1 <<  5) |
		(1 << 12) |
		(1 << 13) |
		(1 << 14) |
		(1 << 15) |
		(1 << 16)
	};
#elif ESP32
//@@
#else
	#error - unsupported platform
#endif

uint8_t builtinArePinsFree(uint8_t bank, uint32_t pins)
{
	return (pins == (gDigitalAvailable[bank] & pins)) ? 1 : 0;
}

uint8_t builtinUsePins(uint8_t bank, uint32_t pins)
{
	if (pins != (gDigitalAvailable[bank] & pins))
		return 0;

	gDigitalAvailable[bank] &= ~pins;
	return 1;
}

void builtinFreePins(uint8_t bank, uint32_t pins)
{
	gDigitalAvailable[bank] |= pins;
}

uint8_t builtinHasCallback(xsMachine *the, xsIndex id)
{
	xsSlot slot;

	xsmcGet(slot, xsArg(0), id);
	return xsmcTest(slot);
}

uint8_t builtinGetCallback(xsMachine *the, xsIndex id, xsSlot *slot)
{
	xsmcGet(*slot, xsArg(0), id);
	return xsmcTest(*slot);
}

void builtinGetFormat(xsMachine *the, uint8_t format)
{
	if (kIOFormatNumber == format)
		xsmcSetString(xsResult, "number");
	else if (kIOFormatBuffer == format)
		xsmcSetString(xsResult, "buffer");
	else if (kIOFormatStringASCII == format)
		xsmcSetString(xsResult, "string;ascii");
	else if (kIOFormatStringUTF8 == format)
		xsmcSetString(xsResult, "string;utf8");
	else if (kIOFormatSocketTCP == format)
		xsmcSetString(xsResult, "socket/tcp");
	else
		xsRangeError("bad format");
}

uint8_t builtinSetFormat(xsMachine *the)
{
	char *format = xsmcToString(xsArg(0));

	if (!c_strcmp("number", format))
		return kIOFormatNumber;
	if (!c_strcmp("buffer", format))
		return kIOFormatBuffer;
	if (!c_strcmp("string;ascii", format))
		return kIOFormatStringASCII;
	if (!c_strcmp("string;utf8", format))
		return kIOFormatStringUTF8;
	if (!c_strcmp("socket/tcp", format))
		return kIOFormatSocketTCP;
	xsRangeError("unimplemented");
}

void builtinInitializeTarget(xsMachine *the)
{
	if (xsmcHas(xsArg(0), xsID_target)) {
		xsSlot target;

		xsmcGet(target, xsArg(0), xsID_target);
		xsmcSet(xsThis, xsID_target, target);
	}
}

uint8_t builtinInitializeFormat(xsMachine *the, uint8_t format)
{
	if (xsmcHas(xsArg(0), xsID_format)) {
		xsSlot slot;
		char *fmt;

		xsmcGet(slot, xsArg(0), xsID_format);
		if (!xsmcTest(slot))
			return format;

		fmt = xsmcToString(slot);
		if (!c_strcmp("number", fmt))
			return kIOFormatNumber;
		if (!c_strcmp("buffer", fmt))
			return kIOFormatBuffer;
		if (!c_strcmp("string;ascii", fmt))
			return kIOFormatStringASCII;
		if (!c_strcmp("string;utf8", fmt))
			return kIOFormatStringUTF8;
		if (!c_strcmp("socket/tcp", fmt))
			return kIOFormatSocketTCP;
		return kIOFormatInvalid;
	}

	return format;
}
