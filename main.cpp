
#include <cstdlib>
#include <cstdio>
#include <list>
#include "opencv2/opencv.hpp"
#include "RunningStat.h"

using namespace std;
using namespace cv;

#define WINDOW "outputvid"
#define SKIP 4

RunningStat*** createFrameStats(Mat_<Vec3b> &frame) {
    int rows = frame.rows/SKIP;
    int cols = frame.cols/SKIP;
    RunningStat*** stats = new RunningStat**[rows];
    for (int i = 0; i < rows; i++) {
        stats[i] = new RunningStat*[cols];
        for (int j = 0; j < cols; j++) {
            stats[i][j] = new RunningStat[3];
        }
    }
    return stats;
}

void updateStats(Mat_<Vec3b> &frame, RunningStat*** stat, bool clearStats) {
    int rows = frame.rows/SKIP;
    int cols = frame.cols/SKIP;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < 3; k++) {
                if (clearStats) {
                    stat[i][j][k].clear();
                }
                stat[i][j][k].push(frame(i*SKIP, j*SKIP)[k]);
            }
        }
    }
}

int detectStaticPixels(Mat_<Vec3b> &frame, RunningStat*** stat, double staticDevThreshold) {
    int rows = frame.rows/SKIP;
    int cols = frame.cols/SKIP;
    int staticPixels = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < 3; k++) {
                if (stat[i][j][k].stddev() < staticDevThreshold) {
                    staticPixels++;
                }
            }
        }
    }
    return staticPixels;
}

void addBlackCornerToFrame(Mat_<Vec3b> &frame) {
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 50; j++) {
            for (int k = 0; k < 3; k++) {
                frame(i, j)[k] = 0;
            }
        }
    }
}

int sum(list<int> &l) {
    int val = 0;
    for (list<int>::iterator i = l.begin(); i != l.end(); i++) {
        val += *i;
    }
    return val;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("USAGE: test video_filename\n");
        return -1;
    }
    VideoCapture cap(argv[1]);
    if (!cap.isOpened()) {
        return -1;
    }

    namedWindow(WINDOW, CV_WINDOW_AUTOSIZE);

    Mat_<Vec3b> frame;
    Size size;
    RunningStat ***stats = NULL;
    while (true) {
        if (cap.grab()) {
            cap.retrieve(frame);
            size.width = frame.cols;
            size.height = frame.rows;
            stats = createFrameStats(frame);
            break;
        }
    }

    list<int> tests;
    int pixelMajority = ((size.width/SKIP) * (size.height/SKIP) * 3) * .85;
    bool gameIsPlaying = true;
    int count = 0;
    while (true) {
        // is there a better way to skip frames?
        if (cap.grab()) {
            cap.retrieve(frame);
            bool clearStats = false;
            count++;
            if (count % 20 == 0) {
                int staticPixels = detectStaticPixels(frame, stats, 2);
                printf("Static pixels: %d\n", staticPixels);
                tests.push_back(staticPixels < pixelMajority);
                if (tests.size() > 10) {
                    tests.pop_front();
                    gameIsPlaying = sum(tests) > 5;
                }
                clearStats = true;
            }
            updateStats(frame, stats, clearStats);
            if (!gameIsPlaying) {
                addBlackCornerToFrame(frame);
            }
            imshow(WINDOW, frame);
        }
        if (waitKey(30) == 27) { break; }
    }

    return 0;
}
