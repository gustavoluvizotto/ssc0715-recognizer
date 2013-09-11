/* 
 * File:   main.cpp
 * Author: Gustavo Luvizotto Cesar - gustavoluvizotto@gmail.com - NUSP: 6783544
 *
 * Created on August 26, 2013, 8:01 PM
 */

#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>

using namespace std;
using namespace cv;

int main() {
    //    Mat frame = imread("0100.png"); /* imagem de entrada */
    Mat frame; /* quadros do video */
    Mat frame2;
    VideoCapture cap("depth.avi"); /* video de entrada */
    int minIE, minJE, maxIE, maxJE; /*para mao esquerda */
    float midYE, midXE;
    int minID, minJD, maxID, maxJD; /* para mao direitra */
    float midYD, midXD;
    int half; /* metade do quadro */
    Point2d centroE, centroD; /* coordenadas centrais para as maos */
    ofstream myfile("saida.txt"); /* arquivo de saida */
    Size ksize(0, 0); /* tipico para os filtros utilizados */
    Point anchor(-1, -1); /* tipico para os filtros utilizados */
    /*VideoWriter outputVideo; 
    int ex;
    Size S;*/

    if (!cap.isOpened()) {
        cout << "Video nao pode ser aberto" << endl;
        exit(1);
    }
    if (!myfile.is_open()) {
        cout << "Arquivo nao pode ser aberto" << endl;
        exit(2);
    }

    /* codigo para gerar video de saida */
    /* que nao funcionou para a minha versao do opencv...
    S = Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH), 
            (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    ex = static_cast<int> (cap.get(CV_CAP_PROP_FOURCC));
    outputVideo.open("final.avi", ex, cap.get(CV_CAP_PROP_FPS), S, true);

    if (!outputVideo.isOpened()) {
        cout << "Could not open the output video for write " << endl;
        exit(3);
    }
     */
    namedWindow("Reconhecedor");

    for (;;) {
        cap >> frame;

        /* transformando imagem de 3 canais em apenas 1 canal*/
        cvtColor(frame, frame2, CV_RGB2GRAY);

        /* filtros */
        erode(frame2, frame2, Mat(), anchor, 1);
        dilate(frame2, frame2, Mat(), anchor, 4);
        GaussianBlur(frame2, frame2, ksize, 0.7);

        /* variaveis usadas para calcular ponto minimo e maximo da mao do rapaz.
         * half indica que sera usado valores ate a metade da imagem ja que o rapaz
         * esta no centro da mesma.
         */
        maxIE = 0;
        maxJE = 0;
        minIE = frame2.rows + 1;
        minJE = frame2.cols + 1;
        maxID = 0;
        maxJD = 0;
        minID = frame2.rows + 1;
        minJD = frame2.cols + 1;
        half = frame2.cols / 2.0;

        /* vou percorrendo os pixels da imagem, retirando-se a região de borda
         * pois nela há valores errados de intensidade (pequeno degrade)
         */
        int gray = 0;
        for (int i = 50; i < frame2.rows - 60; i++) {
            for (int j = 50; j < frame2.cols - 150; j++) {
                gray = frame2.ptr<uchar>(i)[j];

                /* vejo se o nivel de cinza esta no intevalo de valores da mao e se
                 * eh a parte da esquerda ou direita da imagem
                 */
                if (gray >= 60 && gray <= 75) {
                    if (j <= half) { /* mao esquerda */
                        if (i < minIE)
                            minIE = i;
                        if (i > maxIE)
                            maxIE = i;
                        if (j < minJE)
                            minJE = j;
                        if (j > maxJE)
                            maxJE = j;
                    } else { /* mao direita */
                        if (i < minID)
                            minID = i;
                        if (i > maxID)
                            maxID = i;
                        if (j < minJD)
                            minJD = j;
                        if (j > maxJD)
                            maxJD = j;
                    }
                }
                gray = 0;
            }
        }

        /* com os valores das coordenadas minimas e maximas para x e y, calculo o
         * ponto central e faço uma circunferencia ao seu redor com raio de 10.
         */
        if (minIE > 0 && maxIE > 0 && minJE < frame.rows && maxJE < frame.cols) {
            midYE = (minIE + maxIE) / 2.0;
            midXE = (minJE + maxJE) / 2.0;
            centroE.x = midXE;
            centroE.y = midYE;
            myfile << "E " << centroE.x << ", " << centroE.y << endl;
            circle(frame2, centroE, 5, 10);
        }
        if (minID > 0 && maxID > 0 && minJD < frame.rows && maxJD < frame.cols) {
            midYD = (minID + maxID) / 2.0;
            midXD = (minJD + maxJD) / 2.0;
            centroD.x = midXD;
            centroD.y = midYD;
            myfile << "D " << centroD.x << ", " << centroD.y << endl;
            circle(frame2, centroD, 5, 10);
        }

        /*outputVideo << frame2;*/
        imshow("Reconhecedor", frame2);
        if (waitKey(30) >= 0)
            break;
    }

    destroyWindow("Reconhecedor");
    myfile.close();

    return 0;
}
