#include <windows.h>
#include <stdio.h>
#include <time.h>

// Global constants for window size
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define HEADER_HEIGHT 50

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
void DrawHeader(HDC hdc);
void GetCurrentDate(int* year, int* month);
int GetDaysInMonth(int year, int month);
int GetFirstDayOfMonth(int year, int month);

/**
 * Entry point for the Windows application.
 * Registers a window class, creates a window, and runs the message loop.
 */
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

/**
 * Window procedure for handling messages sent to the window.
 * Processes paint, close, and resize events.
 */
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

            // Draw the header
            DrawHeader(hdc);

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

/**
 * Draws the calendar grid and fills it with day names and the dates of the current month.
 * @param hdc The handle to the device context used for drawing.
 */
void DrawCalendar(HDC hdc) {
    const char* dayNames[GRID_COLS] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    // Draw day names
    for (int col = 0; col < GRID_COLS; col++) {
        RECT rect = {col * CELL_WIDTH, HEADER_HEIGHT, (col + 1) * CELL_WIDTH, HEADER_HEIGHT + CELL_HEIGHT};
        DrawText(hdc, dayNames[col], -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    // Get the number of days and the first day of the current month
    int daysInMonth = GetDaysInMonth(currentYear, currentMonth);
    int firstDay = GetFirstDayOfMonth(currentYear, currentMonth);

    // Draw the dates
    int day = 1;
    for (int row = 1; row <= GRID_ROWS; row++) { // Rows start from 1 for day names
        for (int col = 0; col < GRID_COLS; col++) {
            if ((row == 1 && col < firstDay) || day > daysInMonth) {
                continue;
            }

            RECT rect = {
                col * CELL_WIDTH,
                HEADER_HEIGHT + row * CELL_HEIGHT,
                (col + 1) * CELL_WIDTH,
                HEADER_HEIGHT + (row + 1) * CELL_HEIGHT
            };
            char dayText[3];
            sprintf(dayText, "%d", day);
            DrawText(hdc, dayText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            day++;
        }
    }

    // Draw grid lines
    for (int row = 0; row <= GRID_ROWS + 1; row++) {
        MoveToEx(hdc, 0, HEADER_HEIGHT + row * CELL_HEIGHT, NULL);
        LineTo(hdc, GRID_COLS * CELL_WIDTH, HEADER_HEIGHT + row * CELL_HEIGHT);
    }
    for (int col = 0; col <= GRID_COLS; col++) {
        MoveToEx(hdc, col * CELL_WIDTH, HEADER_HEIGHT, NULL);
        LineTo(hdc, col * CELL_WIDTH, HEADER_HEIGHT + (GRID_ROWS + 1) * CELL_HEIGHT);
    }
}

/**
 * Draws the header above the calendar, showing the current month and year.
 * Allows navigation between months (future implementation can add buttons or other controls).
 * @param hdc The handle to the device context used for drawing.
 */
void DrawHeader(HDC hdc)
{
    // Buffer for the header text
    char headerText[100];

    // Format the header with the current month and year
    const char* months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    sprintf(headerText, "%s %d", months[currentMonth - 1], currentYear);

    // Define the rectangle for the header
    RECT headerRect = {0, 0, GRID_COLS * CELL_WIDTH, 50};

    // Draw the header text
    DrawText(hdc, headerText, -1, &headerRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

/**
 * Retrieves the current year and month.
 * @param year Pointer to an integer to store the current year.
 * @param month Pointer to an integer to store the current month.
 */
void GetCurrentDate(int* year, int* month)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    *year = tm.tm_year + 1900;
    *month = tm.tm_mon + 1;
}

/**
 * Calculates the number of days in a given month for a given year.
 * @param year The year.
 * @param month The month (1-12).
 * @return The number of days in the month.
 */
int GetDaysInMonth(int year, int month)
{
    if (month == 2)
    {
        // Check for leap year
        return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
    }
    return (month == 4 || month == 6 || month == 9 || month == 11) ? 30 : 31;
}

/**
 * Determines the day of the week for the first day of a given month.
 * @param year The year.
 * @param month The month (1-12).
 * @return The day of the week (0 = Sunday, 6 = Saturday).
 */
int GetFirstDayOfMonth(int year, int month)
{
    struct tm firstDay = {0};
    firstDay.tm_year = year - 1900; // tm_year is years since 1900
    firstDay.tm_mon = month - 1;    // tm_mon is 0-based
    firstDay.tm_mday = 1;           // First day of the month
    mktime(&firstDay);
    return firstDay.tm_wday;
}

