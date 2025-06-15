#pragma once
#include <windows.h> // For HWND

// Forward declaration for Setting if it's defined elsewhere and complex.
// If it's simple, it can be defined in Payment.cpp or main.cpp where needed.
// struct Setting; 

class Payment {
public:
    Payment(); // Constructor (optional, can be empty if no setup needed)
    ~Payment(); // Destructor (optional)

    // hwnd is the handle to the main game window, needed for ScreenToClient
    void showPaymentInterface(HWND hwnd);
};