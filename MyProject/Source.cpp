#include <d3d9.h> 
#include <d3dx9.h> 

/************************************************************************/
 /*  전역변수 선언                                                       */
 /************************************************************************/

LPDIRECT3D9             g_pD3D = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
LPDIRECT3DINDEXBUFFER9    g_pIB = NULL;

D3DXMATRIXA16            g_matTM;    // TM 
D3DXMATRIXA16            g_matR;        // 회전행렬

D3DXQUATERNION            g_QuaternionRotA;        // Quaternion 구조체 선언
D3DXQUATERNION            g_QuaternionRotB;

float                    g_fRot = 0.0f;

struct CUSTOMVERTEX
{
    FLOAT x, y, z;
    DWORD color;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

struct MYINDEX
{
    WORD    _0, _1, _2;
};

HRESULT InitD3D(HWND hWnd)
{
    if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return E_FAIL;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp, &g_pd3dDevice)))
    {
        return E_FAIL;
    }

    g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    return S_OK;
}

HRESULT InitVB()
{
    CUSTOMVERTEX vertices[] =
    {
        { -1,  1,  1 , 0xffff0000 },
         {  1,  1,  1 , 0xff00ff00 },
         {  1,  1, -1 , 0xff0000ff },
         { -1,  1, -1 , 0xffffff00 },

        { -1, -1,  1 , 0xff00ffff },
         {  1, -1,  1 , 0xffff00ff },
         {  1, -1, -1 , 0xff000000 },
         { -1, -1, -1 , 0xffffffff },
    };

    if (FAILED(g_pd3dDevice->CreateVertexBuffer(8 * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &g_pVB, NULL)))
    {
        return E_FAIL;
    }

    VOID* pVertices;
    if (FAILED(g_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
        return E_FAIL;
    memcpy(pVertices, vertices, sizeof(vertices));
    g_pVB->Unlock();

    return S_OK;
}

HRESULT InitIB()
{
    MYINDEX    indices[] =
    {
        { 0, 1, 2 }, { 0, 2, 3 },
         { 4, 6, 5 }, { 4, 7, 6 },
         { 0, 3, 7 }, { 0, 7, 4 },
         { 1, 5, 6 }, { 1, 6, 2 },
         { 3, 2, 6 }, { 3, 6, 7 },
         { 0, 4, 5 }, { 0, 5, 1 }
    };

    if (FAILED(g_pd3dDevice->CreateIndexBuffer(12 * sizeof(MYINDEX), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL)))
    {
        return E_FAIL;
    }

    VOID* pIndices;
    if (FAILED(g_pIB->Lock(0, sizeof(indices), (void**)&pIndices, 0)))
        return E_FAIL;
    memcpy(pIndices, indices, sizeof(indices));
    g_pIB->Unlock();

    return S_OK;
}

void Quat_Rot()
{
    FLOAT Yaw = D3DX_PI * 90.0f / 180.0f;        /// Y축 90도 회전
    FLOAT Pitch = 0;
    FLOAT Roll = 0;
    D3DXQuaternionRotationYawPitchRoll(&g_QuaternionRotA, Yaw, Pitch, Roll);    /// 사원수 키(Y축90도)

    Yaw = 0;
    Pitch = D3DX_PI * 90.0f / 180.0f;            /// X축 90도 회전
    Roll = 0;
    D3DXQuaternionRotationYawPitchRoll(&g_QuaternionRotB, Yaw, Pitch, Roll);    /// 사원수 키(X축90도)
}

HRESULT InitGeometry()
{
    if (FAILED(InitVB())) return E_FAIL;
    if (FAILED(InitIB())) return E_FAIL;

    Quat_Rot();

    return S_OK;
}

void SetupCamera()
{
    D3DXMATRIXA16 matWorld;
    D3DXMatrixIdentity(&matWorld);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    D3DXVECTOR3 vEyePt(0.0f, 10.0f, -20.0f);
    D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
    g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
    g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

VOID SetupRot()
{
    D3DXQUATERNION quat;

    D3DXMatrixTranslation(&g_matTM, 0.0f, 0.0f, 0.0f);

    // 회전값의 구면선형보간 
    D3DXQuaternionSlerp(&quat, &g_QuaternionRotA, &g_QuaternionRotB, GetTickCount() / 800.f);
    D3DXMatrixRotationQuaternion(&g_matR, &quat);            // 사원수를 회전행렬값으로 변환
}

VOID Cleanup()
{
    if (g_pIB != NULL)
        g_pIB->Release();

    if (g_pVB != NULL)
        g_pVB->Release();

    if (g_pd3dDevice != NULL)
        g_pd3dDevice->Release();

    if (g_pD3D != NULL)
        g_pD3D->Release();
}

void DrawMesh(D3DXMATRIXA16* pMat)
{
    g_pd3dDevice->SetTransform(D3DTS_WORLD, pMat);
    g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
    g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
    g_pd3dDevice->SetIndices(g_pIB);
    g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
}

VOID Render()
{
    D3DXMATRIXA16    matWorld;

    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

    /// 애니메이션 행렬설정 
    SetupRot();

    /// 렌더링 시작 
    if (SUCCEEDED(g_pd3dDevice->BeginScene()))
    {
        matWorld = g_matR * g_matTM;
        DrawMesh(&matWorld);

        g_pd3dDevice->EndScene();
    }

    g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        Cleanup();
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                       GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                       "BasicFrame", NULL };
    RegisterClassEx(&wc);

    HWND hWnd = CreateWindow("BasicFrame", "Keyframe Animation",
        WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
        GetDesktopWindow(), NULL, wc.hInstance, NULL);

    if (SUCCEEDED(InitD3D(hWnd)))
    {
        if (SUCCEEDED(InitGeometry()))
        {
            SetupCamera();

            ShowWindow(hWnd, SW_SHOWDEFAULT);
            UpdateWindow(hWnd);

            MSG msg;
            ZeroMemory(&msg, sizeof(msg));
            while (msg.message != WM_QUIT)
            {
                if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else
                    Render();
            }
        }
    }

    UnregisterClass("D3D Tutorial", wc.hInstance);
    return 0;
}