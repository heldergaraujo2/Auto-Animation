# Research Integration & License Audit

This document records external projects investigated for Auto-Animation and what was intentionally incorporated.

## Principles

- Do not copy external source code merely because it is useful.
- Prefer independent reimplementation of generic algorithms and data models.
- Keep third-party runtime dependencies isolated behind adapters.
- Record license obligations before adding copied or linked code.
- Preserve Auto-Animation's format-agnostic and non-humanoid architecture.

## Audited references

| Project | Relevant contribution | License observed | Action |
|---|---|---|---|
| Automatic-Rigging | automatic skeleton alignment / skinning research | MIT repository; based on Pinocchio sources | Algorithmic reference only; no source copied |
| Pinocchio | medial-surface/distance-field automatic rigging | LGPL library; MIT demo | Not embedded; license-sensitive reference |
| eely | animation graphs, layering, masks, phase sync | MIT | Architecture reference; blending/masking implemented independently |
| GameAnimationProgramming | IK, blending, dual-quaternion skinning, optimization | MIT | Algorithm/architecture reference |
| Animato | natural-language → generated animation workflow | MIT project | Intent pipeline reference; no Blender dependency introduced |
| MU Online BMD Viewer | BMD parsing, animation/attachments, GLTF export | ISC | Parser architecture reference; future BMD adapter planned |
| GodotAnimationRetargeting | custom bone maps, correction, baking/runtime retarget | MIT | Independent name mapping/pose transfer foundation added |
| NVIDIA SOMA Retargeter | retargeting workflow and constraints | Apache-2.0 source; data has separate licenses | Workflow reference only |
| AniGen | shared Shape/Skeleton/Skin representation for non-humanoids | MIT source; some third-party components have non-commercial restrictions | Representation concept only; restricted components excluded |
| marrow | batch-first, allocation-free animation runtime | C11 project; license requires separate audit before reuse | Performance reference only |

## Deliberately excluded

- Pinocchio library source: LGPL, so it is not copied into the core.
- AniGen CUBVH/instant-ngp-derived code: third-party non-commercial/research restrictions make it unsuitable for the core.
- MU BMD source code: although the project is ISC, Auto-Animation keeps the parser behind a future adapter and does not copy the TypeScript implementation into C++.

## Current implementation impact

The research pass produced three concrete reusable foundations:

1. Anatomical marker data that can be edited, validated, and serialized before auto-rigging.
2. Marker-driven initial skeleton generation, including non-humanoid wing/tail extensions.
3. Pose blending, masking, and additive animation operations suitable for animation graphs and procedural generation.
4. Initial skeleton name mapping and pose transfer for future retargeting.

The remaining high-value external integrations are deliberately scheduled behind adapters:
- BMD importer (native unencrypted versions now implemented; encrypted versions remain isolated).
- full automatic rigging backend.
- advanced IK/constraints.
- AI animation backend.
- batch/LOD runtime.
