#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>

#include "definicoes.h"
#include "estruturas.h"

#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))

int aleatorioEntre(int x, int y)
{
    // recebe dois inteiros, x e y
    // retorna um inteiro pseudo-aleatório entre os dois parametros
    int maior = max(x, y);
    int menor = min(x, y);
    return (rand() % ((maior - menor) + 1)) + menor;
}

void atualizaPontosCurva(tipoVetor2d* pontosCurva, tipoInimigo* inimigo, tipoJogador* player)
{
    // recebe um vetor com 6 pontos x, y
    // cada trio de pontos (0-2 e 3-5) representa os três pontos de controle de uma curva quadrática de Bézier
    // recebe um ponteiro para um dos inimigos e um ponteiro para o jogador
    // atualiza o vetor passado como parâmetro com novos pontos a partir das posicoes do inimigo e do jogador

    tipoVetor2d inicioCurva = inimigo->sprite.posicao;

    pontosCurva[0].x = inicioCurva.x;
    pontosCurva[0].y = inicioCurva.y;

    pontosCurva[1].x = aleatorioEntre(0, LARGURA_DO_MUNDO);
    pontosCurva[1].y = aleatorioEntre(inicioCurva.y, player->sprite.posicao.y);

    pontosCurva[2].x = aleatorioEntre(0, LARGURA_DO_MUNDO);
    pontosCurva[2].y = 0;

    pontosCurva[3].x = pontosCurva[2].x;
    pontosCurva[3].y = ALTURA_DO_MUNDO;

    pontosCurva[4].x = aleatorioEntre(0, LARGURA_DO_MUNDO);
    pontosCurva[4].y = aleatorioEntre(inicioCurva.y, ALTURA_DO_MUNDO);

    pontosCurva[5].x = inicioCurva.x;
    pontosCurva[5].y = inicioCurva.y;
}

tipoVetor2d formulaBezier(tipoVetor2d p0, tipoVetor2d p1, tipoVetor2d p2, tipoVetor2d pFinal, float t)
{
    // recebe tres pontos de controle, um ponto final e um float t para interpolação
    // recebe um ponto final para evitar a declaração de uma nova variável a cada chamada
    // retorna o ponto final
    pFinal.x = pow(1 - t, 2) * p0.x + (1 - t) * 2 * t * p1.x + t * t * p2.x;
    pFinal.y = pow(1 - t, 2) * p0.y + (1 - t) * 2 * t * p1.y + t * t * p2.y;
    return pFinal;
}

float formulaDistancia(tipoVetor2d p0, tipoVetor2d p1)
{
    // recebe dois pontos x, y e retorna a distancia entre eles
    float dx = max(p0.x, p1.x) - min(p0.x, p1.x);
    float dy = max(p0.y, p1.y) - min(p0.y, p1.y);
    return sqrt(pow(dx, 2) + pow(dy, 2));
}

int detectaColisao(tipoSprite s1, tipoSprite s2)
{
    // recebe dois sprites
    // retorna 1 se os sprites estão colidindo e 0 caso contrário
    float distancia = formulaDistancia(s1.posicao, s2.posicao);
    if (distancia <= s1.raio + s2.raio)
        return 1;
    return 0;
}

void tocaSom(char arquivo[100], int loop)
{
    Mix_Chunk* som = NULL;
    int canal;
    int canalAudio = 2;
    int taxaAudio = 22050;
    Uint16 formatoAudio = AUDIO_S16SYS;
    int audioBuffers = 1024;

    if(Mix_OpenAudio(taxaAudio, formatoAudio, canalAudio, audioBuffers) != 0)
        printf("Não pode inicializar audio: %s\n", Mix_GetError());

    som = Mix_LoadWAV(arquivo);

    if (som == NULL)
        printf("Não pode inicializar audio: %s\n", Mix_GetError());

    Mix_HaltChannel(-1);
    canal  = Mix_PlayChannel(-1, som, loop);

    if (canal == -1)
        printf("Não pode inicializar audio: %s\n", Mix_GetError());
}


void escreveTexto(void* fonte, char* texto, float x, float y)
{
    // recebe o nome de uma fonte, uma string e dois floats x e y
    // escreve o texto com a fonte nas cordenadas x e y
    glRasterPos2f(x, y);
    for (int i = 0; i < strlen(texto); i++) {
        glutBitmapCharacter(fonte, texto[i]);
    }
}

GLuint carregaTextura(const char* arquivo)
{
    // recebe o endereço de uma imagem e retorna um GLuint representando a textura
    GLuint idTextura = SOIL_load_OGL_texture(
                        arquivo,
                        SOIL_LOAD_AUTO,
                        SOIL_CREATE_NEW_ID,
                        SOIL_FLAG_INVERT_Y);

    if (!idTextura)
        printf("Erro do SOIL: '%s'\n", SOIL_last_result());

    return idTextura;
}

void desenhaFundoJogo(GLuint textura)
{
    // recebe um GLuint representando uma textura e densenha em toda a tela
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textura);
    glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0, 0);
        glVertex3f(0, 0, 0);

        glTexCoord2f(1, 0);
        glVertex3f(LARGURA_DO_MUNDO, 0, 0);

        glTexCoord2f(1, 1);
        glVertex3f(LARGURA_DO_MUNDO, ALTURA_DO_MUNDO, 0);

        glTexCoord2f(0, 1);
        glVertex3f(0, ALTURA_DO_MUNDO, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void desenhaSprite(GLuint textura, tipoSprite sprite, float sheetX, float sheetY, float comprimento, float altura)
{
    // funcao para desenhar sprites a partir de uma spritesheet
    // recebe a textura da spritesheet, o sprite a ser desenhado e quatro floats
    // x e y: canto inferior esquerdo do sprite na spritesheet
    // comprimento e altura: do sprite na spritesheet
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textura);
    glPushMatrix();
        glTranslatef(sprite.posicao.x, sprite.posicao.y, 0);
        glBegin(GL_TRIANGLE_FAN);
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
    glDisable(GL_TEXTURE_2D);
}
