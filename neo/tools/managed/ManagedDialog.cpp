// ManagedDialog.cpp
//

#include "precompiled.h"

//
// Sys_CoCreateInstance
// http://www.codeproject.com/Articles/18433/Emulating-CoCreateInstance
//
HRESULT __stdcall Sys_CoCreateInstance( LPCTSTR szDllName, IN REFCLSID rclsid, IUnknown* pUnkOuter, IN REFIID riid, OUT LPVOID FAR* ppv)
{
  HRESULT hr = REGDB_E_KEYMISSING;
  char dllPath[MAX_OSPATH];

  fileSystem->FindDLL( szDllName, dllPath, false );
  if ( !dllPath[ 0 ] ) {
		return 0;
  }


  HMODULE hDll = ::LoadLibrary(dllPath);
  if (hDll == 0) {
	  common->FatalError( "Sys_CoCreateInstance: %s not found\n", dllPath);
    return hr;
  }

  typedef HRESULT (__stdcall *pDllGetClassObject)(IN REFCLSID rclsid, 
                   IN REFIID riid, OUT LPVOID FAR* ppv);

  pDllGetClassObject GetClassObject = 
     (pDllGetClassObject)::GetProcAddress(hDll, "DllGetClassObject");
  if (GetClassObject == 0)
  {
	  common->FatalError( "Sys_CoCreateInstance: %s missing DllGetClassObject\n", dllPath);
    ::FreeLibrary(hDll);
    return hr;
  }

  IClassFactory *pIFactory;

  hr = GetClassObject(rclsid, IID_IClassFactory, (LPVOID *)&pIFactory);

  if (!SUCCEEDED(hr)) {
	  common->FatalError( "Sys_CoCreateInstance: %s getclassobject failed\n", dllPath);
    return hr;
  }

  hr = pIFactory->CreateInstance(pUnkOuter, riid, ppv);
  pIFactory->Release();

  return hr;
}

/*
==========================
bmManagedDialog::bmManagedDialog
==========================
*/
bmManagedDialog::bmManagedDialog( const wchar_t *name ) {
	_class = new Editors::ModelStudio::ModelStudio();

	_class->Init( this );
}