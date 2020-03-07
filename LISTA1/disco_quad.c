#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>

#define LARGURA_DO_MUNDO 100
#define ALTURA_DO_MUNDO 100
#define TRUE 1
#define FALSE 0

int LINHAS = FALSE;

void desenhaQuadrado(int x, int y, int lado) {
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(x, y, 0);
        glVertex3f(x+lado, y, 0);
        glVertex3f(x+lado, y+lado, 0);
        glVertex3f(x, y+lado, 0);
    glEnd();
}

void desenhaLinhas() {
    glBegin(GL_LINE_LOOP);
        glVertex3f(-LARGURA_DO_MUNDO*0.5, -ALTURA_DO_MUNDO*0.5, 0);
        glVertex3f(-LARGURA_DO_MUNDO*0.3, -ALTURA_DO_MUNDO*0.3, 0);
        glVertex3f(LARGURA_DO_MUNDO*0.5, -ALTURA_DO_MUNDO*0.5, 0);
        glVertex3f(LARGURA_DO_MUNDO*0.3, -ALTURA_DO_MUNDO*0.3, 0);
        glVertex3f(LARGURA_DO_MUNDO*0.5, ALTURA_DO_MUNDO*0.5, 0);
        glVertex3f(LARGURA_DO_MUNDO*0.3, ALTURA_DO_MUNDO*0.3, 0);
        glVertex3f(-LARGURA_DO_MUNDO*0.5, ALTURA_DO_MUNDO*0.5, 0);
        glVertex3f(-LARGURA_DO_MUNDO*0.3, ALTURA_DO_MUNDO*0.3, 0);
    glEnd();
}

void desenharMinhaCena() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.5, 0.8, 0.8);
    desenhaQuadrado(-LARGURA_DO_MUNDO*0.5, -ALTURA_DO_MUNDO*0.5, LARGURA_DO_MUNDO);
    glColor3f(1, 1, 1);
    desenhaQuadrado(-LARGURA_DO_MUNDO*0.3, -ALTURA_DO_MUNDO*0.3, LARGURA_DO_MUNDO*0.6);
    if (LINHAS) {
        glColor3f(0, 0, 0);
        desenhaLinhas();
    }

    glFlush();
}

void redimensionada(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-LARGURA_DO_MUNDO/2, LARGURA_DO_MUNDO/2, -ALTURA_DO_MUNDO/2, ALTURA_DO_MUNDO/2, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void teclaPressionada(unsigned char key, int x, int y) {
    switch(key) {
        case 27:
            exit(0);
            break;
        case 'c':
        case 'C':
            LINHAS ^= 1;
            glutPostRedisplay();
            break;
        default:
            break;
    }
}

void inicializa() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glLineWidth(2);
}

int main(int argc, char** argv) {
    // inicialização do glut
    glutInit(&argc, argv);
    glutInitContextVersion(1, 1);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Disco quadrado");

    // registro de callbacks
    glutDisplayFunc(desenharMinhaCena);
    glutReshapeFunc(redimensionada);
    glutKeyboardFunc(teclaPressionada);

    // configura variaveis de estado
    inicializa();

    // main loop
    glutMainLoop();

    return 0;
}
