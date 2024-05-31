# Auto Nong
A GD mod that can automatically find and download NONG songs when entering a Geometry Dash level.

## Mod Dependencies 
This mod requires [Jukebox](https://github.com/Fleeym/jukebox) version `>=2.9.0`.

## Indexes
The mod works by downloading compressed JSON files(called indexes) when starting GD.  
Each index contains a list of NONG metadata and respective song IDs.  
You can edit which indexes the mod downloads on startup in the mod's settings on GD.  

#### Default Indexes
These are the indexes that come with the mod by default:
- https://cdn.jsdelivr.net/gh/FlafyDev/auto-nong-indexes@dist/official.json.gz
- https://cdn.jsdelivr.net/gh/FlafyDev/auto-nong-indexes@dist/sfh-rooot.json.gz
- https://cdn.jsdelivr.net/gh/FlafyDev/auto-nong-indexes@dist/sfh-yt.json.gz

These are generated from [FlafyDev/auto-nong-indexes](https://github.com/FlafyDev/auto-nong-indexes).

#### Structure
```json5
[
  {
    "name": "Lost (XVA Remix)",
    "artist": "Crim3s",
    "songs": [945695],
    // if source is "youtube", yt-id is required
    "source": "youtube",
    "yt-id": "VZXeoUPPFOY",
    // if source is "host", url is required
    "source": "host",
    "url": "https://example.com/song.mp3"
  }
]
```
For specific examples of how the JSON should look, decompress the default indexes listed above.  
The JSON can't have trailing commas or comments.  

## Special Thanks
Without these the mod wouldn't be possible.

- [Geode](https://github.com/geode-sdk/geode)
- [Jukebox](https://github.com/Fleeym/jukebox)
