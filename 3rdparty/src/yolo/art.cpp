#include "network.h"
#include "utils.h"
#include "parser.h"
#include "option_list.h"
#include "blas.h"
#include "classifier.h"
//#include <sys/time.h>
#include <time.h>
#include <winsock.h>
#include "gettimeofday.h"

#ifdef OPENCV
#include "opencv2/highgui/highgui_c.h"
image get_image_from_stream(CvCapture *cap);
#endif


void demo_art(char *cfgfile, char *weightfile, int cam_index)
{
#ifdef OPENCV
    network net = parse_network_cfg(cfgfile);
    if(weightfile){
        load_weights(&net, weightfile);
    }
    set_batch_network(&net, 1);

    srand(2222222);
    CvCapture * cap;

    cap = cvCaptureFromCAM(cam_index);

    char *window = "ArtJudgementBot9000!!!";
    if(!cap) error("Couldn't connect to webcam.\n");
    cvNamedWindow(window, CV_WINDOW_NORMAL); 
    cvResizeWindow(window, 512, 512);
    int i;
    int idx[] = {37, 401, 434};
    int n = sizeof(idx)/sizeof(idx[0]);

    while(1){
        image in = get_image_from_stream(cap);
        image in_s = resize_image(in, net.w, net.h);
        show_image(in, window);

        float *p = network_predict(net, in_s.data);
#ifndef NO_STDOUT
        printf("\033[2J");
        printf("\033[1;1H");
#endif
        float score = 0;
        for(i = 0; i < n; ++i){
            float s = p[idx[i]];
            if (s > score) score = s;
        }
        score = score;
#ifndef NO_STDOUT
        printf("I APPRECIATE THIS ARTWORK: %10.7f%%\n", score*100);
        printf("[");
#endif
	int upper = 30;
        for(i = 0; i < upper; ++i){
#ifndef NO_STDOUT
            printf("%c", ((i+.5) < score*upper) ? 219 : ' ');
#endif
        }
#ifndef NO_STDOUT
        printf("]\n");
#endif

        free_image(in_s);
        free_image(in);

        cvWaitKey(1);
    }
#endif
}


void run_art(int argc, char **argv)
{
    int cam_index = find_int_arg(argc, argv, "-c", 0);
    char *cfg = argv[2];
    char *weights = argv[3];
    demo_art(cfg, weights, cam_index);
}

