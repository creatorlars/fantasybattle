#include <lzx/synthesis/parameter.hpp>
#include <lzx/platform.hpp>

int main()
{
  auto p1 = lzx::parameter<int32_t>();
  auto p2 = lzx::parameter<int32_t>();
  auto p3 = lzx::parameter<int32_t>();

  p3.connect(p2);
  p2.connect(p1);
  p1.set(13);

  LZXEMB_ASSERT(p3.get() == 13);

  return 0;
}