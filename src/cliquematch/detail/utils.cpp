#include <detail/utils.h>

namespace cliquematch
{
namespace detail
{
    /* I use u64 everywhere, so I know I am definitely getting a 64bit value
     * to work with during any calculation.
     */

    /* Insertion sort performed on a pair of arrays simultaneously.
     * Results in the pair (p0[i], p1[i]) sorted in increasing order
     * from start to end
     */
    void insertionSort(u64* p0, u64* p1, u64 start, u64 end)
    {
        for (u64 i = start + 1; i < end; i++)
        {
            for (u64 j = i; j != start && (p0[j - 1] != p0[j] ? p0[j - 1] > p0[j]
                                                              : p1[j - 1] > p1[j]);
                 j--)
            {
                std::swap(p0[j], p0[j - 1]);
                std::swap(p1[j], p1[j - 1]);
            }
        }
    }

    /* top-down radix sort performed on a pair of vectors simultaneously
     * called recursively downgrades to insertion sort when required
     * uses 4 bits i.e. 16 bins for sorting
     * Results in the pair (p0[i], p1[i]) sorted in increasing order
     * from start to end
     */
    void radixSort(u64* p0, u64* p1, u64 start, u64 end, u64 shift, bool key_first,
                   const u64 max_shift)
    {
        u64 i, j, k;
        u64 starts[16] = {0};
        u64 ends[16] = {0};
        auto p = key_first ? p0 : p1;

        for (i = start; i < end; i++) ends[(p[i] >> shift) & 0xF]++;
        starts[0] = start;
        ends[0] += start;
        for (i = 1; i < 16; i++)
        {
            starts[i] = ends[i - 1];
            ends[i] = ends[i] + ends[i - 1];
        }

        for (i = 0; i < 16; i++)
        {
            for (j = starts[i]; j < ends[i];)
            {
                k = (p[j] >> shift) & 0xF;
                if (k == i)
                    j++;
                else
                {
                    std::swap(p0[j], p0[starts[k]]);
                    std::swap(p1[j], p1[starts[k]]);
                    starts[k]++;
                }
            }
        }

        if (shift == 0)
        {
            if (!key_first) return;
            shift = max_shift;
            key_first = false;
        }
        else
            shift -= 4;

        starts[0] = start;
        for (i = 1; i < 16; i++) starts[i] = ends[i - 1];
        // parallelization gives 2x speedup
        // #pragma omp parallel for
        for (i = 0; i < 16; i++)
        {
            if (starts[i] >= ends[i])
                continue;
            else if (ends[i] - starts[i] <= 32)
                insertionSort(p0, p1, starts[i], ends[i]);
            else
                radixSort(p0, p1, starts[i], ends[i], shift, key_first, max_shift);
        }
    }

    /* modification of std::unique implemented for a pair of vectors
     * if (p0[i] == p0[j] && p1[i] == p1[j]) then p0[j], p1[j] are removed
     * CALLER has to resize the array after completion
     */
    u64 modUnique(u64* p0, u64* p1, u64 start, u64 end)
    {
        if (start == end) return end;
        u64 result = start;
        while (++start != end)
        {
            if ((p0[result] != p0[start] || p1[result] != p1[start]) &&
                ++result != start)
            {
                p0[result] = p0[start];
                p1[result] = p1[start];
            }
        }
        return ++result;
    }

    /* sort edges in increasing order and remove duplicates
     * the custom radixSort+unique is faster than std::sort+unique on a vector<pair>,
     * not to mention the memory savings in the graph constructor
     */
    void clean_edges(u64 n_vert, std::pair<std::vector<u64>, std::vector<u64>>& edges)
    {
        u64 m = n_vert, radix_shift = 0;
        for (; m != 0; m >>= 4) radix_shift += 4;
        radix_shift -= 4;
        radixSort(edges.first.data(), edges.second.data(), 0, edges.first.size(),
                  radix_shift, true, radix_shift);
        m = modUnique(edges.first.data(), edges.second.data(), 0, edges.first.size());
        edges.first.resize(m);
        edges.second.resize(m);
    }
}  // namespace detail
}  // namespace cliquematch
