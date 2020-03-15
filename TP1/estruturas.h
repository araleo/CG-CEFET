typedef struct vetor2dStruct
{
    float x, y;
} tipoVetor2d;

typedef struct spriteStruct
{
    tipoVetor2d posicao;
    tipoVetor2d dimensoes;
    int textura;
} tipoSprite;

typedef struct naveStruct
{
    tipoSprite sprite;
    int velocidade;
} tipoNave;

typedef struct tiroStruct
{
    tipoSprite sprite;
    int velocidade;
} tipoTiro;
