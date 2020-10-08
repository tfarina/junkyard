#include "ab.h"

#include "common.h"

int main(int argc, char **argv) {
  int rc;

  ab_init();

  ab_load_contacts();

  rc = print_contact_list(ab_get_contact_list());

  ab_fini();

  return rc;
}