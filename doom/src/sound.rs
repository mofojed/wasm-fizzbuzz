use std::os::raw::c_int;

extern "C" {
    fn I_MixAudio(out: *mut i16, frames: c_int, rate: c_int);
}

/// Fills `out` with `frames` interleaved stereo 16 bit samples at `rate` Hz:
/// the music and sound effects since the last call. JavaScript calls this to
/// feed Web Audio, ahead of the game loop.
#[no_mangle]
pub extern "C" fn doom_audio_mix(out: *mut i16, frames: c_int, rate: c_int) {
    unsafe { I_MixAudio(out, frames, rate) }
}
