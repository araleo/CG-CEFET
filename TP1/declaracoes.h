float formulaDistancia(float x1, float x2, float y1, float y2);
void inicializaTiro(tipoSprite* tiro, float x, float y);
void desenhaSprite(GLuint textura, tipoSprite sprite, float sheetX, float sheetY, float comprimento, float altura);
void escreveTexto(void* fonte, char* texto, float x, float y);
GLuint carregaTextura(const char* arquivo);
void inicializaJogo();
void desenhaFundoJogo(GLuint textura);
