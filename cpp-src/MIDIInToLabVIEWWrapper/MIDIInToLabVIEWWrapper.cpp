#include "pch.h"
#include <windows.h>
#include <mmsystem.h>
#include <sstream>
#include <string>
#include "extcode.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "labviewv.lib")

// Global variables
LVUserEventRef g_lvUserEvent = 0;
HMIDIIN g_hMidiIn = nullptr;

// Callback function for MIDI input
void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (wMsg == MIM_DATA && g_lvUserEvent != 0) {
        DWORD status = dwParam1 & 0xFF;
        DWORD data1 = (dwParam1 >> 8) & 0xFF;
        DWORD data2 = (dwParam1 >> 16) & 0xFF;

        std::stringstream ss;
        ss << "Status: 0x" << std::hex << (status & 0xFF)
            << ", Data1: " << std::dec << data1
            << ", Data2: " << data2;

        std::string midiMessage = ss.str();

        LStrHandle lvString = (LStrHandle)DSNewHandle(sizeof(int32) + midiMessage.size());
        if (lvString) {
            memcpy(LStrBuf(*lvString), midiMessage.c_str(), midiMessage.size());
            (*lvString)->cnt = midiMessage.size();
            PostLVUserEvent(g_lvUserEvent, lvString);
            DSDisposeHandle(lvString);
        }
    }
}

// Exported function to open MIDI input device
extern "C" __declspec(dllexport) int LV_midiInOpen(HMIDIIN* hMidiIn, LVUserEventRef* lvEventRef) {

    g_lvUserEvent = *lvEventRef;

    UINT deviceId = 0;  // Change this if needed
    MMRESULT result = ::midiInOpen(&g_hMidiIn, deviceId, (DWORD_PTR)MidiInProc, 0, CALLBACK_FUNCTION);
    if (result != MMSYSERR_NOERROR) return result;

    *hMidiIn = g_hMidiIn;  // Pass the handle back
    return MMSYSERR_NOERROR;
}

// Exported function to start receiving MIDI data
extern "C" __declspec(dllexport) int LV_midiInStart(HMIDIIN hMidiIn) {
    if (hMidiIn == nullptr) return -1;  // Invalid input
    return ::midiInStart(hMidiIn);
}

// Exported function to stop receiving MIDI data
extern "C" __declspec(dllexport) int LV_midiInStop(HMIDIIN hMidiIn) {
    if (hMidiIn == nullptr) return -1;  // Invalid input
    return ::midiInStop(hMidiIn);
}

// Exported function to close MIDI input device
extern "C" __declspec(dllexport) int LV_midiInClose(HMIDIIN hMidiIn) {
    if (hMidiIn == nullptr) return -1;  // Invalid input
    MMRESULT result = ::midiInClose(hMidiIn);
    g_hMidiIn = nullptr;
    return result;
}
