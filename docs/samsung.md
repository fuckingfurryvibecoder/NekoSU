# Samsung notes

Samsung's One UI kernel is not stock AOSP. On top of the usual GKI hardening it adds
three proprietary security layers that actively fight root:

| Layer | What it does | How NekoSU handles it |
| :--- | :--- | :--- |
| **KDP** (Knox Defender Platform) | `prepare_ro_creds()` / `kdp_assign_pgd()` make credentials read-only and verify callers | `kernel/compat/kdp.c` hooks `kdp_usecount_*`, `inc_rlimit_ucounts` and friends, then re-implements `ksu_commit_creds()` / `ksu_put_cred()` |
| **RKP** (Real-time Kernel Protection) | Hypervisor-side page table protection: refuses to execute modified kernel text | `kernel/compat/rkp.c` writes through `rkp`-aware stubs and patches branches with BTI/PACIASP-aware instruction sequences (`ksu_rkp_branch_patch`) instead of raw memory writes |
| **DEFEX** | Blocks exec of files from non-system paths by unprivileged processes | `kernel/compat/defex.c` patches `task_defex_enforce()` |

These files are the reason NekoSU can work on One UI while plain KernelSU often cannot.

## Late load

Because DEFEX and RKP are active during early boot, the module cannot always be loaded from
`init`. NekoSU therefore inherits `ksud late-load` / `soft-reboot`:

```sh
su -c '/data/adb/ksud late-load --soft-reboot'
```

The manager exposes this through the "Install / Repair" flow.

## GKI vs non-GKI

**GKI devices** (kernel 5.10+, `CONFIG_KSU=m` style builds) can load the shipped
`kernelsu.ko` as a module. That is what the release workflow builds.

**Non-GKI devices** (many Samsung 4.14 / 4.19 kernels, usually older budget models) cannot
load an LKM. For these you must compile KernelSU into the kernel itself:

1. Obtain your device's kernel sources from Samsung Open Source Release Center.
2. Vendor the `kernel/` directory into `drivers/kernelsu` (or as `KernelSU/`).
3. Add to `drivers/Kconfig`:

   ```
   source "drivers/kernelsu/Kconfig"
   ```

   and to `drivers/Makefile`:

   ```
   obj-$(CONFIG_KSU) += kernelsu/
   ```

4. Build `CONFIG_KSU=y` and flash the resulting `boot.img`.

## How the kernel recognises the manager

`kernel/Kbuild` bakes two values into the module:

```make
KSU_PACKAGE_NAME  ?= dev.nekosu
KSU_EXPECTED_SIZE ?= 0x348
KSU_EXPECTED_HASH ?= d67570...
```

At runtime `kernel/manager/apk_sign.c` parses the APK signing block, and only accepts a
manager whose signing certificate matches size **and** SHA-256. `throne_tracker.c` also
matches the package name.

Consequences:

- You **must** sign the APK with the key in `NekoSU-keys/neko.jks`
  (or whatever key you put into the `KEYSTORE` secret).
- If you rotate the key you **must** rebuild and redistribute `kernelsu.ko`.
- Debug builds signed with the Android debug key will show "not installed as manager".

The certificate DER must stay under 1024 bytes (`CERT_MAX_LENGTH` in `apk_sign.c`).
RSA-2048 gives ~840 bytes; RSA-4096 does **not** fit, which is why the NekoSU key is 2048-bit.

## Troubleshooting

| Symptom | Cause |
| :--- | :--- |
| App says "Kernel: not installed" | `.ko` not loaded, or loaded `.ko` is from another project |
| App works but `su` does nothing | APK signature does not match the one baked into `.ko` |
| `insmod` fails with `Exec format error` | wrong KMI |
| `insmod` fails with `Unknown symbol` | non-GKI kernel, needs built-in build |
| Device loops on boot | wrong KMI — reboot to recovery, remove the module |
