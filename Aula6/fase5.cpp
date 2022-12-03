//fase5.cpp

#include <cekeikon.h>

#define RAZAO 0.8663
#define CONTRASTE 3
#define BRILHO -0.85

MNIST mnist(14, true, true);

int main(int argc, char *argv[]) {
    mnist.le("/home/oliver/Documents/PSI3442/Aula6/mnist");
    vector<int> indices(1); vector<float> dists(1);
    flann::Index ind(mnist.ax,flann::KDTreeIndexParams(4));

    Mat_<FLT> T ;
    Mat_<FLT> TRezise[10];
    le(T,argv[2]);
    VideoCapture vi(argv[1]);

    if (!vi.isOpened())
        erro("Erro abertura video entrada");

    VideoWriter vo(argv[3],
    CV_FOURCC('X','V','I','D'),
    30,
    Size(320+120,240));
    if (!vo.isOpened())
        erro("Erro abertura video saida");
    
    Mat_<COR> a;
    Mat_<FLT> b;
    Mat_<FLT> RCC[10];
    Mat_<COR> tela;
    

    float escala[10];
    escala[0] = 0.1721;
    int iCard, jCard;
    bool card = false;
    TimePoint t1=timePoint();
    int quadros = 0;

    // calculo da escala
    for (int i = 0; i < 10; i++)
    {
        if (i > 0)
            escala[i] = escala[i-1] * RAZAO;
    }


    while (true) {
        COR cinza(128,128,128);
        Mat_<COR> digito;
        Mat_<COR> digitoResize(120,120,cinza);
        Mat_<COR> digitoBoxResize(120,120,cinza);
        Mat_<FLT> digitoFloat, digitoBox;

        quadros++;
        vi >> a;
        if (!a.data)
            break;
        converte(a,b);
        
        // busca por NCC
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
        // encontra os maiores valores
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
        // busca por NCC
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
        
        // busca pelo cartao pelos resultados CC e NCC
        for (int l = 0; l < 20; l++)
        {
            int numero;
            int i = pixel[l][1];
            int j = pixel[l][2];
            int k = pixel[l][3];
            if (RNCC[k][i][j] > 0.55 && card == false){
                card == true;
                iCard = i;
                jCard = j;
                line(a, Point(j - (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Point(j + (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Scalar(0,128,128),thickness);
                line(a, Point(j + (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Point(j + (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Scalar(0,128,128),thickness);
                line(a, Point(j + (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Point(j - (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Scalar(0,128,128),thickness);
                line(a, Point(j - (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Point(j - (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Scalar(0,128,128),thickness);
                
                // leitura do digito para escalas menores que 2
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

                    
                    putText(a, std::to_string(numero), Point(j - (int)(escala[k]*401)/2-20, i+15), 0, 1, Scalar(0,0,255), 2);

                    converte(digitoBox,digitoBoxResize);
                    resize(digito, digitoResize, Size(120,120), 0, 0, INTER_NEAREST);
                    resize(digitoBoxResize, digitoBoxResize, Size(120,120), 0, 0, INTER_NEAREST);
                }

                break;
            }
        }

        tela = grudaH(a,grudaV(digitoBoxResize, digitoResize));
        
        vo << tela;
    }
    TimePoint t2=timePoint();
    double t=timeSpan(t1,t2);
    cout<<"Quadros="<<quadros <<" tempo="<<t<<"s fps="<<quadros/t<<endl;
}
