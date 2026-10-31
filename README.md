# Reel RPG

This repository contains the original Game Boy Advance project and the Godot 4 Android project package.

## Android APK build

The Android project is stored in `android-project.zip`. Every change to that package, or to the Android workflow, triggers the GitHub Actions Android build.

1. Open the [Android APK workflow](https://github.com/Zixnano/ReelRpg/actions/workflows/android-apk.yml).
2. Select the newest successful run.
3. Download the `ReelRPG-Android-APK` artifact.
4. Extract the artifact ZIP and install `ReelRPG.apk` on an Android device.

The APK is a debug-signed installable build for device testing. A release keystore is required for a Play Store release.

## Original GBA project

The original devkitARM/libgba source remains in `src/`, with its `Makefile` and original build workflow preserved. Its ROM artifact is still built separately by `.github/workflows/build-gba.yml`.

## Godot project

Godot 4 project, GDScript source, scenes, and project settings are packaged in `android-project.zip`. The Android build runs on GitHub-hosted runners and does not require a local Android SDK.
