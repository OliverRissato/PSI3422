//raspcam_cek.cpp
//compila raspcam_cek -c
#include <cekeikon.h>
int main() {
	VideoCapture w(0);
	if (!w.isOpened()) erro("Erro: Abertura de webcam 0.");
	w.set(CV_CAP_PROP_FRAME_WIDTH,320);
	w.set(CV_CAP_PROP_FRAME_HEIGHT,240);
	Mat_<COR> a;
	namedWindow("janela");
	
	VideoWriter vo("Aula2_licao1.avi",CV_FOURCC('X','V','I','D'),
					30,Size(320,240));
	
	while (true) {
		w >> a; // get a new frame from camera
		vo << a; //save frames in file
		imshow("janela",a);
		int ch=(signed char)(waitKey(30)); // E necessario (signed char)
		if (ch>=0) break;
	}
}
