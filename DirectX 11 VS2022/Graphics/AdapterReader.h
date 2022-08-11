#pragma once
#include "..\\ErrorLogger.h"
#include <d3d11.h>
#include <wrl\client.h>
#include <vector>

class AdapterData
{
public:
	AdapterData(IDXGIAdapter1* pAdapter);
	IDXGIAdapter1* pAdapter;
	DXGI_ADAPTER_DESC1 description;
};

class AdapterReader
{
public:
	static std::vector<AdapterData> GetAdapters();
private:
	static std::vector<AdapterData> adapters;
};