#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "estruturas.h"
#include "declaracoes.h"
#include "definicoes.h"

enum estadosJogo {gameOver, ativo, pause} JOGO;

GLuint idTexturaFundo;
GLuint idTexturaSheet;

tipoSprite jogador;
tipoSprite tiroJogador;
tipoSprite tiroInimigo;
tipoSprite vetorInimigos[QTD_INIMIGOS];
tipoSprite iconeVidas[MAX_VIDAS];

int VIDAS;

void desenhaHud()
{
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-LARGURA_DO_MUNDO/2, -ALTURA_DO_MUNDO/2, 1);
        glVertex3f(LARGURA_DO_MUNDO/2, -ALTURA_DO_MUNDO/2, 1);
        glVertex3f(LARGURA_DO_MUNDO/2, -ALTURA_DO_MUNDO/2 + ALTURA_HUD, 1);
        glVertex3f(-LARGURA_DO_MUNDO/2, -ALTURA_DO_MUNDO/2 + ALTURA_HUD, 1);
    glEnd();

    // desenha vidas
    for (int i = 0; i < VIDAS; i++) {
        desenhaSprite(idTexturaSheet, iconeVidas[i], 0.490, 0.006, 0.036, 0.025);
    }
}

void inimigoAtira()
{
    int sorteiaTiro = rand() % 32000;
    int sorteiaInimigo = rand() % QTD_INIMIGOS;

    if (sorteiaTiro == 0 && !tiroInimigo.ativo && vetorInimigos[sorteiaInimigo].ativo) {
        inicializaTiro(&tiroInimigo, vetorInimigos[sorteiaInimigo].posicao.x, vetorInimigos[sorteiaInimigo].posicao.y);
        glutPostRedisplay();
    }
}

void detectaTiro(tipoSprite* tiro)
{
    tipoSprite* alvo;
    float dist;

    for (int i = 0; i < QTD_INIMIGOS; i++) {
        if (tiro == &tiroJogador)
            alvo = &vetorInimigos[i];
        else if (tiro == &tiroInimigo)
            alvo = &jogador;

        dist = formulaDistancia(tiro->posicao.x, alvo->posicao.x, tiro->posicao.y, alvo->posicao.y);
        if (tiro->ativo && alvo->ativo && dist <= tiro->raio + alvo->raio) {
            tiro->ativo = FALSE;
            if (alvo == &jogador) {
                VIDAS -= 1;
                verificaGameOver();
            } else if (alvo == &vetorInimigos[i]) {
                alvo->ativo = FALSE;
            }
        }
    }
}

void movimentaTiros()
{
    if (tiroInimigo.posicao.y < -ALTURA_DO_MUNDO/2)
        tiroInimigo.ativo = FALSE;

    if (JOGO == ativo && tiroInimigo.ativo) {
        tiroInimigo.posicao.y -= tiroInimigo.velocidade;
        detectaTiro(&tiroInimigo);
    }

    if (tiroJogador.posicao.y > ALTURA_DO_MUNDO/2)
        tiroJogador.ativo = FALSE;

    if (JOGO == ativo && tiroJogador.ativo) {
        tiroJogador.posicao.y += tiroJogador.velocidade;
        detectaTiro(&tiroJogador);
    }

    glutPostRedisplay();
    glutTimerFunc(33, movimentaTiros, 33);
}

void jogadorAtira()
{
    if (JOGO == ativo && !tiroJogador.ativo) {
        inicializaTiro(&tiroJogador, jogador.posicao.x, jogador.posicao.y + jogador.dimensoes.y/2);
        desenhaSprite(idTexturaSheet, tiroJogador, 0.833, 0.339, 0.008, 0.036);
        glutPostRedisplay();
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
    glutPostRedisplay();
}

void movimentaInimigos()
{
    if (JOGO == ativo) {
        for (int i = 0; i < QTD_INIMIGOS; i++) {
            if (vetorInimigos[i].ativo) {
                if (vetorInimigos[i].posicao.y <= jogador.posicao.y + jogador.dimensoes.y) {
                    // jogador perdeu - um inimigo chegou no "chão"
                    JOGO = gameOver;
                } else if (vetorInimigos[i].posicao.x <= -LARGURA_DO_MUNDO/2 + vetorInimigos[i].dimensoes.x/2 ||
                            vetorInimigos[i].posicao.x >= LARGURA_DO_MUNDO/2 - vetorInimigos[i].dimensoes.x/2) {
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
    }
    glutPostRedisplay();
    glutTimerFunc(33, movimentaInimigos, 33);
}

void desenhaFase()
{
    desenhaSprite(idTexturaSheet, jogador, 0.000, 0.007, 0.109, 0.073);

    if (tiroJogador.ativo)
        desenhaSprite(idTexturaSheet, tiroJogador, 0.834, 0.339, 0.008, 0.036);

    if (tiroInimigo.ativo)
        desenhaSprite(idTexturaSheet, tiroInimigo, 0.835, 0.151, 0.008, 0.055);

    for (int i = 0; i < QTD_INIMIGOS; i++) {
        if (vetorInimigos[i].ativo)
            desenhaSprite(idTexturaSheet, vetorInimigos[i], 0.413, 0.207, 0.090, 0.082);
    }
}

void desenharMinhaCena()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 1, 1);

    // habilita texturas
    glEnable(GL_TEXTURE_2D);

    // desenhos
    desenhaFundoJogo(idTexturaFundo);
    desenhaFase();

    if (JOGO == ativo) {
        movimentaInimigos();
    } else if (JOGO == pause) {
        glColor3f(1, 0, 0);
        escreveTexto(GLUT_BITMAP_HELVETICA_18, "JOGO PAUSADO", -20, 0);
    } else if (JOGO == gameOver) {
        glColor3f(1, 0, 0);
        escreveTexto(GLUT_BITMAP_HELVETICA_18, "GAME OVER", -20, 0);
    }

    desenhaHud();

    glDisable(GL_TEXTURE_2D);
    glutSwapBuffers();
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

void inicializaTiro(tipoSprite* tiro, float x, float y)
{
    inicializaSprite(tiro, x, y, 1, 5, 1);
    tiro->velocidade = 2;
}

void inicializaInimigo()
{
    int x = -LARGURA_DO_MUNDO/2 + 20;
    int y = ALTURA_DO_MUNDO * 0.4;
    for (int i = 0; i < QTD_INIMIGOS; i++) {
        inicializaSprite(&vetorInimigos[i], x, y, 15, 15, 15/2);
        vetorInimigos[i].velocidade = 0.001;
        x += 20;
        if ((i+1) % MAX_INIMIGOS_LINHA == 0){
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

void redimensiona(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-LARGURA_DO_MUNDO/2, LARGURA_DO_MUNDO/2, -ALTURA_DO_MUNDO/2, ALTURA_DO_MUNDO/2, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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

void inicializaJogo()
{
    JOGO = ativo;
    VIDAS = VIDAS_INICIAIS;
    tiroInimigo.ativo = FALSE;
    tiroJogador.ativo = FALSE;
    inicializaJogador();
    inicializaInimigo();
    inicializaHud();
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
    glutInitContextVersion(1, 1);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Jogo");

    // registro de callbacks
    glutDisplayFunc(desenharMinhaCena);
    glutReshapeFunc(redimensiona);
    glutKeyboardFunc(teclaPressionada);
    glutSpecialFunc(teclasEspeciais);
    glutTimerFunc(0, movimentaInimigos, 33);
    glutTimerFunc(0, movimentaTiros, 33);

    // configura variaveis de estado
    inicializa();

    // inicializa variaveis do jogo
    inicializaJogo();

    // main loop
    glutMainLoop();

    return 0;
}
