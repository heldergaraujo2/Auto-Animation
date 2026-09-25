# Auto-Animation — Architecture Foundation

## Layering
- Core — stable domain APIs and shared utilities.
- App — executable/application orchestration.
- Animation — future clips, poses, curves and procedural motion.
- Rigging — future skeleton, markers, IK/FK and auto-rigging.
- Importer/Exporter — future FBX, glTF and other asset pipelines.
- Viewer/UI — future renderer and editor.
- AI — optional interpretation/generation backends.

## Rules
1. Core must not depend on UI or renderer code.
2. Animation and rigging must not depend on a specific game engine.
3. External integrations must be isolated behind interfaces.
4. Every major subsystem must have automated tests.
5. Automation must remain editable by the user.
6. Internal representation must support non-humanoid assets.
7. AI is optional; procedural/manual systems remain first-class.

## Foundation decision
Phase 0 uses standard C++20 and CMake with no third-party runtime dependency. This keeps the foundation deterministic and easy to build in CI. Rendering/input dependencies will be selected in Phase 1 based on viewer requirements.


## Universal asset/importer boundary — Phase 2

External files are never exposed directly to rigging/animation systems.

File
  -> FormatDetector
  -> ImporterRegistry
  -> Format-specific importer adapter
  -> Universal Asset
  -> Viewer / Analysis / Rigging / Animation

The universal asset model currently carries:
- meshes, vertices, normals, UVs and indices;
- materials and texture references;
- skeletons, bones, hierarchy and bind transforms;
- up to four skin influences per vertex;
- animation clips, tracks and transform keyframes.

The first production importer adapter uses Assimp behind an isolated auto_animation_importer target. This keeps the rest of the application independent of Assimp's API.

Current registered formats:
- FBX
- GLTF / GLB
- OBJ
- SMD
- DAE / Collada
- 3DS
- STL
- PLY

BMD remains a future proprietary adapter and is deliberately not routed through Assimp.

The importer test suite contains real fixtures for FBX, GLTF, OBJ, SMD, STL and PLY, plus extension/negative-path checks. DAE and 3DS are registered and covered by format/registry checks; dedicated fixtures can be added as format-specific regressions appear.

The viewer remains format-agnostic. Phase 2 produces the representation that Phase 1 will consume when asset rendering is connected.
