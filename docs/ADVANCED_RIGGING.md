# Advanced Rigging Foundations

This milestone adds native, license-safe foundations inspired by public research and open tooling.

## Automatic weights
generate_heat_weights computes normalized heat-like distance weights from mesh vertices to skeleton bones, capped at four influences. It is an independent implementation and does not copy LGPL source.

## Skinning
Linear Blend Skinning and Dual Quaternion Skinning are provided. DQS falls back to LBS when non-unit scale makes the rigid formulation unsuitable.

## IK
FABRIK is implemented as an engine-independent positional solver with configurable chains, goals, iterations and tolerance.

## Bone recognition
Semantic recognition uses normalized names and aliases. Future versions can combine hierarchy, geometry and anatomical markers.

## Motion database
MotionDatabase stores compact pose/root position/velocity features and performs nearest-neighbor queries, forming the foundation for motion matching.

## Licensing
External repositories remain references. Only original project code is added here; third-party licenses are not silently imported into the core.
