# Roadmap

This roadmap is a planning guide, not a delivery schedule. Versions show the intended order of work and may change as the renderer develops.

## Current work

### v0.3.0

Implemented Dear ImGui controls for model selection, rotation, and shader properties. The release also adds the supporting GL-free state and mesh-processing tests.

## Completed releases

### v0.2.5

Fixed OBJ material-library path resolution for OBJ filenames without a parent directory. Added a Windows fallback for fatal startup errors when file logging cannot start, documented the macOS application-data location, and added this roadmap.

## Planned features

### v0.4.0

Extract a reusable camera and add interactive navigation. Scene composition needs a stable camera before it can be pleasant to inspect multiple objects.

### v0.5.0

Support multiple model instances in one scene, per-object transforms, and basic scene management. This builds on the camera and control work from the earlier milestones.

### v0.6.0

Add texture loading, UV handling, and material support, including relevant OBJ material-library support. Models in managed scenes need per-object material data before richer lighting can be useful.

### v0.7.0

Build a clearer shader and program system with parameter management and safe development-time shader reloading. Material controls and growing shader complexity make this necessary.

### v0.8.0

Improve lighting with specular lighting and multiple lights. More advanced physically based rendering is a later extension, after the material and shader system is dependable.

### v0.9.0

Add a framebuffer-based post-processing pipeline. Start with simple effects, then consider more demanding effects such as bloom after the basic pipeline is stable.

## Exploratory research

Investigate importing a documented subset of Blender material and shader node graphs. Arbitrary Blender graphs cannot be assumed to translate directly into GLSL. Useful compatibility needs a deliberate supported-node subset, texture and material handling, and clear diagnostics when an imported graph uses unsupported nodes.
