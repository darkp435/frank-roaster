#include <iostream>
#include <string>
#include "blackjack.hpp"
#include "frank-roasting.hpp" // he doesn't know how to use c++ include directives

#define WINDOW_RATIO 3

#ifdef _WIN32
# include <windows.h>
# define NO_UNDERLINE        FALSE
# define NO_STRIKETHROUGH    FALSE
# define NO_ITALIC           FALSE
# define WHITE               RGB(255, 255, 255)
# define BLACK               RGB(0, 0, 0)
# define BLACKJACK_BUTTON_ID 1
# define ROAST_INPUT_ID      2
# define ROAST_BUTTON_ID     3
#endif /* _WIN32 */

#ifdef __linux__
extern "C" {
    #include <gtk/gtk.h>
}
#endif /* __linux__ */

using namespace std;

void interpret_command(string& command) {
    if (command == "exit") {
        cout << "Remember: Frank made a Discord server and doesn't manage it properly!" << endl;
        exit(0);
    }

    if (command == "blackjack") {
        start_blackjack_game();
    }
}

// If the program segfaults, it's probably here.
void parse_flags(const int argc,
                 const char* argv[],
                 Verbosity& verbosity,
                 string& file_to_write,
                 int& roast_count,
                 bool& just_print) {
    bool skip = false;

    for (int i = 1; i < argc; i++) {
        if (skip) {
            skip = false;
            continue;
        }

        string arg = argv[i];

        if (arg == "-v0") {
            verbosity = Verbosity::LITTLE;
        } else if (arg == "-v1") {
            verbosity = Verbosity::MEDIUM;
        } else if (arg == "-v2") {
            verbosity = Verbosity::VERBOSE;
        } else if (arg == "-v3") {
            verbosity = Verbosity::MAXIMUM;
        } else if ((arg == "--output" || arg == "-o") && i + 1 < argc) {
            file_to_write = argv[i + 1];
            skip = true;
        } else if (arg == "-h" || arg == "--help") {
            help();
            exit(0);
        } else if (arg == "-j" || arg == "--just-print") {
            just_print = true;
        } else {
            try {
                roast_count += stoi(argv[i]);
            } catch (const invalid_argument& error) {
                help();
                exit(0);
            }
        }
    }
}

void print_loop_help() {
    print("What would you like to do now?");
    print("Enter 'blackjack' to play blackjack while roasting Frank");
    print("Enter 'exit' to exit the program with a message that roasts Frank");
}

#ifdef _WIN32
HWND create_button(LPCWSTR inner_text, 
                   int id, 
                   HINSTANCE h_instance, 
                   HWND hwnd, 
                   int xpos, 
                   int ypos, 
                   int width, 
                   int height) {
    HWND button = CreateWindowExW(
        0, L"BUTTON", inner_text,
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        xpos, ypos, width, height,
        hwnd, (HMENU)id, h_instance, NULL
    );

    return button;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND roast_input;
    static Verbosity verbosity;

    switch (uMsg) { 
        case WM_CREATE: {
            HINSTANCE hInstance = ((CREATESTRUCT*)lParam)->hInstance;

            HWND label = CreateWindowExW(
                0, L"STATIC", 
                L"Frank Roaster", 
                WS_CHILD | WS_VISIBLE,
                50, 50, 300, 50,
                hwnd, NULL, hInstance, NULL
            );

            HFONT h_font = CreateFontW(
                -26, 0, 0, 0,
                FW_BOLD,
                NO_ITALIC,
                NO_UNDERLINE,
                NO_STRIKETHROUGH,
                DEFAULT_CHARSET,
                OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,
                VARIABLE_PITCH,
                L"Consolas"
            );

            roast_input = CreateWindowExW(
                0, L"EDIT", NULL,
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                50, 50, 150, 25,
                hwnd, (HMENU)ROAST_INPUT_ID, hInstance, NULL
            );

            HWND roast_button = create_button(
                L"Roast Frank", BLACKJACK_BUTTON_ID,
                hInstance, hwnd, 85, 60, 100, 35
            );

            HWND blackjack_button = create_button(
                L"Play a game of blackjack", BLACKJACK_BUTTON_ID,
                hInstance, hwnd, 50, 60, 100, 35
            );

            SendMessageW(label, WM_SETFONT, (WPARAM)h_font, TRUE);
        }
        case WM_COMMAND: {
            int wm_id = LOWORD(wParam);
            int wm_event = HIWORD(wParam);
            HWND hwnd_ctrl = (HWND)lParam;

            if (wm_event != BN_CLICKED) {
                break;
            }

            if (wm_id == BLACKJACK_BUTTON_ID) {
                start_blackjack_game();
            }

            if (wm_id == ROAST_INPUT_ID) {
                // +1 for null terminator
                int buffer_length = GetWindowTextLengthW(roast_input) + 1;
                wchar_t* input_buffer = new wchar_t[buffer_length];
                wstring input(input_buffer);
                int roast_times = stoi(input);
                GetWindowTextW(roast_input, input_buffer, buffer_length);
                string poem = roast_frank(roast_times, verbosity);
            }

            break;
        }
        case WM_CTLCOLORSTATIC: {
            HDC hdc_static = (HDC)wParam;
            SetTextColor(hdc_static, WHITE);
            SetBkColor(hdc_static, BLACK);
            
            static HBRUSH h_brush = CreateSolidBrush(BLACK);
            return (INT_PTR)h_brush;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"MainWindowClass";
    WNDCLASSW window_class = {0};
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = CLASS_NAME;
    window_class.hbrBackground = CreateSolidBrush(BLACK);
    RegisterClassW(&window_class);
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    int window_width = screen_width / WINDOW_RATIO;
    int window_height = screen_height / WINDOW_RATIO;
    
    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Frank Roaster 9000",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        window_width, window_height,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    return 0;
}
#else
int main(int argc, const char* argv[]) {
    Verbosity verbosity = Verbosity::MEDIUM;
    string file_to_write = "";
    bool just_print = false;
    int roast_count = 0;

    parse_flags(argc, argv, verbosity, file_to_write, roast_count, just_print);
    cout << roast_frank(roast_count, verbosity);

    if (file_to_write != "") {
        write_file(file_to_write, roast_count, verbosity);
    }

    if (just_print) {
        return 0;
    }

    while (true) {
        print_loop_help();

        string option;
        cin >> option;
        interpret_command(option);
    }
}
#endif