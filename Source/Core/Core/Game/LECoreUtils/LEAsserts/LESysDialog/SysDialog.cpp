#include "SysDialog.h"
#if _MSC_VER
#include <Windows.h>
#elif __linux__
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

namespace LE
{
namespace
{
DialogueData dialogueData;

void LaunchMessageBox()
{
#if defined(_WIN64) || defined(_WIN32)
	UINT type = MB_CANCELTRYCONTINUE;

	std::string footer = dialogueData.footer.empty() ? std::string() : "\n" + std::move(dialogueData.footer);
	std::string suffix = "\n\nCancel\t: " + std::move(dialogueData.resp0.text) + "\nTry\t: " + std::move(dialogueData.resp1.text)
						 + "\nCont\t: " + std::move(dialogueData.resp2.text);
	std::string content = std::move(dialogueData.content) + std::move(footer) + std::move(suffix);
	auto id = MessageBoxA(nullptr, content.data(), dialogueData.title.data(), type);
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

	Display* pD = XOpenDisplay(nullptr);
	if (pD)
	{
		auto s = DefaultScreen(pD);
		w = XCreateSimpleWindow(pD, RootWindow(pD, s), 200, 200, 700, 250, 1, BlackPixel(pD, s), WhitePixel(pD, s));
		XSelectInput(pD, w, ExposureMask | KeyPressMask);
		Atom wmDeleteMessage = XInternAtom(pD, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(pD, w, &wmDeleteMessage, 1);
		XMapWindow(pD, w);

		const KeySym sym_1 = XStringToKeysym("1");
		const auto sym_1_int = XKeysymToKeycode(pD, sym_1);

		while (true)
		{
			XNextEvent(pD, &e);
			if (e.type == Expose)
			{
				std::string resp0 = "1: " + dialogueData.resp0.text;
				std::string resp1 = "2: " + dialogueData.resp1.text;
				std::string resp2 = "3: " + dialogueData.resp2.text;
				XDrawString(pD, w, DefaultGC(pD, s), 10, 50, dialogueData.content.data(), dialogueData.content.length());
				XDrawString(pD, w, DefaultGC(pD, s), 15, 70, dialogueData.footer.data(), dialogueData.footer.length());
				XDrawString(pD, w, DefaultGC(pD, s), 10, 150, resp0.data(), resp0.length());
				XDrawString(pD, w, DefaultGC(pD, s), 10, 170, resp1.data(), resp1.length());
				XDrawString(pD, w, DefaultGC(pD, s), 10, 190, resp2.data(), resp2.length());
			}
			if (e.type == KeyPress || e.type == DestroyNotify)
			{
				auto key = e.xkey.keycode - sym_1_int;
				switch (static_cast<uint32_t>(key))
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
#endif
}
} // namespace

void CreateSystemDialogue(DialogueData data)
{
	dialogueData = std::move(data);
	LaunchMessageBox();
}
} // namespace LE
