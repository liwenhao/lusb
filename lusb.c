/*
 * Copyright (C) 2014 Marvell International Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <Windows.h>

#include "lusb.h"

#include <Cfgmgr32.h>
#include <setupapi.h>
#include <string.h>
#include <stdio.h>

#define snprintf _snprintf

int UsbGetInterfaceNumber(unsigned inst)
{
	char path[MAX_PATH], *p;
	int number = -1;

	if (CM_Get_Device_IDA(inst, path, sizeof(path), 0) != CR_SUCCESS)
		return -2;

	p = strstr(path, "MI_");
	if (p) {
		number = (p[3] - '0')*10;
		number += p[4] - '0';
	}

	return number;
}

int UsbGetDevicePortNumbers(unsigned inst, unsigned char *numbers, int len)
{
	DWORD node, port, size;
	int ret, iface, i;

	/* composite device? */
	iface = UsbGetInterfaceNumber(inst);
	if (iface >= 0) {
		if (CM_Get_Parent(&node, inst, 0) != CR_SUCCESS)
			return -1;
	} else
		node = inst;

	/* track to root */
	for(i = len-1; i >= 0; i--) {
		size = 4;
		if (CM_Get_DevNode_Registry_PropertyA(node, CM_DRP_ADDRESS,
				NULL, (BYTE*)&port, &size, 0) != CR_SUCCESS)
			return -2;
		numbers[i] = (unsigned char)port;
		if (port == 0) {
			if (i == len-1)
				i--;
			break;
		}
		if (CM_Get_Parent(&node, node, 0) != CR_SUCCESS)
			return -1;
	}

	if (i < 0)
		return -3;

	ret = len - ++i;
	memmove(numbers, &numbers[i], ret);

	return ret;
}

int UsbGetDevicePortPath(unsigned inst, char *path, int len, int full)
{
	unsigned char ports[7];
	int i, l, r;
	char *p = path;

	r = UsbGetDevicePortNumbers(inst, ports, 7);
	if (r <= 0)
		return -1;

	l = snprintf(p, len-(p-path), "%d", ports[0]);
	p += l;
	for (i = 1; i < r; i++) {
		l = snprintf(p, len-(p-path), "-%d", ports[i]);
		p += l;
	}

	if (full) {
		r = UsbGetInterfaceNumber(inst);
		if (r >= 0)
			snprintf(p, len-(p-path), ":%d", r);
	}

	return strlen(path);
}

int
UsbFindDevice(const GUID *guid, UsbFindCallback callback, void *usr_data)
{
	int ret, index;
	SP_DEVINFO_DATA info;
	SP_DEVICE_INTERFACE_DATA ifdata;
	HDEVINFO devs;
	PSP_DEVICE_INTERFACE_DETAIL_DATA_A detail = NULL;
	DWORD size, flags = DIGCF_DEVICEINTERFACE|DIGCF_PRESENT;

	info.cbSize = sizeof(SP_DEVINFO_DATA);
	ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	devs = SetupDiGetClassDevs(guid, NULL, NULL, flags);
	if (devs == INVALID_HANDLE_VALUE)
		return -1;

	for (index = 0; ;index++) {
		info.cbSize = sizeof(SP_DEVINFO_DATA);
		if (!SetupDiEnumDeviceInfo(devs, index, &info)) {
			if (GetLastError() != ERROR_NO_MORE_ITEMS) {
				ret = -2;
				goto out;
			} else
				break;
		}

		if (!SetupDiEnumDeviceInterfaces(devs, 0, guid, index, &ifdata)) {
			ret = -3;
			goto out;
		}

		if (!detail) {
			SetupDiGetDeviceInterfaceDetailA(devs, &ifdata, NULL,
				0, &size, NULL);
			detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A)malloc(size);
			detail->cbSize = sizeof(*detail);
		}

		if (!SetupDiGetDeviceInterfaceDetailA(devs, &ifdata, detail, size,
				NULL, NULL)) {
			ret = -4;
			goto out;
		}

		if (!callback(info.DevInst, detail->DevicePath, usr_data)) {
			ret = index;
			goto out;
		}

		free(detail);
		detail = NULL;
	}
	ret = index;
 out:
	SetupDiDestroyDeviceInfoList(devs);
	if (detail)
		free(detail);

	return ret;
}
