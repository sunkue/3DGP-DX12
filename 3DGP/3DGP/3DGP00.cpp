#include "stdafx.h"
#include "GameFramework.h"


int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR cmdLine,
	_In_ int nShowCmd
)
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

	cout << "" << "\n";

	try
	{
		GameFramework application{ hInstance };

		if (!application.Initialize())
		{
			return 0;
		}

		return application.Run();
	}
	catch (const DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
	catch (const exception* e)
	{
		cout << "( "<< e->what() <<" ) exception";
		return 0;
	}

	assert(0);
}