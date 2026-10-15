# Project structure

| Path | Purpose |
| --- | --- |
| `src/` | Renderer source code |
| `assets/` | Shipped shaders, meshes, and default configuration |
| `third_party/` | Generated glad source code |
| `cmake/` | Release packaging helpers |
| `docs/wiki/` | Source Markdown published to the GitHub wiki |
| `.github/workflows/` | Continuous-integration and wiki-publishing workflows |

The GitHub Actions build workflow configures and compiles the Debug preset on Ubuntu, macOS, and Windows. It uploads each build's executable and assets as a short-lived workflow artifact.
