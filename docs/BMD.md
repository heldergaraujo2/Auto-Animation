# MU Online BMD Importer

## Status

Implemented for unencrypted BMD files.

The importer is a native C++ adapter under importer/mu. It converts BMD data into the universal Asset representation:

BMD -> Mesh/Material -> Skeleton -> AnimationClip

## Supported now

- BMD header/version validation.
- Fixed-size model names.
- Mesh vertices.
- Normals.
- UV coordinates.
- Triangle/polygon records.
- Texture path metadata.
- Single-bone vertex assignment from the BMD node field.
- Bone hierarchy.
- Bind pose from action 0 frame 0.
- Animation actions and per-bone position/rotation tracks.
- Defensive count and bounds checks.

## Encryption

BMD versions 12 and 15 are recognized but are not decrypted by the core importer yet. The parser returns an explicit error instead of silently interpreting encrypted bytes.

The next adapter can isolate MU cryptography (including LEA-based and legacy file cryptor paths) without changing the universal Asset model.

## Reference

The parser structure was independently implemented from the public MU BMD format behavior documented by the open-source xulek/muonline-bmd-viewer project. That project is ISC licensed. No TypeScript source was copied into Auto-Animation.

## Validation

The importer test creates a valid binary BMD fixture at runtime and verifies geometry, skeleton, child-bone hierarchy and animation import. This keeps the test deterministic without committing proprietary game assets.
