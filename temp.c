tipoSprite blocos[27]; //BRUNA: Criei um vetor do tipo tipoSprite


void fazquadrado(tipoSprite blocos) //BRUNA: Desenhas os blocos na tela
{
    glColor3f(0,1,0);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(blocos.posicao.x, blocos.posicao.y, 0);
        glVertex3f(blocos.posicao.x + blocos.dimensoes.x, blocos.posicao.y, 0);
        glVertex3f(blocos.posicao.x + blocos.dimensoes.x, blocos.posicao.y + blocos.dimensoes.y, 0);
        glVertex3f(blocos.posicao.x, blocos.posicao.y + blocos.dimensoes.y, 0);
    glEnd();
} //BRUNA


void desenhaBlocos() //BRUNA: Configura as posições e dimensões dos blocos
{
    //glClear (GL_COLOR_BUFFER_BIT);
    int quadradosPorLinha = 0;
    int maxQuadradosLinha = 10;
    int posicaox = 0;
    int posicaoy = 50;
    int quantidadeDeFileiras = 0;
    int quadrados = 0;
    int incremento = 10;

    while(quantidadeDeFileiras < 3)
    {
        while(quadradosPorLinha < maxQuadradosLinha)
        {
            blocos[quadrados].posicao.x = posicaox;
            blocos[quadrados].posicao.y = posicaoy;
            blocos[quadrados].dimensoes.x = 15;
            blocos[quadrados].dimensoes.y = 5;

            fazquadrado(blocos[quadrados]);
            posicaox = posicaox + 20;
            quadradosPorLinha = quadradosPorLinha + 1;
            quadrados = quadrados + 1;
        }

        posicaoy = posicaoy - 10;
        maxQuadradosLinha = maxQuadradosLinha - 1;
        quadradosPorLinha = 0;
        posicaox = incremento;
        incremento = incremento + 10;
        quantidadeDeFileiras = quantidadeDeFileiras + 1;
    }
    // glEnd();
}
