#include "commit-mgr.h"
#include "debug/log.h"
#include "debug/debug-help.h"
#include "bytes-utils.h"
int main()
{
  const bytes_t d = "!";
  bytes_t data;
  for (int i = 0; i < 10240; i++)
  {
    data = data + d;
  }
  auto a = compress_data(data);
  debug("origin siz:%lu, compress siz:%lu", data.size(), a.size());
  auto b = decompress_data(a);
  EQ(b, data);
}
