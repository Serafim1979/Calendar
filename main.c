#include <windows.h>
#include <stdio.h>
#include <time.h>

// Global constants for window size
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Constants for calendar grid
#define CELL_WIDTH 100
#define CELL_HEIGHT 50
#define GRID_ROWS 7
#define GRID_COLS 7

// Global variable for the current month and year
int currentMonth;
int currentYear;

// Window Procedure Declaration
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DrawCalendar(HDC hdc);
void GetCurrentDate(int* year, int* month);
int GetDaysInMonth(int year, int month);
int GetFirstDayOfMonth(int year, int month);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "CalendarWindow";
    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    // Get the current date
    GetCurrentDate(&currentYear, &currentMonth);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Calendar",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL
    );

    if(hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_CLOSE:
        {
            DestroyWindow(hwnd);
        }
        break;

    case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Draw the calendar
            DrawCalendar(hdc);

            EndPaint(hwnd, &ps);
        }
        break;

    case WM_GETMINMAXINFO:
        {
            MINMAXINFO* mmi = (MINMAXINFO*)lParam;
            mmi->ptMinTrackSize.x = WINDOW_WIDTH;
            mmi->ptMinTrackSize.y = WINDOW_HEIGHT;
            mmi->ptMinTrackSize.x = WINDOW_WIDTH;
            mmi->ptMinTrackSize.y = WINDOW_HEIGHT;
            return 0;
        }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


void DrawCalendar(HDC hdc) {
    // Day names
    const char* dayNames[GRID_COLS] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    // Draw day names
    for (int col = 0; col < GRID_COLS; col++) {
        RECT rect = {col * CELL_WIDTH, 0, (col + 1) * CELL_WIDTH, CELL_HEIGHT};
        DrawText(hdc, dayNames[col], -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    // Get the number of days and the first day of the current month
    int daysInMonth = GetDaysInMonth(currentYear, currentMonth);
    int firstDay = GetFirstDayOfMonth(currentYear, currentMonth);

    // Draw the dates
    int day = 1;
    for (int row = 1; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            if ((row == 1 && col < firstDay) || day > daysInMonth) {
                continue;
            }

            RECT rect = {col * CELL_WIDTH, row * CELL_HEIGHT, (col + 1) * CELL_WIDTH, (row + 1) * CELL_HEIGHT};
            char dayText[3];
            sprintf(dayText, "%d", day);
            DrawText(hdc, dayText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            day++;
        }
    }

    // Draw grid lines
    for (int row = 0; row <= GRID_ROWS; row++) {
        MoveToEx(hdc, 0, row * CELL_HEIGHT, NULL);
        LineTo(hdc, GRID_COLS * CELL_WIDTH, row * CELL_HEIGHT);
    }
    for (int col = 0; col <= GRID_COLS; col++) {
        MoveToEx(hdc, col * CELL_WIDTH, 0, NULL);
        LineTo(hdc, col * CELL_WIDTH, GRID_ROWS * CELL_HEIGHT);
    }
}

void GetCurrentDate(int* year, int* month) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    *year = tm.tm_year + 1900;
    *month = tm.tm_mon + 1;
}

int GetDaysInMonth(int year, int month) {
    if (month == 2) {
        return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
    }
    return (month == 4 || month == 6 || month == 9 || month == 11) ? 30 : 31;
}

int GetFirstDayOfMonth(int year, int month) {
    struct tm firstDay = {0};
    firstDay.tm_year = year - 1900;
    firstDay.tm_mon = month - 1;
    firstDay.tm_mday = 1;
    mktime(&firstDay);
    return firstDay.tm_wday;
}

