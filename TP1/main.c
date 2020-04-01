#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "estruturas.h"
#include "declaracoes.h"
#include "definicoes.h"

enum estados_jogo {gameOver, ativo, pause} JOGO;

GLuint idTexturaFundo;
GLuint idTexturaSheet;

tipoSprite jogador;
tipoSprite tiroJogador;
tipoSprite tiroInimigo;
tipoSprite vetorInimigos[QTD_INIMIGOS];

void verificaGameOver()
{
    if (!jogador.ativo)
        JOGO = gameOver;
}

void inimigoAtira()
{
    int sorteiaTiro = rand() % 32000;
    int sorteiaInimigo = rand() % QTD_INIMIGOS;
    if (sorteiaTiro == 0 && !tiroInimigo.ativo) {
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
        if (alvo->ativo && dist <= tiro->raio + alvo->raio) {
            alvo->ativo = FALSE;
            tiro->ativo = FALSE;
            verificaGameOver();
        }
    }
}

void movimentaTiroInimigo()
{
    if (tiroInimigo.posicao.y < -ALTURA_DO_MUNDO/2)
        tiroInimigo.ativo = FALSE;

    if (JOGO == ativo)
        tiroInimigo.posicao.y -= tiroInimigo.velocidade;

    if (tiroInimigo.ativo)
        detectaTiro(&tiroInimigo);

    glutPostRedisplay();
    glutTimerFunc(33, movimentaTiroInimigo, 33);
}

void movimentaTiroJogador()
{
    if (tiroJogador.posicao.y > ALTURA_DO_MUNDO/2)
        tiroJogador.ativo = FALSE;

    if (JOGO == ativo)
        tiroJogador.posicao.y += tiroJogador.velocidade;

    if (tiroJogador.ativo)
        detectaTiro(&tiroJogador);

    glutPostRedisplay();
    glutTimerFunc(33, movimentaTiroJogador, 33);
}

void movimentaInimigos()
{
    if (JOGO == ativo) {
        for (int i = 0; i < QTD_INIMIGOS; i++) {
            if (vetorInimigos[i].ativo) {
                if (vetorInimigos[i].posicao.y <= jogador.posicao.y + jogador.dimensoes.y) {
                    // jogador perdeu
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

void desenhaSprite(tipoSprite sprite, float sheetX, float sheetY, float comprimento, float altura)
{
    glBindTexture(GL_TEXTURE_2D, idTexturaSheet);
    glPushMatrix();
        glTranslatef(sprite.posicao.x, sprite.posicao.y, 0);
        glBegin(GL_TRIANGLE_FAN);
            // Associamos um canto da textura para cada vértice
            glTexCoord2f(sheetX, sheetY);
            glVertex3f(-sprite.dimensoes.x/2, -sprite.dimensoes.y/2, 0);

            glTexCoord2f(sheetX + comprimento, sheetY);
            glVertex3f(sprite.dimensoes.x/2, -sprite.dimensoes.y/2, 0);

            glTexCoord2f(sheetX + comprimento, sheetY + altura);
            glVertex3f(sprite.dimensoes.x/2, sprite.dimensoes.y/2, 0);

            glTexCoord2f(sheetX, sheetY + altura);
            glVertex3f(-sprite.dimensoes.x/2, sprite.dimensoes.y/2, 0);
        glEnd();
    glPopMatrix();
}

void desenhaFundoJogo()
{
    // usa textura
    glBindTexture(GL_TEXTURE_2D, idTexturaFundo);
    glBegin(GL_TRIANGLE_FAN);
        // Associamos um canto da textura para cada vértice
        glTexCoord2f(0, 0);
        glVertex3f(-LARGURA_DO_MUNDO/2, -ALTURA_DO_MUNDO/2, 0);

        glTexCoord2f(1, 0);
        glVertex3f(LARGURA_DO_MUNDO/2, -ALTURA_DO_MUNDO/2, 0);

        glTexCoord2f(1, 1);
        glVertex3f(LARGURA_DO_MUNDO/2, ALTURA_DO_MUNDO/2, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-LARGURA_DO_MUNDO/2, ALTURA_DO_MUNDO/2, 0);
    glEnd();
}

void desenhaFase()
{
    desenhaSprite(jogador, 0.000, 0.007, 0.109, 0.073);

    if (tiroJogador.ativo)
        desenhaSprite(tiroJogador, 0.834, 0.339, 0.008, 0.036);

    if (tiroInimigo.ativo)
        desenhaSprite(tiroInimigo, 0.835, 0.151, 0.008, 0.055);

    for (int i = 0; i < QTD_INIMIGOS; i++)
        if (vetorInimigos[i].ativo)
            desenhaSprite(vetorInimigos[i], 0.413, 0.207, 0.090, 0.082);
}

void desenharMinhaCena()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 1, 1);

    // habilita texturas
    glEnable(GL_TEXTURE_2D);

    // desenhos
    desenhaFundoJogo();
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
            x = -80;
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

void jogadorAtira()
{
    if (JOGO == ativo && !tiroJogador.ativo) {
        inicializaTiro(&tiroJogador, jogador.posicao.x, jogador.posicao.y + jogador.dimensoes.y/2);
        desenhaSprite(tiroJogador, 0.833, 0.339, 0.008, 0.036);
        glutPostRedisplay();
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
            // reinicia o Jogo
            // TODO
            break;
        default:
            break;
    }
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

    // define estado do jogo
    JOGO = ativo;
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
    glutTimerFunc(0, movimentaInimigos, 33);
    glutTimerFunc(0, movimentaTiroJogador, 33);
    glutTimerFunc(0, movimentaTiroInimigo, 33);

    // configura variaveis de estado
    inicializa();

    // inicializa variaveis do jogo
    inicializaJogador();
    inicializaInimigo();

    // main loop
    glutMainLoop();

    return 0;
}
