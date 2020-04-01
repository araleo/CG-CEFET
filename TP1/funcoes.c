#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))

float formulaDistancia(float x1, float x2, float y1, float y2)
{
    float x = max(x1, x2) - min(x1, x2);
    float y = max(y1, y2) - min(y1, y2);
    return sqrt(pow(x, 2) + pow(y, 2));
}

void escreveTexto(void* fonte, char* texto, float x, float y)
{
    glRasterPos2f(x, y);
    for (int i = 0; i < strlen(texto); i++) {
        glutBitmapCharacter(fonte, texto[i]);
    }
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
