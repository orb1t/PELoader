//PELoader.cpp: 定义应用程序的入口点。
//
#pragma once
#include "stdafx.h"
#include "PELoader.h"

//创建实例句柄
HINSTANCE hinstance = NULL;
//Dialog句柄
HWND MainDialogHwnd = NULL;
//创建进程模块processList句柄
extern HWND processHwnd;
//创建外部变量模块ModuleList窗体句柄
extern  HWND moduleHwnd;

/*关于按钮 消息处理函数*/
INT_PTR CALLBACK AboutDialogProc(HWND aboutHwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		EndDialog(aboutHwnd, NULL);
		break;
		//标准组件消息通知
	case WM_COMMAND:
		if (LOWORD(wParam)== AboutIDOK)
		{
			EndDialog(aboutHwnd,NULL);
			break;
		}
		default:
			return false;
	}
	return true;
}
tagNMHDR * tagControl = NULL;
//窗体图标句柄
HICON hIcon = NULL;
INT_PTR CALLBACK DialogProc(HWND hwndDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		//初始化主窗体句柄
		MainDialogHwnd = hwndDlg;
		//初始化窗体时，为进程ListControl模块句柄赋值
		processHwnd = GetDlgItem(hwndDlg, ProcessList);
		//初始化窗体时，为模块ModuleControl句柄赋值
		moduleHwnd = GetDlgItem(hwndDlg, ModuleList);

		initListControl();
		initListModule();
		  hIcon = LoadIcon(hinstance, MAKEINTRESOURCEW(MyIcon));
		::SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		::SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
		//标准组件消息通知
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case AboutButton:
			 DialogBox(hinstance, MAKEINTRESOURCE(AboutDialog), hwndDlg, AboutDialogProc);
			 break;
		case ExitButton:
			PostQuitMessage(0);
			break;
		case LoadPEButton:
		//	showPEParseField();
			break;
		case AddShellCodeButton:
			disposeShellProtectedMessage();
			break;
		case InjectButton:
			remoteInject();
			break;
		case RefreshProcessButton:
			refreshProcess();
			break;
		default:
			break;
		}
		break;
		//通用控件消息通知
	case WM_NOTIFY:
		//lparam为tagNMHDR结构指针，存储消息的结构地址，尽量不使用wparam，而使用结构数组中的idfrom作为组件判断依据
		tagControl = (tagNMHDR*)lParam;
		wchar_t code[100];
		if (tagControl->idFrom == ProcessList && tagControl->code == NM_CLICK)
		{
			getItemDetail(tagControl);
		}
		//else if (tagControl->idFrom == ModuleList && tagControl->code == NM_CLICK)
		//{
		//	//点击模块控件区域 不进行处理
		//}
		break;
	default:
		return false;
	}
	return true;
}
/*新建线程 创建主窗体*/
DWORD WINAPI windowContrlThread(LPVOID lpThreadParameter) {
	DialogBox((HINSTANCE)lpThreadParameter, MAKEINTRESOURCE(MyDialog), NULL, DialogProc);
	return 0;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	hinstance = hInstance;
	//创建子线程来处理窗体的创建
	HANDLE winThreadHand = CreateThread(NULL, 0, windowContrlThread, hInstance, 0, NULL);
	//等待子线程结束,主线程结束
	WaitForSingleObject(winThreadHand, INFINITE);
	return 0;
}