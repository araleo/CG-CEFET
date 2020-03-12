#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdlib.h>
#include <time.h>

#define QUADRADOS_POR_LINHA 3
#define QUADRADOS_POR_COLUNA 3
#define COMPRIMENTO_LADO 30

typedef struct
{
    float vermelho, verde, azul;
} cor;

// vetor de cores como variavel global
cor** VETOR_CORES = NULL;

// quantidade de quadrados como variavel global
const int QUANTIDADE_QUADRADOS = QUADRADOS_POR_LINHA * QUADRADOS_POR_COLUNA;

// dimensoes do mundo como variaveis globais
const int LARGURA_DO_MUNDO = QUADRADOS_POR_LINHA * COMPRIMENTO_LADO;
const int ALTURA_DO_MUNDO = QUADRADOS_POR_COLUNA * COMPRIMENTO_LADO;

void sorteiaCores()
{
    VETOR_CORES = (cor **) malloc(QUANTIDADE_QUADRADOS * sizeof(cor));
    for (int i = 0; i < QUANTIDADE_QUADRADOS; i++)
    {
        VETOR_CORES[i] = (cor *) malloc(sizeof(cor));
        VETOR_CORES[i]->vermelho = (double)rand() / (double)RAND_MAX;
        VETOR_CORES[i]->verde = (double)rand() / (double)RAND_MAX;
        VETOR_CORES[i]->azul = (double)rand() / (double)RAND_MAX;
    }
}

void liberaMemoria(cor **v)
{
    for (int i = 0; i < QUANTIDADE_QUADRADOS; i++)
        free(v[i]);
    free(v);
}

void desenhaQuadrado(int x, int y, cor* color)
{
    glColor3f(color->vermelho, color->verde, color->azul);
    glPushMatrix();
        glTranslatef(x, y, 0);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, 0);
            glVertex2f(COMPRIMENTO_LADO, 0);
            glVertex2f(COMPRIMENTO_LADO, COMPRIMENTO_LADO);
            glVertex2f(0, COMPRIMENTO_LADO);
        glEnd();
    glPopMatrix();
}

void desenharMinhaCena()
{
    glClear(GL_COLOR_BUFFER_BIT);
    int x, y = 0;
    for (int i = 0; i < QUANTIDADE_QUADRADOS; i++)
    {
        desenhaQuadrado(x, y, VETOR_CORES[i]);
        x += COMPRIMENTO_LADO;
        if ((i+1) % QUADRADOS_POR_LINHA == 0)
        {
            x = 0;
            y += COMPRIMENTO_LADO;
        }
    }
    glFlush();
}

void inicializa()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

void redimensionada(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, LARGURA_DO_MUNDO, 0, ALTURA_DO_MUNDO, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void teclaPressionada(unsigned char key, int x, int y)
{
    switch(key) {
        case 27:
            exit(0);
            break;
        default:
            break;
    }
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
    glutCreateWindow("Quadrados coloridos");

    // inicializa vetor de cores
    srand(time(0));
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
