#include "AdapterReader.h"

//列舉裝置容器
std::vector<AdapterData> AdapterReader::adapters;

std::vector<AdapterData> AdapterReader::GetAdapters()
{
	if (adapters.size() > 0)//已經初始化
		return adapters;

	Microsoft::WRL::ComPtr<IDXGIFactory1> pFactory;

	//建立 DXGIFactory 物件
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(pFactory.GetAddressOf()));

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"DXGIFactory列舉適配器創建失敗");
		ErrorLogger::Log(hr, L"Failed to create DXGIFactory for enumerating adapters.");
		exit(-1);
	}

	//遍歷適配器
	IDXGIAdapter1* pAdapter;
	UINT index = 0;
	while (SUCCEEDED(pFactory->EnumAdapters1(index, &pAdapter)))
	{
		adapters.push_back(AdapterData(pAdapter));
		index += 1;
	}
	return adapters;
}

AdapterData::AdapterData(IDXGIAdapter1* pAdapter)
{
	this->pAdapter = pAdapter;
	HRESULT hr = pAdapter->GetDesc1(&this->description);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"IDXGIAdapter描述獲取失敗");
		ErrorLogger::Log(hr, L"Failed to Get Description for IDXGIAdapter.");
	}
}