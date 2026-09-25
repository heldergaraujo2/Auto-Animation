# Auto-Animation — Roadmap Mestre

## Visão do projeto

O Auto-Animation será uma ferramenta independente para criação automática e assistida de animações para assets de jogos, com foco em MMORPGs de mundo aberto e integração com Unreal Engine.

Objetivo central:

Importar uma malha 3D, analisar ou receber marcações da estrutura, gerar automaticamente um sistema de rig/controle adequado ao tipo de criatura e permitir que o usuário descreva a animação desejada em linguagem natural. O sistema deverá gerar, editar, visualizar, validar e exportar a animação.

Exemplo-alvo:

1. Importar Character.fbx.
2. Analisar a geometria.
3. Corrigir/marcar pontos anatômicos quando necessário.
4. Criar o esqueleto e o skinning.
5. Informar: "Crie uma animação de voo parado, com as pernas levemente dobradas, uma mais baixa que a outra, corpo estável e asas em movimento, em loop."
6. Converter a descrição em parâmetros de movimento.
7. Gerar poses, curvas, keyframes e loop.
8. Acompanhar tudo no visualizador 3D.
9. Ajustar parâmetros.
10. Exportar para FBX/GLB e, futuramente, integração direta com Unreal.

O projeto não será limitado a humanoides. A arquitetura deverá suportar humanos, NPCs, monstros, quadrúpedes, animais, aves, criaturas rastejantes, insetos, serpentes, criaturas aquáticas, criaturas com múltiplos membros, criaturas aladas, plantas, objetos animáveis, elementos de cenário, criaturas mecânicas e assets híbridos.

---

# PRINCÍPIOS DE ARQUITETURA

## P1 — Separar o núcleo de animação da interface

Camadas:

Core Animation Engine
→ Rig / Skeleton
→ IK / Constraints
→ Motion Generation
→ Animation Data
→ Import / Export
→ Application UI

O núcleo não deverá depender da UI.

## P2 — Sistema orientado a capacidades

Não assumir anatomia humana.

Cada asset será descrito por capacidades: Geometry, Skeleton, Rig, Motion Controllers, Deformation, Materials e Animation Profile.

Um humano pode ter braços e pernas; uma aranha pode ter oito pernas; uma planta pode usar deformadores/procedural motion sem skeleton tradicional.

## P3 — Separar intenção, movimento e dados finais

A descrição "Crie um ataque pesado com espada" deverá passar por:

Natural Language
→ Animation Intent
→ Motion Plan
→ Pose Generation
→ IK / Constraints
→ Keyframes / Curves
→ Animation Clip

## P4 — Automação nunca bloqueia edição manual

O usuário poderá corrigir pontos anatômicos, hierarquia, orientação dos bones, pesos, IK, poses, timing, curvas e parâmetros.

---

# FASE 0 — FUNDAÇÃO DO PROJETO

## Objetivo

Transformar o repositório vazio em uma base profissional, compilável e testável.

## Entregas

- Estrutura inicial.
- Sistema de build.
- Testes automatizados.
- CI.
- Documentação de arquitetura.
- Logs.
- Configuração de desenvolvimento.
- Convenções de código.
- Assets de teste.
- Primeiro executável.

## Estrutura sugerida

Auto-Animation/
├── app/
├── core/
├── animation/
├── rigging/
├── deformation/
├── motion/
├── ai/
├── importer/
├── exporter/
├── viewer/
├── ui/
├── plugins/
├── tests/
├── examples/
├── assets/
├── docs/
├── scripts/
└── .github/

## Critério

Projeto compila/executa e abre uma aplicação inicial.

---

# FASE 1 — VISUALIZADOR 3D + FRONTEIRA UNIVERSAL DE ASSET

**Status: CONCLUÍDA**

## Objetivo

Criar o visualizador que acompanhará todo o desenvolvimento e estabelecer a fronteira entre a representação interna do projeto e qualquer formato de arquivo externo.

A Fase 1 não deve conhecer FBX, OBJ ou outro formato diretamente. O viewer recebe uma representação de cena/asset que poderá ser produzida futuramente por qualquer importer.

## Implementação inicial

- Janela multiplataforma via SDL2.
- Contexto OpenGL isolado no módulo viewer.
- Câmera orbit.
- Zoom.
- Grid.
- Eixos XYZ.
- Profundidade 3D.
- Renderização sólida.
- Renderização wireframe.
- Animação demonstrativa em loop.
- Play/Pause.
- Frame stepping a 30 FPS.
- Estatísticas/FPS no título da janela.
- Redimensionamento da janela.
- Fechamento limpo.
- Entrada de mouse e teclado.
- Teste automatizado da API do viewer sem exigir janela interativa.
- Demonstração de personagem suspenso com asas para validar o caso FlyIdle visual.

## Recursos preparados para integração posterior

- Seleção de objetos.
- Seleção de bones.
- Skeleton overlay.
- Bone names/axes.
- Bounding box.
- Gizmos.
- Timeline visual.
- Scrubbing.
- Controle de velocidade.
- Comparação de poses.

Esses recursos serão conectados quando a representação universal, skeleton e animação estiverem disponíveis.

## Regra arquitetural

O core não depende de SDL/OpenGL.

O viewer não depende de FBX/OBJ/SMD/GLTF/etc.

A fronteira planejada é:

Arquivo externo
→ Importer
→ Universal Asset/Scene
→ Viewer

Isso permite adicionar novos formatos sem reescrever o viewer.

## Critério

A aplicação abre uma janela 3D, renderiza uma cena demonstrativa animada, permite orbit/zoom, wireframe e play/pause, e possui testes automatizados da camada viewer.

---


## Registro da Fase 1

Implementação concluída no repositório com SDL2 + OpenGL isolados no viewer, demonstração 3D animada, câmera orbit, zoom, grid, eixos, wireframe, play/pause, frame stepping e self-test determinístico. O CI final executou configure, build, CTest e self-test gráfico em Xvfb com sucesso.

A necessidade de múltiplos formatos foi incorporada à arquitetura antes da importação: o viewer não conhecerá formatos externos e a Fase 2 utilizará detector + registry + adapters + representação universal. Formatos-alvo incluem FBX, GLB/GLTF, OBJ, SMD, DAE, 3DS, STL, PLY e formatos proprietários/MU. “SMB” permanece como item a confirmar antes de qualquer parser específico.

# FASE 2 — IMPORTAÇÃO UNIVERSAL DE ASSETS

**Status: NÃO INICIADA**

## Objetivo

Criar uma camada extensível capaz de receber muitos formatos 3D e converter todos eles para uma representação interna única.

## Formatos-alvo

### Prioridade 1 — skeletal/animated
- FBX.
- GLB / GLTF.
- SMD.

### Prioridade 2 — geometry/static
- OBJ.
- DAE / Collada.
- 3DS.
- STL.
- PLY.

### Prioridade 3 — formatos específicos do ecossistema do usuário
- BMD e arquivos auxiliares relacionados a MU.
- Outros formatos proprietários através de plugins/adapters.

O termo “SMB” citado como possível formato deverá ser confirmado antes de implementar um parser específico, pois a sigla possui múltiplos significados.

## Arquitetura

importer/
├── core/
├── fbx/
├── gltf/
├── obj/
├── smd/
├── dae/
├── 3ds/
├── stl/
├── ply/
├── mu/
└── registry/

Pipeline:

Arquivo
→ Format Detector
→ Importer Registry
→ Importer específico
→ Universal Asset/Scene
→ Viewer / Analysis / Rigging / Animation

O restante do programa não deve depender do formato de origem.

## Representação universal mínima

Asset
├── Mesh
│   ├── Vertices
│   ├── Normals
│   ├── UV
│   ├── Materials
│   └── Textures
├── Skeleton
│   ├── Bones
│   ├── Hierarchy
│   └── Bind Pose
├── Skin
│   ├── Weights
│   └── Influences
└── Animations
    ├── Tracks
    ├── Keyframes
    ├── Curves
    └── Metadata

Formatos sem skeleton/animação devem entrar como assets geométricos válidos, sem serem artificialmente convertidos em humanoides.

## Critério

Importar pelo menos FBX, GLB/GLTF, OBJ e SMD na primeira implementação útil, preservando geometry/material/skeleton/skin/animation conforme as capacidades do formato, e registrar os demais formatos como adapters planejados/testáveis.

---

# FASE 3 — MODELO INTERNO UNIVERSAL DE ANIMAÇÃO

## Objetivo

Criar a representação interna que permitirá tratar qualquer família de asset.

## Entidades

Asset, Mesh, Skeleton, Bone, Joint, Constraint, Rig, Pose, AnimationClip, AnimationTrack, Curve, Keyframe, MotionProfile, Deformer e Controller.

## Tipos de movimento

- Transform.
- Rotation.
- Translation.
- Scale.
- Morph target.
- Material parameter.
- Procedural parameter.

## Critério

Criar e reproduzir uma animação internamente sem depender de FBX.

---

# FASE 4 — SKELETON INSPECTOR E EDIÇÃO MANUAL

## Objetivo

Trabalhar com assets que já possuem skeleton.

## Recursos

- Tree hierarchy.
- Renomear bone.
- Reparent.
- Criar bone.
- Remover bone.
- Orientação.
- Pivot.
- Bind pose.
- Mirror.
- Symmetry.
- Bone groups.
- Markers.

## Critério

Abrir personagem rigado, editar hierarquia de forma controlada e visualizar.

---

# FASE 5 — SISTEMA DE MARCAÇÃO ANATÔMICA

## Objetivo

Criar o equivalente conceitual à marcação de auto-rig.

O usuário poderá marcar pontos diretamente sobre a malha.

## Humanoide

Root, Pelvis, Spine, Chest, Neck, Head, Shoulder L/R, Elbow L/R, Wrist L/R, Hand L/R, Hip L/R, Knee L/R, Ankle L/R, Foot L/R.

## Genérico

- Limb root/middle/end.
- Wing root/joint/tip.
- Tail root/segments.
- Jaw.
- Horn.
- Antenna.
- Fin.
- Tentacle root/tip.
- Custom joint.

## Critério

Salvar perfil anatômico reproduzível e utilizável pelo auto-rigger.

---

# FASE 6 — CLASSIFICAÇÃO ESTRUTURAL

## Objetivo

Determinar qual família de rig é adequada.

## Perfis

- Humanoid.
- Quadruped.
- Bird.
- Winged Humanoid.
- Multi-Legged.
- Serpentine.
- Insectoid.
- Aquatic.
- Plant.
- Object.
- Custom Creature.

## Critério

O sistema escolhe ou solicita um perfil sem forçar todos os assets a humanoide.

---

# FASE 7 — AUTO-RIGGING

## Objetivo

Gerar skeleton e rig automaticamente a partir de geometria, marcadores, perfil estrutural e parâmetros.

## Pipeline

Mesh
→ Markers
→ Geometry Analysis
→ Creature Profile
→ Skeleton Template
→ Bone Placement
→ Orientation
→ IK Chains
→ Constraints
→ Rig

## Humanoide

Root, Pelvis, Spine, Neck, Head, Arms, Hands, Legs, Feet.

## Quadrúpede

Pelvis, Spine, Neck, Head, Front limbs, Rear limbs, Tail.

## Asas

Wing root, wing chain, wing tip e controladores de penas quando aplicável.

## Plantas

Skeleton/deformation chains ou controladores procedurais.

## Critério

Asset sem skeleton recebe skeleton funcional através de marcação guiada.

---

# FASE 8 — SKINNING E DEFORMAÇÃO

## Objetivo

Fazer a malha acompanhar o skeleton.

## Recursos

- Automatic weights.
- Weight normalization.
- Influence limits.
- Mirror weights.
- Weight visualization.
- Weight smoothing.
- Manual correction.
- Heatmap.
- Vertex influence inspector.

## Validação

Ombros, cotovelos, joelhos, quadris, pescoço, cauda e asas.

## Critério

Personagem rigado executa poses sem deformações destrutivas nos casos suportados.

---

# FASE 9 — IK/FK E CONTROLE

## Objetivo

Criar controladores necessários para gerar movimento.

## IK

- Two-bone IK.
- Chain IK.
- Foot IK.
- Hand IK.
- Tentacle IK.
- Tail IK.
- Wing IK.

## FK

- Bone rotation.
- Local/global transform.
- Space switching.

## Constraints

- Look-at.
- Aim.
- Parent.
- Copy rotation.
- Limit rotation.
- Limit translation.
- Pole vector.
- Ground contact.

## Critério

Mover um pé ou mão e resolver automaticamente a cadeia.

---

# FASE 10 — POSE EDITOR

## Objetivo

Criar poses sem editar bones individualmente.

## Recursos

- Pose creation.
- Pose library.
- Mirror pose.
- Blend poses.
- Save/load.
- Reset.
- Presets por criatura.
- Interpolação.

## Critério

Criar, salvar e reutilizar uma pose compatível.

---

# FASE 11 — ANIMATION TIMELINE E EDITOR

## Objetivo

Criar editor completo.

## Recursos

- Timeline.
- Keyframes.
- Tracks.
- Curves.
- Dope sheet.
- Graph editor.
- Tangents.
- Interpolation.
- Loop.
- Reverse.
- Time scale.
- Root motion.
- Event markers.
- Animation sections.

## Critério

Criar, editar e reproduzir uma animação completa.

---

# FASE 12 — BIBLIOTECA PROCEDURAL

## Objetivo

Criar movimentos básicos sem depender de IA.

## Humanoides

Idle, Walk, Run, Sprint, Jump, Fall, Land, Turn, Strafe, Crouch, Attack, Heavy Attack, Block, Hit, Death, Cast, Gather, Interact.

## Quadrúpedes

Idle, Walk, Trot, Run, Gallop, Turn, Jump, Land, Attack, Hit, Death.

## Aves

Idle, Wing Flap, Takeoff, Flight, Glide, Landing.

## Rastejantes

Crawl, Slither, Coil, Strike.

## Plantas

Wind Sway, Growth, Open/Close, Branch Movement, Leaf Flutter.

## Critério

Gerar movimentos básicos por parâmetros.

---

# FASE 13 — SISTEMA DE INTENÇÃO DE ANIMAÇÃO

## Objetivo

Transformar solicitação humana em especificação técnica.

Exemplo:

"Crie um voo parado para MMORPG, com pernas levemente dobradas, uma mais baixa, corpo estável e asas batendo lentamente."

Resultado conceitual:

AnimationIntent
- category: FlyingIdle
- locomotion: stationary
- bodyStability: high
- legBend: mild
- legAsymmetry: enabled
- wingMotion: slow
- verticalOscillation: subtle
- loop: true

## Regra

A linguagem natural não gera keyframes diretamente. Primeiro gera uma estrutura validável.

## Critério

Solicitações conhecidas produzem AnimationIntent válido.

---

# FASE 14 — GERAÇÃO PROCEDURAL POR INTENÇÃO

## Objetivo

Transformar AnimationIntent em movimento.

## Pipeline

Intent
→ Motion Planner
→ Pose Planner
→ IK
→ Constraints
→ Timing
→ Curves
→ Animation Clip

## FlyIdle

Deve suportar:
- Pose inicial.
- Pose de flutuação.
- Assimetria das pernas.
- Oscilação de pelvis.
- Oscilação mínima de spine.
- Respiração.
- Movimento de asas.
- Correção de loop.

## Critério

Gerar animação reproduzível sem edição manual obrigatória.

---

# FASE 15 — GERAÇÃO COM IA

## Objetivo

Adicionar IA como camada avançada.

## Entradas

- Texto.
- Texto + parâmetros.
- Referência de pose.
- Vídeo/referência de movimento.
- Biblioteca de movimentos.

## Arquitetura

User Prompt
→ Prompt Parser
→ Animation Intent
→ AI Motion Backend
→ Motion Data
→ Retarget
→ Constraint Solver
→ Cleanup
→ Animation Clip

## Regra

IA é opcional. O núcleo continua funcional sem serviço externo.

## Critério

Pelo menos um backend pode transformar intenção em movimento validável.

---

# FASE 16 — RETARGETING UNIVERSAL

## Objetivo

Transferir movimento entre skeletons diferentes.

## Casos

- Humanoid → Humanoid.
- Quadruped → Quadruped compatível.
- Profiles equivalentes.
- Skeletons com nomes diferentes.
- Proporções diferentes.

## Recursos

- Bone mapping.
- Automatic mapping.
- Manual mapping.
- Pose normalization.
- Scale correction.
- Root correction.
- Contact correction.
- IK correction.

## Critério

Aplicar animação de referência a outro personagem compatível.

---

# FASE 17 — ANIMAÇÃO PROCEDURAL AVANÇADA

## Objetivo

Movimentos adaptativos em tempo real.

## Sistemas

- Foot placement.
- Ground adaptation.
- Look-at.
- Aim.
- Breathing.
- Secondary motion.
- Tail motion.
- Wing motion.
- Ear motion.
- Antenna motion.
- Eye direction.
- Head stabilization.
- Terrain-aware movement.

## Critério

Animações adaptam-se ao mundo aberto.

---

# FASE 18 — ASAS E ACESSÓRIOS

## Objetivo

Tratar elementos acoplados ao personagem.

## Exemplos

Asas, caudas, capas, armaduras, antenas, chifres, tentáculos e mecanismos.

## Caso Age of AetheR

GroundIdle
→ TakeOff
→ FlyIdle

FlyIdle
↔ FlyForward
↔ FlyBackward
↔ FlyLeft
↔ FlyRight

FlyIdle
→ FlyAttack
→ FlyIdle

## Critério

Gerar animações específicas para personagem + acessório.

---

# FASE 19 — PLANTAS E MUNDO VIVO

## Objetivo

Animar elementos não humanoides do mundo.

## Recursos

- Wind simulation.
- Branch hierarchy.
- Leaf flutter.
- Grass movement.
- Flower opening.
- Plant growth.
- Destruction.
- Procedural oscillation.
- Interaction response.

## Critério

Planta recebe movimento procedural sem ser tratada como humanoide.

---

# FASE 20 — CRIATURAS ESPECIAIS

## Objetivo

Cobrir anatomias fora dos perfis iniciais.

## Exemplos

Dragões, aranhas, escorpiões, serpentes, vermes, polvos, peixes, medusas, criaturas com múltiplas cabeças, múltiplos braços, golems, máquinas e bosses gigantes.

## Sistema

Custom Rig Profile.

## Critério

Arquitetura não possui limite fixo de 2 braços/2 pernas.

---

# FASE 21 — MOTION GRAPH / ANIMATION STATE MACHINE

## Objetivo

Transformar clips em comportamento de jogo.

## Recursos

- States.
- Transitions.
- Blend.
- Conditions.
- Parameters.
- Events.
- Layers.
- Upper-body overrides.
- Additive animation.

## Exemplo

Idle
→ speed > 0.1: Walk
→ speed > 3.0: Run
→ attack: Attack
→ flying: FlyIdle

## Critério

Visualizar comportamento completo antes de exportar para o game.

---

# FASE 22 — VISUALIZADOR AVANÇADO / LABORATÓRIO

## Objetivo

Transformar o viewer em ferramenta profissional.

## Recursos

- Multiple assets.
- Side-by-side comparison.
- Motion trails.
- Bone trajectories.
- Contact markers.
- Ground visualization.
- IK targets.
- Control rig visualization.
- Animation graph.
- Timeline.
- Curve editor.
- Performance profiler.

## Critério

Todo o ciclo de criação pode ser acompanhado no programa.

---

# FASE 23 — QUALIDADE E VALIDAÇÃO

## Objetivo

Detectar problemas antes da exportação.

## Verificações

- Missing bones.
- Broken hierarchy.
- Invalid weights.
- Excessive influences.
- NaN transforms.
- Exploding mesh.
- Foot sliding.
- Ground penetration.
- Extreme rotations.
- Loop discontinuity.
- Root motion inconsistency.
- Invalid duration.
- Unsupported export features.

## Critério

Animação validada antes de sair do programa.

---

# FASE 24 — EXPORTAÇÃO

## Objetivo

Produzir arquivos utilizáveis.

## Prioridade

- FBX.
- GLB/glTF.
- Integrações específicas futuramente.

## Opções

- FPS.
- Start/end frame.
- Root motion.
- Scale.
- Coordinate system.
- Bone axes.
- Clips.
- Naming.

## Critério

Exportar asset + skeleton + skin + animation de forma consistente.

---

# FASE 25 — INTEGRAÇÃO COM UNREAL ENGINE

## Objetivo

Reduzir trabalho manual no pipeline do Age of AetheR.

## Etapas

Inicial:
- Exportar FBX correto.

Intermediária:
- Estrutura preparada para importação.

Avançada:
- Plugin Unreal para import/update de skeleton, animations, montages, blend spaces e IK.

## Critério

Asset criado no Auto-Animation chega ao Unreal com mínima intervenção.

---

# FASE 26 — PRESETS MMORPG

## Objetivo

Criar perfis prontos.

## Presets

Humanoid RPG, Flying RPG, Quadruped RPG, Monster, Boss e NPC.

## Flying RPG

Takeoff, FlyIdle, FlyForward, FlyBackward, FlyStrafe, FlyAttack, FlyLand.

---

# FASE 27 — PRODUÇÃO EM ESCALA

## Objetivo

Centenas ou milhares de assets.

## Recursos

- Batch import.
- Batch rig.
- Batch animation generation.
- Batch retarget.
- Batch export.
- Caching.
- Background processing.
- Job queue.
- Progress.
- Cancel/resume.

---

# FASE 28 — SISTEMA DE PROJETOS

## Objetivo

Salvar todo o trabalho.

Project:
- Assets.
- Rig Profiles.
- Animation Profiles.
- Generated Clips.
- Poses.
- Motion Graphs.
- Export Settings.
- Metadata.

## Critério

Fechar e reabrir sem perder o projeto.

---

# FASE 29 — UX E FLUXO PROFISSIONAL

## Objetivo

Permitir uso por alguém sem conhecimento profundo de rigging.

Fluxo:

NEW PROJECT
→ IMPORT ASSET
→ ANALYZE
→ MARK / CONFIRM BODY
→ AUTO RIG
→ TEST RIG
→ CREATE ANIMATION
→ PREVIEW
→ ADJUST
→ VALIDATE
→ EXPORT

Painéis:
- Asset.
- Skeleton.
- Rig.
- Animation.
- Prompt.
- Parameters.
- Timeline.
- Preview.
- Validation.
- Export.

---

# FASE 30 — RELEASE

## Objetivo

Produto utilizável em produção.

## Entregas

- Full regression.
- Documentation.
- Sample assets.
- Windows build.
- Installation package.
- Performance tests.
- Error recovery.
- Unreal pipeline.
- End-to-end tests.

---

# PRIMEIRO MVP FUNCIONAL

Antes de IA avançada e de todos os tipos de criatura, o primeiro marco prático será:

FBX sem skeleton
→ Marcação humanoide
→ Auto-rig
→ Skinning
→ FlyIdle procedural
→ Preview
→ Export FBX

Caso de teste oficial:

- personagem suspenso;
- sem deslocamento horizontal;
- pernas levemente dobradas;
- uma perna mais baixa;
- corpo estável;
- pequena oscilação;
- asas em movimento;
- loop perfeito.

Esse MVP prova o núcleo do conceito.

---

# REGRA DE CONCLUSÃO

Uma fase só será considerada concluída quando:
- implementação existir;
- testes relevantes forem executados;
- casos de erro forem testados;
- documentação mínima existir;
- integração com fases anteriores funcionar;
- nenhum erro bloqueante conhecido permanecer dentro do escopo.

Estados:
- NÃO INICIADA
- EM ANDAMENTO
- BLOQUEADA
- CONCLUÍDA
- REVISÃO NECESSÁRIA

# PRIORIDADE

1. Fases 0–4: Fundamentos e pipeline universal de assets.
2. Fases 5–10: Rigging.
3. Fases 11–14: Animação.
4. Fases 15–16: IA e retarget.
5. Fases 17–20: Mundo MMORPG.
6. Fases 21–25: Pipeline.
7. Fases 26–30: Produção.

# VISÃO FINAL

O Auto-Animation não será um simples gerador de animações humanoides.

Será um Animation Authoring Engine universal, capaz de trabalhar com bones, joints, deformers, morphs, constraints e movimento procedural.

O usuário deverá pensar:

"O que eu quero que esse asset faça?"

e não:

"Como eu mesmo faço todos esses bones e keyframes?"
