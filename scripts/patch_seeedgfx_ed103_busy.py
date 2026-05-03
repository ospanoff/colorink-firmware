# PlatformIO pre: script — fix Seeed_GFX ED103 CHECK_BUSY macro (V3.0.0).
# CHECK_BUSY used `} while (true)` so it never returned after the first wait; it must
# be `} while (0)` (run tconWaitForReady once per invocation), matching other drivers.
from pathlib import Path

Import("env")  # noqa: F821  # SCons injects


def main() -> None:
    project_dir = Path(env["PROJECT_DIR"])  # noqa: F821
    paths = sorted(
        set(project_dir.glob(".pio/libdeps/*/Seeed_GFX/TFT_Drivers/ED103TC2_Defines.h"))
    )
    if not paths:
        print("patch_seeedgfx_ed103_busy: ED103TC2_Defines.h not found yet (skip)")
        return
    old = "    } while (true)"
    new = "    } while (0)"
    for path in paths:
        text = path.read_text(encoding="utf-8")
        if old not in text:
            if new in text:
                print(f"patch_seeedgfx_ed103_busy: already applied ({path.name})")
            else:
                print(f"patch_seeedgfx_ed103_busy: expected pattern missing in {path}")
            continue
        path.write_text(text.replace(old, new, 1), encoding="utf-8")
        print(f"patch_seeedgfx_ed103_busy: patched {path}")


main()
