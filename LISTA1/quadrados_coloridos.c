#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LARGURA_DO_MUNDO 90
#define ALTURA_DO_MUNDO 90

typedef struct {
    float vermelho, verde, azul;
} cor;

// vetor de cores como variavel global
cor** VETOR_CORES = NULL;

void sorteiaCores() {
    srand(time(0));
    VETOR_CORES = (cor **)malloc(9 * sizeof(cor));
    for (int i = 0; i < 9; i++) {
        VETOR_CORES[i] = (cor *)malloc(sizeof(cor));
        VETOR_CORES[i]->vermelho = (double)rand() / (double)RAND_MAX;
        VETOR_CORES[i]->verde = (double)rand() / (double)RAND_MAX;
        VETOR_CORES[i]->azul = (double)rand() / (double)RAND_MAX;
    }
}

void liberaMemoria(cor **v) {
    for (int i = 0; i < 9; i++) {
        free(v[i]);
    }
    free(v);
}

void desenhaQuadrado(int x, int y, int i) {
    cor* color = VETOR_CORES[i];
    glColor3f(color->vermelho, color->verde, color->azul);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(x, y, 0);
        glVertex3f(x+30, y, 0);
        glVertex3f(x+30, y+30, 0);
        glVertex3f(x, y+30, 0);
    glEnd();
}

void desenharMinhaCena() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    int x, y = 0;
    for (int i = 0; i < 9; i++) {
        desenhaQuadrado(x, y, i);
        x += 30;
        if (i > 0 && i % 3 == 0) {
            x = 0;
            y += 30;
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
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Ola, mundo");

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
    liberaMemoria(VETOR_CORES);

    return 0;
}
