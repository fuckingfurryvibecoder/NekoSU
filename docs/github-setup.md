# How the GitHub side works

The repository already exists at
<https://github.com/fuckingfurryvibecoder/NekoSU>, the signing secrets are
configured, and the first release is published. This page explains how the
automation fits together and how to cut the next release.

---

## What lives where

| Path | What it does |
| :--- | :--- |
| `.github/workflows/release.yml` | The full release: signing info → 8 `.ko` builds → ksud → signed APK → publish |
| `.github/workflows/build-manager.yml` | APK only (runs on pushes that touch `manager/`) |
| `.github/workflows/ksud.yml` | Builds the root daemon (`libksud.so`) |
| `.github/workflows/ddk-lkm.yml` | Reusable: builds one `.ko` inside a DDK container |
| `kernel/` | The Samsung-capable KernelSU module (coded for uapi v5) |
| `userspace/ksud/` | The Rust daemon bundled into the APK |
| `manager/` | The Android app (BakaSU UI, package `dev.nekosu`) |

---

## The signing key ties the APK to the kernel

The kernel module only treats an app as *the manager* if the app is signed with
the right certificate. That certificate's size and SHA-256 are compiled into
`kernelsu.ko`:

```
kernel/Kbuild
  KSU_PACKAGE_NAME = dev.nekosu
  KSU_EXPECTED_SIZE = 0x348
  KSU_EXPECTED_HASH = d67570ba…11776
```

The CI takes the keystore from repository secrets, exports the certificate, and
feeds the real size/hash into the kernel build. So:

- **Never commit** `neko.jks` — it is git-ignored.
- **Back it up.** If it is lost, every `.ko` must be rebuilt and every user must
  reinstall.
- **Never re-sign** the released APK; a re-signed build is not the manager.

### Repository secrets

**Settings → Secrets and variables → Actions → New repository secret**

| Name | Value |
| :--- | :--- |
| `KEYSTORE` | base64 of `neko.jks` (`NekoSU-keys\keystore.base64.txt`) |
| `KEYSTORE_PASSWORD` | keystore password |
| `KEY_ALIAS` | `neko` |
| `KEY_PASSWORD` | key password |

---

## Building a release

### Option A — from the GitHub website

1. Open **Actions → Release**.
2. Click **Run workflow**, enter a tag such as `v1.0.1` (leave empty for an
   automatic `v0.1.<run>` tag), then **Run workflow**.
3. Wait ~20–30 minutes. The **publish** job creates the release only if the APK
   job succeeded.

### Option B — from a terminal

```powershell
cd "C:\Users\1\Documents\Типовий проєкт\NekoSU"
git tag v1.0.1
git push origin v1.0.1
```

Pushing a `v*` tag starts `release.yml` with `github.ref_type == tag`, so the
release is named after the tag automatically.

### What you get

```
NekoSU_v1.0.0_33395-release.apk
android12-5.10_kernelsu.ko
android13-5.10_kernelsu.ko
android13-5.15_kernelsu.ko
android14-5.15_kernelsu.ko
android14-6.1_kernelsu.ko
android15-6.6_kernelsu.ko
android16-6.12_kernelsu.ko
android17-6.18_kernelsu.ko
SHA256SUMS.txt
```

---

## Installing on the phone

KernelSU runs in **LKM mode**: the stock kernel is untouched, but the boot image
ramdisk is patched so `kernelsu.ko` loads at boot. This needs an unlocked
bootloader — on Samsung that permanently trips Knox.

1. Find the kernel version:

   ```sh
   adb shell uname -r
   ```

   or *Settings → About phone → Software information → Kernel version*.
   Choose the `.ko` whose KMI prefix matches, e.g. `android15-6.6` for a 6.6
   kernel.

2. Patch the boot image on a PC:

   ```sh
   ksud boot-patch --boot-image boot.img --kernel android15-6.6_kernelsu.ko --out patched.img
   ```

3. Flash `patched.img` to the `boot` partition (Odin, or
   `fastboot flash boot patched.img`).

4. Install the APK. Do **not** re-sign it.

5. Reboot and verify:

   ```sh
   su -c id     # → uid=0(root)
   ```

> `insmod kernelsu.ko` only works when root already exists. It is not a way to
> gain root on a stock device.

> Non-GKI kernels (4.14 / 4.19) cannot load an LKM at all. They need KernelSU
> built into the kernel image instead — see [samsung.md](samsung.md).

---

## Updating the app's self-update links

If the repository is ever renamed, update `REPOSITORY` in
`manager/app/src/main/java/org/bakasu/bakasu/data/update/ManagerUpdateRepository.kt`
and the two `openUri(...)` URLs in
`manager/app/src/main/java/org/bakasu/bakasu/ui/screen/about/About.kt`.
Both already point at `fuckingfurryvibecoder/NekoSU`.
