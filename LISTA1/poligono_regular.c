#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>

#define LARGURA_DO_MUNDO 100
#define ALTURA_DO_MUNDO 100

int NUM_LADOS = 4;
double RAIO = 40.0;

void desenharMinhaCena() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.5, 0.8, 0.8);
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < NUM_LADOS; i++) {
            // x = r * cos ((2 * numeroDoVertice) * PI / numeroDeLados)
            // y = r * sen ((2 * numeroDoVertice) * PI / numeroDeLados)
            glVertex3f(RAIO * cos(2 * (i) * M_PI / NUM_LADOS), RAIO * sin(2 * (i) * M_PI / NUM_LADOS), 0);
        }
    glEnd();
    glFlush();
}

void inicializa() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glLineWidth(2);
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
        case '+': // aumenta o número de lados do polígono
            NUM_LADOS += 1;
            glutPostRedisplay();
            break;
        case '-': // diminui o número de lados
            if (NUM_LADOS > 3)
                NUM_LADOS -= 1;
            glutPostRedisplay();
            break;
        case '*': // aumenta a distância dos vértices até o centro
            if (RAIO < 50)
                RAIO += 0.5;
            glutPostRedisplay();
            break;
        case '/': // diminui a distância dos vérticos até o centro
            if (RAIO > 10)
                RAIO -= 0.5;
            glutPostRedisplay();
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
    glutCreateWindow("Poligono regular");

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
