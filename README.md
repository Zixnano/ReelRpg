# Reel RPG

Reel RPG is a cozy, pixel-art fishing adventure for the Game Boy Advance, built for a 240×160 screen. The look aims for the dense, colorful handheld RPG style in the supplied reference: tiled paths, flowerbeds, trees, a market street, wooden pier, and a small character moving through the world.

## Build and download

Every push to `main` runs the devkitARM/libgba build workflow. Open the [Actions builds](https://github.com/Zixnano/ReelRpg/actions) and download the `ReelRPG-GBA` artifact from the newest successful run. It contains the `.gba` ROM and a short readme.

## Controls

- **D-pad:** move around the village and navigate menus
- **A:** interact, cast, reel, buy, cook, or battle
- **B:** back out, calm the line, or leave an encounter
- **Start:** open the travel bag
- **Select:** open the island guide and journal
- **L / R:** quick island travel / explore

## Current playable systems

- Four selectable anglers: otter, cat, fox, and capybara
- A hand-drawn top-down village with tiled plaza, shops, paths, trees, flowers, canal, and a wooden fishing pier
- Fishing bite timing, tension management, catch/escape outcomes, multiple fish species, rarity-flavored colors, and fish collection tracking
- Coins, bait, rods, experience, levels, and fish values
- Travel bag, fish journal, selling catches, bait/rod shop, cooking, pet companionship and bond
- Island exploration, changing destinations, and simple wild encounters
- Basic SRAM save writing

## Important limitations

This is an expanded playable foundation, not a finished commercial-scale RPG. The visuals are drawn in software to keep this build self-contained, rather than using a full authored tile/sprite asset pipeline. The current island, pet, battle, cooking, and save systems are deliberately lightweight; they need more content, balancing, sound/music, and thorough emulator testing. The save routine writes a small marker and selected values to SRAM but does not yet implement robust versioning, checksum validation, or a complete restore/load flow.

## Build locally

Install devkitPro with the GBA development packages (`gba-dev`) and run `make` in a shell where `DEVKITPRO` and `DEVKITARM` are configured. Output: `reel-rpg.gba`.

A successful CI build confirms the ROM was compiled and packaged; it does not guarantee that every gameplay path has been manually tested in mGBA or on hardware.
