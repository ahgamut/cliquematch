#ifndef PAIR_DIST_H
#define PAIR_DIST_H

// A structure to contain the indices of two elements and the distance between
// them. Given an array of items X, where the distance between two elements is
// measured in T, a pair_dist instance with first=0, second=1, has
// dist=distance(X[0], X[1])
template <class T>
struct pair_dist {
    unsigned int first, second;
    T dist;

    bool operator>(const pair_dist<T> &BB) const {
        return (this->dist > BB.dist);
    }
    bool operator<(const pair_dist<T> &BB) const {
        return (this->dist < BB.dist);
    }
    bool operator==(const pair_dist<T> &BB) const {
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
short binary_find2(pair_dist<T> *a, unsigned int N, T val, unsigned int &loc) {
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

#endif /* PAIR_DIST_H */
