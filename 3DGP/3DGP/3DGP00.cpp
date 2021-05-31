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
	cout << "셰이더를 통해 멀리있는 오브젝트의 색상을 흐릿하게표현.\n";
	cout << "플레이어의 속도는 시간이 지나며 한계까지 증가\n";
	cout << "플레이어의 속도는 오브젝트와 충돌시 감소\n";
	cout << "플레이어좌우 이동시 회전효과, 시간이 지나며 원상복귀. \n";
	cout << "플레이어 충돌시 픽셀셰이더를 사용한 이펙트효과. (오브젝트 : 검정)(벽 : 하양)\n";
	cout << "플레이어 충돌 이펙트 구현을 위해 SRV 를 만들어서 사용했음(Effect.h.cpp)\n";
	cout << "한 프레임에 이동 및 충돌을 4번 수행하여 통과 현상을 방지했음 덕분에 얇은 벽으로도 정확한 충돌 구현이 가능했음\n";
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
		return 0;
	}
	catch (const exception* e)
	{
		cout << "( " << e->what() << " ) exception";
		return 0;
	}

	assert(0);
}