#pragma once
#include <Windows.h>

LRESULT CALLBACK WndProc_SvcCtrlWindow(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_TrayIconWindow(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_SetupWindow(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void RegClass_BackgroundLayeredAlphaWindowClass();
LRESULT CALLBACK WndProc_BackgroundLayeredAlphaWindowClass(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
/*LRESULT CALLBACK WndProc_Dlg_Main(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);*/

