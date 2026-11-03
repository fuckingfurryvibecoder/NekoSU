# Putting NekoSU on GitHub

You only need to do **two** things: create an empty repository, and add four secrets.
Everything else is already prepared — the workflows build the signed APK **and**
`kernelsu.ko` for every supported KMI, then publish a release.

---

## 1. Create the repository

1. Open <https://github.com/new>
2. **Repository name**: `NekoSU`
3. **Owner**: your account
4. **Visibility**: `Public` (required — GPL-3.0)
5. **Do NOT** check any of: *Add a README*, *Add .gitignore*, *Choose a license*
   (the repository must be completely empty)
6. Click **Create repository**
7. Copy the URL — it looks like `https://github.com/YOUR-NAME/NekoSU.git`

---

## 2. Push the code

In a terminal (PowerShell) on this PC:

```powershell
cd "C:\Users\1\Documents\Типовий проєкт\NekoSU"
git remote set-url origin https://github.com/YOUR-NAME/NekoSU.git
git push -u origin main
```

The first push opens a browser window from Git Credential Manager — log in to GitHub
and allow access. No token or CLI needed.

> If Git asks for a password: GitHub no longer accepts account passwords.
> Use **Personal access token** instead: GitHub → Settings → Developer settings →
> Personal access tokens → Tokens (classic) → Generate new token → scope `repo`,
> then paste it as the password.

---

## 3. Add the signing secrets

The APK must be signed with the key whose certificate hash is compiled into
`kernelsu.ko`. Otherwise the kernel refuses to treat the app as the manager.

Go to **Settings → Secrets and variables → Actions → New repository secret** and add:

| Name | Value |
| :--- | :--- |
| `KEYSTORE` | the whole contents of `NekoSU-keys\keystore.base64.txt` (one long line, ~3.5 KB) |
| `KEYSTORE_PASSWORD` | `nekosu2026` |
| `KEY_ALIAS` | `neko` |
| `KEY_PASSWORD` | `nekosu2026` |

`KEYSTORE` is the **base64** of `NekoSU-keys\neko.jks`. Do not commit the `.jks`
itself — it is already in `.gitignore`.

> ⚠️ Back up `NekoSU-keys\neko.jks`. If you lose it you must generate a new key,
> rebuild `kernelsu.ko` and reinstall everything.

---

## 4. Build a release

Either:

- **Actions → Release → Run workflow**, or
- push a tag:

```powershell
git tag v1.0.0
git push origin v1.0.0
```

After ~20–30 minutes you get a release with:

```
NekoSU_<tag>_<code>-release.apk
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

## 5. Install on the phone

Only the `.ko` that matches your kernel will load. Check the kernel first:

```sh
adb shell uname -r
```

Then:

```sh
su -c 'insmod /data/local/tmp/androidXX-X.XX_kernelsu.ko'
```

and install the APK. Do **not** re-sign the APK with a different key.

---

## Updating the update-checker links

Two places contain the repository path, replace `nekosu/NekoSU` with your real
`owner/name`:

- `manager/app/src/main/java/org/bakasu/bakasu/data/update/ManagerUpdateRepository.kt` → `REPOSITORY`
- `manager/app/src/main/java/org/bakasu/bakasu/ui/screen/about/About.kt` → two `openUri(...)` URLs
