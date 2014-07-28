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
#include <windows.h>
#include "lusb.h"
#include <stdio.h>
#include <initguid.h>

DEFINE_GUID(USB_DEVICE, 0xA5DCBF10, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);

int MyCallback(unsigned inst, const char *_path, void *data)
{
	char path[MAX_PATH];
	int x = UsbGetDevicePortPath(inst, path, MAX_PATH, 0);
	if (x > 0) {
		printf("PortPath: %s\n", path);
		printf("IFaceName: %s\n", _path);
		return 1;
	}
	return 1;
}
int main(int argc, char* argv[])
{
	UsbFindDevice(&USB_DEVICE, MyCallback, NULL);
	return 0;
}
