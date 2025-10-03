#include "state.h"
#include "constants.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/backends/imgui_impl_win32.h"
#include <d3d11.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int w = 200;

state* s;
ID3D11ShaderResourceView* pieceTextures[13];
int selectedCell = -1;

uint64_t possibleMoves;

int initDraw();

int localGuiDraw() {
    s = new state();
    s->init();

    initDraw();

    return 0;
}

void drawChessBoard() {
    float cellSize = (w - 20) / 8;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            int i = y * 8 + x;
            ImVec2 topLeft{ x * cellSize, (7 - y) * cellSize };
            ImVec2 bottomRight{ topLeft.x + cellSize, topLeft.y + cellSize };

            ImU32 col = ((x + y) % 2 == 0) ? IM_COL32(240, 217, 181, 255) : IM_COL32(181, 136, 99, 255);
            ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, col);

            ImTextureID tex = 0;
            switch (s->getPieceType(i)) {
            case constants::piece::pawn:
                tex = getBB(s->whitePieces, i) ? (ImTextureID)pieceTextures[0] : (ImTextureID)pieceTextures[6];
                break;
            case constants::piece::rook:
                tex = getBB(s->whitePieces, i) ? (ImTextureID)pieceTextures[1] : (ImTextureID)pieceTextures[7];
                break;
            case constants::piece::knight:
                tex = getBB(s->whitePieces, i) ? (ImTextureID)pieceTextures[2] : (ImTextureID)pieceTextures[8];
                break;
            case constants::piece::bishop:
                tex = getBB(s->whitePieces, i) ? (ImTextureID)pieceTextures[3] : (ImTextureID)pieceTextures[9];
                break;
            case constants::piece::queen:
                tex = getBB(s->whitePieces, i) ? (ImTextureID)pieceTextures[4] : (ImTextureID)pieceTextures[10];
                break;
            case constants::piece::king:
                tex = getBB(s->whitePieces, i) ? (ImTextureID)pieceTextures[5] : (ImTextureID)pieceTextures[11];
                break;
            }
            if (tex)
                ImGui::GetWindowDrawList()->AddImage(tex, topLeft, bottomRight);

            if (getBB(possibleMoves, i)) {
                ImTextureID tex = (ImTextureID)pieceTextures[12];
                ImGui::GetWindowDrawList()->AddImage(tex, topLeft, bottomRight);
            }

            /*if (getBB(s->core.onTakeWhite, i))
                ImGui::GetWindowDrawList()->AddRect(topLeft, bottomRight, IM_COL32(255, 255, 255, 255), 0.f, 0, 6.f);

            if (getBB(s->core.onTakeBlack, i))
                ImGui::GetWindowDrawList()->AddRect(topLeft, bottomRight, IM_COL32(0, 0, 0, 255), 0.f, 0, 3.f);*/

            if (i == s->core.lastMove[0])
                ImGui::GetWindowDrawList()->AddRect(topLeft, bottomRight, IM_COL32(0, 0, 255, 255), 0.f, 0, 3.f);
            if (i == s->core.lastMove[1])
                ImGui::GetWindowDrawList()->AddRect(topLeft, bottomRight, IM_COL32(0, 0, 200, 255), 0.f, 0, 3.f);

#ifdef _DEBUG
            if (i == s->bestMove[0])
                ImGui::GetWindowDrawList()->AddRect(topLeft, bottomRight, IM_COL32(0, 255, 0, 255), 0.f, 0, 3.f);
            if (i == s->bestMove[1])
                ImGui::GetWindowDrawList()->AddRect(topLeft, bottomRight, IM_COL32(0, 200, 0, 255), 0.f, 0, 3.f);

            if (selectedCell >= 0) {
                if (getBB(s->core.attacks[selectedCell], i))
                    ImGui::GetWindowDrawList()->AddRect(topLeft, bottomRight, IM_COL32(255, 0, 0, 255), 0.f, 0, 3.f);
            }
#endif

            if (i == selectedCell)
                ImGui::GetWindowDrawList()->AddRect(topLeft, bottomRight, IM_COL32(255, 255, 0, 255), 0.f, 0, 3.f);

            char id[32];
            sprintf_s(id, "cell_%d", i);
            ImGui::SetCursorScreenPos(topLeft);
            if (ImGui::InvisibleButton(id, ImVec2{ cellSize, cellSize })) {
                if (selectedCell >= 0 && getBB(s->occupied, selectedCell) && getBB(possibleMoves, i)) {
                    s->makeMove(selectedCell, i);
                    selectedCell = -1;
                    possibleMoves = 0;
                }
                else {
                    selectedCell = i;
                    possibleMoves = s->getPossibleMoves(selectedCell);
                }
            }
        }
    }
}

void handleKeyDown(MSG msg) {
    switch (msg.wParam) {
    case 'Z':
        if (GetKeyState(VK_CONTROL) & 0x8000)
            s->undoMove(true);
    }
}

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool g_SwapChainOccluded = false;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

ID3D11ShaderResourceView* LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const char* filename)
{
    int width, height, channels;
    unsigned char* imageData = stbi_load(filename, &width, &height, &channels, 4); // forziamo RGBA
    if (!imageData)
        return nullptr;

    // Descrizione della texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    // Subresource data
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = imageData;
    subResource.SysMemPitch = width * 4;
    subResource.SysMemSlicePitch = 0;

    // Creazione della texture 2D
    ID3D11Texture2D* texture = nullptr;
    HRESULT hr = device->CreateTexture2D(&desc, &subResource, &texture);
    if (FAILED(hr)) {
        stbi_image_free(imageData);
        return nullptr;
    }

    // Creazione dello shader resource view
    ID3D11ShaderResourceView* textureView = nullptr;
    hr = device->CreateShaderResourceView(texture, nullptr, &textureView);
    texture->Release(); // lo SRV tiene il reference

    stbi_image_free(imageData);

    return textureView;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        w = g_ResizeWidth;
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

int initDraw(){
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    RegisterClassExW(&wc);
    HWND hwnd = CreateWindowW(wc.lpszClassName, L"HxTChess", WS_OVERLAPPEDWINDOW, 100, 100, (int)(w * main_scale), (int)((w + 50) * main_scale), nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    style.FontSizeBase = 12.0f;
    io.Fonts->AddFontDefault();

    //Load pieces png
    pieceTextures[0] = LoadTextureFromFile(g_pd3dDevice, g_pd3dDeviceContext, "pieces/wp.png");
    pieceTextures[1] = LoadTextureFromFile(g_pd3dDevice, g_pd3dDeviceContext, "pieces/wr.png");
    pieceTextures[2] = LoadTextureFromFile(g_pd3dDevice, g_pd3dDeviceContext, "pieces/wn.png");
    pieceTextures[3] = LoadTextureFromFile(g_pd3dDevice, g_pd3dDeviceContext, "pieces/wb.png");
    pieceTextures[4] = LoadTextureFromFile(g_pd3dDevice, g_pd3dDeviceContext, "pieces/wq.png");
    pieceTextures[5] = LoadTextureFromFile(g_pd3dDevice, g_pd3dDeviceContext, "pieces/wk.png");
    pieceTextures[6] = LoadTextureFromFile(g_pd3dDevice, g_pd3dDeviceContext, "pieces/bp.png");
    pieceTextures[7] = LoadTextureFromFile(g_pd3dDevice, g_pd3dDeviceContext, "pieces/br.png");
    pieceTextures[8] = LoadTextureFromFile(g_pd3dDevice, g_pd3dDeviceContext, "pieces/bn.png");
    pieceTextures[9] = LoadTextureFromFile(g_pd3dDevice, g_pd3dDeviceContext, "pieces/bb.png");
    pieceTextures[10] = LoadTextureFromFile(g_pd3dDevice, g_pd3dDeviceContext, "pieces/bq.png");
    pieceTextures[11] = LoadTextureFromFile(g_pd3dDevice, g_pd3dDeviceContext, "pieces/bk.png");

    pieceTextures[12] = LoadTextureFromFile(g_pd3dDevice, g_pd3dDeviceContext, "pieces/hint.png");


    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            switch (msg.message) {
            case WM_QUIT:
                done = true;
                break;
            case WM_KEYDOWN:
                handleKeyDown(msg);
            }
        }
        if (done)
            break;

        // Handle window being minimized or screen locked
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::SetNextWindowPos(ImVec2{ 0.f, 0.f });
            ImGui::SetNextWindowSize(ImVec2{ io.DisplaySize.x , io.DisplaySize.y });

            ImGui::Begin("ChessBoard", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
            drawChessBoard();
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, new const float[4]{1.f, 1.f, 1.f, 1.f});
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present
        HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
        //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}