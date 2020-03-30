#include <math.h>

#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))

float formulaDistancia(float x1, float x2, float y1, float y2)
{
    float x = max(x1, x2) - min(x1, x2);
    float y = max(y1, y2) - min(y1, y2);
    return sqrt(pow(x, 2) + pow(y, 2));
}
