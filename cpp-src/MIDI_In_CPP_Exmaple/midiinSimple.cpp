#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>

#pragma comment(lib, "winmm.lib")  // Link with the winmm.lib library

// Callback function for MIDI input
void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (wMsg == MIM_DATA) {
        // dwParam1 holds the MIDI message
        DWORD status = dwParam1 & 0xFF;      // Extract status byte
        DWORD data1 = (dwParam1 >> 8) & 0xFF; // Extract the first data byte (e.g., note number)
        DWORD data2 = (dwParam1 >> 16) & 0xFF; // Extract the second data byte (e.g., velocity)

        if ((status >= 0x80 && status <= 0x8F)) {
            std::cout << "Note Off - Channel: " << (status & 0x0F) + 1 << ", Note: " << data1 << ", Velocity: " << data2 << std::endl;
        }
        else if ((status >= 0x90 && status <= 0x9F)) {
            if (data2 == 0) {
                // Some devices send Note On with velocity 0 as Note Off
                std::cout << "Note Off - Channel: " << (status & 0x0F) + 1 << ", Note: " << data1 << ", Velocity: " << data2 << std::endl;
            }
            else {
                std::cout << "Note On - Channel: " << (status & 0x0F) + 1 << ", Note: " << data1 << ", Velocity: " << data2 << std::endl;
            }
        }
    }
}

int main() {
    HMIDIIN hMidiIn;
    UINT deviceId = 0;  // Change this if your MIDI device is not the first one
    MMRESULT result;

    // Open the MIDI input device
    result = midiInOpen(&hMidiIn, deviceId, (DWORD_PTR)MidiInProc, 0, CALLBACK_FUNCTION);
    if (result != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open MIDI input device. Error: " << result << std::endl;
        return 1;
    }

    // Start receiving MIDI data
    result = midiInStart(hMidiIn);
    if (result != MMSYSERR_NOERROR) {
        std::cerr << "Failed to start MIDI input. Error: " << result << std::endl;
        midiInClose(hMidiIn);
        return 1;
    }

    std::cout << "Press the spacebar to stop...\n";

    // Main loop, waiting for the user to press spacebar to exit
    while (true) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == ' ') {
                break;
            }
        }
    }

    // Stop receiving MIDI data
    midiInStop(hMidiIn);
    midiInClose(hMidiIn);
    std::cout << "MIDI input stopped.\n";

    return 0;
}
