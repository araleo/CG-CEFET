#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define QUADRADOS_POR_LINHA 3
#define QUADRADOS_POR_COLUNA 3
#define COMPRIMENTO_LADO 30

typedef struct {
    float vermelho, verde, azul;
} cor;

// vetor de cores como variavel global
cor** VETOR_CORES = NULL;

// quantidade de quadrados como variavel global
int QUANTIDADE_QUADRADOS = QUADRADOS_POR_LINHA * QUADRADOS_POR_COLUNA;

// dimensoes do mundo como variaveis globais
int LARGURA_DO_MUNDO = QUADRADOS_POR_LINHA * COMPRIMENTO_LADO;
int ALTURA_DO_MUNDO = QUADRADOS_POR_COLUNA * COMPRIMENTO_LADO;

void sorteiaCores() {
    srand(time(0));
    VETOR_CORES = (cor **)malloc(QUANTIDADE_QUADRADOS * sizeof(cor));
    for (int i = 0; i < QUANTIDADE_QUADRADOS; i++) {
        VETOR_CORES[i] = (cor *)malloc(sizeof(cor));
        VETOR_CORES[i]->vermelho = (double)rand() / (double)RAND_MAX;
        VETOR_CORES[i]->verde = (double)rand() / (double)RAND_MAX;
        VETOR_CORES[i]->azul = (double)rand() / (double)RAND_MAX;
    }
}

void liberaMemoria(cor **v) {
    for (int i = 0; i < QUANTIDADE_QUADRADOS; i++) {
        free(v[i]);
    }
    free(v);
}

void desenhaQuadrado(int x, int y, int i) {
    cor* color = VETOR_CORES[i];
    glColor3f(color->vermelho, color->verde, color->azul);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(x, y, 0);
        glVertex3f(x+COMPRIMENTO_LADO, y, 0);
        glVertex3f(x+COMPRIMENTO_LADO, y+COMPRIMENTO_LADO, 0);
        glVertex3f(x, y+COMPRIMENTO_LADO, 0);
    glEnd();
}

void desenharMinhaCena() {
    glClear(GL_COLOR_BUFFER_BIT);
    int x, y = 0;
    for (int i = 0; i < QUANTIDADE_QUADRADOS; i++) {
        desenhaQuadrado(x, y, i);
        x += COMPRIMENTO_LADO;
        if ((i+1) % QUADRADOS_POR_LINHA == 0) {
            x = 0;
            y += COMPRIMENTO_LADO;
        }
    }
    glFlush();
}

void inicializa() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

void redimensionada(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, LARGURA_DO_MUNDO, 0, ALTURA_DO_MUNDO, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void teclaPressionada(unsigned char key, int x, int y) {
    switch(key) {
        case 27:
            exit(0);
            break;
        default:
            break;
    }
}

int main(int argc, char** argv) {
    // inicialização do glut
    glutInit(&argc, argv);
    glutInitContextVersion(1, 1);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Quadrados coloridos");

    // inicializa vetor de cores
    sorteiaCores();

    // registro de callbacks
    glutDisplayFunc(desenharMinhaCena);
    glutReshapeFunc(redimensionada);
    glutKeyboardFunc(teclaPressionada);

    // configura variaveis de estado
    inicializa();

    // main loop
    glutMainLoop();

    // libera memoria do vetor de cores
    // liberaMemoria(VETOR_CORES);

    return 0;
}
