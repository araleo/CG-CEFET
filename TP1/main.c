#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "estruturas.h"
#include "declaracoes.h"
#include "definicoes.h"

enum estadosJogo {gameOver, ativo, pause, vitoria} JOGO;
enum itens {vida, velocidade, tiro} ITENS;

GLuint idTexturaFundo;
GLuint idTexturaSheet;

tipoSprite jogador;
tipoSprite tiroJogador;
tipoSprite tiroInimigo;
tipoSprite vetorInimigos[QTD_INIMIGOS];
tipoSprite iconeVidas[MAX_VIDAS];
tipoSprite itemDrop;

int VIDAS;
int PONTOS;
char strPontos[10];
int TIRO_ESPECIAL = FALSE;

void detectaItem()
{
    if (detectaColisao(jogador, itemDrop)) {
        itemDrop.ativo = FALSE;
        PONTOS += 10;

        if (ITENS == vida && VIDAS < 5)
            VIDAS += 1;
        else if (ITENS == velocidade && jogador.velocidade < 5)
            jogador.velocidade += 1;
        else if (ITENS == tiro)
            TIRO_ESPECIAL = TRUE;
    }
}

void dropaItem(float x, float y)
{
    inicializaItem(&itemDrop, x, y);
    glutPostRedisplay();
}

void sorteiaItem(tipoSprite inimigo)
{
    int sorteio = rand() % 6;
    if (sorteio < 3) {
        ITENS = sorteio;
        dropaItem(inimigo.posicao.x, inimigo.posicao.y);
    }
}

void inimigoAtira()
{
    int sorteiaTiro = rand() % 32000;
    int sorteiaInimigo = rand() % QTD_INIMIGOS;
    if (sorteiaTiro == 0 && !tiroInimigo.ativo && vetorInimigos[sorteiaInimigo].ativo)
        inicializaTiro(&tiroInimigo, vetorInimigos[sorteiaInimigo].posicao.x, vetorInimigos[sorteiaInimigo].posicao.y);
}

void detectaTiro(tipoSprite* tiro)
{
    tipoSprite* alvo = NULL;

    if (tiro == &tiroInimigo) {
        alvo = &jogador;
        if (tiro->ativo && detectaColisao(*alvo, *tiro)) {
            tiro->ativo = FALSE;
            VIDAS -= 1;
            PONTOS -= 10;
            verificaGameOver();
        }
    } else if (tiro == &tiroJogador) {
        for (int i = 0; i < QTD_INIMIGOS; i++) {
            alvo = &vetorInimigos[i];
            if (tiro->ativo && alvo->ativo && detectaColisao(*alvo, *tiro)) {
                tiro->ativo = FALSE;
                alvo->ativo = FALSE;
                PONTOS += ((-alvo->posicao.y + 150) / 10) + 10;
                verificaVitoria();
                if (!itemDrop.ativo)
                    sorteiaItem(*alvo);
            }
        }
    }
}

void movimentaProps()
{
    // movimenta os tiros dos inimigos
    if (tiroInimigo.posicao.y < -ALTURA_DO_MUNDO/2)
        tiroInimigo.ativo = FALSE;

    if (JOGO == ativo && tiroInimigo.ativo) {
        tiroInimigo.posicao.y -= tiroInimigo.velocidade;
        detectaTiro(&tiroInimigo);
    }

    // movimenta os tiros do jogador
    if (tiroJogador.posicao.y > ALTURA_DO_MUNDO/2 || tiroJogador.posicao.y > vetorInimigos[0].posicao.y + 30)
        tiroJogador.ativo = FALSE;

    if (JOGO == ativo && tiroJogador.ativo) {
        tiroJogador.posicao.y += tiroJogador.velocidade;
        detectaTiro(&tiroJogador);
    }

    // movimenta os itens
    if (itemDrop.posicao.y < -ALTURA_DO_MUNDO/2)
        itemDrop.ativo = FALSE;

    if (JOGO == ativo && itemDrop.ativo) {
        itemDrop.posicao.y -= itemDrop.velocidade;
        detectaItem();
    }

    // glutPostRedisplay();
    glutTimerFunc(33, movimentaProps, 0);
}

void jogadorAtira()
{
    if (JOGO == ativo && !tiroJogador.ativo) {
        inicializaTiro(&tiroJogador, jogador.posicao.x, jogador.posicao.y + jogador.dimensoes.y/2);
        desenhaSprite(idTexturaSheet, tiroJogador, 0.833, 0.339, 0.008, 0.036);
        // glutPostRedisplay();
    }
}

void movimentaJogador(char lado)
{
    if (lado == 'e') {
        if (JOGO == ativo && jogador.posicao.x > -LARGURA_DO_MUNDO/2 + jogador.dimensoes.x/2) {
            jogador.posicao.x -= jogador.velocidade;
        }
    } else if (lado == 'd') {
        if (JOGO == ativo && jogador.posicao.x < LARGURA_DO_MUNDO/2 - jogador.dimensoes.x/2) {
            jogador.posicao.x += jogador.velocidade;
        }
    }
    // glutPostRedisplay();
}

void movimentaInimigos()
{
    if (JOGO == ativo) {
        for (int i = 0; i < QTD_INIMIGOS; i++) {
            if (vetorInimigos[i].ativo) {
                if (vetorInimigos[i].posicao.y <= jogador.posicao.y + jogador.dimensoes.y) {
                    // jogador perdeu - um inimigo chegou no "chão"
                    JOGO = gameOver;
                } else if (vetorInimigos[i].posicao.x <= -LARGURA_DO_MUNDO/2 + vetorInimigos[i].dimensoes.x/2
                          || vetorInimigos[i].posicao.x >= LARGURA_DO_MUNDO/2 - vetorInimigos[i].dimensoes.x/2) {
                    // move verticalmente e altera a direção
                    for (int j = 0; j < QTD_INIMIGOS; j++) {
                        vetorInimigos[j].posicao.y -= 20;
                        vetorInimigos[j].velocidade *= -1;
                        vetorInimigos[j].posicao.x += vetorInimigos[j].velocidade;
                    }
                } else {
                    // move horizontalmente
                    vetorInimigos[i].posicao.x += vetorInimigos[i].velocidade;
                    inimigoAtira();
                }
            }
        }
        glutPostRedisplay();
        glutTimerFunc(33, movimentaInimigos, 0);
    }
}

void desenhaFase()
{
    // desenha nave do jogador
    desenhaSprite(idTexturaSheet, jogador, 0.000, 0.007, 0.109, 0.073);

    // desenha tiro do jogador se ativo
    if (tiroJogador.ativo)
        desenhaSprite(idTexturaSheet, tiroJogador, 0.834, 0.339, 0.008, 0.036);

    // desenha tiro dos inimigos se ativo
    if (tiroInimigo.ativo)
        desenhaSprite(idTexturaSheet, tiroInimigo, 0.835, 0.151, 0.008, 0.055);

    // desenha naves dos inimigos
    for (int i = 0; i < QTD_INIMIGOS; i++) {
        if (vetorInimigos[i].ativo)
            desenhaSprite(idTexturaSheet, vetorInimigos[i], 0.413, 0.207, 0.090, 0.082);
    }

    // desenha itens de drop se ativos
    if (itemDrop.ativo) {
        if (ITENS == vida)
            desenhaSprite(idTexturaSheet, itemDrop, 0.217, 0.874, 0.021, 0.020);
        else if (ITENS == velocidade)
            desenhaSprite(idTexturaSheet, itemDrop, 0.790, 0.543, 0.018, 0.029);
        else if (ITENS == tiro)
            desenhaSprite(idTexturaSheet, itemDrop, 0.760, 0.426, 0.030, 0.029);
    }

}

void desenhaHud()
{
    // desenha o fundo
    glColor4f(1, 1, 1, 0.4);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-LARGURA_DO_MUNDO/2, -ALTURA_DO_MUNDO/2, 1);
        glVertex3f(LARGURA_DO_MUNDO/2, -ALTURA_DO_MUNDO/2, 1);
        glVertex3f(LARGURA_DO_MUNDO/2, -ALTURA_DO_MUNDO/2 + ALTURA_HUD, 1);
        glVertex3f(-LARGURA_DO_MUNDO/2, -ALTURA_DO_MUNDO/2 + ALTURA_HUD, 1);
    glEnd();

    // mostra os pontos do jogador
    glColor4f(1, 1, 1, 1);
    snprintf(strPontos, 10, "%d", PONTOS);
    escreveTexto(GLUT_BITMAP_HELVETICA_18, "PONTOS: ", LARGURA_DO_MUNDO/2 - 45, -ALTURA_DO_MUNDO/2 + 2);
    escreveTexto(GLUT_BITMAP_HELVETICA_18, strPontos, LARGURA_DO_MUNDO/2 - 20, -ALTURA_DO_MUNDO/2 + 2);

    // desenha vidas
    for (int i = 0; i < VIDAS; i++)
        desenhaSprite(idTexturaSheet, iconeVidas[i], 0.490, 0.006, 0.036, 0.025);
}

void desenharMinhaCena()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 1, 1);

    // desenhos
    desenhaFundoJogo(idTexturaFundo);
    desenhaFase();
    desenhaHud();

    if (JOGO == ativo) {
        movimentaInimigos();
    } else if (JOGO == pause) {
        glColor3f(1, 0, 0);
        escreveTexto(GLUT_BITMAP_HELVETICA_18, "JOGO PAUSADO", -20, 0);
    } else if (JOGO == gameOver) {
        glColor3f(1, 0, 0);
        escreveTexto(GLUT_BITMAP_HELVETICA_18, "GAME OVER", -15, 0);
    } else if (JOGO == vitoria) {
        glColor3f(0, 1, 0);
        escreveTexto(GLUT_BITMAP_HELVETICA_18, "VITORIA", -10, 0);
    }

    glutSwapBuffers();
}

int verificaVitoria()
{
    for (int i = 0; i < QTD_INIMIGOS; i++)
        if (vetorInimigos[i].ativo)
            return 0;
    JOGO = vitoria;
}

void verificaGameOver()
{
    if (VIDAS == 0) {
        jogador.ativo = FALSE;
        JOGO = gameOver;
    }
}

void pausa()
{
    if (JOGO == ativo)
        JOGO = pause;
    else if (JOGO == pause)
        JOGO = ativo;
    glutPostRedisplay();
}

void teclaPressionada(unsigned char key, int x, int y)
{
    switch(key) {
        case 27:
            exit(0);
            break;
        case 'A':
        case 'a':
            movimentaJogador('e');
            break;
        case 'D':
        case 'd':
            movimentaJogador('d');
            break;
        case ' ':
            jogadorAtira();
            break;
        case 'P':
        case 'p':
            pausa();
            break;
        case 'R':
        case 'r':
            inicializaJogo();
            break;
        default:
            break;
    }
}

void teclasEspeciais(int key, int x, int y)
{
    switch(key) {
        case GLUT_KEY_LEFT:
            movimentaJogador('e');
            break;
        case GLUT_KEY_RIGHT:
            movimentaJogador('d');
            break;
    }
}

void inicializaSprite(tipoSprite* sprite, float x, float y, float comprimento, float altura, float raio)
{
    sprite->posicao.x = x;
    sprite->posicao.y = y;
    sprite->dimensoes.x = comprimento;
    sprite->dimensoes.y = altura;
    sprite->raio = raio;
    sprite->ativo = TRUE;
}

void inicializaHud()
{
    int x = -LARGURA_DO_MUNDO/2 + 20;
    int y = -ALTURA_DO_MUNDO/2 + 5;
    int distancia = 10;

    for (int i = 0; i < MAX_VIDAS; i++) {
        inicializaSprite(&iconeVidas[i], x, y, 5, 5, 5);
        x += distancia;
        if(i >= VIDAS)
            iconeVidas[i].ativo = FALSE;
    }
}

void inicializaItem(tipoSprite* item, float x, float y)
{
    inicializaSprite(item, x, y, 7, 7, 7/2);
    item->velocidade = 2;
}

void inicializaTiro(tipoSprite* tiro, float x, float y)
{
    inicializaSprite(tiro, x, y, 1, 5, 1);
    if (tiro == &tiroJogador) {
        tiro->velocidade = 4;
        if (TIRO_ESPECIAL) {
            tiro->dimensoes.x = 2;
            tiro->dimensoes.y = 10;
            tiro->raio = 3;
        }
    }
    else
        tiro->velocidade = 2;
}

void inicializaInimigos()
{
    int x = -LARGURA_DO_MUNDO/2 + 20;
    int y = ALTURA_DO_MUNDO * 0.4;
    for (int i = 0; i < QTD_INIMIGOS; i++) {
        inicializaSprite(&vetorInimigos[i], x, y, 15, 15, 15/2);
        vetorInimigos[i].velocidade = 0.001;
        x += 20;
        if ((i+1) % MAX_INIMIGOS_LINHA == 0) {
            y -= 30;
            x = -LARGURA_DO_MUNDO/2 + 20;
        }
    }
}

void inicializaJogador()
{
    inicializaSprite(&jogador, 0, -ALTURA_DO_MUNDO * 0.4, 20, 20, 20/2);
    jogador.velocidade = 2;
}

void inicializaJogo()
{
    JOGO = ativo;
    PONTOS = 0;

    VIDAS = VIDAS_INICIAIS;
    TIRO_ESPECIAL = FALSE;
    tiroInimigo.ativo = FALSE;
    tiroJogador.ativo = FALSE;
    itemDrop.ativo = FALSE;
    inicializaJogador();
    inicializaInimigos();
    inicializaHud();
    glutPostRedisplay();
}

void redimensiona(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-LARGURA_DO_MUNDO/2, LARGURA_DO_MUNDO/2, -ALTURA_DO_MUNDO/2, ALTURA_DO_MUNDO/2, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void inicializa()
{
    glClearColor(1, 1, 1, 1);

    // habilita mesclagem de cores
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // carrega texturas
    idTexturaFundo = carregaTextura("imagens/fundo.png");
    idTexturaSheet = carregaTextura("imagens/sheet.png");
}

int main(int argc, char** argv)
{
    // inicializa gerador de números aleatórios
    srand(time(0));

    // inicialização do glut
    glutInit(&argc, argv);
    // glutInitContextVersion(1, 1);
    // glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Jogo");

    // registro de callbacks
    glutDisplayFunc(desenharMinhaCena);
    glutReshapeFunc(redimensiona);
    glutKeyboardFunc(teclaPressionada);
    glutSpecialFunc(teclasEspeciais);

    glutTimerFunc(0, movimentaInimigos, 0);
    glutTimerFunc(0, movimentaProps, 0);

    // configura variaveis de estado
    inicializa();

    // inicializa variaveis do jogo
    inicializaJogo();

    // main loop
    glutMainLoop();

    return 0;
}
