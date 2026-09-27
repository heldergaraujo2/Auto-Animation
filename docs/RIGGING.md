# Skeleton Inspector

The rigging module provides a renderer-independent skeleton inspection and manual-editing layer.

## Supported operations

- Hierarchy inspection through roots and children.
- Bone lookup by index or name.
- Bone creation and controlled removal.
- Renaming with duplicate-name protection.
- Reparenting with cycle prevention.
- Bind/rest local transforms.
- Editable local pose transforms.
- Bind-pose reset.
- Local-to-world transform evaluation.
- Pivot metadata.
- Editable/locked bone state.
- Selection state for editor integration.
- Bone groups.
- Pose mirroring and mirror-to-bone operations.
- Inverse bind matrix preservation for imported skinning.
- Validation of names, parents, cycles and finite transform data.

## Architecture

The skeleton model lives in `auto_animation_rigging` and depends only on the universal animation transform types. It does not depend on SDL, OpenGL, Assimp or UI code.

The viewer consumes a read-only skeleton pointer and renders a hierarchy overlay, keeping the editor/runtime boundary separate from the renderer.

## Safety

Manual hierarchy changes reject invalid parents and cycles. Removal can either reject bones that still have children or explicitly reparent those children. Locked bones reject manual pose edits.

Validation remains available after any edit and is intended to be used before later rigging, deformation and export stages.

## Scope boundary

Anatomical markers are intentionally deferred to Phase 5. Automatic rig generation is deferred to Phase 7. This phase establishes the reliable skeleton data and editing foundation those systems will consume.
