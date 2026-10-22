# Reel RPG

A tiny fishing RPG prototype for the original Game Boy Advance (240×160). This repository has a GitHub Actions workflow intended to compile a real .gba ROM with devkitARM and libgba.

## Get the ROM

1. Open the Actions tab in this repository.
2. Select Build Reel RPG GBA ROM and open the newest run.
3. Under Artifacts, download ReelRPG-GBA. The ZIP contains reel-rpg.gba and a short readme.

Every push to main runs the build. You can also start a build manually from Actions using Run workflow.

## Run it

Open reel-rpg.gba in an emulator such as mGBA, or use compatible GBA hardware/flashcart. Controls: D-pad to choose and move, A to confirm/cast/reel, B to cancel/release, Start to open the bag/menu, Select to open the same menu.

## Current prototype scope

This is an early playable foundation, not the complete game concept. It includes a title screen, animal selection (otter, cat, fox, capybara), a small pier scene, a bite-and-reel fishing loop, a simple bag/save screen, and rudimentary SRAM writing. Art is generated from chunky pixel primitives; it is not yet a polished tile/sprite game. Battle systems, pets and skills, cooking, island travel, tiered fish progression, audio, and full save validation remain future work.

## Local build

Install devkitPro with the GBA development packages (gba-dev), then run make in a shell where DEVKITPRO is configured. The output is reel-rpg.gba.

A ROM is only considered built once GitHub Actions succeeds and uploads the artifact. A successful compile does not by itself prove that every gameplay path has been tested in an emulator.
