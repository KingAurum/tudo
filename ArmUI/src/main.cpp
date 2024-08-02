// Dear ImGui: standalone example application for DirectX 9

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "quaternion.h"
#include "SerialPort.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
using namespace std;

// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Received values:
static bool done = true;

//Sended values:
long degreeBAS = 10;
long degreeOMB = 60;
long degreeCOT = 10;
long degreePUL = 10;
long degreeALL[] = { degreeBAS, degreeOMB, degreeCOT, degreePUL };
bool clutching = false;
bool sendingDegree = true;

// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Arm Controller", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Arm Controller", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.50f, 0.10f, 0.10f, 1.00f);

    // Main loop
    bool doneLoop = false;
    while (!doneLoop)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                doneLoop = true;
        }
        if (doneLoop)
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        #pragma region My Stuff
        //vvvvvv~~IMPORTANTE~~vvvvvv
        //Use Firmware 4!
        //^^^^^^~~IMPORTANTE~~^^^^^^
        string dataString = "0";
        static char outputSerial[MAX_DATA_LENGTH];
        //vvvvvv~~IMPORTANTE~~vvvvvv
        static const char* port = "\\\\.\\COM4";    //Ver qual entrada é usada!
        //^^^^^^~~IMPORTANTE~~^^^^^^
        static char incomingSerial[MAX_DATA_LENGTH];
        static SerialPort arduino(port);
        //if (arduino.isConnected()) cout << "Port connected." << endl << endl; else cout << "Error on port connection." << endl << endl;

        //while (arduino.isConnected()) {
        while (false) {
            cout << "Enter your command: " << endl;
            string data;
            cin >> data;

            char* charArray = new char[data.size() + 1];
            copy(data.begin(), data.end(), charArray);
            charArray[data.size()] = '\n';

            arduino.writeSerialPort(charArray, MAX_DATA_LENGTH);
            arduino.readSerialPort(outputSerial, MAX_DATA_LENGTH);

            cout << ">> " << outputSerial << endl;

            delete[] charArray;
        }

        static bool startMove = false;
        static float degPri[] = { 0.0f, 0.0f, 0.0f };
        static float degTgt[] = { 0.0f, 0.0f, 0.0f };
        static float degAux[] = { 0.0f, 0.0f, 0.0f };
        static float posPri[] = { 0.0f, 0.0f, 0.0f };
        static float posTgt[] = { 0.0f, 0.0f, 0.0f };
        static float posAux[] = { 0.0f, 0.0f, 0.0f };
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
        if (ImGui::Begin("Arm Control", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar)) {
            if (ImGui::Button("Start using Degrees")) {
                degPri[0] = degTgt[0]; degPri[1] = degTgt[1]; degPri[2] = degTgt[2];
                sendingDegree = true;
                startMove = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Start using XYZ")) {
                posPri[0] = posTgt[0]; posPri[1] = posTgt[1]; posPri[2] = posTgt[2];
                sendingDegree = false;
                startMove = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset")) {
                degTgt[0] = 0; degTgt[1] = 0; degTgt[2] = 0;
                degAux[0] = 0; degAux[1] = 0; degAux[2] = 0;

                posTgt[0] = 0; posTgt[1] = 0; posTgt[2] = 0;
                posAux[0] = 0; posAux[1] = 0; posAux[2] = 0;
            }
            ImGui::SameLine();
            ImGui::Checkbox("Done", &done);

            ImGui::Text("Degrees");
            ImGui::Text(""); ImGui::SameLine(45);
            ImGui::Text("%.1f", degPri[0]); ImGui::SameLine(45, 100);
            ImGui::Text("%.1f", degPri[1]); ImGui::SameLine(45, 200);
            ImGui::Text("%.1f", degPri[2]); ImGui::SameLine(45, 300);
            ImGui::Text("Prior Degrees");
            ImGui::SliderFloat3("Target Degrees", degTgt, -90, 90, "%.1f");
            ImGui::SliderFloat3("Auxiliar Degrees", degAux, -90, 90, "%.1f");

            ImGui::Text("XYZ");
            ImGui::Text(""); ImGui::SameLine(45);
            ImGui::Text("%.1f", posPri[0]); ImGui::SameLine(45, 100);
            ImGui::Text("%.1f", posPri[1]); ImGui::SameLine(45, 200);
            ImGui::Text("%.1f", posPri[2]); ImGui::SameLine(45, 300);
            ImGui::Text("Prior Position");
            ImGui::SliderFloat3("Target Position", posTgt, -20, 20, "%.1f");
            ImGui::SliderFloat3("Auxiliar Position", posAux, -20, 20, "%.1f");
        }ImGui::End();

        const int taskSize = 3;
        static string taskName[] = { "Take", "Give", "Rearrange" };
        bool taskAny = true;
        //ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
        static int taskNow = -1;
        if (ImGui::Begin("Built-in Commands", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar)) {
            for (int i = 0; i < taskSize; i++) ImGui::RadioButton(taskName[i].c_str(), &taskNow, i);
            if (ImGui::Button("Pose 01")) {
                degTgt[0] = 0; degTgt[1] = 60; degTgt[2] = 10;
            }ImGui::SameLine();
            if (ImGui::Button("Pose 02")) {
                degTgt[0] = -30; degTgt[1] = 60; degTgt[2] = 10;
            }ImGui::SameLine();
            if (ImGui::Button("Pose 03")) {
                degTgt[0] = 30; degTgt[1] = 60; degTgt[2] = 10;
            }
            if (ImGui::Button("Pose 04")) {
                degTgt[0] = -90; degTgt[1] = 65; degTgt[2] = 65;
            }ImGui::SameLine();
            if (ImGui::Button("Pose 05")) {
                degTgt[0] = 90; degTgt[1] = 65; degTgt[2] = 65;
            }ImGui::SameLine();
        }ImGui::End();
        int repLoop = 0;
        do {
            arduino.readSerialPort(outputSerial, MAX_DATA_LENGTH);
            done = (outputSerial[0] == '1');
            //cout << "receive: " << outputSerial << endl;

            //cout << "repLoop: " << repLoop << endl;
            if (done && startMove) {
                if (repLoop == 0){
                    switch (taskNow) {
                        case (0):   //Take
                            repLoop = 1;
                            clutching = false;
                        break;
                        case (1):   //Give
                            repLoop = 1;
                            clutching = true;
                        break;
                        case (2):   //Rearrange
                            repLoop = 2;
                            clutching = true;
                        break;
                        default:
                        break;
                    }
                }
                if (repLoop > 1) {
                    dataString = "<" +
                        to_string(false) + "," +
                        to_string(sendingDegree) + "," +
                        to_string(sendingDegree?degAux[0]:posAux[0]) + "," +
                        to_string(sendingDegree?degAux[1]:posAux[1]) + "," +
                        to_string(sendingDegree?degAux[2]:posAux[2]) + ">";
                } else {
                    dataString = "<" +
                        to_string(clutching) + "," +
                        to_string(sendingDegree) + "," +
                        to_string(sendingDegree?degPri[0]:posPri[0]) + "," +
                        to_string(sendingDegree?degPri[1]:posPri[1]) + "," +
                        to_string(sendingDegree?degPri[2]:posPri[2]) + ">";
                }
                arduino.writeSerialPort((char*)dataString.c_str(), MAX_DATA_LENGTH);
                cout << "send: " << (char*)dataString.c_str() << endl;
                //Test
                float l_arm = 10;
                float d_forearm = 15;
                float angle2 = acos((pow(l_arm, 2) + pow(d_forearm, 2) - pow(hypot(posPri[1], posPri[2]), 2)) / (2 * l_arm * d_forearm));
                float angle1 = atan(posPri[1] / posPri[2]) + atan((d_forearm * sin(degreeALL[2])) / (l_arm + d_forearm * cos(degreeALL[2])));
                cout << "angle1: " << angle1 * 180 / 3.14159265359 << endl;
                cout << "angle2: " << angle2 * 180 / 3.14159265359 << endl;
                //End Test
                done = false;
                repLoop--;
            }
        } while (repLoop > 0);
        startMove = false;

        //New Arm Stuff
        const int maxJoint = 10;
        static int numJoint = 0;
        static int posJoint;
        vector<float> posStart;
        static Quaternion q(1, 0, 0, 0);
        static Quaternion r(0, 0, 1, 0);
        static Quaternion s(5, 1, 2, 4);
        if (ImGui::Begin("Quaternions", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar)) {
            ImGui::SliderInt("Number of Joints", &numJoint, 1, maxJoint);
            for (int i = 0; i < numJoint; i++) {
                ImGui::Text("%d", maxJoint);
            }
            ImGui::Text("%.1f", q.w);
            ImGui::Text("%.1f", q.u.x);
            ImGui::Text("%.1f", q.u.y);
            ImGui::Text("%.1f", q.u.z);
        }ImGui::End();
        #pragma endregion

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
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
