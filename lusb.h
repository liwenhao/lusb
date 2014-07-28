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
#ifndef _LUSB_H_
#define _LUSB_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*UsbFindCallback)(unsigned inst, const char *path, void *usr_data);

int UsbGetInterfaceNumber(unsigned inst);
int UsbGetDevicePortNumbers(unsigned inst, unsigned char *numbers, int len);
int UsbGetDevicePortPath(unsigned inst, char *path, int len, int full);
int UsbFindDevice(const GUID *guid, UsbFindCallback callback, void *usr_data);

#ifdef __cplusplus
}
#endif

#endif
