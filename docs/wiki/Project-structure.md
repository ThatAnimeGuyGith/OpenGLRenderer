# Project structure

| Path | Purpose |
| --- | --- |
| `src/` | Renderer source code |
| `assets/` | Shipped shaders, meshes, and default configuration |
| `third_party/` | Generated glad source code |
| `cmake/` | Release packaging helpers |
| `docs/wiki/` | Source Markdown published to the GitHub wiki |
| `.github/workflows/` | Continuous-integration and wiki-publishing workflows |

The GitHub Actions build workflow configures and compiles both Debug and Release presets on Ubuntu, macOS, and Windows. It uploads only the Release executable and assets as a short-lived workflow artifact for each platform.
