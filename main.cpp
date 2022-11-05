#include <curl/curl.h>
#include <curl/easy.h>

#include "include/http_client.hpp"

int main(int argc, char **argv) {
  curl_global_init(CURL_GLOBAL_DEFAULT);
  return 0;
}
