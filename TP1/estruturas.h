typedef struct vetor2dStruct
{
    float x, y;
} tipoVetor2d;

typedef struct spriteStruct
{
    tipoVetor2d posicao;
    tipoVetor2d dimensoes;
    float raio;
    float velocidade;
    int ativo;
} tipoSprite;

typedef struct jogadorStruct
{
    tipoSprite sprite;
    int vidas;
} tipoJogador;

typedef struct inimigoStruct
{
    tipoSprite sprite;
    int especial;
    int chefao;
    int vidas;
    int rasante;
    int vetorRasante;
} tipoInimigo;
