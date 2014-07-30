lusb
====

Light weight usb wrapper for windows, easy way to get usb device path(hub port path)

## To Build

[Download FT Jam](http://freetype.org/jam/index.html#where-ftjam)

MS Visual C
```
> set JAM_TOOLSET=VISUALC
> jam
```

MINGW
```
$ JAM_TOOLSET=MINGW jam
```
## Example

Print port path of all USB devices:

```C
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
```

## TODO
Dynamic load denpendend windows api
