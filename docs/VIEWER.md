# Auto-Animation — Viewer (Fase 1)

## Objetivo

A Fase 1 estabelece um visualizador 3D independente do núcleo de animação. O viewer é uma aplicação de inspeção que será reutilizada por importação, rigging, skinning, edição de poses e animação.

## Tecnologia

- C++20.
- CMake.
- SDL2 para janela, entrada e contexto OpenGL.
- OpenGL 2.1/compatibility profile para o protótipo da Fase 1.
- O renderer está isolado em `viewer/`; o core não conhece SDL ou OpenGL.

A escolha é deliberadamente simples para validar o fluxo visual rapidamente. A camada de renderização poderá ser substituída por um backend moderno sem alterar os modelos de domínio.

## Controles atuais

- Arrastar com botão esquerdo: orbit.
- Roda do mouse: zoom.
- `Space`: play/pause da animação demonstrativa.
- `W`: solid/wireframe.
- `Left/Right`: avançar/retroceder 1 frame a 30 FPS.
- `Esc`: fechar.

## Demonstração

A aplicação inicia com um asset procedural simples representando um personagem suspenso com asas. Ele demonstra:

- câmera 3D;
- grid;
- eixos;
- profundidade;
- iluminação básica via material/cor;
- animação em loop;
- controle play/pause;
- wireframe;
- FPS.

A geometria demonstrativa não é o formato interno final do projeto e não substitui o importador. Ela existe para validar o viewer antes da Fase 2.

## Próximo encaixe

O viewer deverá consumir a representação universal de asset definida para a camada de importação, sem conhecer FBX, OBJ, SMD, GLB/GLTF, DAE, 3DS, STL, PLY ou formatos proprietários.

A arquitetura de importação será:

Arquivo
→ detector de formato
→ importer específico
→ representação universal
→ viewer

Formatos com skeleton/animação e formatos somente geométricos terão capacidades diferentes na representação interna.
