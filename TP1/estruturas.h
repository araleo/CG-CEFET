typedef struct vetorStruct
{
    float x, y;
} tipoVetor;

typedef struct spriteStruct
{
    tipoVetor posicao;
    tipoVetor dimensoes;
    int textura;
} tipoSprite;

typedef struct naveStruct
{
    tipoSprite sprite;
    int velocidade;
} navinha;
