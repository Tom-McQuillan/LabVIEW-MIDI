#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <lv_prolog.h>
#include <lv_epilog.h>
#pragma comment(lib, "winmm.lib")  // Link with the winmm.lib library

// LabVIEW MIDI event structure (similar to LV_PoemLine in your example)
typedef struct {
    LStrHandle midi_event_str;  // LabVIEW string handle
} LV_MidiEvent;

#include <lv_epilog.h>

LVUserEventRef lvEventRef = 0;  // Global User Event Ref

// Utility function to allocate and copy string data to LStrHandle
void allocate_and_copy_string_to_lv_str_handle(std::string_view src, LStrHandle* lv_str_handle) {
    if (*lv_str_handle != nullptr) {
        DSDisposeHandle(*lv_str_handle);  // Dispose of any existing handle
    }

    *lv_str_handle = (LStrHandle)DSNewHandle(sizeof(int32) + src.length());
    (*lv_str_handle)->cnt = static_cast<int32>(src.length());
    memcpy((*lv_str_handle)->str, src.data(), src.length());
}

// Callback function to handle MIDI input events
void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (wMsg == MIM_DATA) {
        DWORD midiMessage = static_cast<DWORD>(dwParam1);
        BYTE status = midiMessage & 0xFF;
        BYTE data1 = (midiMessage >> 8) & 0xFF;
        BYTE data2 = (midiMessage >> 16) & 0xFF;

        std::string midiEventString;
        if ((status & 0xF0) == 0x90 && data2 > 0) {
            midiEventString = "Note On: " + std::to_string(data1) + ", Velocity: " + std::to_string(data2);
        } else if ((status & 0xF0) == 0x80 || ((status & 0xF0) == 0x90 && data2 == 0)) {
            midiEventString = "Note Off: " + std::to_string(data1);
        }

        // Post the event to LabVIEW
        if (!midiEventString.empty() && lvEventRef) {
            LV_MidiEvent lvMidiEvent;
            lvMidiEvent.midi_event_str = nullptr;

            // Allocate and copy the string to LabVIEW string handle
            allocate_and_copy_string_to_lv_str_handle(midiEventString, &lvMidiEvent.midi_event_str);

            // Post the event to LabVIEW
            PostLVUserEvent(lvEventRef, &lvMidiEvent);

            // Dispose of the allocated string handle
            DSDisposeHandle(lvMidiEvent.midi_event_str);
        }
    }
}

// StartMIDI function, now returning the MIDI handle (HMIDIIN) to LabVIEW
extern "C" __declspec(dllexport) HMIDIIN __stdcall StartMIDI(LVUserEventRef eventRef) {
    lvEventRef = eventRef;

    HMIDIIN hMidiInLocal = nullptr;
    UINT deviceID = 0;  // Default MIDI device ID
    MMRESULT result = midiInOpen(&hMidiInLocal, deviceID, (DWORD_PTR)MidiInProc, 0, CALLBACK_FUNCTION);

    if (result == MMSYSERR_NOERROR) {
        midiInStart(hMidiInLocal);
        return hMidiInLocal;  // Return the MIDI handle to LabVIEW
    }

    return nullptr;  // Return nullptr on failure
}

// StopMIDI function, now using the MIDI handle from LabVIEW
extern "C" __declspec(dllexport) void __stdcall StopMIDI(HMIDIIN hMidiIn) {
    if (hMidiIn) {
        midiInStop(hMidiIn);
        midiInClose(hMidiIn);
    }
}
