#include "pch.h"
#include <windows.h>
#include <mmsystem.h>
#include <sstream>
#include <string>
#include "extcode.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "labviewv.lib")

// Define the structure for the MIDI data
struct MidiData {
    int32_t Status;
    int32_t data1;
    int32_t data2;
};

// Pointer to the LabVIEW User Event reference
static LVUserEventRef* g_userEventRef = nullptr;

// Callback function for MIDI input
void CALLBACK MidiInCallbackFunction(HMIDIIN hMidiIn, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if (g_userEventRef && *g_userEventRef) {
        // Extract MIDI data
        MidiData midiData;
        midiData.Status = (int32_t)(dwParam1 & 0xFF);
        midiData.data1 = (int32_t)((dwParam1 >> 8) & 0xFF);
        midiData.data2 = (int32_t)((dwParam1 >> 16) & 0xFF);

        // Post the event to LabVIEW
        PostLVUserEvent(*g_userEventRef, &midiData);
    }
}

// DLL export functions
extern "C" __declspec(dllexport) MMRESULT LVmidiInOpen(HMIDIIN* phMidiIn, UINT uDeviceID, LVUserEventRef* pUserEventRef)
{
    g_userEventRef = pUserEventRef;
    return midiInOpen(phMidiIn, uDeviceID, (DWORD_PTR)MidiInCallbackFunction, 0, CALLBACK_FUNCTION);
}

extern "C" __declspec(dllexport) MMRESULT LVmidiInStart(HMIDIIN hMidiIn)
{
    return midiInStart(hMidiIn);
}

extern "C" __declspec(dllexport) MMRESULT LVmidiInStop(HMIDIIN hMidiIn)
{
    return midiInStop(hMidiIn);
}

extern "C" __declspec(dllexport) MMRESULT LVmidiInClose(HMIDIIN hMidiIn)
{
    return midiInClose(hMidiIn);
}