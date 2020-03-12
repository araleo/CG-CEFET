#include <GL/glew.h>
#include <GL/freeglut.h>

#define LARGURA_DO_MUNDO 100
#define ALTURA_DO_MUNDO 100
#define TRUE 1
#define FALSE 0

int LINHAS = FALSE;
int listaQuadradoFurado;

void criaListaQuadradoFurado()
{
    listaQuadradoFurado = glGenLists(1);
    glNewList(listaQuadradoFurado, GL_COMPILE);
        glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(-40, -40, 0);
            glVertex3f(-25, -25, 0);
            glVertex3f(40, -40, 0);
            glVertex3f(25, -25, 0);
            glVertex3f(40, 40, 0);
            glVertex3f(25, 25, 0);
            glVertex3f(-40, 40, 0);
            glVertex3f(-25, 25, 0);
            glVertex3f(-40, -40, 0);
            glVertex3f(-25, -25, 0);
        glEnd();
    glEndList();
}

void desenharMinhaCena()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.5, 0.8, 0.8);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glCallList(listaQuadradoFurado);
    if (LINHAS)
    {
        glColor3f(0, 0, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glCallList(listaQuadradoFurado);
    }
    glFlush();
}

void redimensionada(int w, int h)
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
        case 'c':
        case 'C':
            LINHAS ^= 1;
            glutPostRedisplay();
            break;
        default:
            break;
    }
}

void inicializa()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    criaListaQuadradoFurado();
    glLineWidth(2);
}

int main(int argc, char** argv)
{
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
