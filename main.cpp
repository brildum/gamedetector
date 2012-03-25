
#include <cstdlib>
#include <cstdio>
#include <list>
#include "opencv2/opencv.hpp"
#include "RunningStat.h"

using namespace std;
using namespace cv;

#define WINDOW "outputvid"

RunningStat*** createFrameStats(Mat_<Vec3b> &frame) {
    RunningStat*** stats = new RunningStat**[frame.rows];
    for (int i = 0; i < frame.rows; i++) {
        stats[i] = new RunningStat*[frame.cols];
        for (int j = 0; j < frame.cols; j++) {
            stats[i][j] = new RunningStat[3];
        }
    }
    return stats;
}

void updateStats(Mat_<Vec3b> &frame, RunningStat*** stat, bool clearStats) {
    for (int i = 0; i < frame.rows; i++) {
        for (int j = 0; j < frame.cols; j++) {
            for (int k = 0; k < 3; k++) {
                if (clearStats) {
                    stat[i][j][k].clear();
                }
                stat[i][j][k].push(frame(i, j)[k]);
            }
        }
    }
}

int detectStaticPixels(Mat_<Vec3b> &frame, RunningStat*** stat, double staticDevThreshold) {
    int staticPixels = 0;
    for (int i = 0; i < frame.rows; i++) {
        for (int j = 0; j < frame.cols; j++) {
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
    cap >> frame;
    Size size(frame.cols/3, frame.rows/3);
    resize(frame, frame, size);
    RunningStat*** stats = createFrameStats(frame);

    list<int> tests;

    int pixelMajority = (size.width * size.height * 3) * .85;
    bool gameIsPlaying = true;
    int count = 0;
    while (true) {
        count++;
        bool clearStats = false;
        // is there a better way to skip frames?
        cap >> frame;
        cap >> frame;
        resize(frame, frame, size);
        if (count % 10 == 0) {
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
        if (waitKey(30) == 27) { break; }
    }

    return 0;
}
