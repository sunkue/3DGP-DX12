#include "stdafx.h"
#include "GameFramework.h"

_Use_decl_annotations_
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int showCmd)
{
#ifdef _DEBUG
#endif // _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	{
		AllocConsole();
		FILE* file;
		_tfreopen_s(&file, _T("CONOUT$"), _T("w"), stdout);
		_tfreopen_s(&file, _T("CONOUT$"), _T("w"), stderr);
	}
	cout << "2016182024 wmwwmmm@naver.com 윤선규" << "\n";
	cout << "=========================================\n";
	cout << "조작키 : 좌우 방향키\n";
	cout << "=========================================\n";

	cout << "=========================================\n";

	try
	{
		GameFramework application{ hInstance, showCmd };

		if (!application.Initialize())
		{
			throw DxException();
			return 1;
		}

		return application.Run();
	}
	catch (const DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 1;
	}
	catch (const exception* e)
	{
		cout << "( " << e->what() << " ) exception";
		return 1;
	}

	assert(0);
}