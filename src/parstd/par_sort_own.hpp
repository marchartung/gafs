template <typename T>
void SortParallelMerge(std::vector<T>& data) {
  constexpr size_t thread_fac = 8;
  if (data.size() < thread_fac * omp_get_max_threads()) {
    std::sort(data.begin(), data.end());
    return;
  }
  const size_t d = std::log2(thread_fac * omp_get_max_threads()),
               nt = std::pow(2, d), m = data.size() / nt;
  std::vector<std::vector<T>> recv_data(nt), work_data(nt);
#pragma omp parallel for schedule(static)
  for (size_t i = 0; i < recv_data.size(); ++i) {
    const size_t b = std::min(i * m, data.size()),
                 e = std::min((i + 1) * m, data.size());
    std::vector<T> tdata(data.begin() + b, data.begin() + e);
    std::sort(tdata.begin(), tdata.end());

    if (e < m) {
      std::fill_n(std::back_inserter(tdata), m - e,
                  std::numeric_limits<T>::max());
    }

    std::swap(tdata, recv_data[i]);
    work_data[i] = std::vector<T>(m);
  }

  for (size_t i = 0; i < d; ++i) {
#pragma omp parallel for schedule(static)
    for (size_t tid = 0; tid < recv_data.size(); ++tid) {
      const size_t nid = tid ^ (1 << i);
      std::vector<T> tdata = std::move(work_data[tid]);
      const T *a = recv_data[tid].data(), *b = recv_data[nid].data();
      if (tid & (1 << i)) {
        // high
        size_t k = m - 1, l = m - 1;
        for (size_t i = m; i > 0; --i) {
          const T va = a[k], vb = b[l];
          const bool sm = (va > vb);
          tdata[i - 1] = sm ? va : vb;
          k -= sm;
          l -= !sm;
        }
      } else {
        // low
        size_t k = 0, l = 0;
        for (size_t i = 0; i < m; ++i) {
          const T va = a[k], vb = b[l];
          const bool sm = (va < vb);
          tdata[i] = sm ? va : vb;
          k += sm;
          l += !sm;
        }
      }
      work_data[tid] = std::move(tdata);
    }

#pragma omp parallel for schedule(static)
    for (size_t tid = 0; tid < recv_data.size(); ++tid) {
      std::swap(work_data[tid], recv_data[tid]);
    }
  }
#pragma omp parallel for schedule(static)
  for (size_t tid = 0; tid < recv_data.size(); ++tid) {
    const size_t b = std::min(tid * m, data.size()),
                 e = std::min((tid + 1) * m, data.size());
    std::copy(recv_data[tid].begin(), recv_data[tid].begin() + (e - b),
              data.begin() + b);
  }
}