#include "stdafx.h"
#include "GameFramework.h"

_Use_decl_annotations_
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	{
		AllocConsole();
		FILE* file;
		_tfreopen_s(&file, _T("CONOUT$"), _T("w"), stdout);
		_tfreopen_s(&file, _T("CONOUT$"), _T("w"), stderr);
	}
	cout << "2016182024 wmwwmmm@naver.com 윤선규" << "\n";
#endif // _DEBUG

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
		cout << "( " << e->what() << " ) exception";
		return 0;
	}

	assert(0);
}