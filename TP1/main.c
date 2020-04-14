#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "estruturas.h"
#include "declaracoes.h"
#include "definicoes.h"

enum estadosJogo {gameOver, ativo, pause, vitoria} JOGO;
enum fases {primeira, segunda, terceira, chefe} FASES;
enum itens {vida, velocidade, tiro} ITENS;

GLuint idTexturaFundo;
GLuint idTexturaFundoChefe;
GLuint idTexturaSheet;
GLuint idTexturaChefao;

tipoJogador jogador;
tipoInimigo vetorInimigos[QTD_INIMIGOS];
tipoInimigo chefao;
tipoSprite tiroJogador;
tipoSprite tiroInimigo;
tipoSprite tiroInimigoEspecial;
tipoSprite iconeVidas[MAX_VIDAS];
tipoSprite itemDrop;
tipoVetor2d* vetoresPontos[2];

int TIRO_ESPECIAL;
int ATIVA_RASANTE;
float T;
int PONTOS;
char STR_PONTOS[10];

long int timerRasanteInicio;
long int timerRasanteFim;

void curvaBezier(tipoInimigo* inimigo, tipoVetor2d* pontosCurva)
{
    tipoVetor2d pFinal;

    if (abs(inimigo->sprite.posicao.x - pontosCurva[2].x) <= 1
    && abs(inimigo->sprite.posicao.y - pontosCurva[2].y) <= 1) {
        inimigo->sprite.posicao = pontosCurva[3];
    }

    if (JOGO == ativo) {
        if (T <= 1) {
            pFinal = formulaBezier(pontosCurva[0], pontosCurva[1], pontosCurva[2], pFinal, T);
            inimigo->sprite.posicao.x = pFinal.x;
            inimigo->sprite.posicao.y = pFinal.y;
            T += 0.005;
        } else if (T <= 2) {
            pFinal = formulaBezier(pontosCurva[3], pontosCurva[4], pontosCurva[5], pFinal, T-1);
            inimigo->sprite.posicao.x = pFinal.x;
            inimigo->sprite.posicao.y = pFinal.y;
            T += 0.005;
        } else {
            ATIVA_RASANTE = FALSE;
            T = 0;
        }
    }
}

void ativaRasante()
{
    for (int i = 0; i < QTD_INIMIGOS; i++) {
        if (vetorInimigos[i].rasante && vetorInimigos[i].sprite.ativo) {
            curvaBezier(&vetorInimigos[i], vetoresPontos[vetorInimigos[i].vetorRasante]);
        }
    }
}

void sorteiaRasante()
{
    timerRasanteFim = time(0);
    int qtdRasantes = 0;

    for (int i = 0; i < QTD_INIMIGOS; i++) {
        if (vetorInimigos[i].rasante && vetorInimigos[i].sprite.ativo) {
            qtdRasantes++;
        }
    }

    if (qtdRasantes == 0) {
        ATIVA_RASANTE = FALSE;
    } else if (FASES == terceira && timerRasanteFim - timerRasanteInicio > 15 && !ATIVA_RASANTE) {
        ATIVA_RASANTE = TRUE;
        timerRasanteInicio = timerRasanteFim;
        for (int i = 0; i < QTD_INIMIGOS; i++) {
            if (vetorInimigos[i].rasante && vetorInimigos[i].sprite.ativo) {
                atualizaPontosCurva(&vetorInimigos[i], vetoresPontos[vetorInimigos[i].vetorRasante], &jogador);
            }
        }
    }
}

void detectaColisaoRasante()
{
    for (int i = 0; i < QTD_INIMIGOS; i++) {
        tipoInimigo inimigo = vetorInimigos[i];
        if (inimigo.sprite.ativo && inimigo.rasante && detectaColisao(jogador.sprite, inimigo.sprite)) {
            JOGO = gameOver;
        }
    }
}

void detectaItem()
{
    if (detectaColisao(jogador.sprite, itemDrop)) {
        itemDrop.ativo = FALSE;
        PONTOS += 10;

        if (ITENS == vida && jogador.vidas < 5)
            jogador.vidas += 1;
        else if (ITENS == velocidade && jogador.sprite.velocidade < 5)
            jogador.sprite.velocidade += 1;
        else if (ITENS == tiro)
            TIRO_ESPECIAL = TRUE;
    }
}

void sorteiaItem(tipoInimigo inimigo)
{
    if (inimigo.especial && rand() % 2 == 0) {
        ITENS = tiro;
        inicializaItem(&itemDrop, inimigo.sprite.posicao.x, inimigo.sprite.posicao.y);
    } else if (!inimigo.especial){
        int sorteio = rand() % 6;
        if (sorteio < 2) {
            ITENS = sorteio;
            inicializaItem(&itemDrop, inimigo.sprite.posicao.x, inimigo.sprite.posicao.y);
        }
    }
    // glutPostRedisplay();
}

void detectaTiro(tipoSprite* tiro)
{
    if ((tiro == &tiroInimigo || tiro == &tiroInimigoEspecial) && tiro->ativo) {
        // tido dos inimigos
        tipoJogador* alvo = &jogador;
        if (detectaColisao(alvo->sprite, *tiro)) {
            tiro->ativo = FALSE;
            jogador.vidas -= 1;
            PONTOS -= 10;
            verificaGameOver();
        }
    } else if (tiro == &tiroJogador && tiro->ativo) {
        if (FASES < chefe) {
            // tiro do jogador
            // alvos normais
            for (int i = 0; i < QTD_INIMIGOS; i++) {
                tipoInimigo* alvo = &vetorInimigos[i];
                if (alvo->sprite.ativo && detectaColisao(alvo->sprite, *tiro)) {
                    tiro->ativo = FALSE;
                    alvo->sprite.ativo = FALSE;
                    PONTOS += ((ALTURA_DO_MUNDO - alvo->sprite.posicao.y) / 10) + 10;
                    if (alvo->especial || alvo->rasante)
                        PONTOS += 20;
                    if (!verificaVitoria() && !itemDrop.ativo) {
                        sorteiaItem(*alvo);
                    }
                }
            }
        } else {
            // alvo chefao
            tipoInimigo* alvo = &chefao;
            if (alvo->sprite.ativo && detectaColisao(alvo->sprite, *tiro)) {
                tiro->ativo = FALSE;
                PONTOS += 10;
                if (TIRO_ESPECIAL)
                    alvo->vidas -= 2;
                else
                    alvo->vidas -= 1;
                if (alvo->vidas <= 0) {
                    alvo->sprite.ativo = FALSE;
                    PONTOS += 150;
                    JOGO = vitoria;
                }
            }
        }
    }
}

void inimigoAtira()
{
    tipoInimigo inimigo = FASES < chefe ? vetorInimigos[rand() % QTD_INIMIGOS] : chefao;

    if ((inimigo.chefao || inimigo.especial) && inimigo.sprite.ativo && !tiroInimigoEspecial.ativo)
        inicializaTiro(&tiroInimigoEspecial, inimigo.sprite.posicao.x, inimigo.sprite.posicao.y);
    else if (!tiroInimigo.ativo && inimigo.sprite.ativo)
        inicializaTiro(&tiroInimigo, inimigo.sprite.posicao.x, inimigo.sprite.posicao.y);
}

void jogadorAtira()
{
    if (JOGO == ativo && !tiroJogador.ativo) {
        inicializaTiro(&tiroJogador, jogador.sprite.posicao.x, jogador.sprite.posicao.y + jogador.sprite.dimensoes.y/2);
        desenhaSprite(idTexturaSheet, tiroJogador, 0.833, 0.339, 0.008, 0.036);
        // glutPostRedisplay();
    }
}

void movimentos()
{
    movimentaProps();
    sorteiaRasante();
    if (ATIVA_RASANTE) {
        ativaRasante();
        detectaColisaoRasante();
    }
    else
        movimentaInimigos();
    glutPostRedisplay();
    glutTimerFunc(16, movimentos, 0);
}

void movimentaProps()
{
    // movimenta os tiros dos inimigos
    if (tiroInimigo.posicao.y < 0)
        tiroInimigo.ativo = FALSE;

    if (JOGO == ativo && tiroInimigo.ativo) {
        tiroInimigo.posicao.y -= tiroInimigo.velocidade;
        detectaTiro(&tiroInimigo);
    }

    // movimenta os tiros dos inimigos especiais
    if (tiroInimigoEspecial.posicao.y < 0)
        tiroInimigoEspecial.ativo = FALSE;

    if (JOGO == ativo && tiroInimigoEspecial.ativo) {
        tiroInimigoEspecial.posicao.y -= tiroInimigoEspecial.velocidade;
        detectaTiro(&tiroInimigoEspecial);
    }

    // movimenta os tiros do jogador
    if (tiroJogador.posicao.y > ALTURA_DO_MUNDO)
        tiroJogador.ativo = FALSE;

    // determina o inimigo mais alto da formação
    // para desativar o tiro do jogador depois que passar pelo mais alto
    tipoInimigo inimigoMaisAlto;
    for (int i = 0; i < (MAX_RASANTES + 1); i++) {
        if (!vetorInimigos[i].rasante) {
            inimigoMaisAlto = vetorInimigos[i];
            break;
        }
    }

    if (FASES < chefe && tiroJogador.posicao.y > inimigoMaisAlto.sprite.posicao.y + 30)
        tiroJogador.ativo = FALSE;

    if (JOGO == ativo && tiroJogador.ativo) {
        tiroJogador.posicao.y += tiroJogador.velocidade;
        detectaTiro(&tiroJogador);
    }

    // movimenta os itens
    if (itemDrop.posicao.y < 0)
        itemDrop.ativo = FALSE;

    if (JOGO == ativo && itemDrop.ativo) {
        itemDrop.posicao.y -= itemDrop.velocidade;
        detectaItem();
    }

    // glutPostRedisplay();
}

void movimentaJogador(char lado)
{
    if (lado == 'e') {
        if (JOGO == ativo && jogador.sprite.posicao.x > jogador.sprite.dimensoes.x/2) {
            jogador.sprite.posicao.x -= jogador.sprite.velocidade;
        }
    } else if (lado == 'd') {
        if (JOGO == ativo && jogador.sprite.posicao.x < LARGURA_DO_MUNDO - jogador.sprite.dimensoes.x/2) {
            jogador.sprite.posicao.x += jogador.sprite.velocidade;
        }
    }
    // glutPostRedisplay();
}

void moveInimigo(tipoInimigo* inimigo)
{
    if (inimigo->sprite.posicao.y - inimigo->sprite.raio <= jogador.sprite.posicao.y + jogador.sprite.raio
    && !inimigo->rasante) {
        // jogador perdeu - um inimigo chegou no "chão"
        JOGO = gameOver;
    } else if (inimigo->sprite.posicao.x <= inimigo->sprite.dimensoes.x/2
    || inimigo->sprite.posicao.x >= LARGURA_DO_MUNDO - inimigo->sprite.dimensoes.x/2) {
        // inimigo chegou em um dos cantos laterais da tela
        // move verticalmente e altera a direção
        if (FASES == chefe) {
            inimigo->sprite.posicao.y -= 20;
            inimigo->sprite.velocidade *= -1;
            inimigo->sprite.posicao.x += inimigo->sprite.velocidade;
        } else {
            for (int i = 0; i < QTD_INIMIGOS; i++) {
                vetorInimigos[i].sprite.posicao.y -= 20;
                vetorInimigos[i].sprite.velocidade *= -1;
                vetorInimigos[i].sprite.posicao.x += vetorInimigos[i].sprite.velocidade;
            }
        }
    } else {
        // move horizontalmente
        inimigo->sprite.posicao.x += inimigo->sprite.velocidade;
        inimigoAtira();
    }
}

void movimentaInimigos()
{
    if (JOGO == ativo) {
        if (FASES == chefe) {
            moveInimigo(&chefao);
        } else {
            for (int i = 0; i < QTD_INIMIGOS; i++) {
                if (vetorInimigos[i].sprite.ativo)
                    moveInimigo(&vetorInimigos[i]);

            }
        }
    }
    // glutPostRedisplay();
}

void desenhaItens()
{
    if (itemDrop.ativo) {
        if (ITENS == vida)
            desenhaSprite(idTexturaSheet, itemDrop, 0.217, 0.874, 0.021, 0.020);
        else if (ITENS == velocidade)
            desenhaSprite(idTexturaSheet, itemDrop, 0.790, 0.543, 0.018, 0.029);
        else if (ITENS == tiro)
            desenhaSprite(idTexturaSheet, itemDrop, 0.760, 0.426, 0.030, 0.029);
    }
}

void desenhaInimigos()
{
    if (FASES < chefe)  {
        for (int i = 0; i < QTD_INIMIGOS; i++) {
            if (vetorInimigos[i].sprite.ativo) {
                if (vetorInimigos[i].especial)
                    desenhaSprite(idTexturaSheet, vetorInimigos[i].sprite, 0.140, 0.631, 0.101, 0.082);
                else if (vetorInimigos[i].rasante)
                    desenhaSprite(idTexturaSheet, vetorInimigos[i].sprite, 0.398, 0.032, 0.094, 0.082);
                else
                    desenhaSprite(idTexturaSheet, vetorInimigos[i].sprite, 0.413, 0.207, 0.090, 0.082);
            }
        }
    } else if (chefao.sprite.ativo) {
        desenhaSprite(idTexturaChefao, chefao.sprite, 0, 0, 1, 1);
    }

}

void desenhaFase()
{
    // desenha nave do jogador
    desenhaSprite(idTexturaSheet, jogador.sprite, 0.000, 0.007, 0.109, 0.073);

    // desenha tiro do jogador se ativo
    if (tiroJogador.ativo) {
        if (TIRO_ESPECIAL)
            desenhaSprite(idTexturaSheet, tiroJogador, 0.815, 0.265, 0.012, 0.055);
        else
            desenhaSprite(idTexturaSheet, tiroJogador, 0.834, 0.339, 0.008, 0.036);
    }

    // desenha tiro dos inimigos se ativo
    if (tiroInimigo.ativo)
        desenhaSprite(idTexturaSheet, tiroInimigo, 0.835, 0.151, 0.008, 0.055);

    // desenha tiro dos inimigos especiais se ativo
    if (tiroInimigoEspecial.ativo)
        desenhaSprite(idTexturaSheet, tiroInimigoEspecial, 0.815, 0.265, 0.012, 0.055);

    // desenha naves ativas dos inimigos
    desenhaInimigos();

    // desenha itens de drop se ativos
    desenhaItens();
}

void desenhaHud()
{
    // desenha o fundo
    glColor4f(1, 1, 1, 0.4);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, 1);
        glVertex3f(LARGURA_DO_MUNDO, 0, 1);
        glVertex3f(LARGURA_DO_MUNDO, ALTURA_HUD, 1);
        glVertex3f(0, ALTURA_HUD, 1);
    glEnd();

    // mostra os pontos do jogador
    glColor4f(1, 1, 1, 1);
    snprintf(STR_PONTOS, 10, "%d", PONTOS); // converte int para string
    escreveTexto(GLUT_BITMAP_HELVETICA_18, "PONTOS: ", LARGURA_DO_MUNDO - 45, 2);
    escreveTexto(GLUT_BITMAP_HELVETICA_18, STR_PONTOS, LARGURA_DO_MUNDO - 20, 2);

    // desenha vidas
    for (int i = 0; i < jogador.vidas; i++)
        desenhaSprite(idTexturaSheet, iconeVidas[i], 0.490, 0.006, 0.036, 0.025);
}

void desenharMinhaCena()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 1, 1);

    // desenhos
    if (FASES == chefe)
        desenhaFundoJogo(idTexturaFundoChefe);
    else
        desenhaFundoJogo(idTexturaFundo);
    desenhaFase();
    desenhaHud();

    if (JOGO == pause) {
        glColor3f(1, 0, 0);
        escreveTexto(GLUT_BITMAP_HELVETICA_18, "JOGO PAUSADO", LARGURA_DO_MUNDO/2 - 10, ALTURA_DO_MUNDO/2);
    } else if (JOGO == gameOver) {
        glColor3f(1, 0, 0);
        escreveTexto(GLUT_BITMAP_HELVETICA_18, "GAME OVER", LARGURA_DO_MUNDO/2 - 10, ALTURA_DO_MUNDO/2);
    } else if (JOGO == vitoria) {
        glColor3f(0, 1, 0);
        escreveTexto(GLUT_BITMAP_HELVETICA_18, "VITORIA", LARGURA_DO_MUNDO/2 - 10, ALTURA_DO_MUNDO/2);
    }

    glutSwapBuffers();
}

void proximaFase()
{
    FASES += 1;
    tiroInimigo.ativo = FALSE;
    tiroInimigoEspecial.ativo = FALSE;
    tiroJogador.ativo = FALSE;
    itemDrop.ativo = FALSE;

    if (FASES == terceira)
        timerRasanteInicio = time(0) - 15;

    inicializaInimigos();
    glutPostRedisplay();
}

int verificaVitoria()
{
    for (int i = 0; i < QTD_INIMIGOS; i++)
        if (vetorInimigos[i].sprite.ativo)
            return 0;

    if (FASES < chefe)
        proximaFase();
}

void verificaGameOver()
{
    if (jogador.vidas == 0) {
        jogador.sprite.ativo = FALSE;
        JOGO = gameOver;
    }
}

void pausa()
{
    if (JOGO == ativo)
        JOGO = pause;
    else if (JOGO == pause)
        JOGO = ativo;
    glutPostRedisplay();
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
            inicializaJogo();
            break;
        default:
            break;
    }
}

void teclasEspeciais(int key, int x, int y)
{
    switch(key) {
        case GLUT_KEY_LEFT:
            movimentaJogador('e');
            break;
        case GLUT_KEY_RIGHT:
            movimentaJogador('d');
            break;
    }
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

void inicializaHud()
{
    int x = 20;
    int y = 5;
    int distancia = 10;

    for (int i = 0; i < MAX_VIDAS; i++) {
        inicializaSprite(&iconeVidas[i], x, y, 5, 5, 5);
        x += distancia;
        if(i >= jogador.vidas)
            iconeVidas[i].ativo = FALSE;
    }
}

void inicializaItem(tipoSprite* item, float x, float y)
{
    inicializaSprite(item, x, y, 8, 8, 4);
    item->velocidade = 1;
}

void inicializaTiro(tipoSprite* tiro, float x, float y)
{
    inicializaSprite(tiro, x, y, 1, 5, 1);
    if (tiro == &tiroJogador) {
        tiro->velocidade = 2;
        if (TIRO_ESPECIAL) {
            tiro->dimensoes.x = 2;
            tiro->dimensoes.y = 10;
            tiro->raio = 3;
        }
    }
    else if (tiro == &tiroInimigoEspecial) {
        tiro->dimensoes.x = 2;
        tiro->dimensoes.y = 10;
        tiro->raio = 3;
        tiro->velocidade = 2;
    } else {
        tiro->velocidade = 1;
    }
}

void inicializaChefao()
{
    inicializaSprite(&chefao.sprite, LARGURA_DO_MUNDO/2, ALTURA_DO_MUNDO * 0.8, 60, 60, 60/2);
    chefao.sprite.velocidade = 0.7;
    chefao.vidas = 10;
    chefao.rasante = FALSE;
    chefao.chefao = TRUE;
}

void inicializaRasantes()
{
    for (int i = 0; i < MAX_RASANTES; i++) {
        int sorteio = rand() % QTD_INIMIGOS;
        while (vetorInimigos[sorteio].rasante || vetorInimigos[sorteio].especial)
            sorteio = rand() % QTD_INIMIGOS;
        vetorInimigos[sorteio].rasante = TRUE;
        vetorInimigos[sorteio].vetorRasante = i;
    }
}

void inicializaEspeciais()
{
    for (int i = 0; i < MAX_ESPECIAIS; i++) {
        int sorteio = rand() % QTD_INIMIGOS;
        while (vetorInimigos[sorteio].especial)
            sorteio = rand() % QTD_INIMIGOS;
        vetorInimigos[sorteio].especial = TRUE;
    }
}

void inicializaInimigosNormais()
{
    int x = 20;
    int y = ALTURA_DO_MUNDO - 20;
    int dx = 20;
    int dy = 30;
    for (int i = 0; i < QTD_INIMIGOS; i++) {
        inicializaSprite(&vetorInimigos[i].sprite, x, y, 15, 15, 15/2);
        vetorInimigos[i].sprite.velocidade = 0.5;
        vetorInimigos[i].vidas = 1;
        vetorInimigos[i].especial = FALSE;
        vetorInimigos[i].chefao = FALSE;
        vetorInimigos[i].rasante = FALSE;
        x += dx;
        if ((i+1) % MAX_INIMIGOS_LINHA == 0) {
            y -= dy;
            x = dx;
        }
    }
}

void inicializaInimigos()
{
    if (FASES < chefe) {
        inicializaInimigosNormais();

        if (FASES == segunda || FASES == terceira)
            inicializaEspeciais();

        if (FASES == terceira)
            inicializaRasantes();

    } else if (FASES == chefe)
        inicializaChefao();
}

void inicializaJogador()
{
    inicializaSprite(&jogador.sprite, LARGURA_DO_MUNDO/2, ALTURA_DO_MUNDO * 0.1, 20, 20, 20/2);
    jogador.sprite.velocidade = 2;
}

void inicializaJogo()
{
    FASES = primeira;
    PONTOS = 0;
    TIRO_ESPECIAL = FALSE;
    ATIVA_RASANTE = FALSE;
    T = 0;
    jogador.vidas = VIDAS_INICIAIS;
    tiroInimigo.ativo = FALSE;
    tiroInimigoEspecial.ativo = FALSE;
    tiroJogador.ativo = FALSE;
    itemDrop.ativo = FALSE;
    inicializaJogador();
    inicializaInimigos();
    inicializaHud();
    JOGO = ativo;
    glutPostRedisplay();
}

void redimensiona(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, LARGURA_DO_MUNDO, 0, ALTURA_DO_MUNDO, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void inicializa()
{
    // inicialização do glut
    glutInitContextVersion(1, 1);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Jogo");

    glClearColor(1, 1, 1, 1);

    // habilita mesclagem de cores
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // carrega texturas
    idTexturaFundo = carregaTextura("imagens/fundo.png");
    idTexturaFundoChefe = carregaTextura("imagens/fundochefe.png");
    idTexturaSheet = carregaTextura("imagens/sheet.png");
    idTexturaChefao = carregaTextura("imagens/chefao.png");
}

int main(int argc, char** argv)
{
    // inicializa gerador de números aleatórios
    srand(time(0));

    // aloca espaço para os vetores de pontos para a curva de Bezier
    vetoresPontos[0] = (tipoVetor2d*) malloc(6 * sizeof(tipoVetor2d*));
    vetoresPontos[1] = (tipoVetor2d*) malloc(6 * sizeof(tipoVetor2d*));

    // inicializa o glut
    glutInit(&argc, argv);
    inicializa();

    // registro de callbacks
    glutDisplayFunc(desenharMinhaCena);
    glutReshapeFunc(redimensiona);
    glutKeyboardFunc(teclaPressionada);
    glutSpecialFunc(teclasEspeciais);

    // agrupa funcoes de movimento em um só timer
    glutTimerFunc(16, movimentos, 0);

    // inicializa variaveis do jogo
    inicializaJogo();

    // main loop
    glutMainLoop();

    // libera a memoria dos vetores da curva de Bezier
    free(vetoresPontos[0]);
    free(vetoresPontos[1]);

    return 0;
}
