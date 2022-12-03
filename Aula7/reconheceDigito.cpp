//fase5.cpp

#include <cekeikon.h>

#define RAZAO 0.8663
#define CONTRASTE 3
#define BRILHO -0.85

MNIST mnist(14, true, true);
flann::Index ind(mnist.ax,flann::KDTreeIndexParams(4));

void reconheceDigito(int* digitoReconhecido, MNIST mnist, Mat_<COR> a, int* x, bool* card, int* escalaCard) {
    TimePoint t1=timePoint();
    
    
    Mat_<FLT> T;
    Mat_<FLT> TRezise[10];
    le(T,"quadrado.png");

    vector<int> indices(1); vector<float> dists(1);

    Mat_<FLT> b;
    Mat_<FLT> RCC[10];
    

    float escala[10];
    escala[0] = 0.1721;
    int iCard, jCard;

    Mat_<COR> digito;
    Mat_<FLT> digitoFloat, digitoBox;

    int numero = 0;

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
        if(escalasCC[k] == false){
            RNCC[k]=matchTemplateSame(b,TRezise[k],CV_TM_CCOEFF_NORMED,0.0);
            escalasCC[k] == true;
        }
    }
    bool achou = false;
    
    for (int l = 0; l < 20; l++)
    {
        int i = pixel[l][1];
        int j = pixel[l][2];
        int k = pixel[l][3];
        if (RNCC[k][i][j] > 0.55 && achou == false){
            *card == true;
            iCard = i;
            jCard = j;
            *escalaCard = pixel[l][3];
            line(a, Point(j - (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Point(j + (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Scalar(0,128,128),6);
            line(a, Point(j + (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Point(j + (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Scalar(0,128,128),6);
            line(a, Point(j + (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Point(j - (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Scalar(0,128,128),6);
            line(a, Point(j - (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Point(j - (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Scalar(0,128,128),6);
            if (k < 2){
                    //copia o trecho do frame contendo o digito a ser lido
                    a(Rect(j - (int)(escala[k]*401)*1/5, i - (int)(escala[k]*401)*1/5, (int)(escala[k]*401)*2/5, (int)(escala[k]*401)*2/5)).copyTo(digito);
                    converte(digito,digitoFloat);
                    digitoBox = mnist.bbox(digitoFloat);
                    digitoBox.convertTo(digitoBox, -1, CONTRASTE, BRILHO); //ajuste de contraste e brilho
                    // transforma matriz em array 1D
                    std::vector<float> array;
                    if (digitoBox.isContinuous()) {
                        array.assign((float*)digitoBox.data, (float*)digitoBox.data + digitoBox.total()*digitoBox.channels());
                    } else {
                        for (int i = 0; i < digitoBox.rows; ++i) {
                            array.insert(array.end(), digitoBox.ptr<float>(i), digitoBox.ptr<float>(i)+digitoBox.cols*digitoBox.channels());
                        }
                    }
                    ind.knnSearch(array,indices,dists,1,flann::SearchParams(32));

                    numero = mnist.ay(indices[0]);

                    *digitoReconhecido = numero;
                    putText(a, std::to_string(numero), Point(j - (int)(escala[k]*401)/2-20, i+15), 0, 1, Scalar(0,0,255), 2);
            }
            break;
        }
    }
    *x = (320/2)-jCard;
    //return numero;
}
