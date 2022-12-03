#include <cekeikon.h>

#define RAZAO 0.8663

void templateMatching(Mat_<COR> a, int* x, int* card, int* escalaCard) {
    Mat_<FLT> T;
    Mat_<FLT> TRezise[10];
    le(T,"quadrado.png");

    Mat_<FLT> b;
    Mat_<FLT> RCC[10];
    

    float escala[10];
    escala[0] = 0.1721;
    int iCard, jCard;

    converte(a,b);
    for (int i = 0; i < 10; i++)
    {
        if (i > 0)
            escala[i] = escala[i-1] * RAZAO;
    }
    
    #pragma omp parallel for
    for (int i = 0; i < 10; i++){
        resize(T, TRezise[i], Size(), escala[i], escala[i], INTER_NEAREST);
        TRezise[i] = somaAbsDois(dcReject(TRezise[i],1.0));
        RCC[i]=matchTemplateSame(b,TRezise[i],CV_TM_CCORR,0.0);
    }
    float menor = 0;
    float pixel[20][5];
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            pixel[i][j] = 0;
        }
    }
    int count = 0;
    #pragma omp parallel for
    for (int k = 0; k < 10; k++)
    {
        for (int i = 0; i < 240; i++)
        {
            for (int j = 0; j < 320; j++)
            {
                if (RCC[k][i][j] > menor){
                    for (int l = 0; l < 20; l++)
                    {
                        int nEscala = pixel[l][3];

                        if ((pixel[l][1]+40 > i && pixel[l][1]-40 < i)||(pixel[l][2]+40> j && pixel[l][2]-40< j)){
                            if (pixel[l][0] <  RCC[k][i][j]){
                                pixel[l][0] = RCC[k][i][j];
                                pixel[l][1] = i;
                                pixel[l][2] = j;
                                pixel[l][3] = k;
                                break;
                            }
                        }
                        else if (pixel[l][0] == menor){
                            count++;
                            if (count > 20){
                                menor = RCC[k][i][j];
                            }
                            pixel[l][0] = RCC[k][i][j];
                            pixel[l][1] = i;
                            pixel[l][2] = j;
                            pixel[l][3] = k;
                            break;
                        }
                    }
                    for (int l = 0; l < 20; l++)
                    {
                        if (pixel[l][0] < menor){
                            menor = pixel[l][0];
                        }
                    }
                    
                }
            }
        }
    }
    
    Mat_<FLT> RNCC[10];
    bool escalasCC[10];
    for (int i = 0; i < 10; i++)
    {
        escalasCC[i] = false;
    }
    
    int thickness = 2;
    #pragma omp parallel for
    for (int l = 0; l < 20; l++)
    {
        int i = pixel[l][1];
        int j = pixel[l][2];
        int k = pixel[l][3];
        RNCC[k]=matchTemplateSame(b,TRezise[k],CV_TM_CCOEFF_NORMED,0.0);

        pixel[l][4] = RNCC[k][i][j];
        line(a, Point(j - (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Point(j + (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Scalar(128,128,0),thickness);
        line(a, Point(j + (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Point(j + (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Scalar(128,128,0),thickness);
        line(a, Point(j + (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Point(j - (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Scalar(128,128,0),thickness);
        line(a, Point(j - (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Point(j - (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Scalar(128,128,0),thickness);
        putText(a, std::to_string((int)pixel[l][3]), Point(j + (int)(escala[k]*401)/2+10, i-15), 0, 0.5, Scalar(128,128,0), 0.5);
        putText(a, std::to_string((int)(pixel[l][0]*100)), Point(j + (int)(escala[k]*401)/2+10, i), 0, 0.5, Scalar(128,128,0), 0.5);
        putText(a, std::to_string((int)(pixel[l][4]*100)), Point(j + (int)(escala[k]*401)/2+10, i+15), 0, 0.5, Scalar(128,128,0), 0.5);
    
    }
    bool achou = false;
    for (int l = 0; l < 20; l++)
    {
        int i = pixel[l][1];
        int j = pixel[l][2];
        int k = pixel[l][3];
        if (RNCC[k][i][j] > 0.55 && achou == false){
            *card = 1;
            iCard = i;
            jCard = j;
            *escalaCard = pixel[l][3];
            line(a, Point(j - (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Point(j + (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Scalar(0,128,128),6);
            line(a, Point(j + (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Point(j + (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Scalar(0,128,128),6);
            line(a, Point(j + (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Point(j - (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Scalar(0,128,128),6);
            line(a, Point(j - (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Point(j - (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Scalar(0,128,128),6);
            putText(a, std::to_string((int)pixel[l][3]), Point(j + (int)(escala[k]*401)/2+10, i-15), 0, 0.5, Scalar(0,128,128), 0.5);
            putText(a, std::to_string((int)(pixel[l][0]*100)), Point(j + (int)(escala[k]*401)/2+10, i), 0, 0.5, Scalar(0,128,128), 0.5);
            putText(a, std::to_string((int)(pixel[l][4]*100)), Point(j + (int)(escala[k]*401)/2+10, i+15), 0, 0.5, Scalar(0,128,128), 0.5);
        }
    }
    *x = (320/2)-jCard;
}
