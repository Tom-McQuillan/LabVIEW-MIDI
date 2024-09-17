#ifdef __cplusplus
extern "C" {
#endif

	// Exported functions with LV prefix to avoid overloading issues
	__declspec(dllexport) int LV_midiInOpen(LVUserEventRef* lvEventRef, HMIDIIN* hMidiIn);
	__declspec(dllexport) int LV_midiInStart(HMIDIIN hMidiIn);
	__declspec(dllexport) int LV_midiInStop(HMIDIIN hMidiIn);
	__declspec(dllexport) int LV_midiInClose(HMIDIIN hMidiIn);

#ifdef __cplusplus
}
#endif
