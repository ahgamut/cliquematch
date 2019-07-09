#ifndef TRIPLE_H
#define TRIPLE_H

typedef unsigned int u32;

template <class T>
struct triple {
    u32 first, second;
    T dist;

    bool operator>(const triple<T> &BB) const { return (this->dist > BB.dist); }
    bool operator<(const triple<T> &BB) const { return (this->dist < BB.dist); }
    bool operator==(const triple<T> &BB) const {
        return (this->dist == BB.dist);
    }

    bool operator>(const T &dist_val) const { return (this->dist > dist_val); }
    bool operator<(const T &dist_val) const { return (this->dist < dist_val); }
    bool operator==(const T &dist_val) const {
        return (this->dist == dist_val);
    }

    void disp();
};

template <class T>
short binary_find2(triple<T> *a, u32 N, T val, u32 &loc) {
    int beg = 0, end = N - 1, mid = (beg + end / 2);
    if (a[end] < val) {
        loc = end;
        return -1;
    }
    while (beg <= end) {
        loc = mid;
        if (a[mid] == val)
            return 1;
        else if (a[mid] < val)
            beg = mid + 1;
        else
            end = mid - 1;
        mid = (beg + end) / 2;
    }
    return 0;
}

typedef triple<double> dist_2d;

#endif /* TRIPLE_H */
