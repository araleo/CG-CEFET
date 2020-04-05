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
    int especial;
} tipoSprite;
