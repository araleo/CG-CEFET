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
    float velocidade;
} tipoNave;

typedef struct tiroStruct
{
    tipoSprite sprite;
    float velocidade;
    int ativo;
} tipoTiro;
