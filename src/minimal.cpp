/*
Copyright 2025 Robyn Kirkman

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <badiff/minimal.hpp>

#include <sstream>

#include <badiff/alg/inertial_graph.hpp>
#include <badiff/q/minimize_op_queue.hpp>
#include <badiff/q/vector_op_queue.hpp>

namespace badiff {

int diff(char *dst, int dst_len, const char *original, int original_len,
         const char *target, int target_len) {
  alg::InertialGraph graph;
  graph.Compute(original, original_len, target, target_len);
  std::unique_ptr<q::OpQueue> queue = graph.MakeOpQueue();
  queue.reset(new q::MinimizeOpQueue(std::move(queue)));
  std::ostringstream os(std::ios::binary);
  queue->Serialize(os);
  std::string s = os.str();
  if (s.size() > dst_len)
    return -1;
  std::copy(s.begin(), s.end(), dst);
  return s.size();
}

int patch(char *dst, int dst_len, const char *original, int original_len,
          const char *diff, int diff_len) {
  std::string s1(diff, diff + diff_len);
  std::istringstream is1(s1, std::ios::binary);
  q::VectorOpQueue queue;
  queue.Deserialize(is1);
  std::string s2(original, original + original_len);
  std::istringstream is2(s2, std::ios::binary);
  std::ostringstream os(std::ios::binary);
  queue.Apply(is2, os);
  std::string s3 = os.str();
  if (s3.size() > dst_len)
    return -1;
  std::copy(s3.begin(), s3.end(), dst);
  return s3.size();
}

} // namespace badiff