#include <lzx/platform.hpp>

int main()
{
  auto stamp = lzx::platform_clock::now();

  LZXEMB_DBG("Sleeping 100 milliseconds...\r\n");

  lzx::sleep(lzx::milliseconds(100));

  auto elapsed = lzx::platform_clock::now() - stamp;

  LZXEMB_DBG("%lld us\r\n", std::chrono::duration_cast<lzx::microseconds>(elapsed).count());
  LZXEMB_DBG("%lld ms\r\n", std::chrono::duration_cast<lzx::milliseconds>(elapsed).count());
  LZXEMB_DBG("%f s\r\n", std::chrono::duration_cast<lzx::seconds>(elapsed).count());

  stamp = lzx::platform_clock::now();

  LZXEMB_DBG("Sleeping 100000 microseconds...\r\n");

  lzx::sleep(lzx::microseconds(100000));

  elapsed = lzx::platform_clock::now() - stamp;

  LZXEMB_DBG("%lld us\r\n", std::chrono::duration_cast<lzx::microseconds>(elapsed).count());
  LZXEMB_DBG("%lld ms\r\n", std::chrono::duration_cast<lzx::milliseconds>(elapsed).count());
  LZXEMB_DBG("%f s\r\n", std::chrono::duration_cast<lzx::seconds>(elapsed).count());

  stamp = lzx::platform_clock::now();

  LZXEMB_DBG("Sleeping 0.1 second...\r\n");

  lzx::sleep(lzx::seconds(0.1));

  elapsed = lzx::platform_clock::now() - stamp;

  LZXEMB_DBG("%lld us\r\n", std::chrono::duration_cast<lzx::microseconds>(elapsed).count());
  LZXEMB_DBG("%lld ms\r\n", std::chrono::duration_cast<lzx::milliseconds>(elapsed).count());
  LZXEMB_DBG("%f s\r\n", std::chrono::duration_cast<lzx::seconds>(elapsed).count());

  return 0;
}