#include "stdafx.h"
#include "GameFramework.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR cmdLine,
	int cmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	// 콘솔 창
	{
		AllocConsole();
		FILE* file;
		_tfreopen_s(&file, _T("CONOUT$"), _T("w"), stdout);
		_tfreopen_s(&file, _T("CONOUT$"), _T("w"), stderr);
	}

	cout << "";

	GameFramework APP(hInstance);

	try
	{
		if (!APP.Initialize())
		{
			return FALSE;
		}

		return APP.Run();
	}
	catch (const DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}