#include "pch.h"
#include <windows.h>
#include <mmsystem.h>
#include <sstream>
#include <string>
#include "extcode.h"
#include <vector>
#include <algorithm>  // for std::find

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

// Array of Status messages to filter
static std::vector<uint8_t> statusFilterArray;

// Callback function for MIDI input
void CALLBACK MidiInCallbackFunction(HMIDIIN hMidiIn, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if (g_userEventRef && *g_userEventRef) {
        // Extract MIDI data
        int32_t status = (int32_t)(dwParam1 & 0xFF);
        int32_t data1 = (int32_t)((dwParam1 >> 8) & 0xFF);
        int32_t data2 = (int32_t)((dwParam1 >> 16) & 0xFF);

        // Check if the status is in the filter array
        if (std::find(statusFilterArray.begin(), statusFilterArray.end(), static_cast<uint8_t>(status)) != statusFilterArray.end()) {
            // Status is in the array, send the event to LabVIEW
            MidiData midiData;
            midiData.Status = status;
            midiData.data1 = data1;
            midiData.data2 = data2;

            // Post the event to LabVIEW
            PostLVUserEvent(*g_userEventRef, &midiData);
        }
        // Else: Discard the message
    }
}

// DLL export function to set the Status filter array
extern "C" __declspec(dllexport) void LVSetStatusFilterArray(uint8_t* filterArray, int32_t arraySize)
{
    // Clear the current filter and set the new one
    statusFilterArray.clear();
    statusFilterArray.insert(statusFilterArray.end(), filterArray, filterArray + arraySize);
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