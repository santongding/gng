#pragma once
#include "type.h"
#include "config.h"

void list_impl(const config_t &config, commit_handle_t commit);

void commit_impl(config_t &config);

void checkout_impl(config_t &config, commit_handle_t commit, checkout_op op);