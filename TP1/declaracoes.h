float formulaDistancia(float x1, float x2, float y1, float y2);
int detectaColisao(tipoSprite s1, tipoSprite s2);
void inicializaTiro(tipoSprite* tiro, float x, float y);
void desenhaSprite(GLuint textura, tipoSprite sprite, float sheetX, float sheetY, float comprimento, float altura);
void escreveTexto(void* fonte, char* texto, float x, float y);
GLuint carregaTextura(const char* arquivo);
void inicializaJogo();
void desenhaFundoJogo(GLuint textura);
void verificaGameOver();
int verificaVitoria();
void desenhaHud();
void inicializaItem(tipoSprite* item, float x, float y);
void inicializaInimigos();
tipoVetor2d formulaBezier(tipoVetor2d p0, tipoVetor2d p1, tipoVetor2d p2, tipoVetor2d pFinal, float t);
