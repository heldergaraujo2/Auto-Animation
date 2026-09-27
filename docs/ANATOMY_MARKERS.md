# Sistema de Marcação Anatômica

A Fase 5 fornece um perfil anatômico persistível e editável, sem assumir que o asset seja humanoide.

## Modelo

Cada marcador contém tipo anatômico, nome, posição, normal, espaço (`Object` ou `Surface`), bone hint opcional, confiança e obrigatoriedade.

Os tipos cobrem humanoides, asas, caudas, mandíbula, chifres, antenas, nadadeiras, tentáculos e marcadores customizados.

## Edição visual

O Viewer pode receber um `MarkerSet` editável com `set_editable_markers`.

Modo de demonstração:

`auto-animation --marker-editor`

Controles:

- `F`: ativar/desativar edição;
- clique: selecionar o marcador mais próximo;
- arrastar: mover o marcador no plano de sua altura;
- `Shift + clique`: criar o tipo anatômico ativo no plano do chão;
- `[` / `]`: trocar o tipo ativo;
- setas: mover o marcador selecionado;
- `Shift + setas`: mover em passos maiores;
- `Delete`: remover;
- `M`: espelhar marcador selecionado;
- `A`: espelhar todos os marcadores;
- `Ctrl+S`: salvar em `markers.autoanim`;
- `Ctrl+O`: carregar `markers.autoanim`.

A edição visual é separada da lógica anatômica: seleção, criação, remoção, espelhamento e ajustes ficam em `MarkerEditor`, enquanto SDL/OpenGL somente fornece interação e visualização.

## Persistência

O formato textual versionado é:

`AUTO_ANIMATION_MARKERS 1`

O perfil é validado antes de ser salvo e novamente durante o carregamento. Perfis inválidos, arquivos inexistentes, tipos desconhecidos e registros malformados são rejeitados sem substituir o perfil atual.

## Integração com auto-rigging

`rigging::build_rig_from_markers` consome o mesmo `MarkerSet`. Assim, o fluxo é:

`marcar → validar → salvar → carregar → auto-rig`

O editor não cria uma representação paralela incompatível com o rigging.

## Limite atual

A interação de criação usa um plano 3D de referência para assets ainda não carregados no Viewer. Quando o pipeline universal de Mesh estiver conectado ao editor, o mesmo sistema poderá trocar o plano de criação por raycast contra a superfície real da malha sem alterar o modelo de marcadores.
