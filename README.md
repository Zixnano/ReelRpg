# Reel RPG

A Game Boy Advance fishing-RPG prototype. This repository is configured to build a real `.gba` ROM with devkitARM on GitHub Actions.

## Build

Push to `main` or run the **Build GBA ROM** workflow from the Actions tab. The workflow compiles `reelrpg.gba`, packages it into `ReelRPG-ROM.zip`, and uploads both as a downloadable workflow artifact.

## Current prototype scope

- GBA button input and a simple text UI
- Pick an animal angler
- Move through a tiny fishing-town menu
- Fish with a timing-based bite action and a simple catch log
- Basic local session state

This is an early playable foundation, not yet the full planned game. Pet battles, island progression, cooking, richer pixel art/audio, and SRAM save support remain future work. A successful build proves the ROM compiled; it does not by itself prove every gameplay path works on hardware or in an emulator.
