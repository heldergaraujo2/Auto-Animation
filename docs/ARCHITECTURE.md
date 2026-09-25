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
