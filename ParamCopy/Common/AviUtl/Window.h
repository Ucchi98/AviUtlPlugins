#pragma once

namespace Tools::AviUtl
{
	//
	// このクラスはプラグインウィンドウのように振る舞うためのウィンドウ拡張です。
	//
	struct WindowExtension : Window2::Extension
	{
		//
		// ウィンドウを作成します。
		//
		template<class T>
		static BOOL create(T& window, HINSTANCE instance, HWND parent, LPCTSTR name)
		{
			MY_TRACE(_T("AviUtl::WindowExtension::create()\n"));

			const LPCTSTR className = _T("AviUtl");

			WNDCLASS wc = {};
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = instance;
			wc.lpszClassName = className;
			::RegisterClass(&wc);

			return window.create(
				0,
				className,
				name,
				WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				parent, 0, instance, 0);
		}

		//
		// ウィンドウの表示/非表示を切り替えます。
		//
		void show(HWND hwnd)
		{
			MY_TRACE(_T("AviUtl::WindowExtension::show()\n"));

			::SendMessage(hwnd, WM_CLOSE, 0, 0);
		}

		//
		// ウィンドウ拡張の名前です。
		//
		inline static const LPCWSTR Name = L"Tools::AviUtl::WindowExtension";

		//
		// ウィンドウ拡張の名前を返します。
		//
		constexpr LPCWSTR getExtensionName() const
		{
			return Name;
		}

		//
		// ウィンドウプロシージャです。
		//
		BOOL onWndProc(Window2* window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lr)
		{
			switch (message)
			{
			case WM_CLOSE:
				{
					MY_TRACE(_T("AviUtl::WindowExtension::onWndProc(WM_CLOSE)\n"));

					if (::IsWindowVisible(hwnd))
					{
						::ShowWindow(hwnd, SW_HIDE);
					}
					else
					{
						::ShowWindow(hwnd, SW_SHOW);
					}

					*lr = 0;
					return TRUE;
				}
			}

			return FALSE;
		}
	};
}
