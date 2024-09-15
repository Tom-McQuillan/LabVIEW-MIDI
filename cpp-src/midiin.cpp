#include <iostream>
#include <windows.h>
#include <mmsystem.h>  // Include for midiIn functions
#pragma comment(lib, "winmm.lib")  // Link winmm.dll

// Callback function that will be called when MIDI input is received
void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    // wMsg is the message that indicates what kind of MIDI input event occurred
    switch (wMsg) {
        case MIM_DATA: {
            // dwParam1 contains the MIDI message
            DWORD midiMessage = static_cast<DWORD>(dwParam1);
            BYTE status = midiMessage & 0xFF;       // First byte (status byte)
            BYTE data1 = (midiMessage >> 8) & 0xFF; // Second byte (data1: note number or controller)
            BYTE data2 = (midiMessage >> 16) & 0xFF;// Third byte (data2: velocity or value)

            // Status byte 0x90 indicates a "Note On" message
            if ((status & 0xF0) == 0x90 && data2 > 0) {  // Check if it's a Note On message with velocity > 0
                std::cout << "Note On: " << (int)data1 << ", Velocity: " << (int)data2 << std::endl;
            }
            // Status byte 0x80 or Note On with velocity = 0 indicates a "Note Off"
            else if ((status & 0xF0) == 0x80 || ((status & 0xF0) == 0x90 && data2 == 0)) {
                std::cout << "Note Off: " << (int)data1 << std::endl;
            }
            break;
        }
        case MIM_OPEN:
            std::cout << "MIDI device opened." << std::endl;
            break;
        case MIM_CLOSE:
            std::cout << "MIDI device closed." << std::endl;
            break;
    }
}

int main() {
    HMIDIIN hMidiIn;  // Handle for the MIDI input device
    UINT deviceID = 0;  // Use the default device (usually the first one connected)
    MMRESULT result;

    // Open the MIDI input device and pass in the callback function (MidiInProc)
    result = midiInOpen(&hMidiIn, deviceID, (DWORD_PTR)MidiInProc, 0, CALLBACK_FUNCTION);
    if (result != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open MIDI device." << std::endl;
        return -1;
    }

    // Start receiving MIDI input
    result = midiInStart(hMidiIn);
    if (result != MMSYSERR_NOERROR) {
        std::cerr << "Failed to start MIDI input." << std::endl;
        midiInClose(hMidiIn);
        return -1;
    }

    std::cout << "Press the spacebar to exit..." << std::endl;

    // Simple loop to wait for the spacebar to be pressed
    while (true) {
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {  // Spacebar pressed
            break;
        }
        Sleep(100);  // Sleep to avoid busy-waiting
    }

    // Stop receiving MIDI input
    midiInStop(hMidiIn);
    // Close the MIDI input device
    midiInClose(hMidiIn);

    std::cout << "Program ended." << std::endl;
    return 0;
}
