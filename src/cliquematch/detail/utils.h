#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <vector>
#include <utility>

typedef uint64_t u64;

namespace cliquematch
{
namespace detail
{
    // to help CALLERs of load_memory/clear_memory
    constexpr u64 BITS_PER_U64 = 8 * sizeof(u64);
    
    enum BFResult : int8_t
    {
        FOUND = 1,
        NOT_FOUND = 0,
        OUTSIDE = -1 /* unused, use if a[end] < val */
    };

    /* this function is called to check if two vertices are neighbors
     * the parameters are usually the list of neighbors of a vertex,
     * the vertex degree, a potential neighbor, and the location of the neighbor
     */
    inline BFResult binary_find(const u64* const a, const u64 N, const u64 val,
                                u64& loc)
    {
        /* modified binary search, returns location by reference
         * return FOUND if found, NOT_FOUND if not found
         */
        u64 beg = 0, end = N - 1, mid;
        if (a[beg] > val)
        {
            loc = beg;
            return NOT_FOUND;
        }
        while (beg <= end)
        {
            mid = beg + ((end - beg) >> 1);
            if (a[mid] == val)
            {
                loc = mid;
                return FOUND;
            }
            else if (a[mid] < val)
                beg = mid + 1;
            else
                end = mid - 1;
        }
        return NOT_FOUND;
    }

    void insertionSort(u64* p0, u64* p1, u64 start, u64 end);
    void radixSort(u64* p0, u64* p1, u64 start, u64 end, u64 shift = 0,
                   bool key_first = true, const u64 max_shift = 28);
    u64 modUnique(u64* p0, u64* p1, u64 start, u64 end);
    void clean_edges(u64, std::pair<std::vector<u64>, std::vector<u64>>&);

}  // namespace detail
}  // namespace cliquematch
#endif /* UTILS_H */
