#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>

#include "estruturas.h"

#define TRUE 1
#define FALSE 0

#define ALTURA_DO_MUNDO 200
#define LARGURA_DO_MUNDO 200

float formulaDistancia(float x1, float x2, float y1, float y2);

GLuint idTexturaFundo;
GLuint idTexturaSheet;

tipoNave jogador;
tipoNave inimigo;
tipoTiro tiro;

void detectaTiro()
{
    if (tiro.ativo) {
        float dist = formulaDistancia(tiro.sprite.posicao.x, inimigo.sprite.posicao.x, tiro.sprite.posicao.y, inimigo.sprite.posicao.y);
        if (dist <= 10) {
            printf("acertou\n");
        }
    }
}

void movimentaTiro()
{
    if (tiro.sprite.posicao.y > ALTURA_DO_MUNDO/2)
        tiro.ativo = FALSE;

    tiro.sprite.posicao.y += tiro.velocidade;
    detectaTiro();
    glutPostRedisplay();
    glutTimerFunc(33, movimentaTiro, 33);
}

void movimentaInimigos()
{
    if (inimigo.sprite.posicao.y <= jogador.sprite.posicao.y + jogador.sprite.dimensoes.y) {
        // jogador perdeu
        // TODO
    } else if (inimigo.sprite.posicao.x < LARGURA_DO_MUNDO/2 - inimigo.sprite.dimensoes.x/2) {
        // move horizontalmente
        inimigo.sprite.posicao.x += inimigo.velocidade;
    } else {
        // move verticalmente e volta para a esquerda da tela
        inimigo.sprite.posicao.x = -LARGURA_DO_MUNDO/2 + inimigo.sprite.dimensoes.x/2;
        inimigo.sprite.posicao.y -= inimigo.sprite.dimensoes.y;
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

void desenharMinhaCena()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 1, 1);

    // habilita texturas
    glEnable(GL_TEXTURE_2D);

    // desenhos
    desenhaFundoJogo();
    desenhaSprite(jogador.sprite, 0.000, 0.007, 0.109, 0.073);
    desenhaSprite(inimigo.sprite, 0.413, 0.207, 0.090, 0.082);
    desenhaSprite(tiro.sprite, 0.834, 0.339, 0.008, 0.036);
    // desenhaBlocos(); //BRUNA

    // movimentos
    movimentaInimigos(inimigo);

    glDisable(GL_TEXTURE_2D);

    glutSwapBuffers();
}

GLuint carregaTextura(const char* arquivo)
{
    GLuint idTextura = SOIL_load_OGL_texture(
                        arquivo,
                        SOIL_LOAD_AUTO,
                        SOIL_CREATE_NEW_ID,
                        SOIL_FLAG_INVERT_Y);

    if (!idTextura)
        printf("Erro do SOIL: '%s'\n", SOIL_last_result());

    return idTextura;
}

void inicializaSprite(tipoSprite* sprite, float x, float y, float comprimento, float altura)
{
    sprite->posicao.x = x;
    sprite->posicao.y = y;
    sprite->dimensoes.x = comprimento;
    sprite->dimensoes.y = altura;
}

void inicializaTiro(float x, float y)
{
    inicializaSprite(&tiro.sprite, x, y, 1, 5);
    tiro.velocidade = 2;
    tiro.ativo = TRUE;
}

void inicializaInimigo()
{
    inicializaSprite(&inimigo.sprite, 0, ALTURA_DO_MUNDO * 0.4, 15, 15);
    // inimigo.velocidade = 0.0005;
    inimigo.velocidade = 0.001;
}

void inicializaJogador()
{
    inicializaSprite(&jogador.sprite, 0, -ALTURA_DO_MUNDO * 0.4, 20, 20);
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

void teclaPressionada(unsigned char key, int x, int y)
{
    switch(key) {
        case 27:
            exit(0);
            break;
        case 'a':
            if (jogador.sprite.posicao.x > -LARGURA_DO_MUNDO/2 + jogador.sprite.dimensoes.x/2) {
                jogador.sprite.posicao.x -= jogador.velocidade;
                glutPostRedisplay();
            }
            break;
        case 'd':
            if (jogador.sprite.posicao.x < LARGURA_DO_MUNDO/2 - jogador.sprite.dimensoes.x/2) {
                jogador.sprite.posicao.x += jogador.velocidade;
                glutPostRedisplay();
            }
            break;
        case ' ':
            if (!tiro.ativo) {
                inicializaTiro(jogador.sprite.posicao.x, jogador.sprite.posicao.y + jogador.sprite.dimensoes.y/2);
                desenhaSprite(tiro.sprite, 0.833, 0.339, 0.008, 0.036);
                glutPostRedisplay();
            }
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
}

int main(int argc, char** argv)
{
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
    glutTimerFunc(0, movimentaTiro, 33);

    // configura variaveis de estado
    inicializa();

    // inicializa variaveis do jogo
    inicializaJogador();
    inicializaInimigo();

    // teste
    float a = formulaDistancia(0, 3, 0, 4);
    printf("%f\n", a);

    // main loop
    glutMainLoop();

    return 0;
}
