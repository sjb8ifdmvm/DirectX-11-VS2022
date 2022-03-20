#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE nPrevInstance,
	_In_ LPWSTR	lpCmdLine,
	_In_ int nCmdShow)
{

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))

	{
		ErrorLogger::Log(hr, L"COM初始化失敗\nFailed to call CoInitialize.");
		return -1;
	}

	Engine Engine;
	if (Engine.Initialize(hInstance, L"標題", L"我的窗口類別", 800, 600))
	{
		while (Engine.ProcessMessages() == true)
		{
			Engine.update();
			Engine.RenderFrame();
		}
	}
	return 0;
}

//以下為渲染管道設置
//INPUT ASSEMBLER 輸入 -- 完成
//VERTEX Shader 頂點渲染 -- 完成
//Tessellation  填充
//Geometry Shader 幾何圖形渲染
//Rasterizer 柵格化 -- 完成
//Pixel Shader 像素渲染 -- 完成
//Output Merger 合併輸出 -- 完成
//Pipeline 渲染管道 -- 完成

//Create Vertex Buffer 建立頂點緩衝 -- 完成
//Draw 繪製 -- 完成

//Depth 深度配置(Z-Buffer)