# NekoSU 🐾

<p align="center">
  <img src="docs/assets/icon.png" width="140" alt="NekoSU icon">
</p>

<p align="center">
  <b>A KernelSU-based root manager for Samsung devices</b><br>
  Samsung-safe kernel engine &nbsp;•&nbsp; modern Compose manager &nbsp;•&nbsp; GPL-3.0
</p>

<p align="center">
  <a href="../../releases/latest"><img alt="Latest release" src="https://img.shields.io/github/v/release/nekosu/NekoSU?style=flat-square"></a>
  <a href="../../actions/workflows/release.yml"><img alt="Build" src="https://img.shields.io/github/actions/workflow/status/nekosu/NekoSU/release.yml?style=flat-square"></a>
  <img alt="License" src="https://img.shields.io/badge/license-GPL--3.0-blue?style=flat-square">
  <img alt="Android" src="https://img.shields.io/badge/Android-12%E2%80%9317-green?style=flat-square">
</p>

---

## What is NekoSU?

NekoSU is a fork that puts two things together:

| Part | Origin | What it gives you |
| :--- | :--- | :--- |
| **Kernel engine** (`kernel/`, `userspace/`) | [KernelSU](https://github.com/tiann/KernelSU) + [diabl0w's Samsung fork](https://github.com/diabl0w/KernelSU) | KDP / RKP / DEFEX bypass, `late-load` and soft-reboot support on Samsung One UI |
| **Manager app** (`manager/`) | [BakaSU](https://github.com/Baka-SU/BakaSU) | The full modern Compose UI: dashboard, superuser, modules, module repository, app profiles, themes |

In other words: **BakaSU's interface on top of a Samsung-capable KernelSU kernel.**

### Features

- 🐈 Full Compose manager UI with Material 3 Expressive + Miuix components
- 🧩 Module manager, online module repository, module actions, script runner
- 👤 Superuser list with per-app root profiles, mount namespaces and SELinux domains
- 🎨 Theme engine: key colours, dark/light, custom fonts, translucent effects
- 🌙 **Samsung specific:** KDP, RKP and DEFEX bypasses, late-load jailbreak
- 🔐 Manager is bound to the kernel by APK signing certificate
- 🧱 100 % GPL-3.0, reproducible GitHub Actions builds

---

## Compatibility

NekoSU ships `kernelsu.ko` for **GKI (LKM) devices**. Pick the module that matches your
kernel branch:

| Kernel | KMI file |
| :--- | :--- |
| 5.10 (Android 12/13) | `android12-5.10_kernelsu.ko`, `android13-5.10_kernelsu.ko` |
| 5.15 (Android 13/14) | `android13-5.15_kernelsu.ko`, `android14-5.15_kernelsu.ko` |
| 6.1 (Android 14) | `android14-6.1_kernelsu.ko` |
| 6.6 (Android 15) | `android15-6.6_kernelsu.ko` |
| 6.12 (Android 16) | `android16-6.12_kernelsu.ko` |
| 6.18 (Android 17) | `android17-6.18_kernelsu.ko` |

Check your kernel with:

```sh
adb shell uname -r
# or: Settings → About phone → Software information → Kernel version
```

> **Non-GKI devices** (older Samsung kernels such as 4.14 / 4.19) cannot load an LKM.
> They need KernelSU built into the kernel image instead — see
> [docs/samsung.md](docs/samsung.md).

---

## Installation

1. **Download** the `.ko` that matches your KMI and **`NekoSU_*.apk`** from
   [Releases](../../releases/latest).
2. **Install the APK.** Do not replace it with a re-signed build: the kernel only trusts
   APKs signed with the NekoSU certificate.
3. **Load the module**, then open the app and let it finish setup:

```sh
su -c 'insmod /data/local/tmp/android15-6.6_kernelsu.ko'
```

4. Reboot and verify with `su -c id` → `uid=0(root)`.

---

## Building

Everything is built by GitHub Actions — see [`.github/workflows/release.yml`](.github/workflows/release.yml).
Push a tag:

```sh
git tag v1.0.0
git push origin v1.0.0
```

The workflow produces a signed APK, `kernelsu.ko` for every supported KMI and a
`SHA256SUMS.txt`, then publishes a GitHub release.

### Required repository secrets

| Secret | Value |
| :--- | :--- |
| `KEYSTORE` | your keystore, base64 (`base64 -w0 neko.jks`) |
| `KEYSTORE_PASSWORD` | keystore password |
| `KEY_ALIAS` | key alias |
| `KEY_PASSWORD` | key password |

The APK is signed with this key **and** `kernelsu.ko` is compiled with its certificate
size and SHA-256, so the two always match.

### Local build

```sh
# ksud (root daemon)
cargo ndk -t aarch64-linux-android -P 26 build --release --manifest-path userspace/ksud/Cargo.toml
cp target/aarch64-linux-android/release/ksud manager/app/src/main/jniLibs/arm64-v8a/libksud.so

# manager
cd manager && ./gradlew assembleRelease
```

Requires JDK 21, Android SDK 37, build-tools 36.1.0 and NDK 30.0.16248370.

---

## Project layout

```
kernel/       KernelSU kernel module (+ Samsung KDP/RKP/DEFEX compat layer)
userspace/    ksud and ksuinit (Rust)
uapi/         Shared kernel <-> userspace ABI headers
manager/      Android manager app (Kotlin + Compose)
docs/         Extra documentation
```

---

## Credits & license

NekoSU stands on the shoulders of three projects:

- [KernelSU](https://github.com/tiann/KernelSU) — the original kernel-based root solution
- [diabl0w/KernelSU](https://github.com/diabl0w/KernelSU) — Samsung KDP/RKP/DEFEX support
- [BakaSU](https://github.com/Baka-SU/BakaSU) — the manager UI

All code is **GPL-3.0** — see [LICENSE](LICENSE) and [NOTICE](NOTICE).
The NekoSU launcher icon is an original asset created for this project; it does **not**
use BakaSU's All-Rights-Reserved artwork.

---

## Українською

**NekoSU** — менеджер root-доступу для Samsung на базі KernelSU.

1. Завантаж `*_kernelsu.ko`, що відповідає твоєму ядру (дивись *Налаштування → Про телефон
   → Версія ядра*), і `NekoSU_*.apk`.
2. Встанови APK. Не перепідписуй його іншим ключем — ядро приймає лише підпис NekoSU.
3. Завантаж модуль: `su -c 'insmod /data/local/tmp/<файл>.ko'`.
4. Перезавантаж і перевір: `su -c id`.

Збірка — через GitHub Actions, тегом `v1.0.0`. Усі зміни та сирці відкриті, ліцензія GPL-3.0.
