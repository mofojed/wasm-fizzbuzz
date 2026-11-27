use std::ffi::CStr;
use std::os::raw::{c_char, c_int};
use std::sync::Mutex;

const IWAD_FD: c_int = 42; // file descriptor for the opened IWAD

// The IWAD is not compiled in. JavaScript loads it at runtime, so any IWAD
// works, e.g. the shareware doom1.wad or your own copy of doom2.wad.
struct Iwad {
    data: Vec<u8>,
    seeker: usize,
}

lazy_static! {
    static ref IWAD: Mutex<Iwad> = Mutex::new(Iwad {
        data: Vec::new(),
        seeker: 0,
    });
}

/// Allocates `len` bytes for the IWAD. JavaScript copies the IWAD there before calling `main`.
#[no_mangle]
pub extern "C" fn doom_iwad_buffer(len: usize) -> *mut u8 {
    let mut iwad = IWAD.lock().unwrap();
    iwad.data = vec![0; len];
    iwad.seeker = 0;
    iwad.data.as_mut_ptr()
}

fn has_lump(wad: &[u8], name: &[u8]) -> bool {
    let read_u32 = |at: usize| -> Option<usize> {
        let b = wad.get(at..at + 4)?;
        Some(u32::from_le_bytes([b[0], b[1], b[2], b[3]]) as usize)
    };
    let (numlumps, infotableofs) = match (read_u32(4), read_u32(8)) {
        (Some(n), Some(o)) => (n, o),
        _ => return false,
    };
    (0..numlumps).any(|i| {
        let entry = infotableofs + i * 16;
        match wad.get(entry + 8..entry + 16) {
            Some(lump) => lump.iter().take_while(|&&c| c != 0).eq(name.iter()),
            None => false,
        }
    })
}

/// Doom picks the game mode by the name of the IWAD it finds (see IdentifyVersion in d_main.c).
/// Pretend the loaded IWAD has the name of the game its maps belong to.
fn iwad_path(wad: &[u8]) -> Option<&'static str> {
    if wad.get(0..4) != Some(&b"IWAD"[..]) {
        return None;
    }
    Some(if has_lump(wad, b"MAP01") {
        "./doom2.wad" // also Final Doom (TNT, Plutonia) and Freedoom: Phase 2
    } else if has_lump(wad, b"E4M1") {
        "./doomu.wad"
    } else if has_lump(wad, b"E2M1") {
        "./doom.wad"
    } else {
        "./doom1.wad"
    })
}

static HOME_ENV: &'static [u8; 11] = b"/home/doom\0"; // C string, terminate with \0! //TODO: use CStr safely here?

#[no_mangle]
extern "C" fn getenv(name: *const c_char) -> *const c_char {
    //TODO returning an ffi-safe Option<non-nullable> would be cool!!
    // TODO type!!!
    let name = unsafe { CStr::from_ptr(name) };
    let name = name.to_str().expect("invalid UTF8 getenv call");
    let result = match name {
        "DOOMWADDIR" => std::ptr::null(),
        "HOME" => HOME_ENV.as_ptr() as *const c_char,
        _ => {
            crate::log!("unexepcted getenv({:?}) call", name);
            std::ptr::null()
        }
    };
    result
}

#[no_mangle]
extern "C" fn access(pathname: *const c_char, mode: c_int) -> c_int {
    const ENOENT: c_int = 2;

    let pathname = unsafe { CStr::from_ptr(pathname).to_str().expect("invalid UTF8") };
    match pathname {
        "./doom2f.wad" | "./plutonia.wad" | "./tnt.wad" => ENOENT,
        "./doom2.wad" | "./doomu.wad" | "./doom.wad" | "./doom1.wad" => {
            if iwad_path(&IWAD.lock().unwrap().data) == Some(pathname) {
                0 /* OK */
            } else {
                ENOENT
            }
        }
        _ => panic!("access({}, {}) unimplemented", pathname, mode),
    }
}

#[no_mangle]
extern "C" fn fopen(pathname: *const c_char, mode: c_int) -> i32 /* FILE* */ {
    let pathname = unsafe { CStr::from_ptr(pathname).to_str().expect("invalid UTF8") };

    if pathname == "/home/doom/.doomrc" {
        return 0; // NULL for error
    }

    panic!("fopen({}, {}) unimplemented", pathname, mode);
}

#[no_mangle]
extern "C" fn open(pathname: *const c_char, flags: c_int, mode: i32) -> i32 {
    let pathname = unsafe { CStr::from_ptr(pathname).to_str().expect("invalid UTF8") };

    if iwad_path(&IWAD.lock().unwrap().data) == Some(pathname) {
        IWAD.lock().unwrap().seeker = 0;
        return IWAD_FD;
    }

    panic!("open({}, {}, {}) unimplemented", pathname, flags, mode);
}

#[no_mangle]
extern "C" fn read(fd: c_int, buf: *mut u8 /*TODO is c_char*/, count: usize) -> isize {
    if fd == IWAD_FD {
        let buf = unsafe { std::slice::from_raw_parts_mut(buf, count) };
        let mut iwad = IWAD.lock().unwrap();
        let s = iwad.seeker;
        buf.copy_from_slice(&iwad.data[s..s + count]);
        iwad.seeker += count;
        return count as isize;
    }
    panic!("read({}, buf, {}) unimplemented", fd, count);
}

#[no_mangle]
extern "C" fn lseek(fd: i32, offset: i64, whence: c_int) -> i64 {
    const SEEK_SET: c_int = 0;
    const _SEEK_CUR: c_int = 1;
    const _SEEK_END: c_int = 2;
    if fd == IWAD_FD {
        match whence {
            SEEK_SET => {
                IWAD.lock().unwrap().seeker = offset as usize;
                return offset;
            }
            _ => {
                crate::log!("TODO lseek");
            }
        }
    }
    panic!("lseek({}, {}, {}) unimplemented", fd, offset, whence);
}
