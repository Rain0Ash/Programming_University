#pragma once


#if SQL_NOT_PATCHED

#include <Windows.h>
#include <vector>

void get_all_windows_from_process_id(const DWORD dw_process_id, std::vector<HWND>& vh_wnds)
{
    HWND h_cur_wnd = nullptr;
    do
    {
        h_cur_wnd = FindWindowEx(nullptr, h_cur_wnd, nullptr, nullptr);
        DWORD dw_proc_id = 0;
        GetWindowThreadProcessId(h_cur_wnd, &dw_proc_id);
        if (dw_proc_id == dw_process_id)
        {
            vh_wnds.push_back(h_cur_wnd);
        }

    } while (h_cur_wnd != nullptr);
}

void kill_evaluation_window()
{
    const DWORD process_id = GetCurrentProcessId();

    std::vector<HWND> windows;
    while (windows.empty())
    {
        get_all_windows_from_process_id(process_id, windows);
    }
    
    HWND main = nullptr;
    if (!windows.empty())
    {
        main = windows[0];
    }

    do
    {
        windows.clear();
        get_all_windows_from_process_id(process_id, windows);

        if (main != nullptr && !windows.empty())
        {
            for (auto* element : windows)
            {
                if (element == main)
                {
                    continue;
                }

                SendMessageW(element, WM_CLOSE, 0, 0);
            }
        }
    } while (windows.size() <= 1);
}

#endif