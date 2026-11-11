# Letras Invasoras V4 — Linux (Arch), C e SDL2

Jogo 2D de digitação inspirado em Space Invaders. Gráficos desenhados em tempo real, sem imagens pesadas.

## Mudanças da V4

- **Fase 3 corrigida:** cada letra da palavra precisa de digitação + F + destruição de um meteoro. Só depois aparece a letra seguinte. A palavra fica verde letra por letra depois de cada disparo bem-sucedido.
- **Meteoros:** de 4 a 8 inimigos simultâneos, sorteados novamente a cada letra, todos com letras e apenas um com a resposta correta. Posições começam abaixo do cabeçalho e da linha de instruções.
- **Nave redesenhada:** silhueta com ponta, fuselagem, cabine, asas delta, motores e chamas, girando na direção do alvo.
- **Fase 2 expandida:** agora cobre todas as 26 letras A–Z, uma vez por fase (26 alvos), inclusive as teclas da mão direita. O mini teclado agora desenha todas as letras A–Z.
- **Fase 1:** 15 alvos usando A S D F G; **Fase 3:** 8 palavras da mão esquerda (cada letra tem um tiro).

## Instalar no Arch

```bash
sudo pacman -S --needed base-devel sdl2 pkgconf unzip
```

## Compilar e executar

```bash
make clean && make
./letras_invasoras
```

## Controles

- **A–Z:** digitar a letra do meteoro pedido.
- **F:** depois de digitar a letra, atirar no meteoro (para letra F, pressione F duas vezes).
- **Espaço:** pausa. **Tab:** modo fácil. **F1:** reiniciar. **Esc:** sair.

## Arquivos

`src/main.c`, `src/game.c`, `src/levels.c`, `src/keyboard.c`, `src/render.c`, `src/effects.c`, `src/gfx.c`, `src/font.c` e `include/game.h`.

**Nota:** SDL2 >= 2.0.18 é necessária para desenhar a nave usando `SDL_RenderGeometry`. O SDL2 atual do Arch atende a esse requisito.
