
#ifndef _RUNNING_STAT_H_
#define _RUNNING_STAT_H_

class RunningStat {
private:
    int count;
    double m, oldM, s, oldS;
public:
    RunningStat() {
        count = 0;
    }

    void clear() {
        count = 0;
    }

    void push(int val) {
        count++;
        if (count == 1) {
            oldM = m = val;
            oldS = 0.0;
        }
        else {
            m = oldM + (val - oldM) / count;
            s = oldS + (val - oldM) * (val - m);

            oldM = m;
            oldS = s;
        }
    }

    double mean() {
        return (count > 0) ? m : 0.0;
    }

    double variance() {
        return (count > 1) ? (s / (count - 1)) : 0.0;
    }

    double stddev() {
        return sqrt(variance());
    }
};

#endif
