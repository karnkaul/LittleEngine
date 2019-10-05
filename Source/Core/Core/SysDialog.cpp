#include "SysDialog.h"
#include "Logger.h"
#include "OS.h"
#if defined(TARGET_WIN64)
#include <Windows.h>
#elif defined(TARGET_LINUX)
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

namespace Core
{
namespace
{
DialogueData dialogueData;

void LaunchMessageBox()
{
#if _WIN64
	UINT type = MB_CANCELTRYCONTINUE;

	std::string footer = dialogueData.footer.empty() ? std::string() : "\n" + std::move(dialogueData.footer);
	std::string suffix = "\n\nCancel\t: " + std::move(dialogueData.resp0.text) + "\nTry\t: " + std::move(dialogueData.resp1.text)
					+ "\nCont\t: " + std::move(dialogueData.resp2.text);
	std::string content = std::move(dialogueData.content) + std::move(footer) + std::move(suffix);
	s32 id = MessageBoxA(nullptr, content.c_str(), dialogueData.title.c_str(), type);
	switch (id)
	{
	case IDCANCEL:
		if (dialogueData.resp0.callback)
		{
			dialogueData.resp0.callback();
		}
		break;

	case IDTRYAGAIN:
		if (dialogueData.resp1.callback)
		{
			dialogueData.resp1.callback();
		}
		break;

	case IDCONTINUE:
		if (dialogueData.resp2.callback)
		{
			dialogueData.resp2.callback();
		}
		break;
	}
#elif __linux__
	Window w;
	XEvent e;
	s32 s;

	Display* pD = XOpenDisplay(nullptr);
	if (pD)
	{
		s = DefaultScreen(pD);
		w = XCreateSimpleWindow(pD, RootWindow(pD, s), 200, 200, 700, 250, 1, BlackPixel(pD, s), WhitePixel(pD, s));
		XSelectInput(pD, w, ExposureMask | KeyPressMask);
		Atom wmDeleteMessage = XInternAtom(pD, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(pD, w, &wmDeleteMessage, 1);
		XMapWindow(pD, w);

		const KeySym sym_1 = XStringToKeysym("1");
		const s32 sym_1_int = ToS32(XKeysymToKeycode(pD, sym_1));

		while (true)
		{
			XNextEvent(pD, &e);
			if (e.type == Expose)
			{
				std::string resp0 = "1: " + dialogueData.resp0.text;
				std::string resp1 = "2: " + dialogueData.resp1.text;
				std::string resp2 = "3: " + dialogueData.resp2.text;
				XDrawString(pD, w, DefaultGC(pD, s), 10, 50, dialogueData.content.c_str(), dialogueData.content.length());
				XDrawString(pD, w, DefaultGC(pD, s), 15, 70, dialogueData.footer.c_str(), dialogueData.footer.length());
				XDrawString(pD, w, DefaultGC(pD, s), 10, 150, resp0.c_str(), resp0.length());
				XDrawString(pD, w, DefaultGC(pD, s), 10, 170, resp1.c_str(), resp1.length());
				XDrawString(pD, w, DefaultGC(pD, s), 10, 190, resp2.c_str(), resp2.length());
			}
			if (e.type == KeyPress || e.type == DestroyNotify)
			{
				s32 key = ToS32(e.xkey.keycode) - sym_1_int;
				LOG_I("Key pressed: %d", key);
				switch (key)
				{
				case 0:
					if (dialogueData.resp0.callback)
					{
						dialogueData.resp0.callback();
					}
					break;

				case 1:
					if (dialogueData.resp1.callback)
					{
						dialogueData.resp1.callback();
					}
					break;
				case 2:
					if (dialogueData.resp2.callback)
					{
						dialogueData.resp2.callback();
					}
					break;
				default:
					break;
				}
				break;
			}
		}
		XCloseDisplay(pD);
	}
#elif __APPLE__ && __MACH__
	// TODO: call into Cocoa via .mm file
	return;
#endif
}
} // namespace

void CreateSystemDialogue(DialogueData data)
{
	dialogueData = std::move(data);
	LaunchMessageBox();
}
} // namespace Core
