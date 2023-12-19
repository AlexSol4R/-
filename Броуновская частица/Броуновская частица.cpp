//Импорт библиотек
#include "framework.h"
#include <cmath>
#include "Броуновская частица.h"
#include "time.h"
#include <fstream>


#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

  
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //Объявление переменных
    static int x_scr, y_scr, rad, radB, coef;
    const int N = 76; //В сосуде 75 "молекул" и 1 "броуновская частица"
    static double vx[N], vy[N], x[N], y[N], dt, r, L = 8, m=5, Time, T, p, n, E_pr, E_t;
    //Открываем на дозапись файл. С него в дальшнейшем будем считывать данные для графиков
    std::ofstream fout("test.dat", std::ios::app);

    switch (message)
    {
    case WM_CREATE:
        //Задание начальных значений
        srand(time(0));
        dt = 0.05;
        r = 0.1; //
        coef = 50;
        //Скорости и начальные координаты молекул задаём случайным образом
        for (int i = 0; i < N-1; i++)
        {
            vx[i] = -0.1 + 2.0 * (double)rand() / RAND_MAX;
            vy[i] = -0.1 + 2.0 * (double)rand() / RAND_MAX;
            x[i] = 1 + r + (L - 2.0 * r) * (double)rand() / RAND_MAX;
            y[i] = 1 + r + ( L - 2.0 * r) * (double)rand() / RAND_MAX;
        }
        //Начальные координаты броуновской частицы
        x[N-1] = 9;
        y[N-1] = 3;
        //Изначально частица покоится
        vx[N-1] = 0;
        vy[N-1] = 0;
        //Радиус частицы на экране 
        radB = 2 * r * coef;
        //Радиус молекулы на экране 
        rad = r * coef;
        //Установка частоты обновления экрана
        SetTimer(hWnd, 1, 20, NULL);
        break;
    //Перерисовка окна
    case WM_TIMER:
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HBRUSH hbrush1;
        hbrush1 = CreateSolidBrush(RGB(0, 0, 0));
        HBRUSH hbrush2 = CreateSolidBrush(RGB(255, 0, 0));
        HPEN hpen1;
        hpen1 = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
        HPEN hpen2 = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        //Отрисовка границ сосуда
        SelectObject(hdc, hpen1);
        Rectangle(hdc, 50, 50, 500, 500);
        SelectObject(hdc, hpen2);
        SelectObject(hdc, hbrush2);
        //Отрисовка броуновской частицы
        x_scr = x[N - 1] * coef;
        y_scr = y[N - 1] * coef;
        Ellipse(hdc, x_scr - radB, y_scr - radB, x_scr + radB, y_scr + radB);
        //Изменение скорости броуновской частицы при ударе о стенку сосуда
        if (y_scr < 50 + radB || y_scr > 500 - radB)
        {
            vy[N - 1] = -vy[N - 1];
        }
        if (x_scr < 50 + radB || x_scr > 500 - radB)
        {
            vx[N - 1] = -vx[N - 1];
        }
        //Изменение координат частицы
        x[N - 1] += vx[N - 1] * dt;
        y[N - 1] += vy[N - 1] * dt;

        //Создаём переменную, в которой будем хранить суммарную среднеквадратичную скорость молекул
        double v_kv = 0;
        //Отрисовка шариков, отображающих движение молекул в сосуде
        SelectObject(hdc, hbrush1);
        for (int i = 0; i < N - 1; i++) {
            x_scr = x[i] * coef;
            y_scr = y[i] * coef;
            
            v_kv += vy[i] * vy[i] + vx[i] * vx[i];

            Ellipse(hdc, x_scr - rad, y_scr - rad, x_scr + rad, y_scr + rad);
            //Изменение скорости молекул при ударе о стенку сосуда
            if (y_scr < 50 + rad || y_scr > 500 - rad)
            {
                vy[i] = -vy[i];
            }
            if (x_scr < 50 + rad || x_scr > 500 - rad)
            {
                vx[i] = -vx[i];
            }
            //Изменение скорости броуновской частицы при соударениях с молекулами
            if (sqrt(pow(x[N - 1] - x[i], 2) + pow(y[N - 1] - y[i], 2)) <= 3 * r) {
                vx[i] = -vx[i] + vx[N - 1];
                vx[N - 1] = vx[N - 1] - vx[i] / m;
                vy[i] = -vy[i] + vy[N - 1];
                vy[N - 1] = vy[N - 1] - vy[i] / m;
                x[i] += vx[i] * dt;
                y[i] += vy[i] * dt;
            }
            //Изменение скорости молекул при соударениях
            for (int j = 1; j < N; j++) {
                if (sqrt(pow(x[j] - x[i], 2) + pow(y[i] - y[j], 2)) <= 2 * r) {
                    double c = vx[i];
                    vx[i] = vx[j];
                    vx[j] = c;
                    c = vy[i];
                    vy[i] = vy[j];
                    vy[j] = c;
                    x[i] += vx[i] * dt;
                    y[i] += vy[i] * dt;
                    break;
                }
            }

        }
        n += 1; //Счётчик обновлений окна
        p += v_kv*3.35/16.2/3/n*1000; //Расчёт давления
        T += v_kv*3.35 / 3 / 1.38 / 10 / n; //Расчёт температуры
        E_pr = 5 * 3.35 / 2 * v_kv/(N-1)*10; //Расчёт средней кинетической энергии броуновской частицы (делим на 10^(-23))
        E_t = 3 * 1.38 * T / 2; //Расчёт средней энергии теплового движения молекул (Делим на 10^(-23))
        //Заполнение файлов для последующего построения графиков
            Time += dt; //Ось времени
            //fout1 << x[N - 1] << " " << y[N - 1] << std::endl;
            //fout << Time << " " << p << std::endl;
            //fout << Time << " " << T << std::endl;
            //vout << Time << " " << E_pr << std::endl;
            //fout << Time << " " << E_t << std::endl;
            //fout2 << Time << " " << x[N - 1] << std::endl;
            //fout3 << Time << " " << y[N - 1] << std::endl;
            //vout << Time << " " << sqrt(vy[N - 1]*vy[N-1]+vx[N-1]*vx[N-1]) << std::endl;
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
